#pragma once

#include "../Ids.h"
#include <QIcon>
#include <QImage>
#include <QMap>
#include <QMargins>
#include <QSize>
#include <QVector>

namespace pl::gui {

struct IconSource
{
    enum class Kind { File, Sprite, Existing };
    Kind kind = Kind::File;
    QString filename;
    int row = 0;
    int column = 0;
    QIcon existing;

    static IconSource file(QString path);
    static IconSource sprite(QString path, int row, int column);
    static IconSource fromIcon(QIcon icon);
};

struct IconSheet
{
    QSize cellSize;                       // logical pixels at 1x
    QMargins margins;                    // outer sheet margins at 1x
    QSize spacing{0, 0};                 // horizontal/vertical gaps at 1x
    QMap<int, QString> highDpiFiles;       // e.g. {2, ":/icons/tools@2x.png"}
};

struct IconVariant
{
    IconSource source;
    QIcon::Mode mode = QIcon::Normal;
    QIcon::State state = QIcon::Off;
};

// Use on the GUI thread: QIcons eventually create QPixmaps.
// Registrations are immutable; clearCache() reloads changed on-disk artwork.
class IconProvider
{
public:
    bool registerSheet(const QString& filename, const IconSheet& sheet);
    bool registerIcon(IconKey id, const IconSource& source);
    bool registerIcon(IconKey id, const QVector<IconVariant>& variants);
    QIcon icon(IconKey id);
    QIcon fromSource(const IconSource& source);
    void clearCache();

private:
    QImage image(const QString& filename);
    bool addSource(QIcon& icon, const IconVariant& variant);
    QMap<QString, IconSheet> sheets_;
    QMap<QString, QImage> images_;
    QMap<IconKey, QVector<IconVariant>> definitions_;
    QMap<IconKey, QIcon> icons_;
};

} // namespace pl::gui
