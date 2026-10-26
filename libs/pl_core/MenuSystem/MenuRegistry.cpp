#include "MenuRegistry.h"
#include <QDebug>
#include <QScopedValueRollback>

namespace pl::gui {

MenuRegistry::MenuRegistry(QMenuBar* bar, QObject* parent)
    : QObject(parent ? parent : bar), bar_(bar) {}

MenuRegistry::~MenuRegistry()
{
    // Child menus disappear first. QPointer also handles a window destroyed
    // before an externally owned registry.
    for (auto it = menuOrder_.crbegin(); it != menuOrder_.crend(); ++it)
        delete menus_[*it].widget.data();
}

QMenu* MenuRegistry::menu(MenuKey id) const
{
    const auto it = menus_.constFind(id);
    return it == menus_.cend() ? nullptr : it->widget.data();
}

QAction* MenuRegistry::action(ActionKey id) const
{
    const auto it = actions_.constFind(id);
    return it == actions_.cend() ? nullptr : it->action.data();
}

void MenuRegistry::insert(std::optional<MenuKey> parent, QAction* action, int order)
{
    QWidget* container = parent ? static_cast<QWidget*>(menu(*parent)) : bar_.data();
    QAction* before = nullptr;
    int beforeOrder = 0;
    for (const auto& placement : placements_) {
        if (placement.parent == parent && placement.action && placement.order > order
            && (!before || placement.order < beforeOrder)) {
            before = placement.action;
            beforeOrder = placement.order;
        }
    }
    container->insertAction(before, action);
    placements_.append({parent, action, order});
}

bool MenuRegistry::addMenu(const MenuDefinition& definition)
{
    if (!bar_ || !definition.id.isValid() || menus_.contains(definition.id)
        || (definition.parent && !menu(*definition.parent))) {
        qWarning() << "MenuRegistry: invalid/duplicate menu or missing parent:" << definition.id.name();
        return false;
    }
    QWidget* parent = definition.parent ? static_cast<QWidget*>(menu(*definition.parent)) : bar_.data();
    auto* widget = new QMenu(definition.title.resolve(), parent);
    widget->setObjectName(definition.id.name());
    widget->setIcon(definition.icon);
    widget->menuAction()->setMenuRole(QAction::NoRole);
    widget->menuAction()->setIconVisibleInMenu(true);
    menus_.insert(definition.id, {definition, widget});
    menuOrder_.append(definition.id);
    insert(definition.parent, widget->menuAction(), definition.order);
    refreshVisibility();
    return true;
}

bool MenuRegistry::addAction(MenuKey parent, const ActionDefinition& definition,
                             QObject* context, std::function<void(bool)> handler)
{
    if (!menu(parent) || !definition.id.isValid() || actions_.contains(definition.id)
        || !context || !handler) {
        qWarning() << "MenuRegistry: invalid/duplicate command, parent, or handler:" << definition.id.name();
        return false;
    }
    auto* command = new QAction(definition.icon, definition.text.resolve(), this);
    command->setObjectName(definition.id.name());
    command->setShortcut(definition.shortcut);
    command->setShortcutContext(definition.shortcutContext);
    command->setMenuRole(definition.menuRole);
    command->setToolTip(definition.toolTip.resolve());
    command->setStatusTip(definition.toolTip.resolve());
    command->setCheckable(definition.checkable);
    command->setChecked(definition.checked);
    command->setEnabled(definition.enabled);
    command->setVisible(definition.visible);
    command->setIconVisibleInMenu(true);
    connect(command, &QAction::triggered, context, std::move(handler));
    connect(context, &QObject::destroyed, command, [command] { command->setEnabled(false); });
    connect(command, &QAction::changed, this, [this] { refreshVisibility(); });
    actions_.insert(definition.id, {definition, command});
    insert(parent, command, definition.order);
    refreshVisibility();
    return true;
}

bool MenuRegistry::attachAction(MenuKey parent, ActionKey id, int order)
{
    auto* target = menu(parent);
    auto* command = action(id);
    if (!target || !command || target->actions().contains(command)) {
        qWarning() << "MenuRegistry: invalid or duplicate action placement:" << id.name();
        return false;
    }
    insert(parent, command, order);
    refreshVisibility();
    return true;
}

bool MenuRegistry::addSeparator(MenuKey parent, int order)
{
    if (!menu(parent)) {
        qWarning() << "MenuRegistry: separator parent missing:" << parent.name();
        return false;
    }
    auto* separator = new QAction(this);
    separator->setSeparator(true);
    insert(parent, separator, order);
    return true;
}

bool MenuRegistry::setMenuVisible(MenuKey id, bool visible)
{
    auto it = menus_.find(id);
    if (it == menus_.end() || !it->widget)
        return false;
    it->definition.visible = visible;
    refreshVisibility();
    return true;
}

void MenuRegistry::refreshVisibility()
{
    if (refreshing_)
        return;
    QScopedValueRollback<bool> guard(refreshing_, true);
    for (auto it = menuOrder_.crbegin(); it != menuOrder_.crend(); ++it) {
        auto& entry = menus_[*it];
        if (!entry.widget)
            continue;
        bool hasContent = false;
        for (auto* command : entry.widget->actions())
            hasContent |= command->isVisible() && !command->isSeparator();
        entry.widget->menuAction()->setVisible(entry.definition.visible && hasContent);
    }
}

void MenuRegistry::retranslate()
{
    for (const auto& entry : menus_)
        if (entry.widget)
            entry.widget->setTitle(entry.definition.title.resolve());
    for (const auto& entry : actions_) {
        if (!entry.action)
            continue;
        entry.action->setText(entry.definition.text.resolve());
        entry.action->setToolTip(entry.definition.toolTip.resolve());
        entry.action->setStatusTip(entry.definition.toolTip.resolve());
    }
}

} // namespace pl::gui
