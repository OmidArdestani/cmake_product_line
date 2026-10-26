# One definition, menu and toolbar

Define each tool once. `MenuSystem` builds its menu item and toolbar button using
**the same QAction**, so its handler, icon, tooltip, enabled state and checked state
stay synchronized. Groups and tools appear in the order of the definition list.

## The three application files

| File | What to edit |
|---|---|
| `app/Menus/MenuIds.h` | The `MenuId`, `ToolId`, and `IconId` enums |
| `app/Menus/ApplicationIcons.cpp` | Map each icon ID to SVG, PNG, or a PNG-sheet cell |
| `app/Menus/ApplicationMenus.cpp` | The ordered groups and tools, with application callbacks |

`MainWindow::buildMenus()` registers icons and calls `menus->build(...)` once.
You do **not** add a second toolbar entry or construct QMenu/QAction/QToolButton
objects in the application. The former `WindowMenus` construction path is removed.

## Add one tool to an existing group

1. Add a new value to `ToolId` (for example `ExportReport = 500`).
2. Register an icon if the tool needs new artwork, or reuse an existing `IconId`.
3. Add this entry to the desired group's `tools` list in `ApplicationMenus.cpp`:

```cpp
{ToolId::ExportReport,
 text(QT_TRANSLATE_NOOP("ApplicationMenus", "Export report")),
 IconId::Run,
 text(QT_TRANSLATE_NOOP("ApplicationMenus", "Export the current report")),
 [&window](bool) {
     // Replace this demonstration callback with the application's facility call.
     window.statusBar()->showMessage(QStringLiteral("Export clicked"));
 }}
```

That entry produces both a menu item and an icon button. The callback runs once
whether the menu or toolbar is used. The `bool` argument is the checked state for
checkable tools; other handlers can ignore it. No second registration is required.

## Add a whole group

Add `Reports = 200` to `MenuId` and `ExportReport = 500` to `ToolId`, then insert
this group into the `ui` list before it is returned:

```cpp
ui.append({MenuId::Reports,
    text(QT_TRANSLATE_NOOP("ApplicationMenus", "Reports")), {
        {ToolId::ExportReport,
         text(QT_TRANSLATE_NOOP("ApplicationMenus", "Export report")),
         IconId::Run,
         text(QT_TRANSLATE_NOOP("ApplicationMenus", "Export the current report")),
         [&window](bool) {
             window.statusBar()->showMessage(QStringLiteral("Export clicked"));
         }}
    }});
```

The group becomes a top-level **Reports** menu and a section in the horizontal
icon toolbar. Toolbar sections are separated automatically. Appending adds it to
the end; inserting earlier changes its position. Enum values are identity, not
ordering: give IDs stable, explicit values and reorder the list instead.

## Tool options

The five common fields, in order, are:

```cpp
Tool { id, title, iconId, tooltip, execute }
```

Set optional fields when necessary:

```cpp
Tool tool{ToolId::ExportReport, text("Export report"), IconId::Run,
          text("Export the current report"), exportHandler};
tool.shortcut = QKeySequence(Qt::CTRL | Qt::Key_E);
tool.enabled = canExport;
tool.checkable = true;
tool.checked = false;
tool.separatorBefore = true; // separator inside the menu
```

Both displays are the default. For a command such as Exit, About, or Show Toolbar
that belongs only in a menu, use `ToolPlacement::MenuOnly`. The application has a
small `menuOnly(tool, shortcut)` helper for these declarations. There is no
independent toolbar command list.

An absent icon (`{}`) falls back to a text button. An enabled tool needs a callback;
a disabled placeholder may omit one. Duplicate group/tool IDs and parents declared
after their children are rejected before UI creation. Build the definition once;
update live state by enum afterward:

```cpp
menus.action(ToolId::ExportReport)->setEnabled(canExport);
menus.action(ToolId::ExportReport)->setChecked(selected);
menus.action(ToolId::ExportReport)->setVisible(showExport);
menus.action(ToolId::ExportReport)->setIcon(menus.icon(IconId::Run));
```

Both displays update together. Empty menus and redundant toolbar group separators
hide automatically. A disabled but visible tool still counts as menu content.

## Group options and lifetime

A group contains an ID, title and tools. Its optional `context` is the QObject
that owns its callbacks' lifetime; the default is the application window. Facility
groups use the application `ToolController` as their context. Destroying that
context disconnects and disables the commands. Captured facilities must outlive
that context. Keep definitions and all menu/icon operations on the GUI thread.

`onOpen` optionally refreshes state when a menu opens. `parent` optionally names
an earlier group to create a submenu. Omit `parent` for a top-level group. Nested
groups still contribute their toolbar sections in definition order.

