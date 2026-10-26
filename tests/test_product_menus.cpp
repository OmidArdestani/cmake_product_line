#include "mainwindow.h"
#include "Menus/MenuIds.h"
#include <QApplication>
#include <QMenu>
#include <QStatusBar>
#include <QTcpServer>
#include <QToolButton>
#include <gtest/gtest.h>

namespace {
QStringList expectedFacilities()
{
#if defined(Product_1)
    return {"UniqueAsset1", "UniqueAsset2"};
#elif defined(Product_2)
    return {"UniqueAsset2", "UniqueAsset3"};
#elif defined(Product_3)
    return {"UniqueAsset2", "UniqueAsset3", "UniqueAsset4"};
#elif defined(Product_4)
    return {"UniqueAsset2", "UniqueAsset4"};
#elif defined(Product_5)
    return {"UniqueAsset1", "UniqueAsset3", "UniqueAsset4"};
#endif
}

struct FacilityIds { const char* name; app::MenuId menu; app::ToolId enabled; app::ToolId run; };
const FacilityIds allFacilities[] = {
    {"UniqueAsset1", app::MenuId::Asset1, app::ToolId::EnableAsset1, app::ToolId::RunAsset1},
    {"UniqueAsset2", app::MenuId::Asset2, app::ToolId::EnableAsset2, app::ToolId::RunAsset2},
    {"UniqueAsset3", app::MenuId::Asset3, app::ToolId::EnableAsset3, app::ToolId::RunAsset3},
    {"UniqueAsset4", app::MenuId::Asset4, app::ToolId::EnableAsset4, app::ToolId::RunAsset4}
};

QAction* command(MainWindow& window, app::ToolId id)
{
    // QAction ownership belongs to MenuSystem; its presence in a menu does not
    // reparent it to the window. Inspect the actual displayed command lists.
    const auto name = pl::gui::ActionKey{"app", id}.name();
    for (auto* group : window.findChildren<QMenu*>())
        for (auto* action : group->actions())
            if (action->objectName() == name)
                return action;
    return nullptr;
}
QMenu* menu(MainWindow& window, app::MenuId id)
{
    return window.findChild<QMenu*>(pl::gui::MenuKey{"app", id}.name());
}

TEST(ProductMenus, PlatformMenuBehaviorAndHorizontalToolbarArePreserved)
{
    QMenuBar platformDefault;
    MainWindow window(nullptr, 0);
    window.show();
    QApplication::processEvents();
    auto* bar = window.menuBar();
    EXPECT_EQ(bar->isNativeMenuBar(), platformDefault.isNativeMenuBar());
    if (QApplication::platformName() == QStringLiteral("cocoa"))
        EXPECT_TRUE(bar->isNativeMenuBar());
    auto* toolbar = window.findChild<QToolBar*>("toolsToolbar");
    ASSERT_NE(toolbar, nullptr);
    EXPECT_TRUE(toolbar->isVisible());
    EXPECT_EQ(toolbar->orientation(), Qt::Horizontal);
    EXPECT_EQ(window.toolBarArea(toolbar), Qt::TopToolBarArea);
    EXPECT_GT(toolbar->height(), 0);
    if (!bar->isNativeMenuBar()) {
        EXPECT_TRUE(bar->isVisible());
        EXPECT_GT(bar->height(), 0);
        QRect previous;
        for (auto* action : bar->actions()) {
            const auto rect = bar->actionGeometry(action);
            EXPECT_FALSE(rect.isEmpty());
            EXPECT_TRUE(bar->rect().contains(rect));
            if (!previous.isEmpty()) {
                EXPECT_EQ(rect.top(), previous.top());
                EXPECT_GT(rect.left(), previous.right());
            }
            previous = rect;
        }
        EXPECT_GE(toolbar->mapTo(&window, QPoint()).y(), bar->geometry().bottom());
    }
    for (auto* action : toolbar->actions()) {
        if (action->isSeparator())
            continue;
        auto* button = toolbar->widgetForAction(action);
        ASSERT_NE(button, nullptr);
        EXPECT_TRUE(button->isVisible());
        EXPECT_FALSE(button->geometry().isEmpty());
    }
}

TEST(ProductMenus, EachActiveFacilityIsATopLevelMenuAndToolbarGroup)
{
    MainWindow window(nullptr, 0);
    QStringList expected{"File", "Edit", "View", "Tools"};
    expected.append(expectedFacilities());
    expected.append("Help");
    QStringList actual;
    for (auto* action : window.menuBar()->actions()) {
        EXPECT_TRUE(action->isVisible());
        actual.append(action->text());
    }
    EXPECT_EQ(actual, expected);
    auto* toolbar = window.findChild<QToolBar*>("toolsToolbar");
    ASSERT_NE(toolbar, nullptr);
    QList<QAction*> expectedToolbar{command(window, app::ToolId::ToggleApi), command(window, app::ToolId::ApiSettings)};
    for (const auto& facility : allFacilities) {
        auto* group = menu(window, facility.menu);
        if (!expectedFacilities().contains(QString::fromUtf8(facility.name))) {
            EXPECT_EQ(group, nullptr);
            EXPECT_EQ(command(window, facility.run), nullptr);
            continue;
        }
        ASSERT_NE(group, nullptr);
        EXPECT_TRUE(window.menuBar()->actions().contains(group->menuAction()));
        auto* enabled = command(window, facility.enabled);
        auto* run = command(window, facility.run);
        ASSERT_NE(enabled, nullptr);
        ASSERT_NE(run, nullptr);
        EXPECT_EQ(group->actions(), (QList<QAction*>{enabled, run}));
        expectedToolbar << enabled << run;
        EXPECT_FALSE(enabled->isChecked());
        EXPECT_FALSE(run->isEnabled());
        EXPECT_FALSE(run->icon().isNull());
        EXPECT_TRUE(run->toolTip().contains(QString::fromUtf8(facility.name)));
    }
    QList<QAction*> actualToolbar;
    for (auto* action : toolbar->actions())
        if (!action->isSeparator())
            actualToolbar << action;
    EXPECT_EQ(actualToolbar, expectedToolbar);
    EXPECT_EQ(window.findChild<QToolButton*>("productToolsButton"), nullptr);
    ASSERT_NE(command(window, app::ToolId::ApiSettings), nullptr);
    EXPECT_FALSE(command(window, app::ToolId::ApiSettings)->isEnabled());
}

TEST(ProductMenus, MenuAndToolbarShareFacilityStateAndExecuteTheSameCommand)
{
    MainWindow window(nullptr, 0);
    auto* toolbar = window.findChild<QToolBar*>("toolsToolbar");
    ASSERT_NE(toolbar, nullptr);
    for (const auto& facility : allFacilities) {
        const auto name = QString::fromUtf8(facility.name);
        if (!expectedFacilities().contains(name))
            continue;
        auto* enabled = command(window, facility.enabled);
        auto* run = command(window, facility.run);
        ASSERT_NE(enabled, nullptr);
        ASSERT_NE(run, nullptr);
        auto* enableButton = qobject_cast<QToolButton*>(toolbar->widgetForAction(enabled));
        auto* runButton = qobject_cast<QToolButton*>(toolbar->widgetForAction(run));
        ASSERT_NE(enableButton, nullptr);
        ASSERT_NE(runButton, nullptr);
        enableButton->click();
        EXPECT_TRUE(enabled->isChecked());
        EXPECT_TRUE(run->isEnabled());
        EXPECT_TRUE(runButton->isEnabled());
        run->setText(QStringLiteral("A translated label"));
        runButton->click();
        EXPECT_EQ(window.statusBar()->currentMessage(), name + QStringLiteral(" completed successfully."));
        enabled->trigger();
        EXPECT_FALSE(enableButton->isChecked());
        EXPECT_FALSE(runButton->isEnabled());
    }
}

TEST(ProductMenus, ViewMenuCanHideAndRestoreToolbar)
{
    MainWindow window(nullptr, 0);
    window.show();
    QApplication::processEvents();
    auto* toolbar = window.findChild<QToolBar*>("toolsToolbar");
    auto* toggle = command(window, app::ToolId::ToggleToolsToolbar);
    ASSERT_NE(toolbar, nullptr);
    ASSERT_NE(toggle, nullptr);
    EXPECT_FALSE(toolbar->actions().contains(toggle));
    toggle->trigger();
    EXPECT_TRUE(toolbar->isHidden());
    EXPECT_TRUE(toggle->isVisible());
    toggle->trigger();
    EXPECT_TRUE(toolbar->isVisible());
    EXPECT_TRUE(toggle->isChecked());
}

TEST(ProductMenus, ApiStateDoesNotDependOnActionText)
{
    MainWindow window(nullptr, 0);
    auto* api = command(window, app::ToolId::ToggleApi);
    ASSERT_NE(api, nullptr);
    auto* toolbar = window.findChild<QToolBar*>("toolsToolbar");
    ASSERT_NE(toolbar, nullptr);
    auto* button = qobject_cast<QToolButton*>(toolbar->widgetForAction(api));
    ASSERT_NE(button, nullptr);
    api->setText(QStringLiteral("Arbitrary translated label"));
    button->click();
    EXPECT_EQ(api->text(), QStringLiteral("Stop API"));
    EXPECT_EQ(button->text(), api->text());
    EXPECT_EQ(window.statusBar()->currentMessage(), QStringLiteral("API started."));
    api->setText(QStringLiteral("Another translated label"));
    api->trigger();
    EXPECT_EQ(api->text(), QStringLiteral("Run API"));
    EXPECT_EQ(button->icon().cacheKey(), api->icon().cacheKey());
    EXPECT_EQ(window.statusBar()->currentMessage(), QStringLiteral("API stopped."));
}

TEST(ProductMenus, FailedApiStartKeepsRunStateAndReportsError)
{
    QTcpServer occupied;
    ASSERT_TRUE(occupied.listen(QHostAddress::Any, 0));
    MainWindow window(nullptr, occupied.serverPort());
    auto* api = command(window, app::ToolId::ToggleApi);
    ASSERT_NE(api, nullptr);
    api->trigger();
    EXPECT_EQ(api->text(), QStringLiteral("Run API"));
    EXPECT_TRUE(window.statusBar()->currentMessage().startsWith(QStringLiteral("Could not start the API.")));
}

TEST(ProductMenus, ClosingWindowWithRunningApiReleasesCommandsAndServices)
{
    QPointer<QAction> api;
    QPointer<QToolBar> toolbar;
    {
        MainWindow window(nullptr, 0);
        api = command(window, app::ToolId::ToggleApi);
        toolbar = window.findChild<QToolBar*>("toolsToolbar");
        ASSERT_FALSE(api.isNull());
        api->trigger();
        EXPECT_EQ(api->text(), QStringLiteral("Stop API"));
    }
    EXPECT_TRUE(api.isNull());
    EXPECT_TRUE(toolbar.isNull());
}
} // namespace
