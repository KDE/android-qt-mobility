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

#ifndef QGALLERYQUERYMODEL_H
#define QGALLERYQUERYMODEL_H

#include <qgalleryqueryrequest.h>

#include <QtCore/qabstractitemmodel.h>

QTM_BEGIN_NAMESPACE

class QGalleryFilter;
class QGalleryResultSet;

class QGalleryQueryModelPrivate;

class Q_GALLERY_EXPORT QGalleryQueryModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractGallery *gallery READ gallery WRITE setGallery)
    Q_PROPERTY(QStringList sortPropertyNames READ sortPropertyNames WRITE setSortPropertyNames)
    Q_PROPERTY(bool live READ isLive WRITE setLive)
    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(int limit READ limit WRITE setLimit)
    Q_PROPERTY(QString rootType READ rootType WRITE setRootType)
    Q_PROPERTY(QVariant rootItem READ rootItem WRITE setRootItem)
    Q_PROPERTY(QGalleryQueryRequest::Scope scope READ scope WRITE setScope)
    Q_PROPERTY(QGalleryFilter filter READ filter WRITE setFilter)
    Q_PROPERTY(int result READ result NOTIFY resultChanged)
    Q_PROPERTY(QGalleryAbstractRequest::State state READ state NOTIFY stateChanged)
public:
    QGalleryQueryModel(QObject *parent = 0);
    QGalleryQueryModel(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryQueryModel();

    QAbstractGallery *gallery() const;
    void setGallery(QAbstractGallery *gallery);

    QHash<int, QString> roleProperties(int column) const;
    void setRoleProperties(int column, const QHash<int, QString> &properties);

    void addColumn(const QHash<int, QString> &properties);
    void addColumn(const QString &property, int role = Qt::DisplayRole);
    void insertColumn(int index, const QHash<int, QString> &properties);
    void insertColumn(int index, const QString &property, int role = Qt::DisplayRole);
    void removeColumn(int index);

    QStringList sortPropertyNames() const;
    void setSortPropertyNames(const QStringList &names);

    bool isLive() const;
    void setLive(bool live);

    int offset() const;
    void setOffset(int offset);

    int limit() const;
    void setLimit(int limit);

    QString rootType() const;
    void setRootType(const QString &itemType);

    QVariant rootItem() const;
    void setRootItem(const QVariant &itemId);

    QGalleryQueryRequest::Scope scope() const;
    void setScope(QGalleryQueryRequest::Scope scope);

    QGalleryFilter filter() const;
    void setFilter(const QGalleryFilter &filter);

    void execute();
    void cancel();
    void clear();

    int result() const;
    QGalleryAbstractRequest::State state() const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant itemId(const QModelIndex &index) const;
    QUrl itemUrl(const QModelIndex &index) const;
    QString itemType(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setHeaderData(
            int section,
            Qt::Orientation orientation,
            const QVariant &value,
            int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

Q_SIGNALS:
    void succeeded();
    void cancelled();
    void failed(int result);
    void finished(int result);
    void stateChanged(QGalleryAbstractRequest::State state);
    void resultChanged();

private:
    QScopedPointer<QGalleryQueryModelPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGalleryQueryModel)
    Q_PRIVATE_SLOT(d_func(), void _q_resultSetChanged(QGalleryResultSet*))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsInserted(int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsRemoved(int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsMoved(int, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_metaDataChanged(int, int, const QList<int> &))
};

QTM_END_NAMESPACE

#endif
