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

#include <QtTest/QtTest>

#include <multimedia/qpaintervideosurface_p.h>

#include <QtGui/qapplication.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/qglframebufferobject.h>

class tst_QPainterVideoSurface : public QObject
{
    Q_OBJECT
private slots:
    void cleanup() {}
    void cleanupTestCase() {}

    void colors();

    void supportedFormat_data();
    void supportedFormat();

    void present_data();
    void present();

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    void shaderType();

    void shaderTypeStarted_data();
    void shaderTypeStarted();

    void shaderSupportedFormat_data();
    void shaderSupportedFormat();

    void shaderPresent_data();
    void shaderPresent();
#endif
};

Q_DECLARE_METATYPE(const uchar *)

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
Q_DECLARE_METATYPE(QPainterVideoSurface::ShaderType);
#endif

void tst_QPainterVideoSurface::colors()
{
    QPainterVideoSurface surface;

    QCOMPARE(surface.brightness(), 0);
    QCOMPARE(surface.contrast(), 0);
    QCOMPARE(surface.hue(), 0);
    QCOMPARE(surface.saturation(), 0);

    surface.setBrightness(56);
    QCOMPARE(surface.brightness(), 56);

    surface.setContrast(43);
    QCOMPARE(surface.contrast(), 43);

    surface.setHue(-84);
    QCOMPARE(surface.hue(), -84);

    surface.setSaturation(100);
    QCOMPARE(surface.saturation(), 100);
}

static const uchar rgb32ImageData[] =
{
    0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00,
    0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00
};

static const uchar argb32ImageData[] =
{
    0x00, 0xff, 0x00, 0x00, 0xcc, 0x00, 0xff, 0xcc,
    0x77, 0x00, 0x00, 0x77, 0x00, 0xff, 0xff, 0x00
};

static const uchar rgb24ImageData[] =
{
    0x00, 0xff, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x00,
    0xcc, 0x00, 0xcc, 0x77, 0xff, 0x77, 0x00, 0x00
};

static const uchar rgb565ImageData[] =
{
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00
};

static const uchar yuvPlanarImageData[] =
{
    0x00, 0x00, 0x0f, 0xff, 0xff, 0x0f, 0x00, 0x00,
    0x00, 0x0f, 0xff, 0x0f, 0x0f, 0xff, 0x0f, 0x00,
    0x00, 0xff, 0x0f, 0x00, 0x00, 0x0f, 0xff, 0x00,
    0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff,
    0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff,
    0x0f, 0xff, 0x0f, 0x00, 0x00, 0x0f, 0xff, 0x0f,
    0x00, 0x0f, 0xff, 0x0f, 0x0f, 0xff, 0x0f, 0x00,
    0x00, 0x00, 0x0f, 0xff, 0xff, 0x0f, 0x00, 0x00,
    0x00, 0x0f, 0x0f, 0x00,
    0x0f, 0x00, 0x00, 0x0f,
    0x0f, 0x00, 0x00, 0x0f,
    0x00, 0x0f, 0x0f, 0x00,
    0x00, 0x0f, 0x0f, 0x00,
    0x0f, 0x00, 0x00, 0x0f,
    0x0f, 0x00, 0x00, 0x0f,
    0x00, 0x0f, 0x0f, 0x00,
};

