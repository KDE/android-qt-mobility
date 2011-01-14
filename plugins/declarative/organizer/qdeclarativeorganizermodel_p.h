/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEORGANIZERMODEL_H
#define QDECLARATIVEORGANIZERMODEL_H

#include <QAbstractListModel>
#include <QDeclarativeListProperty>
#include <QDeclarativeParserStatus>

#include "qorganizeritem.h"
#include "qdeclarativeorganizeritem_p.h"
#include "qversitreader.h"
#include "qversitwriter.h"
#include "qorganizercollectionid.h"
#include "qdeclarativeorganizeritemfetchhint_p.h"
#include "qdeclarativeorganizeritemsortorder_p.h"
#include "qdeclarativeorganizeritemfilter_p.h"

QTM_USE_NAMESPACE;
class QDeclarativeOrganizerModelPrivate;
class QDeclarativeOrganizerModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString manager READ manager WRITE setManager NOTIFY managerChanged)
    Q_PROPERTY(QString managerName READ managerName  NOTIFY managerChanged)
    Q_PROPERTY(QStringList availableManagers READ availableManagers)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QDateTime startPeriod READ startPeriod WRITE setStartPeriod NOTIFY startPeriodChanged)
    Q_PROPERTY(QDateTime endPeriod READ endPeriod WRITE setEndPeriod NOTIFY endPeriodChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemFilter* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemFetchHint* fetchHint READ fetchHint WRITE setFetchHint NOTIFY fetchHintChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> sortOrders READ sortOrders NOTIFY sortOrdersChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> items READ items NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> occurrences READ occurrences NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> events READ events NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> eventOccurrences READ eventOccurrences NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> todos READ todos NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> todoOccurrences READ todoOccurrences NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> journals READ journals NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItem> notes READ notes NOTIFY modelChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY modelChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    enum {
        OrganizerItemRole = Qt::UserRole + 500
    };


    explicit QDeclarativeOrganizerModel(QObject *parent = 0);
    explicit QDeclarativeOrganizerModel(QOrganizerManager* manager, const QDateTime& start, const QDateTime& end, QObject *parent = 0);

    QString error() const;
    int itemCount() const;

    QString manager() const;
    void setManager(const QString& managerUri);
    QString managerName() const;
    QStringList availableManagers() const;
    QDateTime startPeriod() const;
    void setStartPeriod(const QDateTime& start);

    QDateTime endPeriod() const;
    void setEndPeriod(const QDateTime& end);

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QDeclarativeOrganizerItemFilter* filter() const;

    QDeclarativeOrganizerItemFetchHint* fetchHint() const;


    QDeclarativeListProperty<QDeclarativeOrganizerItem> items() ;
    QDeclarativeListProperty<QDeclarativeOrganizerItem> occurrences();
    QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> sortOrders() ;
    QDeclarativeListProperty<QDeclarativeOrganizerItem> events();
    QDeclarativeListProperty<QDeclarativeOrganizerItem> eventOccurrences();
    QDeclarativeListProperty<QDeclarativeOrganizerItem> todos();
    QDeclarativeListProperty<QDeclarativeOrganizerItem> todoOccurrences();
    QDeclarativeListProperty<QDeclarativeOrganizerItem> journals();
    QDeclarativeListProperty<QDeclarativeOrganizerItem> notes();

    Q_INVOKABLE void removeItem(const QString& id);
    Q_INVOKABLE void removeItems(const QList<QString>& ids);
    Q_INVOKABLE void saveItem(QDeclarativeOrganizerItem* item);
    Q_INVOKABLE void fetchItems(const QList<QString>& ids);

    Q_INVOKABLE bool containsItems(QDateTime start, QDateTime end = QDateTime());
    Q_INVOKABLE QDeclarativeOrganizerItem* item(const QString& id);
    Q_INVOKABLE QStringList itemIds(QDateTime start=QDateTime(), QDateTime end = QDateTime());
    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);

    void setFilter(QDeclarativeOrganizerItemFilter* filter);
    void setFetchHint(QDeclarativeOrganizerItemFetchHint* fetchHint);


signals:
    void managerChanged();
    void availableManagersChanged();
    void filterChanged();
    void fetchHintChanged();
    void modelChanged();
    void sortOrdersChanged();
    void errorChanged();
    void startPeriodChanged();
    void endPeriodChanged();
    void autoUpdateChanged();

public slots:
    void update();
    void cancelUpdate();
    void exportItems(const QUrl& url, const QStringList& profiles=QStringList());
    void importItems(const QUrl& url, const QStringList& profiles=QStringList());
private slots:
    void fetchAgain();
    void requestUpdated();
    void doUpdate();

    void itemsSaved();

    void itemsRemoved();
    void itemsRemoved(const QList<QOrganizerItemId>& ids);
    void itemsChanged(const QList<QOrganizerItemId>& ids);
    void startImport(QVersitReader::State state);
    void itemsExported(QVersitWriter::State state);



private:
    void clearItems();
    int itemIndex(const QDeclarativeOrganizerItem* item);
    void addSorted(QDeclarativeOrganizerItem* item);
    void removeItemsFromModel(const QList<QString>& ids);
    bool itemHasReccurence(const QOrganizerItem& oi) const;
    void fetchOccurrences(const QOrganizerItem& item);
    QDeclarativeOrganizerItem* createItem(const QOrganizerItem& item);
    static void item_append(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p, QDeclarativeOrganizerItem *item);
    static int  item_count(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p);
    static QDeclarativeOrganizerItem * item_at(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p, int idx);
    static void  item_clear(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p);

    static void sortOrder_append(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p, QDeclarativeOrganizerItemSortOrder *sortOrder);
    static int  sortOrder_count(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p);
    static QDeclarativeOrganizerItemSortOrder * sortOrder_at(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p, int idx);
    static void  sortOrder_clear(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p);

    QDeclarativeOrganizerModelPrivate* d;
};

#endif // QDECLARATIVEORGANIZERMODEL_H
