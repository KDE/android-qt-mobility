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
#ifndef INTERFACETABWIDGET_H
#define INTERFACETABWIDGET_H

#include <qserviceinterfacedescriptor.h>

#include <QTabWidget>
#include <QByteArray>
#include <QXmlStreamWriter>


class InterfaceWidget;
class ErrorCollector;

QTM_BEGIN_NAMESPACE
class QServiceInterfaceDescriptor;
QTM_END_NAMESPACE

class InterfacesTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    InterfacesTabWidget(QWidget *parent = 0);
    void load(const QList<QTM_PREPEND_NAMESPACE(QServiceInterfaceDescriptor)> &descriptors);

    void validate(ErrorCollector *errors);
    void writeXml(QXmlStreamWriter *device) const;

signals:
    void dataChanged();

public slots:
    InterfaceWidget *addInterface(const QtMobility::QServiceInterfaceDescriptor &descriptor = QTM_PREPEND_NAMESPACE(QServiceInterfaceDescriptor)());

protected:
    virtual void tabInserted(int index);
    virtual void tabRemoved(int index);

private slots:
    void tabCloseRequested(int index);
    void interfaceTitleChanged(const QString &text);
};

#endif
