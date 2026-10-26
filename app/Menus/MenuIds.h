#pragma once

#include <MenuSystem/MenuSystem.h>

namespace app {

// All application IDs live here. Labels and list order can change independently.
enum class MenuId {
    File = 1, Edit = 2, Tools = 3, View = 6, Help = 7,
    Asset1 = 101, Asset2 = 102, Asset3 = 103, Asset4 = 104
};
enum class ToolId {
    ToggleApi = 1, ApiSettings = 2, Exit = 3,
    Undo = 4, Redo = 5, Cut = 6, Copy = 7, Paste = 8, SelectAll = 9,
    ToggleStatusBar = 10, AboutQt = 11, ToggleToolsToolbar = 12,
    EnableAsset1 = 101, RunAsset1 = 102,
    EnableAsset2 = 201, RunAsset2 = 202,
    EnableAsset3 = 301, RunAsset3 = 302,
    EnableAsset4 = 401, RunAsset4 = 402
};
enum class IconId { Tools = 1, Api = 2, Run = 3, Stop = 4, Settings = 5, Product = 6 };

using ApplicationMenuSystem = pl::gui::MenuSystem<MenuId, ToolId, IconId>;
using Tool = ApplicationMenuSystem::Tool;
using MenuGroup = ApplicationMenuSystem::Group;
using UiDefinition = ApplicationMenuSystem::Definition;

} // namespace app
