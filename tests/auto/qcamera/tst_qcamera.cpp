/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDebug>

#include <multimedia/qcameracontrol.h>
#include <multimedia/qcameraexposurecontrol.h>
#include <multimedia/qcamerafocuscontrol.h>
#include <multimedia/qimagecapturecontrol.h>
#include <multimedia/qimageencodercontrol.h>
#include <multimedia/qimageprocessingcontrol.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qcamera.h>


class MockCameraControl : public QCameraControl
{
    Q_OBJECT
public:
    MockCameraControl(QObject *parent = 0):
            QCameraControl(parent),
            m_state(QCamera::StoppedState)
    {
    }

    ~MockCameraControl() {}

    void start() { m_state = QCamera::ActiveState; };
    virtual void stop() { m_state = QCamera::StoppedState; }
    QCamera::State state() const { return m_state; }

    QCamera::State m_state;
};

class MockCaptureControl : public QImageCaptureControl
{
    Q_OBJECT
public:
    MockCaptureControl(QObject *parent = 0)
        :QImageCaptureControl(parent)
    {
    }

    ~MockCaptureControl()
    {
    }

    bool isReadyForCapture() const { return true; }

    void capture(const QString &fileName)
    {
        emit imageCaptured(fileName, QImage());
    }

};

class MockCameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT
public:
    MockCameraExposureControl(QObject *parent = 0):
        QCameraExposureControl(parent),
        m_exposureLocked(false),
        m_aperture(2.8),
        m_shutterSpeed(0.01),
        m_isoSensitivity(100),
        m_meteringMode(QCamera::MeteringMatrix),
        m_exposureCompensation(0),
        m_exposureMode(QCamera::ExposureAuto),
        m_flashMode(QCamera::FlashAuto)
    {
    }

    ~MockCameraExposureControl() {}

    QCamera::FlashMode flashMode() const
    {
        return m_flashMode;
    }

    void setFlashMode(QCamera::FlashMode mode)
    {
        if (supportedFlashModes() & mode) {
            m_flashMode = mode;
        }
    }

    QCamera::FlashModes supportedFlashModes() const
    {
        return QCamera::FlashAuto | QCamera::FlashOff | QCamera::FlashOn;
    }

    bool isFlashReady() const
    {
        return true;
    }

    QCamera::ExposureMode exposureMode() const
    {
        return m_exposureMode;
    }

    void setExposureMode(QCamera::ExposureMode mode)
    {
        if (supportedExposureModes() & mode)
            m_exposureMode = mode;
    }

    QCamera::ExposureModes supportedExposureModes() const
    {
        return QCamera::ExposureAuto | QCamera::ExposureManual;
    }

    qreal exposureCompensation() const
    {
        return m_exposureCompensation;
    }

    void setExposureCompensation(qreal ev)
    {
        m_exposureCompensation = ev;
    }

    QCamera::MeteringMode meteringMode() const
    {
        return m_meteringMode;
    }

    void setMeteringMode(QCamera::MeteringMode mode)
    {
        if (supportedMeteringModes() & mode)
            m_meteringMode = mode;
    }

    QCamera::MeteringModes supportedMeteringModes() const
    {
        return QCamera::MeteringAverage | QCamera::MeteringMatrix;
    }

    int isoSensitivity() const
    {
        return m_isoSensitivity;
    }

    int minimumIsoSensitivity() const {return 50;}
    int maximumIsoSensitivity() const {return 3200;}
    QList<int> supportedIsoSenitivities() const {return QList<int>();}


    void setManualIsoSensitivity(int iso)
    {
        m_isoSensitivity = qBound(50, iso, 3200);
    }

    void setAutoIsoSensitivity()
    {
        m_isoSensitivity = 100;
    }

    qreal aperture() const
    {
        return m_aperture;
    }

    qreal minimumAperture() const {return 2.8;}
    qreal maximumAperture() const {return 16.0;}
    QList<qreal> supportedApertures() const {return QList<qreal>();}

    void setManualAperture(qreal aperture)
    {
        m_aperture = qBound<qreal>(2.8, aperture, 16.0);
    }

    void setAutoAperture()
    {
        m_aperture = 2.8;
    }

    qreal shutterSpeed() const
    {
        return m_shutterSpeed;
    }

    qreal minimumShutterSpeed() const {return 0.001;}
    qreal maximumShutterSpeed() const {return 30.0;}
    QList<qreal> supportedShutterSpeeds() const {return QList<qreal>();}

    void setManualShutterSpeed(qreal shutterSpeed)
    {
        m_shutterSpeed = qBound<qreal>(0.001, shutterSpeed, 30.0);
    }

    void setAutoShutterSpeed()
    {
        m_shutterSpeed = 0.01;
    }

    bool isExposureLocked() const
    {
        return m_exposureLocked;
    }

