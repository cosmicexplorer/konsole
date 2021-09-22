/*
    SPDX-FileCopyrightText: 2020-2020 Gustavo Carneiro <gcarneiroa@hotmail.com>
    SPDX-FileCopyrightText: 2007-2008 Robert Knight <robertknight@gmail.com>
    SPDX-FileCopyrightText: 1997, 1998 Lars Doelle <lars.doelle@on-line.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TERMINALCOLOR_HPP
#define TERMINALCOLOR_HPP

// Qt
#include <QColor>
#include <QWidget>

// Konsole
#include "characters/CharacterColor.h"
#include "konsoleprivate_export.h"
#include "profile/Profile.h"

namespace Konsole
{
class Profile;
class ColorScheme;

class KONSOLEPRIVATE_EXPORT TerminalColor : public QObject
{
    Q_OBJECT
public:
    explicit TerminalColor(QObject *parent);

    void applyProfile(const Profile::Ptr &profile, ColorScheme const *colorScheme, uint randomSeed);

    QColor backgroundColor() const;
    QColor foregroundColor() const;
    void setColorTable(const QColor *table);
    const QColor *colorTable() const;

    void setOpacity(qreal opacity);

    void visualBell();

    qreal opacity() const;
    QRgb blendColor() const;

    QColor cursorColor() const
    {
        return m_cursorColor;
    }

    QColor cursorTextColor() const
    {
        return m_cursorTextColor;
    }

public Q_SLOTS:
    void setBackgroundColor(const QColor &color);
    void setForegroundColor(const QColor &color);

private Q_SLOTS:
    void swapFGBGColors();

private:
    qreal m_opacity;
    QRgb m_blendColor;

    QColor m_cursorColor;
    QColor m_cursorTextColor;

    QColor m_colorTable[TABLE_COLORS];
};
}

#endif
