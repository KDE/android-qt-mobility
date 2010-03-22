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

#include "qevrvideooverlay.h"

#include <d3d9.h>
#include <wmp.h>

QEvrVideoOverlay::QEvrVideoOverlay(HINSTANCE evrHwnd)
    : m_ref(1)
    , m_evrHwnd(evrHwnd)
    , ptrMFCreateVideoPresenter(0)
    , m_presenter(0)
    , m_displayControl(0)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
    , m_winId(0)
    , m_fullScreen(0)
{
    ptrMFCreateVideoPresenter = reinterpret_cast<PtrMFCreateVideoPresenter>(
            GetProcAddress(m_evrHwnd, "MFCreateVideoPresenter"));
}

QEvrVideoOverlay::~QEvrVideoOverlay()
{
    FreeLibrary(m_evrHwnd);
}

WId QEvrVideoOverlay::winId() const
{
    return m_winId;
}

void QEvrVideoOverlay::setWinId(WId id)
{
    m_winId = id;

    if (m_displayControl) {
        m_displayControl->SetVideoWindow(id);
        m_displayControl->SetAspectRatioMode(m_aspectRatioMode == Qt::KeepAspectRatio
                ? MFVideoARMode_PreservePicture
                : MFVideoARMode_None);

        setDisplayRect(m_displayRect);
    }
}

QRect QEvrVideoOverlay::displayRect() const
{
    return m_displayRect;
}

void QEvrVideoOverlay::setDisplayRect(const QRect &rect)
{
    if (m_displayControl) {
        RECT displayRect = { rect.left(), rect.top(), rect.right(), rect.bottom() };

        if (m_aspectRatioMode == Qt::KeepAspectRatioByExpanding) {
            SIZE size;
            m_displayControl->GetNativeVideoSize(&size, 0);

            QSize clippedSize = rect.size();
            clippedSize.scale(size.cx, size.cy, Qt::KeepAspectRatio);

            long x = (size.cx - clippedSize.width()) / 2;
            long y = (size.cy - clippedSize.height()) / 2;

            MFVideoNormalizedRect sourceRect =
            { 
                float(x) / size.cx,
                float(y) / size.cy, 
                float(x + clippedSize.width()) / size.cx,
                float(y + clippedSize.height()) / size.cy
            }; 
            m_displayControl->SetVideoPosition(&sourceRect, &displayRect);
        } else {
            m_displayControl->SetVideoPosition(0, &displayRect);
        }
    }

    m_displayRect = rect;
}

bool QEvrVideoOverlay::isFullScreen() const
{
    return m_fullScreen;
}

void QEvrVideoOverlay::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen = fullScreen);
}

QSize QEvrVideoOverlay::nativeSize() const
{
    SIZE size;

    if (m_displayControl && m_displayControl->GetNativeVideoSize(&size, 0) == S_OK) {
        return QSize(size.cx, size.cy);
    } else {
        return QSize();
    }
}

Qt::AspectRatioMode QEvrVideoOverlay::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QEvrVideoOverlay::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;

    if (m_displayControl) {
        m_displayControl->SetAspectRatioMode(mode == Qt::KeepAspectRatio
                ? MFVideoARMode_PreservePicture
                : MFVideoARMode_None);
        setDisplayRect(m_displayRect);
    } 
}

void QEvrVideoOverlay::repaint()
{
    if (m_displayControl)
        m_displayControl->RepaintVideo();
}

int QEvrVideoOverlay::brightness() const
{
    return 0;
}

void QEvrVideoOverlay::setBrightness(int)
{
}

int QEvrVideoOverlay::contrast() const
{
    return 0;
}

void QEvrVideoOverlay::setContrast(int)
{
}

int QEvrVideoOverlay::hue() const
{
    return 0;
}

void QEvrVideoOverlay::setHue(int)
{
}

int QEvrVideoOverlay::saturation() const
{
    return 0;
}

void QEvrVideoOverlay::setSaturation(int)
{
}

void QEvrVideoOverlay::setDisplayControl(IMFVideoDisplayControl *control)
{
    if (m_displayControl)
        m_displayControl->Release();

    m_displayControl = control;

    if (m_displayControl) {
        m_displayControl->AddRef();
        m_displayControl->SetVideoWindow(m_winId);
        m_displayControl->SetAspectRatioMode(m_aspectRatioMode == Qt::KeepAspectRatio
                ? MFVideoARMode_PreservePicture
                : MFVideoARMode_None);

        setDisplayRect(m_displayRect);
    }
}

void QEvrVideoOverlay::openStateChanged(long state)
{
    if (state == wmposMediaOpen) {
        setDisplayRect(m_displayRect);

        emit nativeSizeChanged();
    }
};

// IUnknown
HRESULT QEvrVideoOverlay::QueryInterface(REFIID riid, void **object)
{
    if (!object) {
        return E_POINTER;
    } else if (riid == __uuidof(IUnknown)
            || riid == __uuidof(IMFAttributes)
            || riid == __uuidof(IMFActivate)) {
        *object = static_cast<IMFActivate *>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG QEvrVideoOverlay::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QEvrVideoOverlay::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

// IMFActivate
HRESULT QEvrVideoOverlay::ActivateObject(REFIID riid, void **ppv)
{
    if (riid != __uuidof(IMFVideoPresenter)) {
        return E_NOINTERFACE;
    } else if (!ptrMFCreateVideoPresenter) {
        return E_NOINTERFACE;
    } else if (m_presenter) {
        *ppv = m_presenter;

        return S_OK;
    } else {
        IMFVideoDisplayControl *displayControl = 0;

        IMFGetService *service;
        HRESULT hr;
        if ((hr = (*ptrMFCreateVideoPresenter)(
                0,
                __uuidof(IDirect3DDevice9),
                __uuidof(IMFVideoPresenter),
                reinterpret_cast<void **>(&m_presenter))) != S_OK) {
            qWarning("failed to create video presenter");
        } else if ((hr = m_presenter->QueryInterface(
                __uuidof(IMFGetService), reinterpret_cast<void **>(&service))) != S_OK) {
            qWarning("failed to query IMFGetService interface");
        } else {
            if ((hr = service->GetService(
                    MR_VIDEO_RENDER_SERVICE,
                    __uuidof(IMFVideoDisplayControl),
                    reinterpret_cast<void **>(&displayControl))) != S_OK) {
                qWarning("failed to get IMFVideoDisplayControl service");
            }
            service->Release();
        }

        setDisplayControl(displayControl);

        if (m_presenter && hr != S_OK) {
            m_presenter->Release();
            m_presenter = 0;
        }

        *ppv = m_presenter;

        return hr;
    }
}

HRESULT QEvrVideoOverlay::ShutdownObject()
{
    setDisplayControl(0);

    if (m_presenter) {
        m_presenter->Release();
        m_presenter = 0;
    }
    return S_OK;
}

HRESULT QEvrVideoOverlay::DetachObject()
{
    if (m_presenter) {
        m_presenter->Release();
        m_presenter = 0;
    }

    return S_OK;
}