public Q_SLOTS:
    void lockExposure()
    {
        if (!m_exposureLocked) {
            m_exposureLocked = true;
            emit exposureLocked();
        }
    }

    void unlockExposure()
    {
        m_exposureLocked = false;
    }


private:
    bool m_exposureLocked;
    qreal m_aperture;
    qreal m_shutterSpeed;
    int m_isoSensitivity;
    QCamera::MeteringMode m_meteringMode;
    qreal m_exposureCompensation;
    QCamera::ExposureMode m_exposureMode;
    QCamera::FlashMode m_flashMode;
};

class MockCameraFocusControl : public QCameraFocusControl
{
    Q_OBJECT
public:
    MockCameraFocusControl(QObject *parent = 0):
        QCameraFocusControl(parent),
        m_focusLocked(false),
        m_zoomValue(1.0),
        m_macroFocusingEnabled(false),
        m_focusMode(QCamera::AutoFocus)
    {
    }

    ~MockCameraFocusControl() {}

    QCamera::FocusMode focusMode() const
    {
        return m_focusMode;
    }

    void setFocusMode(QCamera::FocusMode mode)
    {
        if (supportedFocusModes() & mode)
            m_focusMode = mode;
    }

    QCamera::FocusModes supportedFocusModes() const
    {
        return QCamera::AutoFocus | QCamera::ContinuousFocus;
    }

    QCamera::FocusStatus focusStatus() const
    {
        return QCamera::FocusReached;
    }

    bool macroFocusingEnabled() const
    {
        return m_macroFocusingEnabled;
    }

    bool isMacroFocusingSupported() const
    {
        return true;
    }

    void setMacroFocusingEnabled(bool flag)
    {
        if (isMacroFocusingSupported())
            m_macroFocusingEnabled = flag;
    }

    qreal maximumOpticalZoom() const
    {
        return 3.0;
    }

    qreal maximumDigitalZoom() const
    {
        return 4.0;
    }

    qreal zoomValue() const
    {
        return m_zoomValue;
    }


    void zoomTo(qreal value)
    {
        m_zoomValue = value;
    }

    bool isFocusLocked() const
    {
        return m_focusLocked;
    }

public Q_SLOTS:
    void lockFocus()
    {
        if (!m_focusLocked) {
            m_focusLocked = true;
            emit focusLocked();
        }
    }

    void unlockFocus()
    {
        m_focusLocked = false;
    }

private:
    bool m_focusLocked;
    qreal m_zoomValue;
    bool m_macroFocusingEnabled;
    QCamera::FocusMode m_focusMode;
};

class MockImageProcessingControl : public QImageProcessingControl
{
public:
    MockImageProcessingControl(QObject *parent = 0)
        : QImageProcessingControl(parent)
        , m_supportedWhiteBalance(QCamera::WhiteBalanceAuto)
        , m_contrast(0.0)
        , m_saturation(0.0)
        , m_sharpeningLevel(0.0)
        , m_denoisingLevel(0.0)
        , m_sharpeningSupported(false)
        , m_denoisingSupported(true)
    {
    }

    QCamera::WhiteBalanceMode whiteBalanceMode() const { return m_whiteBalanceMode; }
    void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode) { m_whiteBalanceMode = mode; }

    QCamera::WhiteBalanceModes supportedWhiteBalanceModes() const {
        return m_supportedWhiteBalance; }
    void setSupportedWhiteBalanceModes(QCamera::WhiteBalanceModes modes) {
        m_supportedWhiteBalance = modes; }

    int manualWhiteBalance() const { return m_manualWhiteBalance; }
    void setManualWhiteBalance(int colorTemperature) { m_manualWhiteBalance = colorTemperature; }

    qreal contrast() const { return m_contrast; }
    void setContrast(qreal value) { m_contrast = value; }

    qreal saturation() const { return m_saturation; }
    void setSaturation(qreal value) { m_saturation = value; }

    bool isSharpeningSupported() const { return m_sharpeningSupported; }
    void setSharpendingSupported(bool supported) { m_sharpeningSupported = supported; }

    qreal sharpeningLevel() const { return m_sharpeningLevel; }
    void setSharpeningLevel(qreal value) { m_sharpeningLevel = value; }

    bool isDenoisingSupported() const { return m_denoisingSupported; }
    void setDenoisingSupported(bool supported) { m_denoisingSupported = supported; }
    qreal denoisingLevel() const { return m_denoisingLevel; }
    void setDenoisingLevel(qreal value) { m_denoisingLevel = value; }

