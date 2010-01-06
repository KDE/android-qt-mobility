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

#include "qwmpvideooverlay.h"

#include "qwmpglobal.h"

QWmpVideoOverlay::QWmpVideoOverlay(IWMPPlayer4 *player, IOleObject *object, QWmpPlayerService *service)
    : m_service(service)
    , m_player(player)
    , m_object(object)
    , m_inPlaceObject(0)
    , m_aspectRatioMode(QVideoWidget::KeepAspectRatio)
    , m_enabled(false)
{
    HRESULT hr;

    if ((hr = m_object->QueryInterface(
            __uuidof(IOleInPlaceObject),
            reinterpret_cast<void **>(&m_inPlaceObject))) != S_OK) {
        qWarning("No IOleInPlaceObject interface, %x: %s", hr, qwmp_error_string(hr));
    }
}

QWmpVideoOverlay::~QWmpVideoOverlay()
{
    if (m_inPlaceObject)
        m_inPlaceObject->Release();
}

void QWmpVideoOverlay::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if (m_inPlaceObject && winId()) {
        QRect rect = displayRect();

        RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
        m_object->DoVerb(OLEIVERB_INPLACEACTIVATE, 0, m_service, 0, winId(), &rcPos);
    }
}

WId QWmpVideoOverlay::winId() const
{
    return m_winId;
}

void QWmpVideoOverlay::setWinId(WId id)
{
    if (m_inPlaceObject && m_enabled) {
        QRect rect = displayRect();

        RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
        m_object->DoVerb(OLEIVERB_INPLACEACTIVATE, 0, m_service, 0, id, &rcPos);
    }

    if (!id)
        m_enabled = false;

    m_winId = id;
}

