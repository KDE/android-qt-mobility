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

#include <QtCore/qdebug.h>
#include <QWidget>
#include <QFile>
#include <QtMultimedia/qabstractvideobuffer.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#include "dscamerasession.h"
#include "dsvideorenderer.h"
#include "directshowglobal.h"

QT_BEGIN_NAMESPACE

// If frames come in quicker than we display them, we allow the queue to build
// up to this number before we start dropping them.
const int LIMIT_FRAME = 5;

namespace {
// DirectShow helper implementation
void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0) {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL) {
        // pUnk should not be used.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

} // end namespace

class SampleGrabberCallbackPrivate : public ISampleGrabberCB
{
public:
    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        if (NULL == ppvObject) 
            return E_POINTER;
        if (riid == IID_IUnknown /*__uuidof(IUnknown) */ ) {
            *ppvObject = static_cast<IUnknown*>(this);
            return S_OK;
        }
        if (riid == IID_ISampleGrabberCB /*__uuidof(ISampleGrabberCB)*/ ) {
            *ppvObject = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen)
    {
        if (!cs || active) {
            return S_OK;
        }

        if ((cs->StillMediaType.majortype != MEDIATYPE_Video) ||
                (cs->StillMediaType.formattype != FORMAT_VideoInfo) ||
                (cs->StillMediaType.cbFormat < sizeof(VIDEOINFOHEADER))) {
            return VFW_E_INVALIDMEDIATYPE;
        }

        active = true;

        if(toggle == true) {
            toggle = false;
        }
        else {
            toggle = true;
        }

        if(toggle) {
            active = false;
            return S_OK;
        }

        bool check = false;
        cs->mutex.lock();

        if (cs->frames.size() > LIMIT_FRAME) {
            check = true;
        }

        if (check) {
            cs->mutex.unlock();
            // Frames building up. We're going to drop some here
            Sleep(100);
            active = false;
            return S_OK;
        }
        cs->mutex.unlock();

        unsigned char* vidData = new unsigned char[BufferLen];
        memcpy(vidData, pBuffer, BufferLen);

        cs->mutex.lock();

        video_buffer* buf = new video_buffer;
        buf->buffer = vidData;
        buf->length = BufferLen;
        buf->time   = (qint64)Time;

        cs->frames.append(buf);

        cs->mutex.unlock();

        QMetaObject::invokeMethod(cs, "captureFrame", Qt::QueuedConnection);

        active = false;

        return S_OK;
    }

    DSCameraSession* cs;
    bool active;
    bool toggle;
};


DSCameraSession::DSCameraSession(QObject *parent)
    : QObject(parent)
      ,m_currentImageId(0)
{
    pBuild = NULL;
    pGraph = NULL;
    pCap = NULL;
    pSG_Filter = NULL;
    pSG = NULL;

    opened = false;
    available = false;
    resolutions.clear();
    m_state = QCamera::UnloadedState;
    m_device = "default";

    StillCapCB = new SampleGrabberCallbackPrivate;
    StillCapCB->cs = this;
    StillCapCB->active = false;
    StillCapCB->toggle = false;

    m_output = 0;
    m_surface = 0;
    m_windowSize = QSize(320,240);
    pixelF = QVideoFrame::Format_RGB24;
    actualFormat = QVideoSurfaceFormat(m_windowSize,pixelF);

    graph = false;
    active = false;

    ::CoInitialize(NULL);
}

DSCameraSession::~DSCameraSession()
{
    if (opened) {
        closeStream();
    }

    CoUninitialize();

    SAFE_RELEASE(pCap);
    SAFE_RELEASE(pSG_Filter);
    SAFE_RELEASE(pGraph);
    SAFE_RELEASE(pBuild);

    if (StillCapCB) {
        delete StillCapCB;
    }
}

int DSCameraSession::captureImage(const QString &fileName)
{
    emit readyForCaptureChanged(false);

    // We're going to do this in one big synchronous call
    m_currentImageId++;
    if (fileName.isEmpty()) {
        m_snapshot = "img.jpg";
    } else {
        m_snapshot = fileName;
    }

    if (!active) {
        startStream();
    }

    return m_currentImageId;
}

