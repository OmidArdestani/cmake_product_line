#pragma once

#include <MenuSystem/Ids.h>

namespace app {

enum class MenuId { File = 1, Edit = 2, Tools = 3, Api = 4, Product = 5, View = 6, Help = 7 };
enum class ActionId {
    ToggleApi = 1, ApiSettings = 2, Exit = 3,
    Undo = 4, Redo = 5, Cut = 6, Copy = 7, Paste = 8, SelectAll = 9,
    ToggleStatusBar = 10, AboutQt = 11, ToggleToolsToolbar = 12
};
enum class IconId { Tools = 1, Api = 2, Run = 3, Stop = 4, Settings = 5, Product = 6 };
enum class FacilityId { Asset1 = 1, Asset2 = 2, Asset3 = 3, Asset4 = 4 };
enum class FacilityAction { Enabled = 1, Run = 2 };

inline pl::gui::MenuKey menuKey(MenuId id) { return {QStringLiteral("app"), id}; }
inline pl::gui::ActionKey actionKey(ActionId id) { return {QStringLiteral("app"), id}; }
inline pl::gui::IconKey iconKey(IconId id) { return {QStringLiteral("app"), id}; }
inline pl::gui::MenuKey facilityMenuKey(FacilityId id)
{ return {QStringLiteral("app.facilities"), id}; }

} // namespace app