extern HDC Q_GUI_EXPORT qt_win_display_dc();

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   ((HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   (((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH)

QRect QWmpVideoOverlay::displayRect() const
{
    return m_displayRect;
}

void QWmpVideoOverlay::setDisplayRect(const QRect &rect)
{
    if (m_inPlaceObject && m_enabled) {
        HDC gdc = QT_PREPEND_NAMESPACE(qt_win_display_dc)();

        SIZEL hmSize = {
                MAP_PIX_TO_LOGHIM(rect.width(), GetDeviceCaps(gdc, LOGPIXELSX)),
                MAP_PIX_TO_LOGHIM(rect.height(), GetDeviceCaps(gdc, LOGPIXELSY)) };

        m_object->SetExtent(DVASPECT_CONTENT, &hmSize);

        RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
        m_inPlaceObject->SetObjectRects(&rcPos, &rcPos);
    }

    m_displayRect = rect;
}

bool QWmpVideoOverlay::isFullScreen() const
{
    return m_fullScreen;
}

void QWmpVideoOverlay::setFullScreen(bool fullScreen)
{
    m_player->put_fullScreen(fullScreen);

    emit fullScreenChanged(m_fullScreen = fullScreen);
}

QSize QWmpVideoOverlay::nativeSize() const
{
    return m_sizeHint;
}

void QWmpVideoOverlay::setNativeSize(const QSize &size)
{    
    if (m_sizeHint != size) {
        m_sizeHint = size;

        emit nativeSizeChanged();
    }
}

QVideoWidget::AspectRatioMode QWmpVideoOverlay::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QWmpVideoOverlay::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
        switch (mode) {
        case QVideoWidget::KeepAspectRatio:
        m_player->put_stretchToFit(FALSE);

        m_aspectRatioMode = mode;
        break;
    case QVideoWidget::IgnoreAspectRatio:
        m_player->put_stretchToFit(TRUE);

        m_aspectRatioMode = mode;
        break;
    default:
        break;
    }
}

void QWmpVideoOverlay::repaint()
{
}

int QWmpVideoOverlay::brightness() const
{
    return 0;
}

void QWmpVideoOverlay::setBrightness(int)
{
}

int QWmpVideoOverlay::contrast() const
{
    return 0;
}

void QWmpVideoOverlay::setContrast(int)
{
}

int QWmpVideoOverlay::hue() const
{
    return 0;
}

void QWmpVideoOverlay::setHue(int)
{
}

int QWmpVideoOverlay::saturation() const
{
    return 0;
}

void QWmpVideoOverlay::setSaturation(int)
{
}

// IUnknown
HRESULT QWmpVideoOverlay::QueryInterface(REFIID riid, void **object)
{
    return m_service->QueryInterface(riid, object);
}

ULONG QWmpVideoOverlay::AddRef()
{
    return m_service->AddRef();
}

ULONG QWmpVideoOverlay::Release()
{
    return m_service->Release();
}

// IOleWindow
HRESULT QWmpVideoOverlay::GetWindow(HWND *phwnd)
{
    if (!phwnd) {
        return E_POINTER;
    } else if (!m_enabled) {
        *phwnd = 0;
        return E_UNEXPECTED;
    } else {
        *phwnd = winId();
        return S_OK;
    }
}

HRESULT QWmpVideoOverlay::ContextSensitiveHelp(BOOL fEnterMode)
{
    Q_UNUSED(fEnterMode);

    return E_NOTIMPL;
}

// IOleInPlaceSite
HRESULT QWmpVideoOverlay::CanInPlaceActivate()
{
    return S_OK;
}

HRESULT QWmpVideoOverlay::OnInPlaceActivate()
{
    return S_OK;
}

HRESULT QWmpVideoOverlay::OnUIActivate()
{
    return S_OK;
}

HRESULT QWmpVideoOverlay::GetWindowContext(
        IOleInPlaceFrame **ppFrame,
        IOleInPlaceUIWindow **ppDoc,
        LPRECT lprcPosRect,
        LPRECT lprcClipRect,
        LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    if (!ppFrame || !ppDoc || !lprcPosRect || !lprcClipRect || !lpFrameInfo)
        return E_POINTER;

    QueryInterface(IID_IOleInPlaceFrame, reinterpret_cast<void **>(ppFrame));
    QueryInterface(IID_IOleInPlaceUIWindow, reinterpret_cast<void **>(ppDoc));

    if (m_enabled) {
        QRect rect = displayRect();

        SetRect(lprcPosRect, rect.left(), rect.top(), rect.right(), rect.bottom());
        SetRect(lprcClipRect, rect.left(), rect.top(), rect.right(), rect.bottom());
    } else {
        SetRectEmpty(lprcPosRect);
        SetRectEmpty(lprcClipRect);
    }

    lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp = false;
    lpFrameInfo->haccel = 0;
    lpFrameInfo->cAccelEntries = 0;
    lpFrameInfo->hwndFrame = m_enabled ? winId() : 0;

    return S_OK;
}

HRESULT QWmpVideoOverlay::Scroll(SIZE scrollExtant)
{
    Q_UNUSED(scrollExtant);

    return S_FALSE;
}

HRESULT QWmpVideoOverlay::OnUIDeactivate(BOOL fUndoable)
{
    Q_UNUSED(fUndoable);

    return S_OK;
}

HRESULT QWmpVideoOverlay::OnInPlaceDeactivate()
{
    return S_OK;
}

HRESULT QWmpVideoOverlay::DiscardUndoState()
{
    return S_OK;
}

HRESULT QWmpVideoOverlay::DeactivateAndUndo()
{
    if (m_inPlaceObject)
        m_inPlaceObject->UIDeactivate();

    return S_OK;
}

HRESULT QWmpVideoOverlay::OnPosRectChange(LPCRECT lprcPosRect)
{
    Q_UNUSED(lprcPosRect);

    return S_OK;
}

// IOleInPlaceUIWindow
HRESULT QWmpVideoOverlay::GetBorder(LPRECT lprectBorder)
{
    Q_UNUSED(lprectBorder);

    return INPLACE_E_NOTOOLSPACE;
}

HRESULT QWmpVideoOverlay::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
    Q_UNUSED(pborderwidths);

    return INPLACE_E_NOTOOLSPACE;
}

HRESULT QWmpVideoOverlay::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
    Q_UNUSED(pborderwidths);

    return OLE_E_INVALIDRECT;
}

HRESULT QWmpVideoOverlay::SetActiveObject(
        IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
    Q_UNUSED(pActiveObject);
    Q_UNUSED(pszObjName);

    return  S_OK;
}

// IOleInPlaceFrame
HRESULT QWmpVideoOverlay::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
    Q_UNUSED(hmenuShared);
    Q_UNUSED(lpMenuWidths);

    return E_NOTIMPL;
}

HRESULT QWmpVideoOverlay::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
    Q_UNUSED(hmenuShared);
    Q_UNUSED(holemenu);
    Q_UNUSED(hwndActiveObject);

    return E_NOTIMPL;
}

HRESULT QWmpVideoOverlay::RemoveMenus(HMENU hmenuShared)
{
    Q_UNUSED(hmenuShared);

    return E_NOTIMPL;
}

HRESULT QWmpVideoOverlay::SetStatusText(LPCOLESTR pszStatusText)
{
    Q_UNUSED(pszStatusText);

    return E_NOTIMPL;
}

HRESULT QWmpVideoOverlay::EnableModeless(BOOL fEnable)
{
    Q_UNUSED(fEnable);

    return E_NOTIMPL;
}

HRESULT QWmpVideoOverlay::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
    return TranslateAccelerator(lpmsg, static_cast<DWORD>(wID));
}
