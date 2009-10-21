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

#include <QtCore/qdir.h>

#include <multimedia/qmediaimageviewer.h>
#include <multimedia/qmediaimageviewerservice_p.h>
#include <multimedia/qmediaplaylist.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qvideooutputcontrol.h>
#include <multimedia/qvideorenderercontrol.h>
#include <multimedia/qvideowidgetcontrol.h>

#include <QtCore/qfile.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#endif

class QtTestNetworkAccessManager;

class tst_QMediaImageViewer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void isValid();
    void timeout();
    void setMedia_data();
    void setMedia();
    void setInvalidMedia();
    void playlist();
    void outputControl();
    void widgetControl();
#ifndef QT_NO_MULTIMEDIA
    void rendererControl();
#endif

public:
    tst_QMediaImageViewer() : m_network(0), m_imageDir(QLatin1String(TESTDATA_DIR)) {}

private:
    QUrl imageUri(const char *fileName) const {
        return QUrl::fromLocalFile(m_imageDir.absoluteFilePath(QLatin1String(fileName))); }
    QString imageFileName(const char *fileName) {
        return m_imageDir.absoluteFilePath(QLatin1String(fileName)); }

    QtTestNetworkAccessManager *m_network;
    QDir m_imageDir;
    QString m_fileProtocol;
};

Q_DECLARE_METATYPE(QMediaImageViewer::State)

#ifndef QT_NO_MULTIMEDIA
class QtTestVideoSurface : public QAbstractVideoSurface
{
public:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType) const {
        QList<QVideoFrame::PixelFormat> formats;
        if (handleType == QAbstractVideoBuffer::NoHandle) {
            formats << QVideoFrame::Format_RGB32;
        }
        return formats;
    }

    QVideoFrame frame() const { return m_frame;  }

    bool present(const QVideoFrame &frame) { m_frame = frame; return true; }

private:
    QVideoFrame m_frame;
};
#endif

class QtTestNetworkReply : public QNetworkReply
{
public:
    QtTestNetworkReply(
            const QNetworkRequest &request,
            const QByteArray &mimeType,
            QObject *parent)
        : QNetworkReply(parent)
    {
        setRequest(request);
        setOperation(QNetworkAccessManager::HeadOperation);
        setRawHeader("content-type", mimeType);

        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }

    QtTestNetworkReply(
            const QNetworkRequest &request,
            const QByteArray &mimeType,
            const QString &fileName,
            QObject *parent)
        : QNetworkReply(parent)
        , m_file(fileName)
    {
        setRequest(request);
        setOperation(QNetworkAccessManager::GetOperation);
        setRawHeader("content-type", mimeType);

        if (m_file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
            setOpenMode(QIODevice::ReadOnly);
        }

        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }

    void abort() { m_file.close(); }

    bool atEnd () const { return m_file.atEnd(); }
    qint64 bytesAvailable() const { return m_file.bytesAvailable() + QIODevice::bytesAvailable(); }
    void close() { m_file.close(); setOpenMode(QIODevice::NotOpen); }
    bool isSequential() const { return true; }
    bool open(OpenMode) { return false; }
    qint64 pos() const { return 0; }
    bool seek(qint64) { return false; }
    qint64 size() const { return m_file.size(); }
    qint64 readData(char * data, qint64 maxSize) { return m_file.read(data, maxSize); }
    qint64 writeData(const char *, qint64) { return -1; }

protected:
    void customEvent(QEvent *event)
    {
        if (event->type() == QEvent::User) {
            event->accept();
            emit finished();
        }
    }

private:
    QFile m_file;
};

class QtTestNetworkAccessManager : public QNetworkAccessManager
{
public:
    QtTestNetworkAccessManager(QObject *parent = 0)
        : QNetworkAccessManager(parent)
    {
    }

