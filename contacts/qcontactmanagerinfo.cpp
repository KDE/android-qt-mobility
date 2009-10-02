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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactmanagerinfo.h"

#include "qcontactmanager_p.h"
#include "qcontactfilter.h"

/*!
  \class QContactManagerInfo
 
  \brief The QContactManagerInfo class provides information about a particular QContactManager.
 
  This class allows managers to report information about their supported functionality to clients.
  The supported functionality of a manager includes which data types the backend supports,
  which definitions are natively filterable in the backend (and thus are high performance),
  and what capabilities the manager supports.
 
  If the manager that provided an information object is subsequently destroyed, the
  returned QContactManagerInfo object will be invalid.
 
  \sa QContactManager
 */

/*!
 * \enum QContactManagerInfo::ManagerFeature
 * This enum describes the possible features that a particular manager may support
 * \value Groups The manager supports all QContactGroup related operations, and emits the appropriate signals
 * \value ActionPreferences The manager supports saving preferred details per action per contact
 * \value MutableDefinitions The manager supports saving, updating or removing detail definitions.  Some built-in definitions may still be immutable
 * \value Anonymous The manager is isolated from other managers
 */

/*! Frees the memory used by this instance */
QContactManagerInfo::~QContactManagerInfo()
{
}

/*! \internal */
QContactManagerInfo::QContactManagerInfo()
{
}

/*!
 * Returns true if the given \a feature is supported by the manager from whom this information object was received.
 */
bool QContactManagerInfo::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    return d->m_engine->hasFeature(feature);
}

/*!
 * Returns the list of data types supported by the manager
 * associated with this object.
 */
QList<QVariant::Type> QContactManagerInfo::supportedDataTypes() const
{
    return d->m_engine->supportedDataTypes();
}

/*!
 * Returns true if the given \a filter is supported natively by the
 * manager, and false if the filter behaviour would be emulated.
 *
 * Note: In some cases, the behaviour of an unsupported filter
 * cannot be emulated.  For example, a filter that requests contacts
 * that have changed since a given time depends on having that information
 * available.  In these cases, the filter will fail.
 */
bool QContactManagerInfo::filterSupported(const QContactFilter& filter) const
{
    return d->m_engine->filterSupported(filter);
}