void DSCameraSession::setSurface(QAbstractVideoSurface* surface)
{
    m_surface = surface;
}

bool DSCameraSession::deviceReady()
{
    return available;
}

bool DSCameraSession::pictureInProgress()
{
    return m_snapshot.isEmpty();
}

int DSCameraSession::framerate() const
{
    return -1;
}

void DSCameraSession::setFrameRate(int rate)
{
    Q_UNUSED(rate)
}

int DSCameraSession::brightness() const
{
    return -1;
}

void DSCameraSession::setBrightness(int b)
{
    Q_UNUSED(b)
}

int DSCameraSession::contrast() const
{
    return -1;
}

void DSCameraSession::setContrast(int c)
{
    Q_UNUSED(c)
}

int DSCameraSession::saturation() const
{
    return -1;
}

void DSCameraSession::setSaturation(int s)
{
    Q_UNUSED(s)
}

int DSCameraSession::hue() const
{
    return -1;
}

void DSCameraSession::setHue(int h)
{
    Q_UNUSED(h)
}

int DSCameraSession::sharpness() const
{
    return -1;
}

void DSCameraSession::setSharpness(int s)
{
    Q_UNUSED(s)
}

int DSCameraSession::zoom() const
{
    return -1;
}

void DSCameraSession::setZoom(int z)
{
    Q_UNUSED(z)
}

bool DSCameraSession::backlightCompensation() const
{
    return false;
}

void DSCameraSession::setBacklightCompensation(bool b)
{
    Q_UNUSED(b)
}

int DSCameraSession::whitelevel() const
{
    return -1;
}

void DSCameraSession::setWhitelevel(int w)
{
    Q_UNUSED(w)
}

int DSCameraSession::rotation() const
{
    return 0;
}

void DSCameraSession::setRotation(int r)
{
    Q_UNUSED(r)
}

bool DSCameraSession::flash() const
{
    return false;
}

void DSCameraSession::setFlash(bool f)
{
    Q_UNUSED(f)
}

bool DSCameraSession::autofocus() const
{
    return false;
}

void DSCameraSession::setAutofocus(bool f)
{
    Q_UNUSED(f)
}

QSize DSCameraSession::frameSize() const
{
    return m_windowSize;
}

void DSCameraSession::setFrameSize(const QSize& s)
{
	if (supportedResolutions(pixelF).contains(s)) 
        m_windowSize = s;
    else 
        qWarning() << "frame size if not supported for current pixel format, no change";
}

void DSCameraSession::setDevice(const QString &device)
{
    if(opened)
        stopStream();

    if(graph) {
        SAFE_RELEASE(pCap);
        SAFE_RELEASE(pSG_Filter);
        SAFE_RELEASE(pGraph);
        SAFE_RELEASE(pBuild);
    }

    available = false;
    m_state = QCamera::LoadedState;

    CoInitialize(NULL);

    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;

    // Create the System device enumerator
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                  CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                  reinterpret_cast<void**>(&pDevEnum));
    if(SUCCEEDED(hr)) {
        // Create the enumerator for the video capture category
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        if (S_OK == hr) {
            pEnum->Reset();
            // go through and find all video capture devices
            IMoniker* pMoniker = NULL;
            while(pEnum->Next(1, &pMoniker, NULL) == S_OK) {
                IPropertyBag *pPropBag;
                hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                                             (void**)(&pPropBag));
                if(FAILED(hr)) {
                    pMoniker->Release();
                    continue; // skip this one
                }
                // Find the description
                WCHAR str[120];
                VARIANT varName;
                varName.vt = VT_BSTR;
                hr = pPropBag->Read(L"Description", &varName, 0);
                if(FAILED(hr))
                    hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                if(SUCCEEDED(hr)) {
                    wcsncpy(str, varName.bstrVal, sizeof(str)/sizeof(str[0]));
                    QString temp(QString::fromUtf16((unsigned short*)str));
                    if(temp.contains(device)) {
                        available = true;
                    }
                }
                pPropBag->Release();
                pMoniker->Release();
            }
            pEnum->Release();
        }
        pDevEnum->Release();
    }
    CoUninitialize();

    if(available) {
        m_device = QByteArray(device.toLocal8Bit().constData());
        graph = createFilterGraph();
        if(!graph)
            available = false;
    }
}

