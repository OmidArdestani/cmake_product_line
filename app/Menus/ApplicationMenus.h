#pragma once

#include "MenuIds.h"

namespace app {
class ToolController;
UiDefinition applicationMenus(QMainWindow& window, ApplicationMenuSystem& menus, ToolController& tools);
}
