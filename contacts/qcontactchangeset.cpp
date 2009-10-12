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


#include "qcontactchangeset.h"
#include "qcontactchangeset_p.h"
#include "qcontactmanagerengine.h"

/*!
 * \class QContactChangeSet
 *
 * \brief Provides a simple API to simplify the emission of state-change signals from QContactManagerEngine implementations.
 *
 * This class can be utilised by backend implementations to ensure correct emission of the \l QContactManagerEngine::dataChanged(),
 * \l QContactManagerEngine::contactsAdded(), \l QContactManagerEngine::contactsChanged() and \l QContactManagerEngine::contactsRemoved().
 *
 * \sa QContactManagerEngine
 */

/*!
 * Constructs a new change set
 */
QContactChangeSet::QContactChangeSet()
    : d(new QContactChangeSetData)
{
}

/*!
 * Constructs a copy of the \a other change set
 */
QContactChangeSet::QContactChangeSet(const QContactChangeSet& other)
    : d(other.d)
{
}

/*!
 * Frees the memory used by this change set
 */
QContactChangeSet::~QContactChangeSet()
{
}

/*!
 * Assigns this change set to be equal to \a other
 */
QContactChangeSet& QContactChangeSet::operator=(const QContactChangeSet& other)
{
    d = other.d;
    return *this;
}

/*!
 * Sets the data changed flag to \a dataChanged.  If this is set to true prior to calling \l emitSignals(),
 * only the \l QContactManagerEngine::dataChanged() signal will be emitted; otherwise, the appropriate
 * finer-grained signals will be emitted.
 */
void QContactChangeSet::setDataChanged(bool dataChanged)
{
    d->m_dataChanged = dataChanged;
}

/*!
 * Returns the value of the data changed flag
 */
bool QContactChangeSet::dataChanged()
{
    return d->m_dataChanged;
}

/*!
 * Returns a reference to the set of ids of contacts which have been added to
 * the database.
 */
QSet<QContactId>& QContactChangeSet::addedContacts()
{
    return d->m_addedContacts;
}

/*!
 * Returns a reference to the set of ids of contacts which have been changed in
 * the database.
 */
QSet<QContactId>& QContactChangeSet::changedContacts()
{
    return d->m_changedContacts;
}

/*!
 * Returns a reference to the set of ids of contacts which have been removed from
 * the database.
 */
QSet<QContactId>& QContactChangeSet::removedContacts()
{
    return d->m_removedContacts;
}

/*!
 * Clears all flags and sets of ids in this change set
 */
void QContactChangeSet::clear()
{
    d->m_dataChanged = false;
    d->m_addedContacts.clear();
    d->m_changedContacts.clear();
    d->m_removedContacts.clear();
}

/*!
 * Emits the appropriate signals from the given \a engine given the state of the change set
 */
void QContactChangeSet::emitSignals(QContactManagerEngine *engine)
{
    if (!engine)
        return;

    if (d->m_dataChanged) {
        emit engine->dataChanged();
    } else {
        if (!d->m_addedContacts.isEmpty())
            emit engine->contactsAdded(d->m_addedContacts.toList());
        if (!d->m_changedContacts.isEmpty())
            emit engine->contactsChanged(d->m_changedContacts.toList());
        if (!d->m_removedContacts.isEmpty())
            emit engine->contactsRemoved(d->m_removedContacts.toList());
    }
}
