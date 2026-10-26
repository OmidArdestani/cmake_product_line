#pragma once

#include "MenuRegistry.h"
#include "Icons/IconProvider.h"
#include <QMainWindow>
#include <QDebug>
#include <QToolBar>
#include <QToolButton>
#include <QScopedValueRollback>
#include <memory>

namespace pl::gui {

enum class ToolPlacement { MenuAndToolbar, MenuOnly };

// The application supplies its three enum types. No string IDs or widget
// construction are needed at call sites. Use one system per window.
template<class MenuId, class ToolId, class IconId>
class MenuSystem : public QObject
{
    static_assert(std::is_enum_v<MenuId> && std::is_enum_v<ToolId> && std::is_enum_v<IconId>);
public:
    struct Tool {
        ToolId id;
        MenuText title;
        std::optional<IconId> icon;
        MenuText toolTip;
        std::function<void(bool)> execute;
        ToolPlacement placement = ToolPlacement::MenuAndToolbar;
        bool enabled = true;
        bool checkable = false;
        bool checked = false;
        QKeySequence shortcut;
        bool separatorBefore = false;
    };

    struct Group {
        MenuId id;
        MenuText title;
        QVector<Tool> tools;
        QObject* context = nullptr; // defaults to the window; bounds handler lifetime
        std::function<void()> onOpen;
        std::optional<MenuId> parent; // omit for a top-level menu
    };

    using Definition = QVector<Group>;

    explicit MenuSystem(QMainWindow& window, QString module = QStringLiteral("app"))
        : window_(&window), module_(std::move(module)),
          registry_(std::make_unique<MenuRegistry>(window.menuBar(), this)) {}

    ~MenuSystem() override
    {
        delete toolbar_.data();
        registry_.reset(); // tear down connections while all members are still alive
    }

    bool registerSheet(const QString& filename, const IconSheet& sheet)
    { return icons_.registerSheet(filename, sheet); }
    bool registerIcon(IconId id, const IconSource& source)
    { return icons_.registerIcon({module_, id}, source); }
    bool registerIcon(IconId id, const QVector<IconVariant>& variants)
    { return icons_.registerIcon({module_, id}, variants); }
    QIcon icon(IconId id) { return icons_.icon({module_, id}); }

    QAction* action(ToolId id) const { return registry_->action({module_, id}); }
    QMenu* menu(MenuId id) const { return registry_->menu({module_, id}); }
    QToolBar* toolbar() const { return toolbar_.data(); }

