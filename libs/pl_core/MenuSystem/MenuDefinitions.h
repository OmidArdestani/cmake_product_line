#pragma once

#include "Ids.h"
#include <QAction>
#include <QCoreApplication>
#include <QIcon>
#include <QKeySequence>
#include <optional>

namespace pl::gui {

struct MenuText
{
    QByteArray context;
    QByteArray source;

    QString resolve() const
    {
        return context.isEmpty() ? QString::fromUtf8(source)
                                 : QCoreApplication::translate(context.constData(), source.constData());
    }
};

struct MenuDefinition
{
    MenuKey id;
    std::optional<MenuKey> parent; // no parent = menu bar
    MenuText title;
    int order = 0;
    QIcon icon;
    bool visible = true;
};

struct ActionDefinition
{
    ActionKey id;
    MenuText text;
    int order = 0;
    QIcon icon;
    QKeySequence shortcut;
    MenuText toolTip;
    bool enabled = true;
    bool visible = true;
    bool checkable = false;
    bool checked = false;
    Qt::ShortcutContext shortcutContext = Qt::WindowShortcut;
    QAction::MenuRole menuRole = QAction::NoRole;
};

} // namespace pl::gui