void tst_QPainterVideoSurface::supportedFormat_data()
{
    QTest::addColumn<QAbstractVideoBuffer::HandleType>("handleType");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormat");
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<bool>("supportedPixelFormat");
    QTest::addColumn<bool>("supportedFormat");

    QTest::newRow("rgb32 640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB32
            << QSize(640, 480)
            << true
            << true;
    QTest::newRow("rgb32 -640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB32
            << QSize(-640, 480)
            << true
            << false;
    QTest::newRow("rgb24 1024x768")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(1024, 768)
            << true
            << true;
    QTest::newRow("rgb24 -1024x-768")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(-1024, -768)
            << true
            << false;
    QTest::newRow("rgb565 0x0")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB565
            << QSize(0, 0)
            << true
            << false;
    QTest::newRow("YUV420P 640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_YUV420P
            << QSize(640, 480)
            << false
            << false;
    QTest::newRow("YUV420P 640x-480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_YUV420P
            << QSize(640, -480)
            << false
            << false;
    QTest::newRow("Y8 640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_Y8
            << QSize(640, 480)
            << false
            << false;
    QTest::newRow("Texture: rgb32 640x480")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_RGB32
            << QSize(640, 480)
            << false
            << false;
    QTest::newRow("Texture: rgb32 -640x480")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_RGB32
            << QSize(-640, 480)
            << false
            << false;
    QTest::newRow("Texture: rgb565 0x0")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_RGB565
            << QSize(0, 0)
            << false
            << false;
    QTest::newRow("Texture: rgb24 1024x768")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_RGB24
            << QSize(1024, 768)
            << false
            << false;
    QTest::newRow("Texture: rgb24 -1024x-768")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_RGB24
            << QSize(-1024, -768)
            << false
            << false;
    QTest::newRow("Texture: YUV420P 640x480")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_YUV420P
            << QSize(640, 480)
            << false
            << false;
    QTest::newRow("Texture: YUV420P 640x-480")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_YUV420P
            << QSize(640, -480)
            << false
            << false;
}

void tst_QPainterVideoSurface::supportedFormat()
{
    QFETCH(QAbstractVideoBuffer::HandleType, handleType);
    QFETCH(QVideoFrame::PixelFormat, pixelFormat);
    QFETCH(QSize, frameSize);
    QFETCH(bool, supportedPixelFormat);
    QFETCH(bool, supportedFormat);

    QPainterVideoSurface surface;

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface.supportedPixelFormats(handleType);

    QCOMPARE(pixelFormats.contains(pixelFormat), QBool(supportedPixelFormat));

    QVideoSurfaceFormat format(frameSize, pixelFormat, handleType);

    QCOMPARE(surface.isFormatSupported(format), supportedFormat);
    QCOMPARE(surface.start(format), supportedFormat);
}

void tst_QPainterVideoSurface::present_data()
{
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormatA");
    QTest::addColumn<QSize>("frameSizeA");
    QTest::addColumn<const uchar *>("frameDataA");
    QTest::addColumn<int>("bytesA");
    QTest::addColumn<int>("bytesPerLineA");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormatB");
    QTest::addColumn<QSize>("frameSizeB");
    QTest::addColumn<const uchar *>("frameDataB");
    QTest::addColumn<int>("bytesB");
    QTest::addColumn<int>("bytesPerLineB");

    QTest::newRow("rgb32 -> argb32")
            << QVideoFrame::Format_RGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb32ImageData)
            << int(sizeof(rgb32ImageData))
            << 8
            << QVideoFrame::Format_ARGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(argb32ImageData)
            << int(sizeof(argb32ImageData))
            << 8;

    QTest::newRow("rgb32 -> rgb24")
            << QVideoFrame::Format_RGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb32ImageData)
            << int(sizeof(rgb32ImageData))
            << 8
            << QVideoFrame::Format_RGB24
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb24ImageData)
            << int(sizeof(rgb24ImageData))
            << 8;

    QTest::newRow("rgb32 -> rgb565")
            << QVideoFrame::Format_RGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb32ImageData)
            << int(sizeof(rgb32ImageData))
            << 8
            << QVideoFrame::Format_RGB565
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb565ImageData)
            << int(sizeof(rgb565ImageData))
            << 4;

    QTest::newRow("rgb24 -> rgb565")
            << QVideoFrame::Format_RGB24
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb24ImageData)
            << int(sizeof(rgb24ImageData))
            << 8
            << QVideoFrame::Format_RGB565
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb565ImageData)
            << int(sizeof(rgb565ImageData))
            << 4;
}

