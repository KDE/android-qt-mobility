/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qudevservice_linux_p.h"

QTM_BEGIN_NAMESPACE

QUdevService::QUdevService()
{
    // NOTE: udev_new can fail, if we get a NULL pointer back, can't create udev
    context = udev_new();
}

QUdevService::~QUdevService()
{
    if (context)
        udev_unref(context);
}

bool QUdevService::isSubsystemAvailable(const char *subsystem)
{
    bool available = false;
    struct udev_enumerate *enumerate = 0;
    if (context) {
        if ((enumerate = udev_enumerate_new(context))
            && (0 == udev_enumerate_add_match_subsystem(enumerate, subsystem))
            && (0 == udev_enumerate_scan_devices(enumerate))) {
            available = (0 != udev_enumerate_get_list_entry(enumerate));
        }
    }

    if (enumerate)
        udev_enumerate_unref(enumerate);

    return available;
}

bool QUdevService::isPropertyAvailable(const char *property, const char *value)
{
    bool available = false;
    struct udev_enumerate *enumerate = 0;
    if (context) {
        if ((enumerate = udev_enumerate_new(context))
            && (0 == udev_enumerate_add_match_property(enumerate, property, value))
            && (0 == udev_enumerate_scan_devices(enumerate))) {
            available = (0 != udev_enumerate_get_list_entry(enumerate));
        }
    }

    if (enumerate)
        udev_enumerate_unref(enumerate);

    return available;
}

#include "moc_qudevservice_linux_p.cpp"

QTM_END_NAMESPACE
