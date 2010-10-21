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

#ifndef QGALLERYTRACKERLISTCOLUMN_P_H
#define QGALLERYTRACKERLISTCOLUMN_P_H

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

#include <qmobilityglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>


QTM_BEGIN_NAMESPACE

class QM_AUTOTEST_EXPORT QGalleryTrackerValueColumn
{
public:
    virtual ~QGalleryTrackerValueColumn() {}

    virtual QVariant toVariant(const QString &string) const = 0;
    virtual QString toString(const QVariant &variant) const { return variant.toString(); }
};

class QM_AUTOTEST_EXPORT QGalleryTrackerCompositeColumn
{
public:
    virtual ~QGalleryTrackerCompositeColumn() {}

    virtual QVariant value(QVector<QVariant>::const_iterator row) const = 0;
};

class QM_AUTOTEST_EXPORT QGalleryTrackerImageColumn : public QObject
{
    Q_OBJECT
public:
    QGalleryTrackerImageColumn(QObject *parent = 0) : QObject(parent), m_offset(0) {}

    void moveOffset(int offset) { m_offset = offset; }

    QVariant image(int index) const { return m_images.at(index); }

    virtual void insertImages(
            int index, int count, QVector<QVariant>::const_iterator begin, int tableWidth) = 0;

    virtual void removeImages(int index, int count) = 0;

Q_SIGNALS:
    void imagesChanged(int index, int count, const QList<int> &keys);

protected:
    int m_offset;
    QVector<QVariant> m_images;
};

class QGalleryTrackerStringColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
};

class QGalleryTrackerStringListColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerStringListColumn()
        : m_separatorChar(QLatin1Char('|')), m_separatorString(QLatin1String("|")) {}
    QVariant toVariant(const QString &string) const;
    QString toString(const QVariant &variant) const;

private:
    const QChar m_separatorChar;
    const QString m_separatorString;
};

class QGalleryTrackerIntegerColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
};

class QGalleryTrackerDoubleColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
};

class QGalleryTrackerDateTimeColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
    QString toString(const QVariant &variant) const;
};

class QGalleryTrackerStaticColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerStaticColumn(const QVariant &value) : m_value(value) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const QVariant m_value;
};

class QGalleryTrackerPrefixColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerPrefixColumn(int column, const QString &prefix)
        : m_column(column), m_prefix(prefix) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_column;
    const QString m_prefix;
};

class QGalleryTrackerCompositeIdColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerCompositeIdColumn(const QVector<int> columns, const QString &prefix)
        : m_columns(columns), m_prefix(prefix) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const QVector<int> m_columns;
    const QString m_prefix;
};

class QGalleryTrackerFileUrlColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerFileUrlColumn(int column) : m_column(column) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

    static QGalleryTrackerCompositeColumn *create(const QVector<int> &);

private:
    const int m_column;
};

class QGalleryTrackerFilePathColumn : public QGalleryTrackerCompositeColumn
{
public:
    QVariant value(QVector<QVariant>::const_iterator row) const;

    static QGalleryTrackerCompositeColumn *create(const QVector<int> &columns);
};

QTM_END_NAMESPACE

#endif