    void appendDocument(const QUrl &url, const QByteArray &mimeType, const QString &fileName)
    {
        m_documents.append(Document(url, mimeType, fileName));
    }

protected:
    QNetworkReply *createRequest(
            Operation op, const QNetworkRequest &request, QIODevice *outgoingData = 0)
    {
        foreach (const Document &document, m_documents) {
            if (document.url == request.url()) {
                if (op == GetOperation) {
                    return new QtTestNetworkReply(
                            request, document.mimeType, document.fileName, this);
                } else if (op == HeadOperation) {
                    return new QtTestNetworkReply(request, document.mimeType, this);
                }
            }
        }
        return QNetworkAccessManager::createRequest(op, request, outgoingData);
    }

private:
    struct Document
    {
        Document(const QUrl url, const QByteArray mimeType, const QString &fileName)
            : url(url), mimeType(mimeType), fileName(fileName)
        {
        }

        QUrl url;
        QByteArray mimeType;
        QString fileName;
    };

    QList<Document> m_documents;
};

void tst_QMediaImageViewer::initTestCase()
{
    qRegisterMetaType<QMediaImageViewer::State>();

    m_network = new QtTestNetworkAccessManager(this);

    m_network->appendDocument(
            QUrl(QLatin1String("test://image/png?id=1")),
            "image/png",
            imageFileName("image.png"));
    m_network->appendDocument(
            QUrl(QLatin1String("test://image/png?id=2")),
            QByteArray(),
            imageFileName("image.png"));
    m_network->appendDocument(
            QUrl(QLatin1String("test://image/invalid?id=1")),
            "image/png",
            imageFileName("invalid.png"));
    m_network->appendDocument(
            QUrl(QLatin1String("test://image/invalid?id=2")),
            QByteArray(),
            imageFileName("invalid.png"));
#ifdef QTEST_HAVE_JPEG
    m_network->appendDocument(
            QUrl(QLatin1String("test://image/jpeg?id=1")),
            "image/jpeg",
            imageFileName("image.jpg"));
#endif
    m_network->appendDocument(
            QUrl(QLatin1String("test://music/songs/mp3?id=1")),
             "audio/mpeg",
             QString());
    m_network->appendDocument(
            QUrl(QLatin1String("test://music/covers/small?id=1")),
            "image/png",
            imageFileName("coverart.png"));
    m_network->appendDocument(
            QUrl(QLatin1String("test://music/covers/large?id=1")),
            "image/png",
            imageFileName("coverart.png"));
    m_network->appendDocument(
            QUrl(QLatin1String("test://video/movies/mp4?id=1")),
            "video/mp4",
            QString());
    m_network->appendDocument(
            QUrl(QLatin1String("test://video/posters/png?id=1")),
            "image/png",
            imageFileName("poster.png"));
}

void tst_QMediaImageViewer::isValid()
{
    QMediaImageViewer viewer;

    QVERIFY(viewer.service() != 0);
}

void tst_QMediaImageViewer::timeout()
{
    QMediaImageViewer viewer;

    QCOMPARE(viewer.timeout(), 3000);

    viewer.setTimeout(0);
    QCOMPARE(viewer.timeout(), 0);

    viewer.setTimeout(45);
    QCOMPARE(viewer.timeout(), 45);

    viewer.setTimeout(-3000);
    QCOMPARE(viewer.timeout(), 0);
}