private:
    QCamera::WhiteBalanceMode m_whiteBalanceMode;
    QCamera::WhiteBalanceModes m_supportedWhiteBalance;
    int m_manualWhiteBalance;
    qreal m_contrast;
    qreal m_saturation;
    qreal m_sharpeningLevel;
    qreal m_denoisingLevel;
    bool m_sharpeningSupported;
    bool m_denoisingSupported;
};

class MockImageEncoderControl : public QImageEncoderControl
{
public:
    MockImageEncoderControl(QObject *parent = 0)
        : QImageEncoderControl(parent)
        , m_quality(QtMedia::HighQuality)
    {
    }

    QSize resolution() const { return m_resolution; }
    void setResolution(const QSize &resolution) { m_resolution = resolution; }
    QSize minimumResolution() const { return m_minimumResolution; }
    void setMinimumResolution(const QSize &resolution) { m_minimumResolution = resolution; }
    QSize maximumResolution() const { return m_maximumResolution; }
    void setMaximumResolution(const QSize &resolution) { m_maximumResolution = resolution; }
    QList<QSize> supportedResolutions() const { return m_supportedResolutions; }
    void setSupportedResolutions(const QList<QSize> &resolutions) {
        m_supportedResolutions = resolutions; }

    QStringList supportedImageCodecs() const { return m_supportedCodecs; }
    void setSupportedImageCodecs(const QStringList &codecs) { m_supportedCodecs = codecs; }
    QString imageCodec() const { return m_imageCodec; }
    bool setImageCodec(const QString &codecName) { m_imageCodec = codecName; return true; }

    QString imageCodecDescription(const QString &codecName) const {
        return m_codecDescriptions.value(codecName); }
    void setImageCodecDescriptions(const QMap<QString, QString> &descriptions) {
        m_codecDescriptions = descriptions; }

    QtMedia::EncodingQuality quality() const { return m_quality; }
    void setQuality(QtMedia::EncodingQuality quality) { m_quality = quality; }

private:
    QSize m_resolution;
    QSize m_minimumResolution;
    QSize m_maximumResolution;
    QList<QSize> m_supportedResolutions;
    QStringList m_supportedCodecs;
    QString m_imageCodec;
    QMap<QString, QString> m_codecDescriptions;
    QtMedia::EncodingQuality m_quality;
};

class MockSimpleCameraService : public QMediaService
{
    Q_OBJECT

public:
    MockSimpleCameraService(): QMediaService(0)
    {
        mockControl = new MockCameraControl(this);
    }

    ~MockSimpleCameraService()
    {
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;

        return 0;
    }

    MockCameraControl *mockControl;
};

class MockCameraService : public QMediaService
{
    Q_OBJECT

public:
    MockCameraService(): QMediaService(0)
    {
        mockControl = new MockCameraControl(this);
        mockExposureControl = new MockCameraExposureControl(this);
        mockFocusControl = new MockCameraFocusControl(this);
        mockCaptureControl = new MockCaptureControl(this);
        mockImageProcessingControl = new MockImageProcessingControl(this);
        mockImageEncoderControl = new MockImageEncoderControl(this);
    }

    ~MockCameraService()
    {
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;

        if (qstrcmp(iid, QCameraExposureControl_iid) == 0)
            return mockExposureControl;

        if (qstrcmp(iid, QCameraFocusControl_iid) == 0)
            return mockFocusControl;

        if (qstrcmp(iid, QImageCaptureControl_iid) == 0)
            return mockCaptureControl;

        if (qstrcmp(iid, QImageProcessingControl_iid) == 0)
            return mockImageProcessingControl;

        if (qstrcmp(iid, QImageEncoderControl_iid) == 0)
            return mockImageEncoderControl;

        return 0;
    }

