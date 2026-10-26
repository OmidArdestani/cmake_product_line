# Menus, tools, and icons

The GUI uses Qt `QMenuBar`, `QMenu`, and `QAction`. Shared infrastructure lives in
`libs/pl_core/MenuSystem`, built as the independent `pl_core_gui` target. It does
not link product facilities, protobuf, or gRPC. Facility consumers can continue
linking `pl_core` without depending on Qt Widgets or Qt SVG.

Application responsibilities are split between:

- `app/Menus`: application/product IDs, layouts, translated labels, and icon mappings.
- `app/Tools`: GUI commands that call facilities and report results.
- `app/ProductBuilder`: selection and construction of facilities only.

Facility libraries expose APIs and never register menus or refer to menu IDs.
The path from a click is `QAction -> ToolController -> facility API -> GUI result`.

## Current layout

```text
Tools
├── API
│   ├── Run / Stop
│   └── Settings… (disabled placeholder)
└── Product N
    └── UniqueAssetX (one submenu per facility shipped in this product)
        ├── Enabled (checkable)
        └── Run feature
```

File and Edit are registered extension points but hidden until they have content.
Run feature becomes available when its facility is enabled. The sample facilities
accept an empty request; completion/errors appear in the status bar. Enabling a
facility also changes the state exposed by gRPC. The enable flag is atomic because
the GUI and gRPC workers share it. A menu refreshes facility state when opened,
and the handler checks state again before execution. This does not cancel a
facility call already in progress.

API Run/Stop uses `GrpcApiServer::isRunning()`, never the action label. A start
failure keeps Run available and reports the error. The window stops the server
before destroying controllers, actions, or facilities.

## Add a menu and command

IDs combine a stable module namespace and an explicitly numbered scoped enum.
Use one menu enum, one action enum, and one icon enum per namespace. `MenuKey`,
`ActionKey`, and `IconKey` are distinct types. Values may repeat in different
namespaces; duplicates within the same namespace/category are rejected. Do not
renumber IDs when changing labels or ordering.

```cpp
#include <MenuSystem/MenuRegistry.h>

enum class ReportMenu { Reports = 1 };
enum class ReportAction { Export = 1 };

const pl::gui::MenuKey reports{"product.reports", ReportMenu::Reports};
const pl::gui::ActionKey exportReport{"product.reports", ReportAction::Export};

// Register parents before children. std::nullopt means a top-level menu.
menus.addMenu({reports, app::menuKey(app::MenuId::Tools),
    {"Reports", QT_TRANSLATE_NOOP("Reports", "Reports")}, 50});

pl::gui::ActionDefinition command;
command.id = exportReport;
command.text = {"Reports", QT_TRANSLATE_NOOP("Reports", "Export…")};
command.order = 10;
command.icon = icons.icon(exportIconKey);
command.shortcut = QKeySequence(Qt::CTRL | Qt::Key_E);

// controller is a QObject. Capture only objects that outlive it.
// UI input collection and facility calls belong to this application controller.
menus.addAction(reports, command, controller, [controller](bool) {
    controller->exportReport();
});
```

Check boolean return values during application registration. Invalid IDs,
unknown parents, duplicate definitions, and missing handler contexts are rejected
without adding a partial command. Integer `order` values control placement; equal
values retain registration order. Separators have their own order:

```cpp
menus.addSeparator(reports, 20);
menus.action(exportReport)->setEnabled(canExport);
menus.action(exportReport)->setVisible(showExport);
menus.setMenuVisible(reports, showReports);
```

Empty menus and their empty ancestors hide automatically; separators do not count
as content. Disabled but visible actions still count. Use `setMenuVisible` for
explicit menu visibility so automatic empty-menu handling respects that choice.
Qt collapses redundant separators.

`attachAction(otherMenu, exportReport, order)` reuses the same action in another
menu. Its checked/enabled state and handler are shared. The returned `QAction*`
also works in a toolbar or context menu; the registry remains its owner. Do not
manually reparent registry-owned objects. Destroying the handler context disables
its commands and disconnects callbacks. All registry operations use the GUI thread.

`MenuRegistry::retranslate()` resolves each definition's translation context and
source string again. `MainWindow` calls it on `LanguageChange`, then refreshes the
dynamic API Run/Stop label. Future dynamic labels should likewise be refreshed
by their application controller after retranslation.

## Select an icon from a PNG sheet

A PNG atlas contains raster cells. Register cell dimensions and margins/gaps once;
individual sources require only a filename, zero-based row, and zero-based column.
Qt resource paths (`:/...`) and filesystem paths both work.

