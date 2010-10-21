/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QVALUESPACE_H
#define QVALUESPACE_H

#include "qmobilityglobal.h"

#include <QList>
#include <QUuid>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

//QTM_SYNC_HEADER_EXPORT QValueSpace

namespace QValueSpace {
    enum LayerOption {
        UnspecifiedLayer = 0x0000,
        PermanentLayer = 0x0001,
        TransientLayer = 0x0002,
        // UnspecifiedLayerPermanence = 0x0000,
        // InvalidLayerPermanence = 0x0003,
        WritableLayer = 0x0004,
        ReadOnlyLayer = 0x0008,
        // UnspecifiedLayerWriteability = 0x0000,
        // InvalidLayerWriteability = 0x000C,
    };
    Q_DECLARE_FLAGS(LayerOptions, LayerOption)

    Q_PUBLISHSUBSCRIBE_EXPORT void initValueSpaceServer();

    Q_PUBLISHSUBSCRIBE_EXPORT QList<QUuid> availableLayers();
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QValueSpace::LayerOptions);

#define QVALUESPACE_SHAREDMEMORY_LAYER QUuid(0xd81199c1, 0x6f60, 0x4432, 0x93, 0x4e, \
                                             0x0c, 0xe4, 0xd3, 0x7e, 0xf2, 0x52)
#define QVALUESPACE_VOLATILEREGISTRY_LAYER QUuid(0x8ceb5811, 0x4968, 0x470f, 0x8f, 0xc2, \
                                                 0x26, 0x47, 0x67, 0xe0, 0xbb, 0xd9)
#define QVALUESPACE_NONVOLATILEREGISTRY_LAYER QUuid(0x8e29561c, 0xa0f0, 0x4e89, 0xba, 0x56, \
                                                    0x08, 0x06, 0x64, 0xab, 0xc0, 0x17)
#define QVALUESPACE_CONTEXTKIT_LAYER QUuid(0x2c769b9e, 0xd949, 0x4cd1, 0x84, 0x8f, \
                                           0xd3, 0x22, 0x41, 0xfe, 0x07, 0xff)
#define QVALUESPACE_SYMBIAN_SETTINGS_LAYER QUuid(0x40d7b059, 0x66ac, 0x442f, 0xb2, 0x22, \
                                                 0x9c, 0x8a, 0xb9, 0x8b, 0x9c, 0x2d)
#define QVALUESPACE_GCONF_LAYER QUuid(0x0e2e5da0, 0x0044, 0x11df, 0x94, 0x1c, \
                                                 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b)

QTM_END_NAMESPACE

QT_END_HEADER

#endif // _QVALUESPACE_H_
