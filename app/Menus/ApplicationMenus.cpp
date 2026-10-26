#include "ApplicationMenus.h"
#include "Tools/ToolController.h"
#include <QMessageBox>
#include <QStatusBar>

namespace app {
namespace {
pl::gui::MenuText text(const char* source) { return {"ApplicationMenus", source}; }
pl::gui::MenuText literal(const QString& value) { return {{}, value.toUtf8()}; }

Tool menuOnly(Tool tool, QKeySequence shortcut = {})
{
    tool.placement = pl::gui::ToolPlacement::MenuOnly;
    tool.shortcut = shortcut;
    return tool;
}

Tool editing(ToolId id, const char* label, QKeySequence shortcut, bool separator = false)
{
    Tool tool{id, text(label), {}, text(QT_TRANSLATE_NOOP("ApplicationMenus", "No editable document is open")), {}};
    tool = menuOnly(tool, shortcut);
    tool.enabled = false;
    tool.separatorBefore = separator;
    return tool;
}
} // namespace

UiDefinition applicationMenus(QMainWindow& window, ApplicationMenuSystem& menus, ToolController& tools)
{
    // Add groups and tools here in display order. Every tool goes into BOTH its
    // menu and the toolbar unless explicitly marked menuOnly(). No widget code.
    UiDefinition ui{
        {MenuId::File, text(QT_TRANSLATE_NOOP("ApplicationMenus", "File")), {
            menuOnly({ToolId::Exit, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Exit")), {}, {},
                      [&window](bool) { window.close(); }}, QKeySequence::Quit)
        }},
        {MenuId::Edit, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Edit")), {
            editing(ToolId::Undo, QT_TRANSLATE_NOOP("ApplicationMenus", "Undo"), QKeySequence::Undo),
            editing(ToolId::Redo, QT_TRANSLATE_NOOP("ApplicationMenus", "Redo"), QKeySequence::Redo),
            editing(ToolId::Cut, QT_TRANSLATE_NOOP("ApplicationMenus", "Cut"), QKeySequence::Cut, true),
            editing(ToolId::Copy, QT_TRANSLATE_NOOP("ApplicationMenus", "Copy"), QKeySequence::Copy),
            editing(ToolId::Paste, QT_TRANSLATE_NOOP("ApplicationMenus", "Paste"), QKeySequence::Paste),
            editing(ToolId::SelectAll, QT_TRANSLATE_NOOP("ApplicationMenus", "Select all"), QKeySequence::SelectAll, true)
        }},
        {MenuId::View, text(QT_TRANSLATE_NOOP("ApplicationMenus", "View")), {}},
        {MenuId::Tools, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Tools")), {
            {ToolId::ToggleApi, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Run API")), IconId::Run,
             text(QT_TRANSLATE_NOOP("ApplicationMenus", "Start or stop the API server")),
             [&tools](bool) { tools.toggleApi(); }},
            {ToolId::ApiSettings, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Settings…")), IconId::Settings,
             text(QT_TRANSLATE_NOOP("ApplicationMenus", "API settings are not available yet")), {},
             pl::gui::ToolPlacement::MenuAndToolbar, false}
        }, &tools, [&tools] { tools.syncApi(); }}
    };

    // View commands are menu-only: hiding the toolbar must not hide its restore control.
    Tool showToolbar = menuOnly({ToolId::ToggleToolsToolbar, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Tools toolbar")), {}, {},
        [&menus](bool checked) { menus.toolbar()->setVisible(checked); }});
    showToolbar.checkable = showToolbar.checked = true;
    Tool showStatus = menuOnly({ToolId::ToggleStatusBar, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Status bar")), {}, {},
        [&window](bool checked) { window.statusBar()->setVisible(checked); }});
    showStatus.checkable = showStatus.checked = true;
    // Find by ID so inserting another group doesn't change the receiving menu.
    for (auto& group : ui) {
        if (group.id == MenuId::View) {
            group.tools = {showToolbar, showStatus};
            group.onOpen = [&menus, &window] {
                menus.action(ToolId::ToggleToolsToolbar)->setChecked(!menus.toolbar()->isHidden());
                menus.action(ToolId::ToggleStatusBar)->setChecked(!window.statusBar()->isHidden());
            };
        }
    }

    // Each facility is a top-level group. These same two definitions also add
    // its toolbar buttons; there is no Product wrapper or separate toolbar list.
    for (const auto& facility : tools.facilities()) {
        Tool enabled{facility.enabledAction, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Enabled")), IconId::Settings,
            {"ApplicationMenus", QT_TRANSLATE_NOOP("ApplicationMenus", "Enable %1"), {facility.name}},
            [&tools, facility](bool checked) { tools.setFacilityEnabled(facility, checked); }};
        enabled.checkable = true;
        enabled.checked = facility.asset->isEnable();
        Tool run{facility.runAction, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Run feature")), IconId::Run,
            {"ApplicationMenus", QT_TRANSLATE_NOOP("ApplicationMenus", "Run %1"), {facility.name}},
            [&tools, facility](bool) { tools.runFacility(facility); }};
        run.enabled = facility.asset->isEnable();
        ui.append({facility.menu, literal(facility.name), {enabled, run}, &tools,
                   [&tools, facility] { tools.syncFacility(facility); }});
    }

    ui.append({MenuId::Help, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Help")), {
        menuOnly({ToolId::AboutQt, text(QT_TRANSLATE_NOOP("ApplicationMenus", "About Qt…")), {}, {},
                  [&window](bool) { QMessageBox::aboutQt(&window); }})
    }});
    return ui;
}

} // namespace app
