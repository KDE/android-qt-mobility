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

#include "qcontactactionfactory.h"

QContactActionFactory::~QContactActionFactory()
{
}

uint qHash(const QContactActionDescriptor& ad)
{
    return qHash(ad.actionName()) + qHash(ad.vendorName()) + ad.vendorVersion();
}

/*!
 * \fn QContactActionFactory::~QContactActionFactory()
 * Clears any memory in use by this factory
 */

/*!
 * \fn QContactActionFactory::name() const
 * Returns the name of this factory.  The name is used to identify the factory
 * when it is retrieved using the Qt Plugin framework.
 */

/*!
 * \fn QContactActionFactory::actionDescriptors() const
 * Returns a list of descriptors of the actions of which instances of their implementations are able to be retrieved
 * from this factory.
 */

/*!
 * \fn QContactActionFactory::instance(const ActionDescriptor& descriptor) const
 * Returns a pointer to an instance of the implementation of the action described by the given \a descriptor
 */

