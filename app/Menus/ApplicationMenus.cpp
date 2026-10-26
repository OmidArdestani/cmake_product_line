#include "ApplicationMenus.h"
#include "MenuIds.h"
#include "Tools/ToolController.h"

namespace app {
namespace {
pl::gui::MenuText text(const char* source) { return {"ApplicationMenus", source}; }

const char* productTitle()
{
#if defined(Product_1)
    return QT_TRANSLATE_NOOP("ApplicationMenus", "Product 1");
#elif defined(Product_2)
    return QT_TRANSLATE_NOOP("ApplicationMenus", "Product 2");
#elif defined(Product_3)
    return QT_TRANSLATE_NOOP("ApplicationMenus", "Product 3");
#elif defined(Product_4)
    return QT_TRANSLATE_NOOP("ApplicationMenus", "Product 4");
#elif defined(Product_5)
    return QT_TRANSLATE_NOOP("ApplicationMenus", "Product 5");
#else
#error A product variant must be selected
#endif
}
} // namespace

bool registerApplicationIcons(pl::gui::IconProvider& icons)
{
    using pl::gui::IconSource;
    return icons.registerIcon(iconKey(IconId::Tools), IconSource::file(QStringLiteral(":/icons/tools.svg")))
        && icons.registerIcon(iconKey(IconId::Api), IconSource::file(QStringLiteral(":/icons/api.svg")))
        && icons.registerIcon(iconKey(IconId::Run), IconSource::file(QStringLiteral(":/icons/run.svg")))
        && icons.registerIcon(iconKey(IconId::Stop), IconSource::file(QStringLiteral(":/icons/stop.svg")))
        && icons.registerIcon(iconKey(IconId::Settings), IconSource::file(QStringLiteral(":/icons/settings.svg")))
        && icons.registerIcon(iconKey(IconId::Product), IconSource::file(QStringLiteral(":/icons/product.svg")));
}

bool registerApplicationMenus(pl::gui::MenuRegistry& menus, pl::gui::IconProvider& icons,
                              ToolController& tools)
{
    using namespace pl::gui;
    if (!menus.addMenu({menuKey(MenuId::File), {}, text(QT_TRANSLATE_NOOP("ApplicationMenus", "File")), 10})
        || !menus.addMenu({menuKey(MenuId::Edit), {}, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Edit")), 20})
        || !menus.addMenu({menuKey(MenuId::View), {}, text(QT_TRANSLATE_NOOP("ApplicationMenus", "View")), 30})
        // An in-window QMenuBar can display an icon instead of its title.
        // Keep the top-level Tools label; submenu entries retain their icons.
        || !menus.addMenu({menuKey(MenuId::Tools), {}, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Tools")), 40})
        || !menus.addMenu({menuKey(MenuId::Help), {}, text(QT_TRANSLATE_NOOP("ApplicationMenus", "Help")), 50})
        || !menus.addMenu({menuKey(MenuId::Api), menuKey(MenuId::Tools), text(QT_TRANSLATE_NOOP("ApplicationMenus", "API")), 10,
                           icons.icon(iconKey(IconId::Api))})
        || !menus.addMenu({menuKey(MenuId::Product), menuKey(MenuId::Tools), text(productTitle()), 20,
                           icons.icon(iconKey(IconId::Product))}))
        return false;

    ActionDefinition api;
    api.id = actionKey(ActionId::ToggleApi);
    api.text = text(QT_TRANSLATE_NOOP("ApplicationMenus", "Run"));
    api.icon = icons.icon(iconKey(IconId::Run));
    api.toolTip = text(QT_TRANSLATE_NOOP("ApplicationMenus", "Start or stop the API server"));
    if (!menus.addAction(menuKey(MenuId::Api), api, &tools, [&tools](bool) { tools.toggleApi(); }))
        return false;

    ActionDefinition settings;
    settings.id = actionKey(ActionId::ApiSettings);
    settings.text = text(QT_TRANSLATE_NOOP("ApplicationMenus", "Settings…"));
    settings.icon = icons.icon(iconKey(IconId::Settings));
    settings.order = 10;
    settings.enabled = false;
    settings.toolTip = text(QT_TRANSLATE_NOOP("ApplicationMenus", "API settings are not available yet"));
    if (!menus.addAction(menuKey(MenuId::Api), settings, &tools, [](bool) {}))
        return false;
    QObject::connect(menus.menu(menuKey(MenuId::Api)), &QMenu::aboutToShow, &tools, [&tools] { tools.syncApi(); });

    int order = 0;
    for (const auto& facility : tools.facilities()) {
        if (!menus.addMenu({facility.menu, menuKey(MenuId::Product), {{}, facility.name.toUtf8()}, order++,
                           icons.icon(iconKey(IconId::Product))}))
            return false;
        ActionDefinition enabled;
        enabled.id = facility.enabledAction;
        enabled.text = text(QT_TRANSLATE_NOOP("ApplicationMenus", "Enabled"));
        enabled.icon = icons.icon(iconKey(IconId::Settings));
        enabled.checkable = true;
        enabled.checked = facility.asset->isEnable();
        ActionDefinition run;
        run.id = facility.runAction;
        run.text = text(QT_TRANSLATE_NOOP("ApplicationMenus", "Run feature"));
        run.order = 10;
        run.icon = icons.icon(iconKey(IconId::Run));
        run.enabled = facility.asset->isEnable();
        if (!menus.addAction(facility.menu, enabled, &tools, [&tools, facility](bool checked) {
                tools.setFacilityEnabled(facility, checked);
            }) || !menus.addAction(facility.menu, run, &tools, [&tools, facility](bool) {
                tools.runFacility(facility);
            }))
            return false;
        QObject::connect(menus.menu(facility.menu), &QMenu::aboutToShow, &tools, [&tools, facility] {
            tools.syncFacility(facility);
        });
    }
    tools.syncApi();
    return true;
}

} // namespace app