    MockCameraControl *mockControl;
    MockCaptureControl *mockCaptureControl;
    MockCameraExposureControl *mockExposureControl;
    MockCameraFocusControl *mockFocusControl;
    MockImageProcessingControl *mockImageProcessingControl;
    MockImageEncoderControl *mockImageEncoderControl;
};

class MockProvider : public QMediaServiceProvider
{
public:
    QMediaService *requestService(const QByteArray &, const QMediaServiceProviderHint &)
    {
        return service;
    }

    void releaseService(QMediaService *) {}

    QMediaService *service;
};


class tst_QCamera: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();


private slots:
    void testSimpleCamera();
    void testSimpleCameraWhiteBalance();
    void testSimpleCameraExposure();
    void testSimpleCameraFocus();
    void testSimpleCameraCapture();

    void testCameraWhiteBalance();
    void testCameraExposure();
    void testCameraFocus();
    void testCameraCapture();

private:
    MockSimpleCameraService  *mockSimpleCameraService;
    MockProvider *provider;
};

void tst_QCamera::initTestCase()
{
    provider = new MockProvider;
    mockSimpleCameraService = new MockSimpleCameraService;
    provider->service = mockSimpleCameraService;

    qRegisterMetaType<QCamera::Error>();
}

void tst_QCamera::cleanupTestCase()
{
    delete mockSimpleCameraService;
    delete provider;
}


void tst_QCamera::testSimpleCamera()
{
    QCamera camera(0, provider);

    QCOMPARE(camera.service(), mockSimpleCameraService);

    QCOMPARE(camera.state(), QCamera::StoppedState);
    camera.start();
    QCOMPARE(camera.state(), QCamera::ActiveState);
    camera.stop();
    QCOMPARE(camera.state(), QCamera::StoppedState);
}

void tst_QCamera::testSimpleCameraWhiteBalance()
{
    QCamera camera(0, provider);

    //only WhiteBalanceAuto is supported
    QCOMPARE(camera.supportedWhiteBalanceModes(), QCamera::WhiteBalanceAuto);
    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceAuto);
    camera.setWhiteBalanceMode(QCamera::WhiteBalanceCloudy);
    QCOMPARE(camera.supportedWhiteBalanceModes(), QCamera::WhiteBalanceAuto);
    QCOMPARE(camera.manualWhiteBalance(), -1);
    camera.setManualWhiteBalance(5000);
    QCOMPARE(camera.manualWhiteBalance(), -1);
}

void tst_QCamera::testSimpleCameraExposure()
{
    QCamera camera(0, provider);

    QCOMPARE(camera.supportedExposureModes(), QCamera::ExposureAuto);
    QCOMPARE(camera.exposureMode(), QCamera::ExposureAuto);
    camera.setExposureMode(QCamera::ExposureManual);//should be ignored
    QCOMPARE(camera.exposureMode(), QCamera::ExposureAuto);

    QCOMPARE(camera.supportedFlashModes(), QCamera::FlashOff);
    QCOMPARE(camera.flashMode(), QCamera::FlashOff);
    QCOMPARE(camera.isFlashReady(), false);
    camera.setFlashMode(QCamera::FlashOn);
    QCOMPARE(camera.flashMode(), QCamera::FlashOff);

    QCOMPARE(camera.supportedMeteringModes(), QCamera::MeteringMatrix);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringMatrix);
    camera.setMeteringMode(QCamera::MeteringSpot);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringMatrix);

    QCOMPARE(camera.exposureCompensation(), 0.0);
    camera.setExposureCompensation(2.0);
    QCOMPARE(camera.exposureCompensation(), 0.0);

    QCOMPARE(camera.isoSensitivity(), -1);
    QCOMPARE(camera.minimumIsoSensitivity(), -1);
    QCOMPARE(camera.maximumIsoSensitivity(), -1);
    QVERIFY(camera.supportedIsoSenitivities().isEmpty());
    camera.setManualIsoSensitivity(100);
    QCOMPARE(camera.isoSensitivity(), -1);
    camera.setAutoIsoSensitivity();
    QCOMPARE(camera.isoSensitivity(), -1);

    QVERIFY(camera.aperture() < 0);
    QVERIFY(camera.minimumAperture() < 0);
    QVERIFY(camera.maximumAperture() < 0);
    QVERIFY(camera.supportedApertures().isEmpty());
    camera.setAutoAperture();
    QVERIFY(camera.aperture() < 0);
    camera.setManualAperture(5.6);
    QVERIFY(camera.aperture() < 0);

    QVERIFY(camera.shutterSpeed() < 0);
    QVERIFY(camera.minimumShutterSpeed() < 0);
    QVERIFY(camera.maximumShutterSpeed() < 0);
    QVERIFY(camera.supportedShutterSpeeds().isEmpty());
    camera.setAutoShutterSpeed();
    QVERIFY(camera.shutterSpeed() < 0);
    camera.setManualShutterSpeed(1/128.0);
    QVERIFY(camera.shutterSpeed() < 0);

    QCOMPARE(camera.isExposureLocked(), true);

}

