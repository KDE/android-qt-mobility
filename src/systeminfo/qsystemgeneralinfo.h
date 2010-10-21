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
#ifndef QSYSTEMGENERALINFO_H
#define QSYSTEMGENERALINFO_H

#include "qmobilityglobal.h"

#include <QObject>
#include <QSize>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QFileSystemWatcher>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemInfoPrivate;

class Q_SYSINFO_EXPORT QSystemInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages)
    Q_PROPERTY(QString currentCountryCode READ currentCountryCode)
    Q_ENUMS(Version)
    Q_ENUMS(Feature)

public:

    QSystemInfo(QObject *parent = 0);
     virtual ~QSystemInfo();

    QString currentLanguage(); // 2 letter ISO 639-1 //signal
    QStringList availableLanguages(); // 2 letter ISO 639-1
    QString currentCountryCode(); //2 letter ISO 3166-1
    enum Version {
        Os = 1,
        QtCore,
        Firmware,
        QtMobility
    };

    QString version(QSystemInfo::Version type, const QString &parameter = QString());

    enum Feature {
        BluetoothFeature=0,
        CameraFeature,
        FmradioFeature,
        IrFeature,
        LedFeature,
        MemcardFeature,
        UsbFeature,
        VibFeature,
        WlanFeature,
        SimFeature,
        LocationFeature,
        VideoOutFeature,
        HapticsFeature
	};

    bool hasFeatureSupported(QSystemInfo::Feature feature);

Q_SIGNALS:
    void currentLanguageChanged(const QString &);
private:
    QSystemInfoPrivate *d;
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSGENERALINFO_H*/

// End of file