void tst_QMediaImageViewer::setMedia_data()
{
    QTest::addColumn<QMediaContent>("media");

    {
        QMediaContent media(imageUri("image.png"));

        QTest::newRow("file: png image")
                << media;
    } {
        QMediaContent media(QUrl(QLatin1String("test://image/png?id=1")));

        QTest::newRow("network: png image")
                << media;
    } {
        QMediaContent media(QMediaResource(
                QUrl(QLatin1String("test://image/png?id=1")), QLatin1String("image/png")));

        QTest::newRow("network: png image, explicit mime type")
                << media;
    } {
        QMediaContent media(QUrl(QLatin1String("test://image/png?id=2")));

        QTest::newRow("network: png image, no mime type")
                << media;
    } {
        QMediaContent media(imageUri("song.mp3"));
        media.setCoverArtUriLarge(imageUri("coverart.png"));

        QTest::newRow("file: png cover art")
                << media;
    } {
        QMediaContent media(QUrl(QLatin1String("test://music/songs/mp3?id=1")));
        media.setCoverArtUriLarge(QUrl(QLatin1String("test://music/covers/large?id=1")));

        QTest::newRow("network: png cover art")
                << media;
    } {
        QMediaContent media(QMediaResource(
                QUrl(QLatin1String("test://music/songs/mp3?id=1")), QLatin1String("audio/mpeg")));
        media.setCoverArtUriSmall(QUrl(QLatin1String("test://music/covers/small?id=1")));
        media.setCoverArtUriLarge(QUrl(QLatin1String("test://music/covers/large?id=1")));

        QTest::newRow("network: png cover art, explicit mime type")
                << media;
    } {
        QMediaContent media(imageUri("movie.mp4"));
        media.setPosterUri(imageUri("poster.png"));

        QTest::newRow("file: png poster")
                << media;
    } {
        QMediaContent media(QUrl(QLatin1String("test/movies/mp4?id=1")));
        media.setPosterUri(QUrl(QLatin1String("test://video/posters/png?id=1")));

        QTest::newRow("network: png poster")
                << media;
    } {
        QMediaContent media(imageUri("image.png"));
        media.setCoverArtUriLarge(imageUri("coverart.png"));
        media.setPosterUri(imageUri("poster.png"));

        QTest::newRow("file: png image with cover art and poster")
                << media;
#ifdef QTEST_HAVE_JPEG
    } {
        QMediaContent media(imageUri("image.jpg"));

        QTest::newRow("file: jpg image")
                << media;
    } {
        QMediaContent media(QUrl(QLatin1String("test://image/jpeg?id=1")));

        QTest::newRow("network: jpg image")
                << media;
#endif
    }
}

void tst_QMediaImageViewer::setMedia()
{
    QFETCH(QMediaContent, media);

    QMediaImageViewer viewer;

    QMediaImageViewerService *service = qobject_cast<QMediaImageViewerService *>(viewer.service());
    service->setNetworkManager(m_network);

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    viewer.setMedia(media);
    QCOMPARE(viewer.media(), media);

    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

    QTestEventLoop::instance().enterLoop(2);

    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
}

void tst_QMediaImageViewer::setInvalidMedia()
{
    QMediaImageViewer viewer;

    QMediaImageViewerService *service = qobject_cast<QMediaImageViewerService *>(viewer.service());
    service->setNetworkManager(m_network);

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    {
        QMediaContent media(imageUri("invalid.png"));

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    } {
        QMediaContent media(imageUri("deleted.png"));

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    } {
        QMediaResource invalidResource(imageUri("invalid.png"));
        QMediaResource deletedResource(imageUri("deleted.png"));
        QMediaContent media(QMediaResourceList() << invalidResource << deletedResource);

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    } {
        QMediaResource resource(imageUri("image.png"), QLatin1String("audio/mpeg"));
        QMediaContent media(resource);

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    } {
        QMediaResource audioResource(imageUri("image.png"), QLatin1String("audio/mpeg"));
        QMediaResource invalidResource(imageUri("invalid.png"));
        QMediaContent media(QMediaResourceList() << audioResource << invalidResource);

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    } {
        QMediaContent media(QUrl(QLatin1String("test://image/invalid?id=1")));

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    } {
        QMediaContent media(QUrl(QLatin1String("test://image/invalid?id=2")));

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    } {
        QMediaContent media(QUrl(QLatin1String("test://image/invalid?id=3")));

        viewer.setMedia(media);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.media(), media);
    }
}

