#pragma once

#include <MenuSystem/MenuRegistry.h>
#include <MenuSystem/Icons/IconProvider.h>

namespace app {
class ToolController;
bool registerApplicationIcons(pl::gui::IconProvider& icons);
bool registerApplicationMenus(pl::gui::MenuRegistry& menus, pl::gui::IconProvider& icons,
                              ToolController& tools);
} // namespace app
