#include "IconProvider.h"
#include <QDebug>
#include <QFileInfo>
#include <QPixmap>
#include <QSvgRenderer>
#include <algorithm>

namespace pl::gui {

IconSource IconSource::file(QString path)
{
    IconSource source;
    source.filename = std::move(path);
    return source;
}

IconSource IconSource::sprite(QString path, int row, int column)
{
    auto source = file(std::move(path));
    source.kind = Kind::Sprite;
    source.row = row;
    source.column = column;
    return source;
}

IconSource IconSource::fromIcon(QIcon icon)
{
    IconSource source;
    source.kind = Kind::Existing;
    source.existing = std::move(icon);
    return source;
}

bool IconProvider::registerSheet(const QString& filename, const IconSheet& sheet)
{
    const auto& m = sheet.margins;
    if (filename.isEmpty() || sheets_.contains(filename)
        || sheet.cellSize.width() <= 0 || sheet.cellSize.height() <= 0
        || sheet.spacing.width() < 0 || sheet.spacing.height() < 0
        || m.left() < 0 || m.top() < 0 || m.right() < 0 || m.bottom() < 0) {
        qWarning() << "IconProvider: invalid/duplicate sheet:" << filename;
        return false;
    }
    for (auto it = sheet.highDpiFiles.cbegin(); it != sheet.highDpiFiles.cend(); ++it) {
        if (it.key() < 2 || it.value().isEmpty()) {
            qWarning() << "IconProvider: invalid high-DPI sheet:" << filename;
            return false;
        }
    }
    sheets_.insert(filename, sheet);
    return true;
}

bool IconProvider::registerIcon(IconKey id, const IconSource& source)
{
    return registerIcon(id, QVector<IconVariant>{{source}});
}

bool IconProvider::registerIcon(IconKey id, const QVector<IconVariant>& variants)
{
    if (!id.isValid() || definitions_.contains(id) || variants.isEmpty()) {
        qWarning() << "IconProvider: invalid/duplicate icon:" << id.name();
        return false;
    }
    // Existing QIcons already carry their own modes, states and resolutions.
    for (int i = 0; i < variants.size(); ++i) {
        if (variants[i].source.kind == IconSource::Kind::Existing
            && (variants.size() != 1 || variants[i].mode != QIcon::Normal
                || variants[i].state != QIcon::Off)) {
            qWarning() << "IconProvider: an existing QIcon must be registered on its own:" << id.name();
            return false;
        }
        for (int j = 0; j < i; ++j) {
            if (variants[i].mode == variants[j].mode && variants[i].state == variants[j].state) {
                qWarning() << "IconProvider: duplicate mode/state:" << id.name();
                return false;
            }
        }
    }
    definitions_.insert(id, variants);
    return true;
}

QImage IconProvider::image(const QString& filename)
{
    if (!images_.contains(filename)) {
        QImage loaded(filename);
        if (loaded.isNull())
            qWarning() << "IconProvider: could not load sheet:" << filename;
        // Sheet geometry is expressed in physical pixels before explicit DPR handling.
        loaded.setDevicePixelRatio(1);
        images_.insert(filename, loaded);
    }
    return images_.value(filename);
}

bool IconProvider::addSource(QIcon& result, const IconVariant& variant)
{
    const auto& source = variant.source;
    if (source.kind == IconSource::Kind::Existing) {
        result = source.existing;
        return !result.isNull();
    }
    if (source.kind == IconSource::Kind::File) {
        bool valid = QFileInfo::exists(source.filename);
        if (valid && source.filename.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive))
            valid = QSvgRenderer(source.filename).isValid();
        if (!valid || QIcon(source.filename).pixmap(16, 16).isNull()) {
            qWarning() << "IconProvider: could not load icon:" << source.filename;
            return false;
        }
        result.addFile(source.filename, QSize(), variant.mode, variant.state);
        return true;
    }

    const auto it = sheets_.constFind(source.filename);
    if (it == sheets_.cend() || source.row < 0 || source.column < 0) {
        qWarning() << "IconProvider: unknown sheet or negative coordinates:" << source.filename;
        return false;
    }
    const auto& sheet = it.value();
    const auto base = image(source.filename);
    if (base.isNull())
        return false;
    const qint64 width = qint64(base.width()) - sheet.margins.left() - sheet.margins.right();
    const qint64 height = qint64(base.height()) - sheet.margins.top() - sheet.margins.bottom();
    const qint64 strideX = qint64(sheet.cellSize.width()) + sheet.spacing.width();
    const qint64 strideY = qint64(sheet.cellSize.height()) + sheet.spacing.height();
    const qint64 columns = (width + sheet.spacing.width()) / strideX;
    const qint64 rows = (height + sheet.spacing.height()) / strideY;
    if (width < sheet.cellSize.width() || height < sheet.cellSize.height()
        || (width + sheet.spacing.width()) % strideX != 0
        || (height + sheet.spacing.height()) % strideY != 0
        || source.column >= columns || source.row >= rows) {
        qWarning() << "IconProvider: invalid grid or out-of-range cell:"
                   << source.filename << source.row << source.column;
        return false;
    }
    const QRect rect(int(sheet.margins.left() + source.column * strideX),
                     int(sheet.margins.top() + source.row * strideY),
                     sheet.cellSize.width(), sheet.cellSize.height());
    auto addCrop = [&](const QImage& sheetImage, int scale) {
        auto pixmap = QPixmap::fromImage(sheetImage.copy(
            QRect(rect.x() * scale, rect.y() * scale, rect.width() * scale, rect.height() * scale)));
        pixmap.setDevicePixelRatio(scale);
        result.addPixmap(pixmap, variant.mode, variant.state);
    };
    addCrop(base, 1);
    for (auto scaled = sheet.highDpiFiles.cbegin(); scaled != sheet.highDpiFiles.cend(); ++scaled) {
        const auto highDpi = image(scaled.value());
        const qint64 expectedWidth = qint64(base.width()) * scaled.key();
        const qint64 expectedHeight = qint64(base.height()) * scaled.key();
        if (highDpi.isNull() || highDpi.width() != expectedWidth || highDpi.height() != expectedHeight) {
            qWarning() << "IconProvider: high-DPI sheet must match scaled base geometry:" << scaled.value();
            continue; // keep usable base-resolution icon
        }
        addCrop(highDpi, scaled.key());
    }
    return true;
}

QIcon IconProvider::icon(IconKey id)
{
    if (icons_.contains(id))
        return icons_.value(id);
    const auto it = definitions_.constFind(id);
    if (it == definitions_.cend()) {
        qWarning() << "IconProvider: unregistered icon:" << id.name();
        return {};
    }
    auto variants = it.value();
    // QIcon selects its engine from the first source. Vector sources go first.
    std::stable_sort(variants.begin(), variants.end(), [](const IconVariant& a, const IconVariant& b) {
        auto svg = [](const IconVariant& v) {
            return v.source.kind == IconSource::Kind::File
                && v.source.filename.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive);
        };
        return svg(a) && !svg(b);
    });
    QIcon result;
    for (const auto& variant : variants)
        addSource(result, variant);
    icons_.insert(id, result);
    return result;
}

QIcon IconProvider::fromSource(const IconSource& source)
{
    QIcon result;
    addSource(result, {source});
    return result;
}

void IconProvider::clearCache()
{
    icons_.clear();
    images_.clear();
}

} // namespace pl::gui