```cpp
#include <MenuSystem/Icons/IconProvider.h>

enum class ReportIcon { Export = 1 };
const pl::gui::IconKey exportIconKey{"product.reports", ReportIcon::Export};

pl::gui::IconSheet sheet;
sheet.cellSize = QSize(24, 24);
sheet.margins = QMargins(0, 0, 0, 0); // defaults to zero
sheet.spacing = QSize(0, 0);         // defaults to zero
sheet.highDpiFiles.insert(2, ":/icons/reports@2x.png"); // optional
icons.registerSheet(":/icons/reports.png", sheet);

icons.registerIcon(exportIconKey,
    pl::gui::IconSource::sprite(":/icons/reports.png", 1, 3));
command.icon = icons.icon(exportIconKey); // ordinary QIcon
```

At 2x, the entire sheet, including margins and spacing, must be exactly twice the
base dimensions. Cropped pixmaps receive the matching device pixel ratio. The
provider explicitly crops each supplied resolution; it does not rely on QIcon
automatically cropping sheets. Supply higher-resolution PNGs for high-DPI detail.

The usable image area must contain whole cells separated by the declared spacing.
Invalid geometry, missing files, and out-of-range cells produce a warning and an
empty icon. A bad optional high-DPI sheet falls back to the valid base sheet.
Transparency is preserved. Missing icons do not prevent commands from working.

Sheets load once per provider. Named icons are lazily constructed and cached.
`clearCache()` reloads artwork on the next request; reassign refreshed icons to
existing QActions if their artwork changed. `fromSource(IconSource::sprite(...))`
supports one-off icons without an ID; sheet data is still cached.

## SVG, individual PNG, and custom states

Application artwork is packaged in `app/resources/icons.qrc`. Included SVG icons
are project-owned vector drawings. Individual SVGs retain their scalable icon
engine; switching a named icon from an atlas to SVG leaves menu definitions alone:

```cpp
icons.registerIcon(exportIconKey,
    pl::gui::IconSource::file(":/icons/export.svg"));
// Individual PNGs work with file() too.
// Ready-made QIcons use IconSource::fromIcon(existingIcon).
```

Registrations are immutable: choose one mapping per icon ID at startup. For
distinct disabled or checked artwork, register variants:

```cpp
icons.registerIcon(exportIconKey, QVector<pl::gui::IconVariant>{
    {pl::gui::IconSource::file(":/icons/export.svg")},
    {pl::gui::IconSource::file(":/icons/export-disabled.svg"), QIcon::Disabled},
    {pl::gui::IconSource::file(":/icons/export-checked.svg"), QIcon::Normal, QIcon::On}
});
```

Qt generates fallback modes when custom artwork is absent. Each mode/state pair
must be unique. An existing QIcon carries its own variants and is registered on
its own. SVG sources are added first to select the vector icon engine. Qt SVG is
a required dependency of `pl_core_gui`; deployment must include the Qt SVG icon
engine plugin with the application's other Qt runtime dependencies.

The application sets `nativeMenuBar` to false so Tools stays visible inside the
window on macOS as well as other platforms. The top-level Tools entry uses its
text label; Qt menu bars can replace that label with an icon if one is assigned.
Submenus and commands retain their icons and request icon visibility explicitly.
The reusable registry leaves menu-bar placement to its host application. Menu
roles remain explicit (`NoRole` by default).

## Build and validation

```sh
cmake --preset macos-qt
cmake --build --preset macos-qt -j 8
ctest --preset macos-qt
```

`MenuSystemTests` covers menu nesting/ordering, visibility, registration failures,
module IDs, dispatch, translation, context lifetime, ownership, PNG extraction,
transparency, high-DPI sources, SVG scaling, caching, and invalid artwork.
`Product_1MenuTests` through `Product_5MenuTests` compile the actual application
window/controller/builder for each variant, linking only that variant's facility
libraries. They exercise composition, facility Enable/Run, API start/stop/failure,
and shutdown cleanup. A shown-window regression verifies that Tools has visible,
clickable geometry inside the window and that its popup opens. GUI tests default
to Qt's offscreen platform; use `QT_QPA_PLATFORM=cocoa` to run the visibility
regression against the macOS backend. Existing
facility and gRPC tests remain separate.

The implementation uses Qt5/Qt6-compatible APIs. Primary validation uses the
installed Qt 6.11 SDK; Qt5 compilation requires a separate Qt5 SDK.
