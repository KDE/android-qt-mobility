/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef V4LMEDIAFORMATCONTROL_H
#define V4LMEDIAFORMATCONTROL_H

#include "qmediaformatcontrol.h"
#include <QtCore/qstringlist.h>

class V4LCameraSession;

class V4LMediaFormatControl : public QMediaFormatControl
{
Q_OBJECT
public:
    V4LMediaFormatControl(QObject *parent);
    virtual ~V4LMediaFormatControl() {};

    virtual QStringList supportedFormats() const { return m_supportedFormats; }
    virtual QString format() const { return m_format; }
    virtual void setFormat(const QString &formatMimeType) { m_format = formatMimeType; }

    virtual QString formatDescription(const QString &formatMimeType) const { return m_formatDescriptions.value(formatMimeType); }

private:
    V4LCameraSession* m_session;

    QString m_format;
    QStringList m_supportedFormats;
    QMap<QString, QString> m_formatDescriptions;
};

#endif // V4LMEDIAFORMATCONTROL_H
