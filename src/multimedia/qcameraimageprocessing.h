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

#ifndef QCAMERAIMAGEPROCESSING_H
#define QCAMERAIMAGEPROCESSING_H

#include <QtCore/qstringlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qsize.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>

#include <qmediacontrol.h>
#include <qmediaobject.h>
#include <qmediaservice.h>

QT_BEGIN_NAMESPACE

class QCamera;

class QCameraImageProcessingPrivate;
class Q_MULTIMEDIA_EXPORT QCameraImageProcessing : public QObject
{
    Q_OBJECT
public:
    enum WhiteBalanceMode {        
        WhiteBalanceAuto = 0,
        WhiteBalanceManual = 1,
        WhiteBalanceSunlight = 2,
        WhiteBalanceCloudy = 3,
        WhiteBalanceShade = 4,
        WhiteBalanceTungsten = 5,
        WhiteBalanceFluorescent = 6,
        WhiteBalanceIncandescent = 7,
        WhiteBalanceFlash = 8,
        WhiteBalanceSunset = 9,
        WhiteBalanceVendor = 1000
    };

    bool isAvailable() const;

    WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(WhiteBalanceMode mode);
    bool isWhiteBalanceModeSupported(WhiteBalanceMode mode) const;
    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);

    int contrast() const;
    void setContrast(int value);

    int saturation() const;
    void setSaturation(int value);

    bool isSharpeningSupported() const;
    int sharpeningLevel() const;
    void setSharpeningLevel(int value);

    bool isDenoisingSupported() const;
    int denoisingLevel() const;
    void setDenoisingLevel(int value);

private:
    friend class QCamera;
    QCameraImageProcessing(QCamera *camera);
    ~QCameraImageProcessing();

    Q_DISABLE_COPY(QCameraImageProcessing)
    Q_DECLARE_PRIVATE(QCameraImageProcessing)
    QCameraImageProcessingPrivate *d_ptr;
};

QT_END_NAMESPACE

#endif  // QCAMERAIMAGEPROCESSING_H
