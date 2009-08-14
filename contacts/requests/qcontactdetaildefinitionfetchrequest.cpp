/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetaildefinitionfetchrequest.h"
#include "qcontactrequests_p.h"

/*!
 * \class QContactDetailDefinitionFetchRequest
 * \brief Allows a client to asynchronously request detail definitions from a contacts store manager
 */

/*!
 * \fn QContactDetailDefinitionFetchRequest::progress(QContactDetailDefinitionFetchRequest* self, bool appendOnly)
 * This signal is emitted when some progress has been made on the request, causing either a change of
 * status or an update of results, or both.  It identifies which request the signal originated from
 * by including a pointer to \a self, and contains an \a appendOnly flag which signifies whether or not the total
 * ordering of the results have been maintained since the last progress signal was emitted.
 */

/*! Constructs a new detail definition fetch request */
QContactDetailDefinitionFetchRequest::QContactDetailDefinitionFetchRequest()
    : QContactAbstractRequest(new QContactDetailDefinitionFetchRequestPrivate)
{
}

/*! Cleans up the memory in use by this detail definition fetch request */
QContactDetailDefinitionFetchRequest::~QContactDetailDefinitionFetchRequest()
{
}

Q_IMPLEMENT_CONTACTREQUEST_PRIVATE(QContactDetailDefinitionFetchRequest)

/*! Sets the names of the detail definitions to retrieve to \a names */
void QContactDetailDefinitionFetchRequest::setNames(const QStringList& names)
{
    Q_D(QContactDetailDefinitionFetchRequest);
    d->m_names = names;
}

/*! Returns the list of names of the detail definitions that will be retrieved */
QStringList QContactDetailDefinitionFetchRequest::names() const
{
    Q_D(const QContactDetailDefinitionFetchRequest);
    return d->m_names;
}

/*! Returns the map of detail definition names to detail definitions that was the result of the request */
QMap<QString, QContactDetailDefinition> QContactDetailDefinitionFetchRequest::definitions() const
{
    Q_D(const QContactDetailDefinitionFetchRequest);
    return d->m_definitions;
}
