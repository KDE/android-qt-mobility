/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QORGANIZERITEMVISUALREMINDER_H
#define QORGANIZERITEMVISUALREMINDER_H

#include "qorganizeritemreminder.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerItemVisualReminder : public QOrganizerItemReminder
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldMessage;
    const char* FieldDataUrl;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_REMINDER_DETAIL(QOrganizerItemVisualReminder, "VisualReminder")
    Q_DECLARE_LATIN1_CONSTANT(FieldMessage, "Message");
    Q_DECLARE_LATIN1_CONSTANT(FieldDataUrl, "DataUrl");
#endif

    // message and visual data to be displayed if visual notification.
    void setMessage(const QString& message) {setValue(FieldMessage, message);}
    QString message() const {return value<QString>(FieldMessage);}
    void setDataUrl(const QUrl& dataUrl) {setValue(FieldDataUrl, dataUrl);}
    QUrl dataUrl() const {return value<QUrl>(FieldDataUrl);}
};

QTM_END_NAMESPACE

#endif

