/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsystemstorageinfo.h"
#include "qsysteminfocommon_p.h"
#include <QMetaType>

QTM_BEGIN_NAMESPACE

        Q_GLOBAL_STATIC(QSystemStorageInfoPrivate, storageInfoPrivate)

/*!
   \class QSystemStorageInfo
   \ingroup systeminfo
   \inmodule QtSystemInfo
        \brief The QSystemStorageInfo class provides access to memory and disk information from the system.

   \fn QSystemStorageInfo::QSystemStorageInfo(QObject *parent)
  Constructs a QSystemStorageInfo object with the given \a parent.
*/

        /*!
            \enum QSystemStorageInfo::DriveType
            This enum describes the type of drive or volume

            \value NoDrive               Drive type undetermined.
            \value InternalDrive         Is internal drive.
            \value RemovableDrive        Is removable.
            \value RemoteDrive           Is a network drive.
            \value CdromDrive            Is a cd rom drive.
        */

        /*!
           \fn void QSystemStorageInfo::logicalDriveChanged(bool,const QString &))

           This signal gets emitted when new storage has been added or removed from the system.
        */


QSystemStorageInfo::QSystemStorageInfo(QObject *parent)
   : QObject(parent), d(storageInfoPrivate())
{
    qRegisterMetaType<QSystemStorageInfo::DriveType>("QSystemStorageInfo::DriveType");
    connect(d,SIGNAL(logicalDriveChanged(bool,const QString &)),
           this,SIGNAL(logicalDriveChanged(bool,const QString &)));
}

/*!
 Destroys the QSystemStorageInfo object.
*/
QSystemStorageInfo::~QSystemStorageInfo()
{
}

/*!
   Returns the amount of total space on the \a volumeDrive,
   in bytes.
*/
qlonglong QSystemStorageInfo::totalDiskSpace(const QString &volumeDrive)
{
   return storageInfoPrivate()->totalDiskSpace(volumeDrive);
}

/*!
   Returns the amount of available free space on the \a volumeDrive,
in bytes.
*/
qlonglong QSystemStorageInfo::availableDiskSpace(const QString &volumeDrive)
{
   return storageInfoPrivate()->availableDiskSpace(volumeDrive);
}

/*!
 \property QSystemStorageInfo::logicalDrives
 \brief The logical drives.

   Returns a QStringList of volumes or partitions.
*/
QStringList QSystemStorageInfo::logicalDrives()
{
   return storageInfoPrivate()->logicalDrives();
}

/*!
 Returns the type of volume \a driveVolume
*/
QSystemStorageInfo::DriveType QSystemStorageInfo::typeForDrive(const QString &driveVolume)
{
   return storageInfoPrivate()->typeForDrive(driveVolume);
}


#include "moc_qsystemstorageinfo.cpp"

QTM_END_NAMESPACE
