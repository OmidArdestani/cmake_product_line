#pragma once

#include "MenuDefinitions.h"
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QPointer>
#include <QVector>
#include <functional>

namespace pl::gui {

// GUI-thread only. The registry must outlive its menus and registered commands.
// It owns QActions; QMenus are parented to their visual parent and cleaned up
// when either the window or registry is destroyed.
class MenuRegistry : public QObject
{
public:
    explicit MenuRegistry(QMenuBar* bar, QObject* parent = nullptr);
    ~MenuRegistry() override;

    bool addMenu(const MenuDefinition& definition);
    bool addAction(MenuKey parent, const ActionDefinition& definition,
                   QObject* context, std::function<void(bool)> handler);
    bool attachAction(MenuKey parent, ActionKey id, int order = 0);
    bool addSeparator(MenuKey parent, int order = 0);
    bool setMenuVisible(MenuKey id, bool visible);

    QMenu* menu(MenuKey id) const;
    QAction* action(ActionKey id) const;
    void retranslate();
    void refreshVisibility();

private:
    struct MenuEntry { MenuDefinition definition; QPointer<QMenu> widget; };
    struct ActionEntry { ActionDefinition definition; QPointer<QAction> action; };
    struct Placement {
        std::optional<MenuKey> parent;
        QPointer<QAction> action;
        int order;
    };
    void insert(std::optional<MenuKey> parent, QAction* action, int order);

    QPointer<QMenuBar> bar_;
    QMap<MenuKey, MenuEntry> menus_;
    QMap<ActionKey, ActionEntry> actions_;
    QVector<MenuKey> menuOrder_;
    QVector<Placement> placements_;
    bool refreshing_ = false;
};

} // namespace pl::gui
