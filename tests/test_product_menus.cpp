#include "mainwindow.h"
#include "Menus/MenuIds.h"
#include <QApplication>
#include <QMenu>
#include <QStatusBar>
#include <QTcpServer>
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

QAction* command(MainWindow& window, app::ActionId id)
{
    return window.findChild<QAction*>(app::actionKey(id).name());
}

QAction* facilityCommand(MainWindow& window, const QString& name, app::FacilityAction id)
{
    const pl::gui::ActionKey key{QStringLiteral("app.facility.") + name, id};
    return window.findChild<QAction*>(key.name());
}

TEST(ProductMenus, ToolsMenuIsVisibleInsideShownWindow)
{
    MainWindow window(nullptr, 0);
    window.show();
    QApplication::processEvents();

    auto* bar = window.menuBar();
    auto* tools = window.findChild<QMenu*>(app::menuKey(app::MenuId::Tools).name());
    ASSERT_NE(tools, nullptr);
    EXPECT_FALSE(bar->isNativeMenuBar());
    EXPECT_TRUE(bar->isVisible());
    EXPECT_GT(bar->height(), 0);
    EXPECT_TRUE(tools->menuAction()->isVisible());
    const auto itemRect = bar->actionGeometry(tools->menuAction());
    ASSERT_FALSE(itemRect.isEmpty());
    EXPECT_TRUE(bar->rect().contains(itemRect));
    EXPECT_EQ(bar->actionAt(itemRect.center()), tools->menuAction());

    tools->popup(bar->mapToGlobal(itemRect.bottomLeft()));
    QApplication::processEvents();
    EXPECT_TRUE(tools->isVisible());
    ASSERT_EQ(tools->actions().size(), 2);
    EXPECT_FALSE(tools->actionGeometry(tools->actions().first()).isEmpty());
    tools->close();
}

TEST(ProductMenus, OnlyActiveProductFacilitiesAppearUnderTools)
{
    MainWindow window(nullptr, 0);
    auto* tools = window.findChild<QMenu*>(app::menuKey(app::MenuId::Tools).name());
    auto* api = window.findChild<QMenu*>(app::menuKey(app::MenuId::Api).name());
    auto* product = window.findChild<QMenu*>(app::menuKey(app::MenuId::Product).name());
    ASSERT_NE(tools, nullptr);
    ASSERT_NE(api, nullptr);
    ASSERT_NE(product, nullptr);
    ASSERT_EQ(tools->actions().size(), 2);
    EXPECT_EQ(tools->actions()[0], api->menuAction());
    EXPECT_EQ(tools->actions()[1], product->menuAction());
    EXPECT_EQ(product->actions().size(), expectedFacilities().size());
    for (auto* item : product->actions()) {
        ASSERT_NE(item->menu(), nullptr);
        EXPECT_TRUE(expectedFacilities().contains(item->text()));
        EXPECT_FALSE(item->icon().isNull());
        EXPECT_EQ(item->menu()->actions().size(), 2);
    }
    for (auto id : {app::MenuId::File, app::MenuId::Edit}) {
        auto* menu = window.findChild<QMenu*>(app::menuKey(id).name());
        ASSERT_NE(menu, nullptr);
        EXPECT_FALSE(menu->menuAction()->isVisible());
    }
    ASSERT_NE(command(window, app::ActionId::ApiSettings), nullptr);
    EXPECT_FALSE(command(window, app::ActionId::ApiSettings)->isEnabled());
    for (const auto& name : expectedFacilities()) {
        auto* enabled = facilityCommand(window, name, app::FacilityAction::Enabled);
        auto* run = facilityCommand(window, name, app::FacilityAction::Run);
        ASSERT_NE(enabled, nullptr);
        ASSERT_NE(run, nullptr);
        EXPECT_FALSE(enabled->isChecked());
        EXPECT_FALSE(run->isEnabled());
        EXPECT_FALSE(run->icon().isNull());
    }
}

TEST(ProductMenus, GuiCommandsEnableAndRunActualFacilities)
{
    MainWindow window(nullptr, 0);
    for (const auto& name : expectedFacilities()) {
        auto* enabled = facilityCommand(window, name, app::FacilityAction::Enabled);
        auto* run = facilityCommand(window, name, app::FacilityAction::Run);
        ASSERT_NE(enabled, nullptr);
        ASSERT_NE(run, nullptr);
        enabled->trigger();
        EXPECT_TRUE(enabled->isChecked());
        EXPECT_TRUE(run->isEnabled());
        run->setText(QStringLiteral("A translated label"));
        run->trigger();
        EXPECT_EQ(window.statusBar()->currentMessage(), name + QStringLiteral(" completed successfully."));
        enabled->trigger();
        EXPECT_FALSE(enabled->isChecked());
        EXPECT_FALSE(run->isEnabled());
    }
}

TEST(ProductMenus, ApiStateDoesNotDependOnActionText)
{
    MainWindow window(nullptr, 0); // ephemeral port, no collision with a running app
    auto* api = command(window, app::ActionId::ToggleApi);
    ASSERT_NE(api, nullptr);
    api->setText(QStringLiteral("Arbitrary translated label"));
    api->trigger();
    EXPECT_EQ(api->text(), QStringLiteral("Stop"));
    EXPECT_EQ(window.statusBar()->currentMessage(), QStringLiteral("API started."));
    api->setText(QStringLiteral("Another translated label"));
    api->trigger();
    EXPECT_EQ(api->text(), QStringLiteral("Run"));
    EXPECT_EQ(window.statusBar()->currentMessage(), QStringLiteral("API stopped."));
}

TEST(ProductMenus, FailedApiStartKeepsRunStateAndReportsError)
{
    QTcpServer occupied;
    ASSERT_TRUE(occupied.listen(QHostAddress::Any, 0));
    MainWindow window(nullptr, occupied.serverPort());
    auto* api = command(window, app::ActionId::ToggleApi);
    ASSERT_NE(api, nullptr);
    api->trigger();
    EXPECT_EQ(api->text(), QStringLiteral("Run"));
    EXPECT_TRUE(window.statusBar()->currentMessage().startsWith(QStringLiteral("Could not start the API.")));
}

TEST(ProductMenus, ClosingWindowWithRunningApiReleasesCommandsAndServices)
{
    QPointer<QAction> api;
    {
        MainWindow window(nullptr, 0);
        api = command(window, app::ActionId::ToggleApi);
        ASSERT_FALSE(api.isNull());
        api->trigger();
        EXPECT_EQ(api->text(), QStringLiteral("Stop"));
    }
    EXPECT_TRUE(api.isNull());
}
} // namespace