QList<QVideoFrame::PixelFormat> DSCameraSession::supportedPixelFormats()
{
    return types;
}

QVideoFrame::PixelFormat DSCameraSession::pixelFormat() const
{
    return pixelF;
}

void DSCameraSession::setPixelFormat(QVideoFrame::PixelFormat fmt)
{
    pixelF = fmt;
}

QList<QSize> DSCameraSession::supportedResolutions(QVideoFrame::PixelFormat format)
{
    if (!resolutions.contains(format)) 
		return QList<QSize>();
    return resolutions.value(format);
}

bool DSCameraSession::setOutputLocation(const QUrl &sink)
{
    m_sink = sink;

    return true;
}

QUrl DSCameraSession::outputLocation() const
{
    return m_sink;
}

qint64 DSCameraSession::position() const
{
    return timeStamp.elapsed();
}

int DSCameraSession::state() const
{
    return int(m_state);
}

void DSCameraSession::record()
{
    if(opened) {
        return;
    }

    if(m_surface) {
        bool match = false;

        if (!m_surface->isFormatSupported(actualFormat)) {
            QList<QVideoFrame::PixelFormat> fmts;
            foreach(QVideoFrame::PixelFormat f, types) {
                if (fmts.contains(f)) {
                    match = true;
                    pixelF = f;
                    actualFormat = QVideoSurfaceFormat(m_windowSize,pixelF);
                    break;
                }
            }
        }
        if (!m_surface->isFormatSupported(actualFormat) && !match) {
            // fallback
            if (types.contains(QVideoFrame::Format_RGB24)) {
                // get RGB24 from camera and convert to RGB32 for surface!
                pixelF = QVideoFrame::Format_RGB32;
                actualFormat = QVideoSurfaceFormat(m_windowSize,pixelF);
            }
        }

        if (m_surface->isFormatSupported(actualFormat)) {
            m_surface->start(actualFormat);
            m_state = QCamera::ActiveState;
            emit stateChanged(QCamera::ActiveState);
        } else {
            qWarning() << "surface doesn't support camera format, cant start";
            m_state = QCamera::LoadedState;
            emit stateChanged(QCamera::LoadedState);
            return;
        }
    } else {
        qWarning() << "no video surface, cant start";
        m_state = QCamera::LoadedState;
        emit stateChanged(QCamera::LoadedState);
        return;
    }

    opened = startStream();

    if (!opened) {
        qWarning() << "Stream did not open";
        m_state = QCamera::LoadedState;
        emit stateChanged(QCamera::LoadedState);
    }
}

void DSCameraSession::pause()
{
    suspendStream();
}

void DSCameraSession::stop()
{
    if(!opened) {
        return;
    }

    stopStream();
    opened = false;
    m_state = QCamera::LoadedState;
    emit stateChanged(QCamera::LoadedState);
}

