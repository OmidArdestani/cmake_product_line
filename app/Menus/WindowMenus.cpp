#include "WindowMenus.h"
#include "MenuIds.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>

namespace app {
namespace {
pl::gui::MenuText text(const char* source) { return {"WindowMenus", source}; }
}

bool registerWindowMenus(pl::gui::MenuRegistry& menus, QMainWindow& window)
{
    using namespace pl::gui;
    ActionDefinition exit;
    exit.id = actionKey(ActionId::Exit);
    exit.text = text(QT_TRANSLATE_NOOP("WindowMenus", "Exit"));
    exit.shortcut = QKeySequence::Quit;
    if (!menus.addAction(menuKey(MenuId::File), exit, &window, [&window](bool) { window.close(); }))
        return false;

    // The current product shell has no editable document. Keep standard edit
    // commands visible but disabled until a product supplies editing handlers.
    struct EditCommand { ActionId id; const char* label; QKeySequence::StandardKey shortcut; int order; };
    const EditCommand edits[] = {
        {ActionId::Undo, QT_TRANSLATE_NOOP("WindowMenus", "Undo"), QKeySequence::Undo, 10},
        {ActionId::Redo, QT_TRANSLATE_NOOP("WindowMenus", "Redo"), QKeySequence::Redo, 20},
        {ActionId::Cut, QT_TRANSLATE_NOOP("WindowMenus", "Cut"), QKeySequence::Cut, 40},
        {ActionId::Copy, QT_TRANSLATE_NOOP("WindowMenus", "Copy"), QKeySequence::Copy, 50},
        {ActionId::Paste, QT_TRANSLATE_NOOP("WindowMenus", "Paste"), QKeySequence::Paste, 60},
        {ActionId::SelectAll, QT_TRANSLATE_NOOP("WindowMenus", "Select all"), QKeySequence::SelectAll, 80}
    };
    for (const auto& edit : edits) {
        ActionDefinition action;
        action.id = actionKey(edit.id);
        action.text = text(edit.label);
        action.shortcut = edit.shortcut;
        action.order = edit.order;
        action.enabled = false;
        action.toolTip = text(QT_TRANSLATE_NOOP("WindowMenus", "No editable document is open"));
        if (!menus.addAction(menuKey(MenuId::Edit), action, &window, [](bool) {}))
            return false;
    }
    if (!menus.addSeparator(menuKey(MenuId::Edit), 30)
        || !menus.addSeparator(menuKey(MenuId::Edit), 70))
        return false;

    auto* toolbar = new QToolBar(&window);
    toolbar->setObjectName(QStringLiteral("toolsToolbar"));
    toolbar->setWindowTitle(text(QT_TRANSLATE_NOOP("WindowMenus", "Tools")).resolve());
    toolbar->setIconSize(QSize(20, 20));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    toolbar->setFloatable(false);
    window.addToolBar(Qt::TopToolBarArea, toolbar);
    toolbar->addAction(menus.action(actionKey(ActionId::ToggleApi)));
    toolbar->addAction(menus.action(actionKey(ActionId::ApiSettings)));
    toolbar->addSeparator();

    auto* productMenu = menus.menu(menuKey(MenuId::Product));
    auto* productButton = new QToolButton(toolbar);
    productButton->setObjectName(QStringLiteral("productToolsButton"));
    productButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    productButton->setPopupMode(QToolButton::InstantPopup);
    productButton->setMenu(productMenu);
    auto updateProductButton = [productButton, productMenu] {
        productButton->setText(productMenu->title());
        productButton->setToolTip(productMenu->title());
        productButton->setAccessibleName(productMenu->title());
        productButton->setIcon(productMenu->icon());
    };
    updateProductButton();
    QObject::connect(productMenu->menuAction(), &QAction::changed, productButton, updateProductButton);
    toolbar->addWidget(productButton);

    ActionDefinition showToolbar;
    showToolbar.id = actionKey(ActionId::ToggleToolsToolbar);
    showToolbar.text = text(QT_TRANSLATE_NOOP("WindowMenus", "Tools toolbar"));
    showToolbar.checkable = true;
    showToolbar.checked = true;
    if (!menus.addAction(menuKey(MenuId::View), showToolbar, toolbar,
                         [toolbar](bool checked) { toolbar->setVisible(checked); }))
        return false;

    auto* statusBar = window.statusBar();
    ActionDefinition showStatus;
    showStatus.id = actionKey(ActionId::ToggleStatusBar);
    showStatus.text = text(QT_TRANSLATE_NOOP("WindowMenus", "Status bar"));
    showStatus.order = 10;
    showStatus.checkable = true;
    showStatus.checked = true;
    if (!menus.addAction(menuKey(MenuId::View), showStatus, statusBar,
                         [statusBar](bool checked) { statusBar->setVisible(checked); }))
        return false;
    QObject::connect(menus.menu(menuKey(MenuId::View)), &QMenu::aboutToShow, &window,
        [&menus, toolbar, statusBar] {
            menus.action(actionKey(ActionId::ToggleToolsToolbar))->setChecked(!toolbar->isHidden());
            menus.action(actionKey(ActionId::ToggleStatusBar))->setChecked(!statusBar->isHidden());
        });

    ActionDefinition about;
    about.id = actionKey(ActionId::AboutQt);
    about.text = text(QT_TRANSLATE_NOOP("WindowMenus", "About Qt…"));
    return menus.addAction(menuKey(MenuId::Help), about, &window, [&window](bool) {
        QMessageBox::aboutQt(&window);
    });
}
} // namespace app
