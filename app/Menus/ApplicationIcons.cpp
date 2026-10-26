#include "ApplicationIcons.h"

namespace app {

bool registerApplicationIcons(ApplicationMenuSystem& menus)
{
    using pl::gui::IconSource;
    // For an atlas, register cell geometry once with registerSheet(), then use
    // sprite(filename, zeroBasedRow, zeroBasedColumn) instead of file(...).
    return menus.registerIcon(IconId::Tools, IconSource::file(QStringLiteral(":/icons/tools.svg")))
        && menus.registerIcon(IconId::Api, IconSource::file(QStringLiteral(":/icons/api.svg")))
        && menus.registerIcon(IconId::Run, IconSource::file(QStringLiteral(":/icons/run.svg")))
        && menus.registerIcon(IconId::Stop, IconSource::file(QStringLiteral(":/icons/stop.svg")))
        && menus.registerIcon(IconId::Settings, IconSource::file(QStringLiteral(":/icons/settings.svg")))
        && menus.registerIcon(IconId::Product, IconSource::file(QStringLiteral(":/icons/product.svg")));
}

} // namespace app