void tst_QCamera::testSimpleCameraFocus()
{
    QCamera camera(0, provider);

    QCOMPARE(camera.supportedFocusModes(), QCamera::AutoFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    QCOMPARE(camera.focusStatus(), QCamera::FocusDisabled);

    QVERIFY(!camera.isMacroFocusingSupported());
    QVERIFY(!camera.macroFocusingEnabled());
    camera.setMacroFocusingEnabled(true);
    QVERIFY(!camera.macroFocusingEnabled());

    QCOMPARE(camera.maximumOpticalZoom(), 1.0);
    QCOMPARE(camera.maximumDigitalZoom(), 1.0);
    QCOMPARE(camera.zoomValue(), 1.0);
    camera.zoomTo(100);
    QCOMPARE(camera.zoomValue(), 1.0);

    QCOMPARE(camera.isFocusLocked(), true);
}

void tst_QCamera::testSimpleCameraCapture()
{
    QCamera camera(0, provider);

    QVERIFY(!camera.isReadyForCapture());

    QCOMPARE(camera.error(), QCamera::NoError);
    QVERIFY(camera.errorString().isEmpty());

    QSignalSpy errorSignal(&camera, SIGNAL(error(QCamera::Error)));
    camera.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(camera.error(), QCamera::NotReadyToCaptureError);
    QVERIFY(!camera.errorString().isEmpty());
}

void tst_QCamera::testCameraCapture()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    QVERIFY(camera.isReadyForCapture());

    QSignalSpy capturedSignal(&camera, SIGNAL(imageCaptured(QString,QImage)));
    camera.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(capturedSignal.size(), 1);
    QCOMPARE(camera.error(), QCamera::NoError);
}

void tst_QCamera::testCameraWhiteBalance()
{
    QCamera::WhiteBalanceModes whiteBalanceModes
            = QCamera::WhiteBalanceAuto
            | QCamera::WhiteBalanceFlash
            | QCamera::WhiteBalanceIncandescent;
    
    MockCameraService service;
    service.mockImageProcessingControl->setWhiteBalanceMode(QCamera::WhiteBalanceFlash);
    service.mockImageProcessingControl->setSupportedWhiteBalanceModes(whiteBalanceModes);
    service.mockImageProcessingControl->setManualWhiteBalance(34);

    MockProvider provider;
    provider.service = &service;
    
    QCamera camera(0, &provider);
    
    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceFlash);
    QCOMPARE(camera.supportedWhiteBalanceModes(), whiteBalanceModes);

    camera.setWhiteBalanceMode(QCamera::WhiteBalanceIncandescent);
    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceIncandescent);

    camera.setWhiteBalanceMode(QCamera::WhiteBalanceManual);
    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceManual);
    QCOMPARE(camera.manualWhiteBalance(), 34);

    camera.setManualWhiteBalance(432);
    QCOMPARE(camera.manualWhiteBalance(), 432);
}

