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

#ifndef QDECLARATIVEORGANIZERCOLLECTION_H
#define QDECLARATIVEORGANIZERCOLLECTION_H

#include "qdeclarative.h"
#include "qorganizercollection.h"

#include <QColor>
#include <QUrl>

QTM_USE_NAMESPACE


class QDeclarativeOrganizerCollection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString collectionId READ id WRITE setId NOTIFY valueChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY valueChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY valueChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY valueChanged)
    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY valueChanged)
public:
    QDeclarativeOrganizerCollection(QObject* parent = 0)
        :QObject(parent)
    {
    }


    QString id() const
    {
        return d.id().toString();
    }

    void setId(const QString& newId)
    {
        d.setId(QOrganizerCollectionId::fromString(newId));
    }

    QString name() const
    {
        return metaData(QOrganizerCollection::KeyName).toString();
    }

    void setName(const QString& name)
    {
        setMetaData(QOrganizerCollection::KeyName, name);
    }

    QString description() const
    {
        return metaData(QOrganizerCollection::KeyDescription).toString();
    }

    void setDescription(const QString& desc)
    {
        setMetaData(QOrganizerCollection::KeyDescription, desc);
    }

    QColor color() const
    {
        return metaData(QOrganizerCollection::KeyColor).value<QColor>();
    }

    void setColor(const QColor& color)
    {
        setMetaData(QOrganizerCollection::KeyColor, color);
    }

    QUrl image() const
    {
        //image or image url?
        return QUrl(metaData(QOrganizerCollection::KeyImage).toString());
    }

    void setImage(const QUrl& url)
    {
        setMetaData(QOrganizerCollection::KeyImage, url);
    }

    Q_INVOKABLE void setMetaData(const QString& key, const QVariant& value)
    {
        if (metaData(key) != value) {
            d.setMetaData(key, value);
            emit valueChanged();
        }
    }

    Q_INVOKABLE  QVariant metaData(const QString& key) const
    {
        return d.metaData(key);
    }

    QOrganizerCollection collection() const
    {
        return d;
    }

    void setCollection(const QOrganizerCollection& collection)
    {
        d = collection;
    }
signals:
    void valueChanged();
private:
    QOrganizerCollection d;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerCollection)
#endif