void tst_QPainterVideoSurface::present()
{
    QFETCH(QVideoFrame::PixelFormat, pixelFormatA);
    QFETCH(QSize, frameSizeA);
    QFETCH(const uchar *, frameDataA);
    QFETCH(int, bytesA);
    QFETCH(int, bytesPerLineA);
    QFETCH(QVideoFrame::PixelFormat, pixelFormatB);
    QFETCH(QSize, frameSizeB);
    QFETCH(const uchar *, frameDataB);
    QFETCH(int, bytesB);
    QFETCH(int, bytesPerLineB);

    QPainterVideoSurface surface;

    QImage image(320, 240, QImage::Format_RGB32);

    QSignalSpy frameSpy(&surface, SIGNAL(frameChanged()));

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface.supportedPixelFormats();

    QVideoSurfaceFormat formatA(frameSizeA, pixelFormatA);

    QVERIFY(surface.start(formatA));
    QCOMPARE(surface.isStarted(), true);
    QCOMPARE(surface.isReady(), true);

    QVideoFrame frameA(bytesA, frameSizeA, bytesPerLineA, pixelFormatA);

    frameA.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frameA.bits(), frameDataA, frameA.numBytes());
    frameA.unmap();

    QVERIFY(surface.present(frameA));
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(frameSpy.count(), 1);

    {
        QPainter painter(&image);
        surface.paint(&painter, QRect(0, 0, 320, 240));
    }
    QCOMPARE(surface.isStarted(), true);
    QCOMPARE(surface.isReady(), false);

    // Not ready.
    QVERIFY(!surface.present(frameA));
    QCOMPARE(frameSpy.count(), 1);

    surface.setReady(true);
    QCOMPARE(surface.isReady(), true);
    QVERIFY(surface.present(frameA));
    QCOMPARE(frameSpy.count(), 2);

    // Try switching to a different format after starting.
    QVideoSurfaceFormat formatB(frameSizeB, pixelFormatB);

    QVERIFY(surface.start(formatB));
    QCOMPARE(surface.isStarted(), true);
    QCOMPARE(surface.isReady(), true);

    QVideoFrame frameB(bytesB, frameSizeB, bytesPerLineB, pixelFormatB);

    frameB.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frameB.bits(), frameDataB, frameB.numBytes());
    frameB.unmap();

    QVERIFY(surface.present(frameB));
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(frameSpy.count(), 3);

    {
        QPainter painter(&image);
        surface.paint(&painter, QRect(0, 0, 320, 240));
    }
    QVERIFY(surface.isStarted());

    surface.stop();

    QCOMPARE(surface.isStarted(), false);
    QCOMPARE(surface.isReady(), false);

    // Try presenting a frame while stopped.
    QVERIFY(!surface.present(frameB));
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(surface.error(), QAbstractVideoSurface::StoppedError);

    // Try presenting a frame with a different format.
    QVERIFY(surface.start(formatB));
    QVERIFY(!surface.present(frameA));
    QCOMPARE(surface.isStarted(), false);
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(surface.error(), QAbstractVideoSurface::IncorrectFormatError);
}

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)

void tst_QPainterVideoSurface::shaderType()
{
    QPainterVideoSurface surface;
    QGLWidget widget;
    widget.makeCurrent();

    QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
    QCOMPARE(surface.supportedShaderTypes(), QPainterVideoSurface::NoShaders);

    surface.setGLContext(const_cast<QGLContext *>(widget.context()));

    {
        QSignalSpy spy(&surface, SIGNAL(supportedFormatsChanged()));

        surface.setShaderType(QPainterVideoSurface::NoShaders);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
        QCOMPARE(spy.count(), 0);
    }

#ifndef QT_OPENGL_ES
    if (surface.supportedShaderTypes() & QPainterVideoSurface::FragmentProgramShader) {
        QSignalSpy spy(&surface, SIGNAL(supportedFormatsChanged()));

        surface.setShaderType(QPainterVideoSurface::FragmentProgramShader);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::FragmentProgramShader);
        QCOMPARE(spy.count(), 1);

        surface.setShaderType(QPainterVideoSurface::FragmentProgramShader);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::FragmentProgramShader);
        QCOMPARE(spy.count(), 1);
    }
