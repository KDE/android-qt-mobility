/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2009 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "maemo/pending-ready.h"

namespace Tp
{

struct PendingReady::Private
{
    Private(const Features &requestedFeatures, QObject *object) :
        requestedFeatures(requestedFeatures),
        object(object)
    {
    }

    Features requestedFeatures;
    QObject *object;
};

/**
 * \class PendingReady
 * \headerfile TelepathyQt4/pending-ready.h "PendingReady"
 *
 * \brief Class containing the features requested and the reply to a request
 * for an object to become ready. Instances of this class cannot be
 * constructed directly; the only way to get one is via Object::becomeReady().
 */

/**
 * Construct a PendingReady object.
 *
 * \param object The object that will become ready.
 */
PendingReady::PendingReady(const Features &requestedFeatures,
        QObject *object, QObject *parent)
    : PendingOperation(parent),
      mPriv(new Private(requestedFeatures, object))
{
}

/**
 * Class destructor.
 */
PendingReady::~PendingReady()
{
    delete mPriv;
}

/**
 * Return the object through which the request was made.
 *
 * \return The object through which the request was made.
 */
QObject *PendingReady::object() const
{
    return mPriv->object;
}

/**
 * Return the Features that were requested to become ready on the
 * object.
 *
 * \return Features The requested features
 */
Features PendingReady::requestedFeatures() const
{
    return mPriv->requestedFeatures;
}

} // Tp