void DSCameraSession::captureFrame()
{
    if(m_surface && frames.count() > 0) {

        QImage image;

        if(pixelF == QVideoFrame::Format_RGB24) {

            mutex.lock();

            image = QImage(frames.at(0)->buffer,m_windowSize.width(),m_windowSize.height(),
                    QImage::Format_RGB888).rgbSwapped().mirrored(true);

            QVideoFrame frame(image);
            frame.setStartTime(frames.at(0)->time);

            mutex.unlock();

            m_surface->present(frame);

        } else if (pixelF == QVideoFrame::Format_RGB32) {

            mutex.lock();

            image = QImage(frames.at(0)->buffer,m_windowSize.width(),m_windowSize.height(),
                    QImage::Format_RGB888).rgbSwapped().mirrored(true);

            QVideoFrame frame(image.convertToFormat(QImage::Format_RGB32));
            frame.setStartTime(frames.at(0)->time);

            mutex.unlock();

            m_surface->present(frame);

        } else {
            qWarning() << "TODO:captureFrame() format =" << pixelF;
        }

        if (m_snapshot.length() > 0) {
            emit imageCaptured(m_currentImageId, image);
            image.save(m_snapshot,"JPG");
            emit imageSaved(m_currentImageId, m_snapshot);
            m_snapshot.clear();
            emit readyForCaptureChanged(true);
        }

        mutex.lock();
        if (frames.isEmpty()) {
            qWarning() << "Frames over-run";
        }

        video_buffer* buf = frames.takeFirst();
        delete buf->buffer;
        delete buf;
        mutex.unlock();
    }
}

HRESULT DSCameraSession::getPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    *ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) {
        return hr;
    }

    pEnum->Reset();
    while(pEnum->Next(1, &pPin, NULL) == S_OK) {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if(ThisPinDir == PinDir) {
            pEnum->Release();
            *ppPin = pPin;
            return S_OK;
        }
        pEnum->Release();
    }
    pEnum->Release();
    return E_FAIL;
}

bool DSCameraSession::createFilterGraph()
{
    HRESULT hr;
    IMoniker* pMoniker = NULL;
    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;

    CoInitialize(NULL);

    // Create the filter graph
    hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC,
            IID_IGraphBuilder, (void**)&pGraph);
    if (FAILED(hr)) {
        qWarning()<<"failed to create filter graph";
        return false;
    }

    // Create the capture graph builder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
                          IID_ICaptureGraphBuilder2, (void**)&pBuild);
    if (FAILED(hr)) {
        qWarning()<<"failed to create graph builder";
        return false;
    }

    // Attach the filter graph to the capture graph
    hr = pBuild->SetFiltergraph(pGraph);
    if (FAILED(hr)) {
        qWarning()<<"failed to connect capture graph and filter graph";
        return false;
    }

    // Find the Capture device
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                          reinterpret_cast<void**>(&pDevEnum));
    if (SUCCEEDED(hr)) {
        // Create an enumerator for the video capture category
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        pDevEnum->Release();
        if (S_OK == hr) {
            pEnum->Reset();
            //go through and find all video capture devices
            while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
                IPropertyBag *pPropBag;
                hr = pMoniker->BindToStorage(0, 0,
                                             IID_IPropertyBag, (void**)(&pPropBag));
                if(FAILED(hr)) {
                    pMoniker->Release();
                    continue; // skip this one
                }
                // Find the description
                WCHAR str[120];
                VARIANT varName;
                varName.vt = VT_BSTR;
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                if (SUCCEEDED(hr)) {
                    // check if it is the selected device
                    wcsncpy(str, varName.bstrVal, sizeof(str)/sizeof(str[0]));
                    QString output = QString::fromUtf16((unsigned short*)str);
                    if (m_device.contains(output.toLocal8Bit().constData())) {
                        hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap);
                        if (SUCCEEDED(hr)) {
                            pPropBag->Release();
                            pMoniker->Release();
                            break;
                        }
                    }
                }
                pPropBag->Release();
                pMoniker->Release();
            }
            if (NULL == pCap)
            {
                if (m_device.contains("default"))
                {
                    pEnum->Reset();
                    // still have to loop to discard bind to storage failure case
                    while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
                        IPropertyBag *pPropBag = 0;

                        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
                        if (FAILED(hr)) {
                            pMoniker->Release();
                            continue; // Don't panic yet
                        }

                        // No need to get the description, just grab it

                        hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap);
                        pPropBag->Release();
                        pMoniker->Release();
                        if (SUCCEEDED(hr)) {
                            break; // done, stop looping through
                        }
                        else
                        {
                            qWarning() << "Object bind failed";
                        }
                    }
                }
            }
            pEnum->Release();
        }
    }

    // Sample grabber filter
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL,CLSCTX_INPROC,
                          IID_IBaseFilter, (void**)&pSG_Filter);
    if (FAILED(hr)) {
        qWarning() << "failed to create sample grabber";
        return false;
    }

    pSG_Filter->QueryInterface(IID_ISampleGrabber, (void**)&pSG);
    if (FAILED(hr)) {
        qWarning() << "failed to get sample grabber";
        return false;
    }
    pSG->SetOneShot(FALSE);
    pSG->SetBufferSamples(TRUE);
    pSG->SetCallback(StillCapCB, 1);

    CoUninitialize();

    return true;
}