#endif

    if (surface.supportedShaderTypes() & QPainterVideoSurface::GlslShader) {
        QSignalSpy spy(&surface, SIGNAL(supportedFormatsChanged()));

        surface.setShaderType(QPainterVideoSurface::GlslShader);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::GlslShader);
        QCOMPARE(spy.count(), 1);

        surface.setShaderType(QPainterVideoSurface::GlslShader);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::GlslShader);
        QCOMPARE(spy.count(), 1);
    }

    surface.setGLContext(0);
    QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
    QCOMPARE(surface.supportedShaderTypes(), QPainterVideoSurface::NoShaders);

    {
        QSignalSpy spy(&surface, SIGNAL(supportedFormatsChanged()));

        surface.setShaderType(QPainterVideoSurface::NoShaders);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
        QCOMPARE(spy.count(), 0);

#ifndef QT_OPENGL_ES
        surface.setShaderType(QPainterVideoSurface::FragmentProgramShader);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
        QCOMPARE(spy.count(), 0);
#endif

        surface.setShaderType(QPainterVideoSurface::GlslShader);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
        QCOMPARE(spy.count(), 0);
    }
}

void tst_QPainterVideoSurface::shaderTypeStarted_data()
{
    QTest::addColumn<QPainterVideoSurface::ShaderType>("shaderType");

#ifndef QT_OPENGL_ES
    QTest::newRow("ARBfp")
            << QPainterVideoSurface::FragmentProgramShader;
#endif
    QTest::newRow("GLSL")
            << QPainterVideoSurface::GlslShader;
}

void tst_QPainterVideoSurface::shaderTypeStarted()
{
    QFETCH(QPainterVideoSurface::ShaderType, shaderType);

    QGLWidget widget;
    widget.makeCurrent();

    QPainterVideoSurface surface;

    surface.setGLContext(const_cast<QGLContext *>(widget.context()));

    if (!(surface.supportedShaderTypes() & shaderType))
        QSKIP("Shader type unsupported on this platform", SkipSingle);

    surface.setShaderType(shaderType);
    QCOMPARE(surface.shaderType(), shaderType);

    QVERIFY(surface.start(QVideoSurfaceFormat(QSize(640, 480), QVideoFrame::Format_RGB32)));
    {
        QSignalSpy spy(&surface, SIGNAL(startedChanged(bool)));

        surface.setShaderType(QPainterVideoSurface::NoShaders);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
        QCOMPARE(surface.isStarted(), false);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.last().value(0).toBool(), false);
    }

    QVERIFY(surface.start(QVideoSurfaceFormat(QSize(640, 480), QVideoFrame::Format_RGB32)));
    {
        QSignalSpy spy(&surface, SIGNAL(startedChanged(bool)));

        surface.setShaderType(QPainterVideoSurface::NoShaders);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
        QCOMPARE(surface.isStarted(), true);
        QCOMPARE(spy.count(), 0);

        surface.setShaderType(shaderType);
        QCOMPARE(surface.shaderType(), shaderType);
        QCOMPARE(surface.isStarted(), false);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.last().value(0).toBool(), false);
    }

    QVERIFY(surface.start(QVideoSurfaceFormat(QSize(640, 480), QVideoFrame::Format_RGB32)));
    {
        QSignalSpy spy(&surface, SIGNAL(startedChanged(bool)));

        surface.setShaderType(shaderType);
        QCOMPARE(surface.shaderType(), shaderType);
        QCOMPARE(surface.isStarted(), true);
        QCOMPARE(spy.count(), 0);

        surface.setGLContext(0);
        QCOMPARE(surface.shaderType(), QPainterVideoSurface::NoShaders);
        QCOMPARE(surface.isStarted(), false);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.last().value(0).toBool(), false);
    }
}

