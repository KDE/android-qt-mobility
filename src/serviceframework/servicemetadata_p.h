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

#ifndef SERVICEMETADATA_H
#define SERVICEMETADATA_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include <QXmlStreamReader>
#include <QStringList>
#include <QList>
#include <QSet>
#include "qserviceinterfacedescriptor.h"

#ifdef SERVICE_XML_GENERATOR
#undef QM_AUTOTEST_EXPORT
#define QM_AUTOTEST_EXPORT
#endif

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

// FORWARD DECLARATIONS
class QServiceInterfaceDescriptor;

class ServiceMetaDataResults 
{
public:
    ServiceMetaDataResults() {}

    ServiceMetaDataResults(const ServiceMetaDataResults& other)
    {
        type = other.type;;
        location = other.location;
        name = other.name;
        description = other.description;
        interfaces = other.interfaces;
        latestInterfaces = other.latestInterfaces;
    }
   
    int type;
    QString location;
    QString name;
    QString description;
    QList<QServiceInterfaceDescriptor> interfaces;
    QList<QServiceInterfaceDescriptor> latestInterfaces;
};

#ifndef QT_NO_DATASTREAM
QM_AUTOTEST_EXPORT QDataStream &operator<<(QDataStream &, const ServiceMetaDataResults &);
QM_AUTOTEST_EXPORT QDataStream &operator>>(QDataStream &, ServiceMetaDataResults &);
#endif

class QM_AUTOTEST_EXPORT ServiceMetaData
{
public:

    //! ServiceMetaData::ServiceMetadataErr
    /*!
     This enum describes the errors that may be returned by the Service metadata parser.
     */
    enum ServiceMetadataErr {
        SFW_ERROR_NO_SERVICE = 0,                           /* Can not find service root node in XML file*/
        SFW_ERROR_NO_SERVICE_NAME,                          /* Can not find service name in XML file */
        SFW_ERROR_NO_SERVICE_PATH,                          /* Can not find service filepath in XML file */
        SFW_ERROR_NO_SERVICE_INTERFACE,                     /* No interface for the service in XML file*/
        SFW_ERROR_NO_INTERFACE_VERSION,                     /* Can not find interface version in XML file */
        SFW_ERROR_NO_INTERFACE_NAME,                        /* Can not find interface name in XML file*/
        SFW_ERROR_UNABLE_TO_OPEN_FILE,                      /* Error opening XML file*/
        SFW_ERROR_INVALID_XML_FILE,                         /* Not a valid XML file*/
        SFW_ERROR_PARSE_SERVICE,                            /* Error parsing service node */
        SFW_ERROR_PARSE_INTERFACE,                          /* Error parsing interface node */
        SFW_ERROR_DUPLICATED_INTERFACE,                     /* The same interface is defined twice */
        SFW_ERROR_INVALID_VERSION,
        SFW_ERROR_DUPLICATED_TAG,                           /* The tag appears twice */
        SFW_ERROR_INVALID_CUSTOM_TAG,                       /* The customproperty tag is not corectly formatted or otherwise incorrect*/
        SFW_ERROR_DUPLICATED_CUSTOM_KEY,                    /* The customproperty appears twice*/
        SFW_ERROR_MULTIPLE_SERVICE_TYPES,                   /* Both filepath and ipcaddress found in the XML file */
        SFW_ERROR_INVALID_FILEPATH,                         /* Service path cannot contain IPC prefix */
        SFW_ERROR_INVALID_XML_VERSION,                      /* Error parsing serficefw version node */
        SFW_ERROR_UNSUPPORTED_IPC,                          /* Servicefw version doesn't support IPC */
        SFW_ERROR_UNSUPPORTED_XML_VERSION                   /* Unsupported servicefw version supplied */
    };

public:

    ServiceMetaData(const QString &aXmlFilePath);

    ServiceMetaData(QIODevice *device);

    ~ServiceMetaData();

    void setDevice(QIODevice *device);

    QIODevice *device() const;

    bool extractMetadata();

    int getLatestError() const;

    ServiceMetaDataResults parseResults() const;

private:
    QList<QServiceInterfaceDescriptor> latestInterfaces() const;
    QServiceInterfaceDescriptor latestInterfaceVersion(const QString &interfaceName);
    bool processVersionElement(QXmlStreamReader &aXMLReader);
    bool processServiceElement(QXmlStreamReader &aXMLReader);
    bool processInterfaceElement(QXmlStreamReader &aXMLReader);
    void clearMetadata();

private:
    bool lessThan(const QServiceInterfaceDescriptor &d1,
                    const QServiceInterfaceDescriptor &d2) const;
    bool greaterThan(const QString &v1, const QString &v2) const;
    bool checkVersion(const QString &version) const;
    void transformVersion(const QString &version, int *major, int *minor) const;

    QIODevice *xmlDevice;
    bool ownsXmlDevice;
    QString xmlVersion;
    QString serviceName;
    QString serviceLocation;
    QString serviceDescription;
    QService::Type serviceType;
    QList<QServiceInterfaceDescriptor> serviceInterfaces;
    QSet<QString> duplicates;
    int latestError;
    QHash<QString, int> m_latestIndex;
};

QTM_END_NAMESPACE

#endif // SERVICEMETADATA_H