void DSCameraSession::updateProperties()
{
    HRESULT hr;
    AM_MEDIA_TYPE *pmt = NULL;
    VIDEOINFOHEADER *pvi = NULL;
    VIDEO_STREAM_CONFIG_CAPS scc;
    IAMStreamConfig* pConfig = 0;

    hr = pBuild->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,pCap,
                               IID_IAMStreamConfig, (void**)&pConfig);
    if (FAILED(hr)) {
        qWarning()<<"failed to get config on capture device";
        return;
    }

    int iCount;
    int iSize;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (FAILED(hr)) {
        qWarning()<<"failed to get capabilities";
        return;
    }

    QList<QSize> sizes;
    QVideoFrame::PixelFormat f = QVideoFrame::Format_Invalid;

    types.clear();
    resolutions.clear();

    for (int iIndex = 0; iIndex < iCount; iIndex++) {
        hr = pConfig->GetStreamCaps(iIndex, &pmt, reinterpret_cast<BYTE*>(&scc));
        if (hr == S_OK) {
            pvi = (VIDEOINFOHEADER*)pmt->pbFormat;
            if ((pmt->majortype == MEDIATYPE_Video) &&
                    (pmt->formattype == FORMAT_VideoInfo)) {
                // Add types
                if (pmt->subtype == MEDIASUBTYPE_RGB24) {
                    if (!types.contains(QVideoFrame::Format_RGB24)) {
                        types.append(QVideoFrame::Format_RGB24);
                        f = QVideoFrame::Format_RGB24;
                    }
                } else if (pmt->subtype == MEDIASUBTYPE_RGB32) {
                    if (!types.contains(QVideoFrame::Format_RGB32)) {
                        types.append(QVideoFrame::Format_RGB32);
                        f = QVideoFrame::Format_RGB32;
                    }
                } else if (pmt->subtype == MEDIASUBTYPE_YUY2) {
                    if (!types.contains(QVideoFrame::Format_YUYV)) {
                        types.append(QVideoFrame::Format_YUYV);
                        f = QVideoFrame::Format_YUYV;
                    }
                } else if (pmt->subtype == MEDIASUBTYPE_MJPG) {
                } else if (pmt->subtype == MEDIASUBTYPE_I420) {
                    if (!types.contains(QVideoFrame::Format_YUV420P)) {
                        types.append(QVideoFrame::Format_YUV420P);
                        f = QVideoFrame::Format_YUV420P;
                    }
                } else if (pmt->subtype == MEDIASUBTYPE_RGB555) {
                    if (!types.contains(QVideoFrame::Format_RGB555)) {
                        types.append(QVideoFrame::Format_RGB555);
                        f = QVideoFrame::Format_RGB555;
                    }
                } else if (pmt->subtype == MEDIASUBTYPE_YVU9) {
                } else if (pmt->subtype == MEDIASUBTYPE_UYVY) {
                    if (!types.contains(QVideoFrame::Format_UYVY)) {
                        types.append(QVideoFrame::Format_UYVY);
                        f = QVideoFrame::Format_UYVY;
                    }
                } else {
                    qWarning() << "UNKNOWN FORMAT: " << pmt->subtype.Data1;
                }
                // Add resolutions
                QSize res(pvi->bmiHeader.biWidth, pvi->bmiHeader.biHeight);
                if (!resolutions.contains(f)) {
                    sizes.clear();
                    resolutions.insert(f,sizes);
                }
                resolutions[f].append(res);
            }
        }
    }
    pConfig->Release();
}

