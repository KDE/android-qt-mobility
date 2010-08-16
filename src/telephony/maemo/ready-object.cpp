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

#include "maemo/ready-object.h"
#include "maemo/readiness-helper.h"

namespace Tp
{

struct ReadyObject::Private
{
    Private(ReadyObject *parent, QObject *object,
            Feature featureCore);
    ~Private();

    ReadyObject *parent;
    Feature featureCore;
    ReadinessHelper *readinessHelper;
};

ReadyObject::Private::Private(ReadyObject *parent, QObject *object,
        Feature featureCore)
    : parent(parent),
      featureCore(featureCore),
      readinessHelper(new ReadinessHelper(object))
{
}

ReadyObject::Private::~Private()
{
    delete readinessHelper;
}

/**
 * \class ReadyObject
 * \ingroup clientreadiness
 * \headerfile TelepathyQt4/ready-object.h> "ReadyObject"
 */

/**
 * Construct a new ReadyObject object.
 *
 * \param object The QObject the object refers to.
 * \param featureCore The core feature of the object.
 */
ReadyObject::ReadyObject(QObject *object, const Feature &featureCore)
    : mPriv(new Private(this, object, featureCore))
{
}

/**
 * Class destructor.
 */
ReadyObject::~ReadyObject()
{
    delete mPriv;
}

/**
 * Return whether this object has finished its initial setup.
 *
 * This is mostly useful as a sanity check, in code that shouldn't be run
 * until the object is ready. To wait for the object to be ready, call
 * becomeReady() and connect to the finished signal on the result.
 *
 * \param features The features which should be tested
 * \return \c true if the object has finished its initial setup for basic
 *         functionality plus the given features
 */
bool ReadyObject::isReady(const Features &features) const
{
    if (features.isEmpty()) {
        return mPriv->readinessHelper->isReady(Features() << mPriv->featureCore);
    }
    return mPriv->readinessHelper->isReady(features);
}

/**
 * Return a pending operation which will succeed when this object finishes
 * its initial setup, or will fail if a fatal error occurs during this
 * initial setup.
 *
 * If an empty set is used FeatureCore will be considered as the requested
 * feature.
 *
 * \param requestedFeatures The features which should be enabled
 * \return A PendingReady object which will emit finished
 *         when this object has finished or failed initial setup for basic
 *         functionality plus the given features
 */
PendingReady *ReadyObject::becomeReady(const Features &requestedFeatures)
{
    if (requestedFeatures.isEmpty()) {
        return mPriv->readinessHelper->becomeReady(Features() << mPriv->featureCore);
    }
    return mPriv->readinessHelper->becomeReady(requestedFeatures);

}

Features ReadyObject::requestedFeatures() const
{
    return mPriv->readinessHelper->requestedFeatures();
}

Features ReadyObject::actualFeatures() const
{
    return mPriv->readinessHelper->actualFeatures();
}

Features ReadyObject::missingFeatures() const
{
    return mPriv->readinessHelper->missingFeatures();
}

ReadinessHelper *ReadyObject::readinessHelper() const
{
    return mPriv->readinessHelper;
}

} // Tp
