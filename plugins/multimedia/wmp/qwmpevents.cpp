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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwmpevents.h"

#include "qwmpglobal.h"

QWmpEvents::QWmpEvents(IUnknown *source, QObject *parent)
    : QObject(parent)
    , m_ref(1)
    , m_connectionPoint(0)
    , m_adviseCookie(0)
{
    HRESULT hr;
    IConnectionPointContainer *container = 0;

    if ((hr = source->QueryInterface(
            IID_IConnectionPointContainer, reinterpret_cast<void **>(&container))) != S_OK) {
        qWarning("No connection point container, %x: %d", hr, qwmp_error_string(hr));
    } else {
        if ((hr = container->FindConnectionPoint(
                __uuidof(IWMPEvents), &m_connectionPoint)) != S_OK) {
            qWarning("No connection point for IWMPEvents %d", hr);
        } else if ((hr = m_connectionPoint->Advise(
                static_cast<IWMPEvents3 *>(this), &m_adviseCookie)) != S_OK) {
            qWarning("Failed to link to connection point, %x, %s", hr, qwmp_error_string(hr));

            m_connectionPoint->Release();
            m_connectionPoint = 0;
        }
        container->Release();
    }
}

QWmpEvents::~QWmpEvents()
{
    if (m_connectionPoint) {
        m_connectionPoint->Unadvise(m_adviseCookie);
        m_connectionPoint->Release();
    }

    Q_ASSERT(m_ref == 1);
}

// IUnknown
HRESULT QWmpEvents::QueryInterface(REFIID riid, void **object)
{
    if (!object) {
        return E_POINTER;
    } else if (riid == __uuidof(IUnknown)
            || riid == __uuidof(IWMPEvents)
            || riid == __uuidof(IWMPEvents2)
            || riid == __uuidof(IWMPEvents3)) {
        *object = static_cast<IWMPEvents3 *>(this);

        AddRef();

        return S_OK;
    } else {
        return E_NOINTERFACE;
    }
}

ULONG QWmpEvents::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QWmpEvents::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}