bool DSCameraSession::setProperties()
{
    CoInitialize(NULL);

    HRESULT hr;
    AM_MEDIA_TYPE am_media_type;
    AM_MEDIA_TYPE *pmt = NULL;
    VIDEOINFOHEADER *pvi = NULL;
    VIDEO_STREAM_CONFIG_CAPS scc;

    IAMStreamConfig* pConfig = 0;
    hr = pBuild->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCap,
                               IID_IAMStreamConfig, (void**)&pConfig);
    if(FAILED(hr)) {
        qWarning()<<"failed to get config on capture device";
        return false;
    }

    int iCount;
    int iSize;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if(FAILED(hr)) {
        qWarning()<<"failed to get capabilities";
        return false;
    }

    bool setFormatOK = false;
    for (int iIndex = 0; iIndex < iCount; iIndex++) {
        hr = pConfig->GetStreamCaps(iIndex, &pmt, reinterpret_cast<BYTE*>(&scc));
        if (hr == S_OK) {
            pvi = (VIDEOINFOHEADER*)pmt->pbFormat;

            if ((pmt->majortype == MEDIATYPE_Video) &&
                (pmt->formattype == FORMAT_VideoInfo)) {
                if ((actualFormat.frameWidth() == pvi->bmiHeader.biWidth) &&
                    (actualFormat.frameHeight() == pvi->bmiHeader.biHeight)) {
                    hr = pConfig->SetFormat(pmt);
                    _FreeMediaType(*pmt);
                    if(FAILED(hr)) {
                        qWarning()<<"failed to set format:" << hr;
                        qWarning()<<"but going to continue";
                        continue; // We going to continue
                    } else {
                        setFormatOK = true;
                        break;
                    }
                }
            }
        }
    }
    pConfig->Release();

    if (!setFormatOK) {
            qWarning() << "unable to set any format for camera";
            return false;
    }

    // Set Sample Grabber config to match capture
    ZeroMemory(&am_media_type, sizeof(am_media_type));
    am_media_type.majortype = MEDIATYPE_Video;

    if (actualFormat.pixelFormat() == QVideoFrame::Format_RGB32)
        am_media_type.subtype = MEDIASUBTYPE_RGB24;
    else if (actualFormat.pixelFormat() == QVideoFrame::Format_RGB24)
        am_media_type.subtype = MEDIASUBTYPE_RGB24;
    else if (actualFormat.pixelFormat() == QVideoFrame::Format_YUYV)
        am_media_type.subtype = MEDIASUBTYPE_YUY2;
    else if (actualFormat.pixelFormat() == QVideoFrame::Format_YUV420P)
        am_media_type.subtype = MEDIASUBTYPE_I420;
    else if (actualFormat.pixelFormat() == QVideoFrame::Format_RGB555)
        am_media_type.subtype = MEDIASUBTYPE_RGB555;
    else if (actualFormat.pixelFormat() == QVideoFrame::Format_UYVY)
        am_media_type.subtype = MEDIASUBTYPE_UYVY;
    else {
        qWarning()<<"unknown format? for SG";
        return false;
    }

    am_media_type.formattype = FORMAT_VideoInfo;
    hr = pSG->SetMediaType(&am_media_type);
    if (FAILED(hr)) {
        qWarning()<<"failed to set video format on grabber";
        return false;
    }

    pSG->GetConnectedMediaType(&StillMediaType);

    CoUninitialize();

    return true;
}