The generic type is `pl::gui::MenuSystem<MenuId, ToolId, IconId>`.
`ApplicationMenuSystem`, `Tool`, `MenuGroup` and `UiDefinition` are application
aliases declared beside the enums. The underlying registry and icon loader remain
in `libs/pl_core/MenuSystem` and the separate `pl_core_gui` target. Facilities
still depend only on their core APIs; the generic GUI library knows nothing about
product IDs, assets, protobuf, or gRPC.

## Register PNG-sheet icons

In `ApplicationIcons.cpp`, register the geometry once and choose each icon by
filename, **zero-based row**, and **zero-based column**:

```cpp
pl::gui::IconSheet sheet;
sheet.cellSize = QSize(24, 24);
sheet.margins = QMargins(0, 0, 0, 0); // optional, defaults to zero
sheet.spacing = QSize(0, 0);         // optional, defaults to zero
sheet.highDpiFiles.insert(2, ":/icons/tools@2x.png"); // optional
menus.registerSheet(":/icons/tools.png", sheet);

menus.registerIcon(IconId::Run,
    pl::gui::IconSource::sprite(":/icons/tools.png", 1, 3));
```

Add bundled artwork to `app/resources/icons.qrc`. Filesystem paths work too.
For a standalone PNG/SVG or an existing QIcon:

```cpp
menus.registerIcon(IconId::Run,
    pl::gui::IconSource::file(":/icons/run.svg"));
// Or: IconSource::fromIcon(existingQIcon)
```

Choose one registration for each icon ID. Changing the file or sheet coordinates
in this mapping does not change tool definitions. Named icons and sheet images are
cached. Sprite transparency is preserved. High-DPI sheets must scale the entire
base geometry exactly (including margins and gaps); crops receive the matching
device pixel ratio. Invalid files/cells produce diagnostics and an empty icon;
an invalid optional high-DPI sheet falls back to the base sheet.

Custom disabled/checked artwork is also supported:

```cpp
menus.registerIcon(IconId::Run, QVector<pl::gui::IconVariant>{
    {pl::gui::IconSource::file(":/icons/run.svg")},
    {pl::gui::IconSource::file(":/icons/run-disabled.svg"), QIcon::Disabled},
    {pl::gui::IconSource::file(":/icons/run-checked.svg"), QIcon::Normal, QIcon::On}
});
```

Qt generates fallback appearances when variants are absent. SVG remains scalable;
deployment needs the Qt SVG icon engine plugin with the other Qt runtime libraries.

## Current application layout

For Product 1:

```text
Native menu bar:
File   Edit   View   Tools   UniqueAsset1   UniqueAsset2   Help
                     |           |              |
                 Run API       Enabled        Enabled
                 Settings      Run feature    Run feature

Horizontal toolbar inside the window:
[Run API] [Settings]  |  [Enable Asset1] [Run Asset1]  |  [Enable Asset2] [Run Asset2]
```

Each product exposes only its own facilities, directly in the top-level row.
There is no Product wrapper or product dropdown toolbar button. View can hide and
restore the toolbar or status bar. Editing actions are disabled because this shell
has no editable document yet. API Settings remains a disabled placeholder.

Qt chooses the platform's default menu-bar behavior: on macOS the menu bar belongs
to the system menu, while the toolbar stays inside the application window. The
application no longer forces an in-window menu bar. Other platforms follow their
Qt desktop integration. Top-level menu titles are textual; tools have icons.

API Run/Stop uses the actual server state. Facility enable state is shared safely
with gRPC workers. The application refreshes facility actions when their menu
opens and after local enable changes; a running operation is not cancelled by a
later disable. For external state notifications, call `syncFacility` in the GUI
thread to refresh both displays. Input collection and result/error presentation
belong to application handlers, not to the facility or the menu framework.

`MenuSystem::retranslate()` updates both displays from their original translation
contexts. `MenuText` supports arguments for translated tooltips such as
`{"ApplicationMenus", "Run %1", {facilityName}}`. Controllers refresh dynamic
labels such as API Run/Stop afterward.

## Validation

```sh
cmake --build --preset macos-qt -j 8
ctest --preset macos-qt
QT_QPA_PLATFORM=cocoa ~/pl_build/bin/Product_1MenuTests
```

Core tests cover the shared action model, single-entry additions, list order,
menu-only commands, visibility, ownership, invalid definitions, translations,
and PNG/SVG sources. Product tests compile all five variants and check native
menu policy, top-level facilities, toolbar geometry, shared command/state behavior,
View toggles, API start/stop/failure, and shutdown. Headless tests use the offscreen
backend; the Cocoa run checks the actual macOS policy. Qt5-compatible APIs are
used; Qt5 compilation needs a separately installed SDK.