void tst_QMediaImageViewer::playlist()
{
    QMediaContent imageMedia(imageUri("image.png"));
    QMediaContent posterMedia(imageUri("poster.png"));
    QMediaContent coverArtMedia(imageUri("coverart.png"));

    QMediaImageViewer viewer;
    viewer.setTimeout(250);

    QVideoWidget widget(&viewer);
    widget.show();

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    QSignalSpy stateSpy(&viewer, SIGNAL(stateChanged(QMediaImageViewer::State)));

    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);

    // No playlist so can't exit stopped state.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);
    viewer.pause();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);

    QMediaPlaylist playlist(&viewer);

    // Empty playlist so can't exit stopped state.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);
    viewer.pause();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);

    playlist.appendItem(imageMedia);
    playlist.appendItem(posterMedia);
    playlist.appendItem(coverArtMedia);

    // Play progresses immediately to the first image and starts loading.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(0).value(0)),
             QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 0);
    QCOMPARE(viewer.media(), imageMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 0);

    // Time out causes progression to second image, which starts loading.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 1);
    QCOMPARE(viewer.media(), posterMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 1);

    // Pausing stops progression at current image.
    viewer.pause();
    QCOMPARE(viewer.state(), QMediaImageViewer::PausedState);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(1).value(0)),
             QMediaImageViewer::PausedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 1);

    // No time out.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::PausedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 1);

    // Resuming playback moves progress directly to the next image.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(stateSpy.count(), 3);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(2).value(0)),
             QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 2);
    QCOMPARE(viewer.media(), coverArtMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 2);

    // Time out causes progression to end of list
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(3).value(0)),
             QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);
    QCOMPARE(viewer.media(), QMediaContent());

    // Paused, no time out.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);

    // Play progresses immediately to the first image and starts loading.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(4).value(0)),
             QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 0);
    QCOMPARE(viewer.media(), imageMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 0);

    // Stop ends progress and removed
    viewer.stop();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(5).value(0)),
             QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);
    QCOMPARE(viewer.media(), QMediaContent());

    // Stoppped, No time out.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);
}

void tst_QMediaImageViewer::outputControl()
{
    QMediaImageViewer viewer;

    QMediaService *service = viewer.service();
    if (service == 0)
        QSKIP("Image viewer object has no service.", SkipSingle);

    QVideoOutputControl *outputControl = qobject_cast<QVideoOutputControl *>(
            service->control(QVideoOutputControl_iid));

    QVERIFY(outputControl != 0);

    QVERIFY(outputControl->availableOutputs().contains(QVideoOutputControl::WidgetOutput));
#ifndef QT_NO_MULTIMEDIA
    QVERIFY(outputControl->availableOutputs().contains(QVideoOutputControl::RendererOutput));
#endif
    QVERIFY(!outputControl->availableOutputs().contains(QVideoOutputControl::UserOutput));

    QCOMPARE(outputControl->output(), QVideoOutputControl::NoOutput);

    outputControl->setOutput(QVideoOutputControl::WidgetOutput);
    QCOMPARE(outputControl->output(), QVideoOutputControl::WidgetOutput);

#ifndef QT_NO_MULTIMEDIA
    outputControl->setOutput(QVideoOutputControl::RendererOutput);
    QCOMPARE(outputControl->output(), QVideoOutputControl::RendererOutput);
#endif

    outputControl->setOutput(QVideoOutputControl::UserOutput);
    QCOMPARE(outputControl->output(), QVideoOutputControl::NoOutput);
}