bool DSCameraSession::openStream()
{
    //Opens the stream for reading and allocates any necessary resources needed
    //Return true if success, false otherwise

    if (opened) {
        return true;
    }

    if (!graph) {
        graph = createFilterGraph();
        if(!graph) {
            qWarning()<<"failed to create filter graph in openStream";
            return false;
        }
    }

    CoInitialize(NULL);

    HRESULT hr;

    hr = pGraph->AddFilter(pCap, L"Capture Filter");
    if (FAILED(hr)) {
        qWarning()<<"failed to create capture filter";
        return false;
    }

    hr = pGraph->AddFilter(pSG_Filter, L"Sample Grabber");
    if (FAILED(hr)) {
        qWarning()<<"failed to add sample grabber";
        return false;
    }

    hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                              pCap, NULL, pSG_Filter);
    if (FAILED(hr)) {
        qWarning() << "failed to renderstream" << hr;
        return false;
    }
    pSG->GetConnectedMediaType(&StillMediaType);
    pSG_Filter->Release();

    CoUninitialize();

    return true;
}

void DSCameraSession::closeStream()
{
    // Closes the stream and internally frees any resources used
    HRESULT hr;
    IMediaControl* pControl = 0;

    hr = pGraph->QueryInterface(IID_IMediaControl,(void**)&pControl);
    if (FAILED(hr)) {
        qWarning()<<"failed to get stream control";
        return;
    }

    hr = pControl->StopWhenReady();
    if (FAILED(hr)) {
        qWarning()<<"failed to stop";
        pControl->Release();
        return;
    }

    pControl->Release();

    opened = false;
    IPin *pPin = 0;

    if (pCap)
    {
        hr = getPin(pCap, PINDIR_OUTPUT, &pPin);
        if(FAILED(hr)) {
            qWarning()<<"failed to disconnect capture filter";
            return;
        }
    }

    pGraph->Disconnect(pPin);
    if (FAILED(hr)) {
        qWarning()<<"failed to disconnect grabber filter";
        return;
    }

    hr = getPin(pSG_Filter,PINDIR_INPUT,&pPin);
    pGraph->Disconnect(pPin);
    pGraph->RemoveFilter(pSG_Filter);
    pGraph->RemoveFilter(pCap);

    SAFE_RELEASE(pCap);
    SAFE_RELEASE(pSG_Filter);
    SAFE_RELEASE(pGraph);
    SAFE_RELEASE(pBuild);

    graph = false;
}

bool DSCameraSession::startStream()
{
    // Starts the stream, by emitting either QVideoPackets
    // or QvideoFrames, depending on Format chosen
    if (!graph)
        graph = createFilterGraph();

    if (!setProperties()) {
        qWarning() << "Couldn't set properties (retrying)";
        closeStream();
        if (!openStream()) {
            qWarning() << "Retry to open strean failed";
            return false;
        }
    }

    if (!opened) {
        opened = openStream();
        if (!opened) {
            qWarning() << "failed to openStream()";
            return false;
        }
    }

    HRESULT hr;
    IMediaControl* pControl = 0;

    hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if (FAILED(hr)) {
        qWarning() << "failed to get stream control";
        return false;
    }

    hr = pControl->Run();
    pControl->Release();

    if (FAILED(hr)) {
        qWarning() << "failed to start";
        return false;
    }
    active = true;
    return true;
}

void DSCameraSession::stopStream()
{
    // Stops the stream from emitting packets
    HRESULT hr;

    IMediaControl* pControl = 0;
    hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if (FAILED(hr)) {
        qWarning() << "failed to get stream control";
        return;
    }

    hr = pControl->Stop();
    pControl->Release();
    if (FAILED(hr)) {
        qWarning() << "failed to stop";
        return;
    }
    active = false;

    if (opened) {
        closeStream();
    }
}

void DSCameraSession::suspendStream()
{
    // Pauses the stream
    HRESULT hr;

    IMediaControl* pControl = 0;
    hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if (FAILED(hr)) {
        qWarning() << "failed to get stream control";
        return;
    }

    hr = pControl->Pause();
    pControl->Release();
    if (FAILED(hr)) {
        qWarning() << "failed to pause";
        return;
    }

    active = false;
}

void DSCameraSession::resumeStream()
{
    // Resumes a paused stream
    startStream();
}

QT_END_NAMESPACE