void tst_QCamera::testCameraExposure()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    QVERIFY(camera.supportedExposureModes() & QCamera::ExposureAuto);
    QCOMPARE(camera.exposureMode(), QCamera::ExposureAuto);
    camera.setExposureMode(QCamera::ExposureManual);
    QCOMPARE(camera.exposureMode(), QCamera::ExposureManual);

    QCOMPARE(camera.flashMode(), QCamera::FlashAuto);
    QCOMPARE(camera.isFlashReady(), true);
    camera.setFlashMode(QCamera::FlashOn);
    QCOMPARE(camera.flashMode(), QCamera::FlashOn);

    camera.setFlashMode(QCamera::FlashRedEyeReduction); // not expected to be supported
    QCOMPARE(camera.flashMode(), QCamera::FlashOn);

    QCOMPARE(camera.meteringMode(), QCamera::MeteringMatrix);
    camera.setMeteringMode(QCamera::MeteringAverage);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringAverage);
    camera.setMeteringMode(QCamera::MeteringSpot);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringAverage);


    QCOMPARE(camera.exposureCompensation(), 0.0);
    camera.setExposureCompensation(2.0);
    QCOMPARE(camera.exposureCompensation(), 2.0);

    int minIso = camera.minimumIsoSensitivity();
    int maxIso = camera.maximumIsoSensitivity();
    QVERIFY(camera.isoSensitivity() > 0);
    QVERIFY(minIso > 0);
    QVERIFY(maxIso > 0);
    camera.setManualIsoSensitivity(100);
    QCOMPARE(camera.isoSensitivity(), 100);
    camera.setManualIsoSensitivity(maxIso*10);
    QCOMPARE(camera.isoSensitivity(), maxIso);
    camera.setManualIsoSensitivity(-10);
    QCOMPARE(camera.isoSensitivity(), minIso);
    camera.setAutoIsoSensitivity();
    QCOMPARE(camera.isoSensitivity(), 100);

    qreal minAperture = camera.minimumAperture();
    qreal maxAperture = camera.maximumAperture();
    QVERIFY(minAperture > 0);
    QVERIFY(maxAperture > 0);
    QVERIFY(camera.aperture() >= minAperture);
    QVERIFY(camera.aperture() <= maxAperture);

    camera.setAutoAperture();
    QVERIFY(camera.aperture() >= minAperture);
    QVERIFY(camera.aperture() <= maxAperture);

    camera.setManualAperture(0);
    QCOMPARE(camera.aperture(), minAperture);

    camera.setManualAperture(10000);
    QCOMPARE(camera.aperture(), maxAperture);


    qreal minShutterSpeed = camera.minimumShutterSpeed();
    qreal maxShutterSpeed = camera.maximumShutterSpeed();
    QVERIFY(minShutterSpeed > 0);
    QVERIFY(maxShutterSpeed > 0);
    QVERIFY(camera.shutterSpeed() >= minShutterSpeed);
    QVERIFY(camera.shutterSpeed() <= maxShutterSpeed);

    camera.setAutoShutterSpeed();
    QVERIFY(camera.shutterSpeed() >= minShutterSpeed);
    QVERIFY(camera.shutterSpeed() <= maxShutterSpeed);

    camera.setManualShutterSpeed(0);
    QCOMPARE(camera.shutterSpeed(), minShutterSpeed);

    camera.setManualShutterSpeed(10000);
    QCOMPARE(camera.shutterSpeed(), maxShutterSpeed);

    QCOMPARE(camera.isExposureLocked(), false);

    camera.lockExposure();
    QCOMPARE(camera.isExposureLocked(), true);

    camera.unlockExposure();
    QCOMPARE(camera.isExposureLocked(), false);
}

void tst_QCamera::testCameraFocus()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    QCOMPARE(camera.supportedFocusModes(), QCamera::AutoFocus | QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ManualFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::ContinuousFocus);
    QCOMPARE(camera.focusStatus(), QCamera::FocusReached);

    QVERIFY(camera.isMacroFocusingSupported());
    QVERIFY(!camera.macroFocusingEnabled());
    camera.setMacroFocusingEnabled(true);
    QVERIFY(camera.macroFocusingEnabled());

    QVERIFY(camera.maximumOpticalZoom() >= 1.0);
    QVERIFY(camera.maximumDigitalZoom() >= 1.0);
    QCOMPARE(camera.zoomValue(), 1.0);
    camera.zoomTo(0.5);
    QCOMPARE(camera.zoomValue(), 1.0);
    camera.zoomTo(2.0);
    QCOMPARE(camera.zoomValue(), 2.0);
    camera.zoomTo(2000000.0);
    QVERIFY(qFuzzyCompare(camera.zoomValue(), camera.maximumOpticalZoom()*camera.maximumDigitalZoom()));

    QCOMPARE(camera.isFocusLocked(), false);
    camera.lockFocus();
    QCOMPARE(camera.isFocusLocked(), true);
    camera.unlockFocus();
    QCOMPARE(camera.isFocusLocked(), false);
}


QTEST_MAIN(tst_QCamera)

#include "tst_qcamera.moc"