void tst_QMediaImageViewer::widgetControl()
{
    QMediaImageViewer viewer;

    QMediaService *service = viewer.service();
    if (service == 0)
        QSKIP("Image viewer object has no service.", SkipSingle);

    QVideoOutputControl *outputControl = qobject_cast<QVideoOutputControl *>(
            service->control(QVideoOutputControl_iid));
    if (outputControl == 0)
        QSKIP("Image viewer object has no video output control.", SkipSingle);

    QVideoWidgetControl *widgetControl = qobject_cast<QVideoWidgetControl *>(
            service->control(QVideoWidgetControl_iid));
    QVERIFY(widgetControl != 0);

    QWidget *widget = widgetControl->videoWidget();
    QVERIFY(widget != 0);

    outputControl->setOutput(QVideoOutputControl::WidgetOutput);

    // Test initial values.
    QCOMPARE(widgetControl->brightness(), 0);
    QCOMPARE(widgetControl->contrast(), 0);
    QCOMPARE(widgetControl->hue(), 0);
    QCOMPARE(widgetControl->saturation(), 0);
    QCOMPARE(widgetControl->isFullScreen(), false);
    QCOMPARE(widgetControl->aspectRatioMode(), QVideoWidget::KeepAspectRatio);

    // Test setting color adjustment values.  Unsupported, so they stay at 0.
    widgetControl->setBrightness(12);
    QCOMPARE(widgetControl->brightness(), 0);
    widgetControl->setContrast(12);
    QCOMPARE(widgetControl->contrast(), 0);
    widgetControl->setHue(12);
    QCOMPARE(widgetControl->hue(), 0);
    widgetControl->setSaturation(12);
    QCOMPARE(widgetControl->saturation(), 0);

    {   // Test full screen mode.  Doesn't do anything more than adknowledge the change in mode.
        QSignalSpy fullScreenSpy(widgetControl, SIGNAL(fullScreenChanged(bool)));

        widgetControl->setFullScreen(true);
        QCOMPARE(widgetControl->isFullScreen(), true);
        QCOMPARE(fullScreenSpy.count(), 1);
        QCOMPARE(fullScreenSpy.value(0).value(0).toBool(), true);

        widgetControl->setFullScreen(false);
        QCOMPARE(widgetControl->isFullScreen(), false);
        QCOMPARE(fullScreenSpy.count(), 2);
        QCOMPARE(fullScreenSpy.value(1).value(0).toBool(), false);
    }

    // Load an image so the viewer has some dimensions to work with.
    viewer.setMedia(QMediaContent(imageUri("image.png")));

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(2);

    if (viewer.mediaStatus() != QMediaImageViewer::LoadedMedia)
        QSKIP("failed to load test image", SkipSingle);


    QCOMPARE(widget->sizeHint(), QSize(75, 50));

    widgetControl->setAspectRatioMode(QVideoWidget::IgnoreAspectRatio);
    QCOMPARE(widgetControl->aspectRatioMode(), QVideoWidget::IgnoreAspectRatio);
}

#ifndef QT_NO_MULTIMEDIA
void tst_QMediaImageViewer::rendererControl()
{
    QtTestVideoSurface surface;

    QMediaImageViewer viewer;

    QMediaService *service = viewer.service();
    if (service == 0)
        QSKIP("Image viewer object has no service.", SkipSingle);

    QVideoOutputControl *outputControl = qobject_cast<QVideoOutputControl *>(
            service->control(QVideoOutputControl_iid));
    if (outputControl == 0)
        QSKIP("Image viewer object has no video output control.", SkipSingle);

    QVideoRendererControl *rendererControl = qobject_cast<QVideoRendererControl *>(
            service->control(QVideoRendererControl_iid));
    QVERIFY(rendererControl != 0);

    rendererControl->setSurface(&surface);
    QCOMPARE(rendererControl->surface(), &surface);

    outputControl->setOutput(QVideoOutputControl::RendererOutput);

    // Load an image so the viewer has some dimensions to work with.
    viewer.setMedia(QMediaContent(imageUri("image.png")));

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(2);

    if (viewer.mediaStatus() != QMediaImageViewer::LoadedMedia)
        QSKIP("failed to load test image", SkipSingle);

    QCOMPARE(surface.isStarted(), true);

    QVideoSurfaceFormat format = surface.surfaceFormat();
    QCOMPARE(format.handleType(), QAbstractVideoBuffer::NoHandle);
    QCOMPARE(format.pixelFormat(), QVideoFrame::Format_RGB32);
    QCOMPARE(format.frameSize(), QSize(75, 50));

    QVideoFrame frame = surface.frame();
    QCOMPARE(frame.handleType(), QAbstractVideoBuffer::NoHandle);
    QCOMPARE(frame.pixelFormat(), QVideoFrame::Format_RGB32);
    QCOMPARE(frame.size(), QSize(75, 50));

    viewer.setMedia(QMediaContent());

    QCOMPARE(surface.isStarted(), false);
}
#endif

QTEST_MAIN(tst_QMediaImageViewer)

#include "tst_qmediaimageviewer.moc"
