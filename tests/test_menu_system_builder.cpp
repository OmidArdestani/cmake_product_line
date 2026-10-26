#include <MenuSystem/MenuSystem.h>
#include <QApplication>
#include <gtest/gtest.h>

namespace {
enum class Menu { First = 1, Second = 2, Child = 3 };
enum class Tool { Run = 1, Enable = 2, Extra = 3 };
enum class Icon { Run = 1 };
using System = pl::gui::MenuSystem<Menu, Tool, Icon>;

TEST(MenuSystemBuilder, OneDefinitionBuildsBothSurfacesInListOrder)
{
    QMainWindow window;
    const bool nativeBefore = window.menuBar()->isNativeMenuBar();
    System system(window);
    int calls = 0;
    QPixmap image(16, 16);
    image.fill(Qt::blue);
    ASSERT_TRUE(system.registerIcon(Icon::Run, pl::gui::IconSource::fromIcon(QIcon(image))));
    System::Definition definition{
        {Menu::Second, {{}, "Second"}, {
            {Tool::Run, {{}, "Run"}, Icon::Run, {{}, "Run the tool"}, [&](bool) { ++calls; }}
        }},
        {Menu::First, {{}, "First"}, {
            {Tool::Extra, {{}, "Extra"}, {}, {{}, "An extra tool"}, [](bool) {}}
        }}
    };
    ASSERT_TRUE(system.build(definition));
    EXPECT_EQ(window.menuBar()->isNativeMenuBar(), nativeBefore);
    ASSERT_EQ(window.menuBar()->actions().size(), 2);
    EXPECT_EQ(window.menuBar()->actions()[0], system.menu(Menu::Second)->menuAction());
    EXPECT_EQ(window.menuBar()->actions()[1], system.menu(Menu::First)->menuAction());
    auto* toolbar = system.toolbar();
    ASSERT_NE(toolbar, nullptr);
    ASSERT_EQ(toolbar->actions().size(), 3);
    EXPECT_EQ(toolbar->actions()[0], system.action(Tool::Run));
    EXPECT_TRUE(toolbar->actions()[1]->isSeparator());
    EXPECT_EQ(toolbar->actions()[2], system.action(Tool::Extra));
    EXPECT_EQ(system.menu(Menu::Second)->actions().first(), toolbar->actions().first());
    auto* button = qobject_cast<QToolButton*>(toolbar->widgetForAction(system.action(Tool::Run)));
    ASSERT_NE(button, nullptr);
    button->click();
    EXPECT_EQ(calls, 1); // a toolbar click invokes the single shared handler once
    system.action(Tool::Run)->trigger();
    EXPECT_EQ(calls, 2);
    system.action(Tool::Run)->setText(QStringLiteral("New label"));
    EXPECT_EQ(button->text(), QStringLiteral("New label"));
    EXPECT_EQ(button->toolTip(), QStringLiteral("Run the tool"));
    auto* noIcon = qobject_cast<QToolButton*>(toolbar->widgetForAction(system.action(Tool::Extra)));
    ASSERT_NE(noIcon, nullptr);
    EXPECT_EQ(noIcon->toolButtonStyle(), Qt::ToolButtonTextOnly);
}

TEST(MenuSystemBuilder, AddingOneToolEntryAutomaticallyAddsMenuItemAndToolbarButton)
{
    QMainWindow window;
    System system(window);
    System::Definition definition{{Menu::First, {{}, "First"}, {}}};
    // This is the complete extension: no registration or toolbar code follows.
    definition[0].tools.append({Tool::Extra, {{}, "New tool"}, {}, {{}, "Added once"}, [](bool) {}});
    ASSERT_TRUE(system.build(definition));
    auto* action = system.action(Tool::Extra);
    ASSERT_NE(action, nullptr);
    EXPECT_TRUE(system.menu(Menu::First)->actions().contains(action));
    EXPECT_TRUE(system.toolbar()->actions().contains(action));
    EXPECT_NE(system.toolbar()->widgetForAction(action), nullptr);
}

TEST(MenuSystemBuilder, MenuOnlyToolsAndSharedCheckStateWork)
{
    QMainWindow window;
    System system(window);
    System::Tool enable{Tool::Enable, {{}, "Enable"}, {}, {}, [](bool) {}};
    enable.checkable = true;
    System::Tool menuOnly{Tool::Extra, {{}, "Menu only"}, {}, {}, [](bool) {}};
    menuOnly.placement = pl::gui::ToolPlacement::MenuOnly;
    ASSERT_TRUE(system.build({{Menu::First, {{}, "First"}, {enable, menuOnly}}}));
    auto* button = qobject_cast<QToolButton*>(system.toolbar()->widgetForAction(system.action(Tool::Enable)));
    ASSERT_NE(button, nullptr);
    button->click();
    EXPECT_TRUE(system.action(Tool::Enable)->isChecked());
    system.action(Tool::Enable)->setChecked(false);
    EXPECT_FALSE(button->isChecked());
    system.action(Tool::Enable)->setEnabled(false);
    EXPECT_FALSE(button->isEnabled());
    EXPECT_EQ(system.toolbar()->widgetForAction(system.action(Tool::Extra)), nullptr);
    EXPECT_TRUE(system.menu(Menu::First)->actions().contains(system.action(Tool::Extra)));
}

TEST(MenuSystemBuilder, InvalidDefinitionsDoNotCreatePartialMenusOrToolbar)
{
    QMainWindow window;
    System system(window);
    System::Tool tool{Tool::Run, {{}, "Run"}, {}, {}, [](bool) {}};
    EXPECT_FALSE(system.build({{Menu::First, {{}, "First"}, {tool}}, {Menu::Second, {{}, "Second"}, {tool}}}));
    EXPECT_TRUE(window.menuBar()->actions().isEmpty());
    EXPECT_EQ(system.toolbar(), nullptr);
    System::Group orphan{Menu::Child, {{}, "Child"}, {tool}};
    orphan.parent = Menu::First;
    EXPECT_FALSE(system.build({orphan}));
    EXPECT_TRUE(window.menuBar()->actions().isEmpty());
    EXPECT_FALSE(system.build({{Menu::First, {{}, "First"}, {{Tool::Run, {{}, "Missing handler"}, {}, {}, {}}}}}));
    ASSERT_TRUE(system.build({{Menu::First, {{}, "First"}, {tool}}}));
    EXPECT_FALSE(system.build({{Menu::Second, {{}, "Second"}, {}}}));
    EXPECT_EQ(window.menuBar()->actions().size(), 1);
}

TEST(MenuSystemBuilder, GroupContextBoundsHandlersAndEmptyToolbarGroupsHideSeparators)
{
    QMainWindow window;
    System system(window);
    auto receiver = std::make_unique<QObject>();
    int calls = 0;
    ASSERT_TRUE(system.build({
        {Menu::First, {{}, "First"}, {{Tool::Run, {{}, "Run"}, {}, {}, [&](bool) { ++calls; }}}, receiver.get()},
        {Menu::Second, {{}, "Second"}, {{Tool::Extra, {{}, "Extra"}, {}, {}, [](bool) {}}}}
    }));
    auto* separator = system.toolbar()->actions()[1];
    EXPECT_TRUE(separator->isVisible());
    system.action(Tool::Run)->setVisible(false);
    EXPECT_FALSE(separator->isVisible());
    system.action(Tool::Run)->setVisible(true);
    EXPECT_TRUE(separator->isVisible());
    system.action(Tool::Extra)->setVisible(false);
    EXPECT_FALSE(separator->isVisible());
    receiver.reset();
    EXPECT_FALSE(system.action(Tool::Run)->isEnabled());
    system.action(Tool::Run)->trigger();
    EXPECT_EQ(calls, 0);
}

TEST(MenuSystemBuilder, NestedGroupsAndRegistryLifetimeKeepQtOwnershipSafe)
{
    QMainWindow window;
    QPointer<QAction> action;
    QPointer<QToolBar> toolbar;
    {
        System system(window);
        System::Group nested{Menu::Child, {{}, "Child"}, {{Tool::Run, {{}, "Run"}, {}, {}, [](bool) {}}}};
        nested.parent = Menu::First;
        ASSERT_TRUE(system.build({{Menu::First, {{}, "First"}, {}}, nested}));
        EXPECT_EQ(system.menu(Menu::First)->actions().first(), system.menu(Menu::Child)->menuAction());
        toolbar = system.toolbar();
        action = system.action(Tool::Run);
    }
    EXPECT_TRUE(action.isNull());
    EXPECT_TRUE(toolbar.isNull());
    EXPECT_TRUE(window.menuBar()->actions().isEmpty());
}
} // namespace
