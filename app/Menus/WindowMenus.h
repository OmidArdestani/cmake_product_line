#pragma once

#include <MenuSystem/MenuRegistry.h>
#include <QMainWindow>

namespace app {
// Desktop window commands and toolbar composition stay in the application.
bool registerWindowMenus(pl::gui::MenuRegistry& menus, QMainWindow& window);
} // namespace app