void tst_QPainterVideoSurface::shaderSupportedFormat_data()
{
    QTest::addColumn<QPainterVideoSurface::ShaderType>("shaderType");
    QTest::addColumn<QAbstractVideoBuffer::HandleType>("handleType");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormat");
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<bool>("supportedPixelFormat");
    QTest::addColumn<bool>("supportedFormat");

    QList<QPair<QPainterVideoSurface::ShaderType, QByteArray> > types;
#ifndef QT_OPENGL_ES
    types << qMakePair(QPainterVideoSurface::FragmentProgramShader, QByteArray("ARBfp: "));
#endif
    types << qMakePair(QPainterVideoSurface::GlslShader, QByteArray("GLSL"));

    QPair<QPainterVideoSurface::ShaderType, QByteArray> type;
    foreach (type, types) {
        QTest::newRow((type.second + "rgb32 640x480").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_RGB32
                << QSize(640, 480)
                << true
                << true;
        QTest::newRow((type.second + "rgb32 -640x480").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_RGB32
                << QSize(-640, 480)
                << true
                << false;
        QTest::newRow((type.second + "rgb565 0x0").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_RGB565
                << QSize(0, 0)
                << true
                << false;
        QTest::newRow((type.second + "rgb24 1024x768").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_RGB24
                << QSize(1024, 768)
                << false
                << false;
        QTest::newRow((type.second + "rgb24 -1024x-768").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_RGB24
                << QSize(-1024, -768)
                << false
                << false;
        QTest::newRow((type.second + "YUV420P 640x480").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_YUV420P
                << QSize(640, 480)
                << true
                << true;
        QTest::newRow((type.second + "YUV420P 640x-480").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_YUV420P
                << QSize(640, -480)
                << true
                << false;
        QTest::newRow((type.second + "Y8 640x480").constData())
                << type.first
                << QAbstractVideoBuffer::NoHandle
                << QVideoFrame::Format_Y8
                << QSize(640, 480)
                << false
                << false;
        QTest::newRow((type.second + "Texture: rgb32 640x480").constData())
                << type.first
                << QAbstractVideoBuffer::GLTextureHandle
                << QVideoFrame::Format_RGB32
                << QSize(640, 480)
                << true
                << true;
        QTest::newRow((type.second + "Texture: rgb32 -640x480").constData())
                << type.first
                << QAbstractVideoBuffer::GLTextureHandle
                << QVideoFrame::Format_RGB32
                << QSize(-640, 480)
                << true
                << false;
        QTest::newRow((type.second + "Texture: rgb565 0x0").constData())
                << type.first
                << QAbstractVideoBuffer::GLTextureHandle
                << QVideoFrame::Format_RGB565
                << QSize(0, 0)
                << true
                << false;
        QTest::newRow((type.second + "Texture: rgb24 1024x768").constData())
                << type.first
                << QAbstractVideoBuffer::GLTextureHandle
                << QVideoFrame::Format_RGB24
                << QSize(1024, 768)
                << false
                << false;
        QTest::newRow((type.second + "Texture: rgb24 -1024x-768").constData())
                << type.first
                << QAbstractVideoBuffer::GLTextureHandle
                << QVideoFrame::Format_RGB24
                << QSize(-1024, -768)
                << false
                << false;
        QTest::newRow((type.second + "Texture: YUV420P 640x480").constData())
                << type.first
                << QAbstractVideoBuffer::GLTextureHandle
                << QVideoFrame::Format_YUV420P
                << QSize(640, 480)
                << false
                << false;
        QTest::newRow((type.second + "Texture: YUV420P 640x-480").constData())
                << type.first
                << QAbstractVideoBuffer::GLTextureHandle
                << QVideoFrame::Format_YUV420P
                << QSize(640, -480)
                << false
                << false;
    }
}

void tst_QPainterVideoSurface::shaderSupportedFormat()
{
    QFETCH(QPainterVideoSurface::ShaderType, shaderType);
    QFETCH(QAbstractVideoBuffer::HandleType, handleType);
    QFETCH(QVideoFrame::PixelFormat, pixelFormat);
    QFETCH(QSize, frameSize);
    QFETCH(bool, supportedPixelFormat);
    QFETCH(bool, supportedFormat);

    QGLWidget widget;
    widget.makeCurrent();

    QPainterVideoSurface surface;
    surface.setGLContext(const_cast<QGLContext *>(widget.context()));


    if (!(surface.supportedShaderTypes() & shaderType))
        QSKIP("Shader type not supported on this platform", SkipSingle);

    surface.setShaderType(shaderType);
    if (surface.shaderType() != shaderType)
        QSKIP("Shader type couldn't be set", SkipSingle);

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface.supportedPixelFormats(handleType);

    QCOMPARE(pixelFormats.contains(pixelFormat), QBool(supportedPixelFormat));

    QVideoSurfaceFormat format(frameSize, pixelFormat, handleType);

    QCOMPARE(surface.isFormatSupported(format), supportedFormat);
    QCOMPARE(surface.start(format), supportedFormat);
}

void tst_QPainterVideoSurface::shaderPresent_data()
{
    QTest::addColumn<QPainterVideoSurface::ShaderType>("shaderType");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormatA");
    QTest::addColumn<QSize>("frameSizeA");
    QTest::addColumn<const uchar *>("frameDataA");
    QTest::addColumn<int>("bytesA");
    QTest::addColumn<int>("bytesPerLineA");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormatB");
    QTest::addColumn<QSize>("frameSizeB");
    QTest::addColumn<const uchar *>("frameDataB");
    QTest::addColumn<int>("bytesB");
    QTest::addColumn<int>("bytesPerLineB");

    QList<QPair<QPainterVideoSurface::ShaderType, QByteArray> > types;
#ifndef QT_OPENGL_ES
    types << qMakePair(QPainterVideoSurface::FragmentProgramShader, QByteArray("ARBfp: "));
#endif
    types << qMakePair(QPainterVideoSurface::GlslShader, QByteArray("GLSL: "));

    QPair<QPainterVideoSurface::ShaderType, QByteArray> type;
    foreach (type, types) {
        QTest::newRow((type.second + "rgb32 -> argb32").constData())
                << type.first
                << QVideoFrame::Format_RGB32
                << QSize(2, 2)
                << static_cast<const uchar *>(rgb32ImageData)
                << int(sizeof(rgb32ImageData))
                << 8
                << QVideoFrame::Format_ARGB32
                << QSize(2, 2)
                << static_cast<const uchar *>(argb32ImageData)
                << int(sizeof(argb32ImageData))
                << 8;

        QTest::newRow((type.second + "rgb32 -> rgb565").constData())
                << type.first
                << QVideoFrame::Format_RGB32
                << QSize(2, 2)
                << static_cast<const uchar *>(rgb32ImageData)
                << int(sizeof(rgb32ImageData))
                << 8
                << QVideoFrame::Format_RGB565
                << QSize(2, 2)
                << static_cast<const uchar *>(rgb565ImageData)
                << int(sizeof(rgb565ImageData))
                << 4;

        QTest::newRow((type.second + "rgb32 -> yuv420p").constData())
                << type.first
                << QVideoFrame::Format_RGB32
                << QSize(2, 2)
                << static_cast<const uchar *>(rgb32ImageData)
                << int(sizeof(rgb32ImageData))
                << 8
                << QVideoFrame::Format_YUV420P
                << QSize(8, 8)
                << static_cast<const uchar *>(yuvPlanarImageData)
                << int(sizeof(yuvPlanarImageData))
                << 8;

        QTest::newRow((type.second + "yv12 -> rgb32").constData())
                << type.first
                << QVideoFrame::Format_YV12
                << QSize(8, 8)
                << static_cast<const uchar *>(yuvPlanarImageData)
                << int(sizeof(yuvPlanarImageData))
                << 8
                << QVideoFrame::Format_RGB32
                << QSize(2, 2)
                << static_cast<const uchar *>(rgb32ImageData)
                << int(sizeof(rgb32ImageData))
                << 8;
    }
}

void tst_QPainterVideoSurface::shaderPresent()
{
    QFETCH(QPainterVideoSurface::ShaderType, shaderType);
    QFETCH(QVideoFrame::PixelFormat, pixelFormatA);
    QFETCH(QSize, frameSizeA);
    QFETCH(const uchar *, frameDataA);
    QFETCH(int, bytesA);
    QFETCH(int, bytesPerLineA);
    QFETCH(QVideoFrame::PixelFormat, pixelFormatB);
    QFETCH(QSize, frameSizeB);
    QFETCH(const uchar *, frameDataB);
    QFETCH(int, bytesB);
    QFETCH(int, bytesPerLineB);

    QGLWidget widget;
    widget.makeCurrent();

    QPainterVideoSurface surface;
    surface.setGLContext(const_cast<QGLContext *>(widget.context()));

    if (!(surface.supportedShaderTypes() & shaderType))
        QSKIP("Shader type unsupported on this platform", SkipSingle);

    surface.setShaderType(shaderType);
    if (surface.shaderType() != shaderType)
        QSKIP("Shader type couldn't be set", SkipSingle);

    QSignalSpy frameSpy(&surface, SIGNAL(frameChanged()));

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface.supportedPixelFormats();

    QVideoSurfaceFormat formatA(frameSizeA, pixelFormatA);

    QVERIFY(surface.start(formatA));
    QCOMPARE(surface.isStarted(), true);
    QCOMPARE(surface.isReady(), true);

    QVideoFrame frameA(bytesA, frameSizeA, bytesPerLineA, pixelFormatA);

    frameA.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frameA.bits(), frameDataA, frameA.numBytes());
    frameA.unmap();

    QVERIFY(surface.present(frameA));
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(frameSpy.count(), 1);

    {
        QPainter painter(&widget);
        surface.paint(&painter, QRect(0, 0, 320, 240));
    }

    QCOMPARE(surface.isStarted(), true);

    // Not ready.
    QVERIFY(!surface.present(frameA));
    QCOMPARE(frameSpy.count(), 1);

    surface.setReady(true);
    QCOMPARE(surface.isReady(), true);
    QVERIFY(surface.present(frameA));
    QCOMPARE(frameSpy.count(), 2);

    // Try switching to a different format after starting.
    QVideoSurfaceFormat formatB(frameSizeB, pixelFormatB);

    QVERIFY(surface.start(formatB));
    QCOMPARE(surface.isStarted(), true);
    QCOMPARE(surface.isReady(), true);

    QVideoFrame frameB(bytesB, frameSizeB, bytesPerLineB, pixelFormatB);

    frameB.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frameB.bits(), frameDataB, frameB.numBytes());
    frameB.unmap();

    QVERIFY(surface.present(frameB));
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(frameSpy.count(), 3);

    {
        QPainter painter(&widget);
        surface.paint(&painter, QRect(0, 0, 320, 240));
    }

    QCOMPARE(surface.isStarted(), true);

    surface.stop();

    QCOMPARE(surface.isStarted(), false);
    QCOMPARE(surface.isReady(), false);

    // Try presenting a frame while stopped.
    QVERIFY(!surface.present(frameB));
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(surface.error(), QAbstractVideoSurface::StoppedError);

    // Try presenting a frame with a different format.
    QVERIFY(surface.start(formatB));
    QVERIFY(!surface.present(frameA));
    QCOMPARE(surface.isStarted(), false);
    QCOMPARE(surface.isReady(), false);
    QCOMPARE(surface.error(), QAbstractVideoSurface::IncorrectFormatError);
}

#endif

QTEST_MAIN(tst_QPainterVideoSurface)

#include "tst_qpaintervideosurface.moc"
