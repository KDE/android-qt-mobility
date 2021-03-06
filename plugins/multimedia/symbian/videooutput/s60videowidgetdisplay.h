/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOWIDGETDISPLAY_H
#define S60VIDEOWIDGETDISPLAY_H

#include "s60videodisplay.h"
#include <QtGui/qwindowdefs.h>
#include <QtGui/QPixmap>

class CFbsBitmap;
class S60VideoWidget;
class QWidget;

QT_USE_NAMESPACE

class S60VideoWidgetDisplay : public S60VideoDisplay
{
    Q_OBJECT
public:
    S60VideoWidgetDisplay(QObject *parent);
    ~S60VideoWidgetDisplay();

    // QObject
    bool eventFilter(QObject *object, QEvent *e);

    // S60VideoDisplay
    WId winId() const;
    QRect extentRect() const;
    void setFrame(const CFbsBitmap &bitmap);

    QWidget *widget() const;
    WId topWinId() const;
    void setTopWinId(WId id);
    void setOrdinalPosition(int ordinalPosition);
    int ordinalPosition() const;
    const QRect &explicitExtentRect() const;
    void setExplicitExtentRect(const QRect &rect);

private:
    S60VideoWidget *m_widget;
    QPixmap m_pixmap;
    QRect m_explicitExtentRect;
};

#endif // S60VIDEOWIDGETDISPLAY_H