    // Validate the full ordered definition before creating any UI. Definitions
    // are built once; runtime state is updated through action(enumId).
    bool build(const Definition& definition)
    {
        if (built_ || !window_ || module_.isEmpty()) {
            qWarning("MenuSystem: already built, missing window, or empty module");
            return false;
        }
        QMap<MenuKey, bool> groups;
        QMap<ActionKey, bool> commands;
        for (const auto& group : definition) {
            const MenuKey id{module_, group.id};
            if (groups.contains(id) || (group.parent && !groups.contains({module_, *group.parent}))) {
                qWarning("MenuSystem: duplicate group or parent must precede its child");
                return false;
            }
            groups.insert(id, true);
            for (const auto& tool : group.tools) {
                const ActionKey id{module_, tool.id};
                if (commands.contains(id) || (tool.enabled && !tool.execute)) {
                    qWarning("MenuSystem: duplicate tool or enabled tool has no handler");
                    return false;
                }
                commands.insert(id, true);
            }
        }

        toolbar_ = new QToolBar(window_);
        toolbar_->setObjectName(QStringLiteral("toolsToolbar"));
        toolbar_->setWindowTitle(toolbarTitle_.resolve());
        toolbar_->setIconSize(QSize(20, 20));
        toolbar_->setToolButtonStyle(Qt::ToolButtonIconOnly);
        toolbar_->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
        toolbar_->setFloatable(false);
        window_->addToolBar(Qt::TopToolBarArea, toolbar_);

        int groupOrder = 0;
        for (const auto& group : definition) {
            const MenuKey groupKey{module_, group.id};
            const auto parent = group.parent ? std::optional<MenuKey>{{module_, *group.parent}} : std::nullopt;
            // Top-level menus use text; assigning an icon may replace their label.
            if (!registry_->addMenu({groupKey, parent, group.title, groupOrder++}))
                return false;
            QObject* context = group.context ? group.context : window_.data();
            if (group.onOpen)
                connect(menu(group.id), &QMenu::aboutToShow, context, group.onOpen);

            ToolbarGroup toolbarGroup;
            int order = 0;
            for (const auto& tool : group.tools) {
                if (tool.separatorBefore)
                    registry_->addSeparator(groupKey, order++);
                ActionDefinition command;
                command.id = {module_, tool.id};
                command.text = tool.title;
                command.order = order++;
                command.icon = tool.icon ? icon(*tool.icon) : QIcon();
                command.toolTip = tool.toolTip;
                command.enabled = tool.enabled;
                command.checkable = tool.checkable;
                command.checked = tool.checked;
                command.shortcut = tool.shortcut;
                auto handler = tool.execute ? tool.execute : std::function<void(bool)>([](bool) {});
                if (!registry_->addAction(groupKey, command, context, std::move(handler)))
                    return false;
                if (tool.placement == ToolPlacement::MenuOnly)
                    continue;

                if (toolbarGroup.actions.isEmpty() && !toolbarGroups_.isEmpty())
                    toolbarGroup.separator = toolbar_->addSeparator();
                auto* shared = action(tool.id);
                toolbar_->addAction(shared); // the same QAction, not a second command
                toolbarGroup.actions.append(shared);
                auto* button = qobject_cast<QToolButton*>(toolbar_->widgetForAction(shared));
                if (button) {
                    auto updateButton = [button, shared] {
                        button->setToolButtonStyle(shared->icon().isNull()
                            ? Qt::ToolButtonTextOnly : Qt::ToolButtonIconOnly);
                        button->setAccessibleName(shared->toolTip().isEmpty()
                            ? shared->text() : shared->toolTip());
                    };
                    updateButton();
                    connect(shared, &QAction::changed, button, updateButton);
                }
                connect(shared, &QAction::changed, this, [this] { updateSeparators(); });
            }
            if (!toolbarGroup.actions.isEmpty())
                toolbarGroups_.append(toolbarGroup);
        }
        built_ = true;
        toolbar_->setVisible(!toolbarGroups_.isEmpty());
        updateSeparators();
        return true;
    }

    void retranslate()
    {
        registry_->retranslate();
        if (toolbar_)
            toolbar_->setWindowTitle(toolbarTitle_.resolve());
    }

private:
    struct ToolbarGroup {
        QPointer<QAction> separator;
        QVector<QPointer<QAction>> actions;
    };
    void updateSeparators()
    {
        if (updatingSeparators_ || !toolbar_)
            return;
        QScopedValueRollback<bool> guard(updatingSeparators_, true);
        bool previousVisible = false;
        for (const auto& group : toolbarGroups_) {
            bool visible = false;
            for (const auto& command : group.actions)
                visible |= command && command->isVisible();
            if (group.separator)
                group.separator->setVisible(previousVisible && visible);
            previousVisible |= visible;
        }
    }

    QPointer<QMainWindow> window_;
    QString module_;
    IconProvider icons_;
    std::unique_ptr<MenuRegistry> registry_;
    QPointer<QToolBar> toolbar_;
    QVector<ToolbarGroup> toolbarGroups_;
    MenuText toolbarTitle_{"MenuSystem", QT_TRANSLATE_NOOP("MenuSystem", "Tools")};
    bool built_ = false;
    bool updatingSeparators_ = false;
};

} // namespace pl::gui
