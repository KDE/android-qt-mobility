/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore/qtimer.h>

#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qmediaplayer.h>

QT_USE_NAMESPACE
class tst_QMediaObject : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void isMetaDataAvailable();
    void metaData();
    void availableMetaData();
    void extendedMetaData();
    void availableExtendedMetaData();

private:
    QString metaDataKeyAsString(QtMultimediaKit::MetaData key) const;
};

void tst_QMediaObject::initTestCase_data()
{
    QTest::addColumn<bool>("valid");
    //QTest::addColumn<QMediaPlayer::State>("state");
    //QTest::addColumn<QMediaPlayer::MediaStatus>("status");
    QTest::addColumn<QMediaContent>("mediaContent");
    QTest::addColumn<bool>("metaDataAvailable");
    QTest::addColumn<bool>("metaDataWritable"); // Is this needed ???
    //QTest::addColumn<int>("bufferStatus");
    //QTest::addColumn<qreal>("playbackRate");
    //QTest::addColumn<QMediaPlayer::Error>("error");
    //QTest::addColumn<QString>("errorString");

    QTest::newRow("TestDataNull")
    << false // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent() // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    /*
    QTest::newRow("test_3gp.3gp")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_3gp.3gp")) // mediaContent
    << qint64(-1) // duration
    << qint64(0) // position
    << false // seekable
    << 0 // volume
    << false // muted
    << false; // videoAvailable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString
    */

    QTest::newRow("test_amr.amr")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_amr.amr")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_flash_video.flv")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_flash_video.flv")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_invalid_extension_mp4.xyz")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_invalid_extension_mp4.xyz")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_invalid_extension_wav.xyz")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_invalid_extension_wav.xyz")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_mp3.mp3")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_mp3.mp3")) // mediaContent
#if !defined(__WINS__) || !defined(__WINSCW__)
    << true // metaDataAvailable
#else
    << false // metaDataAvailable
#endif // !defined(__WINS__) || defined(__WINSCW__)
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_mp3_no_metadata.mp3")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_mp3_no_metadata.mp3")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_mp4.mp4")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_mp4.mp4")) // mediaContent
#if defined(__WINS__) || defined(__WINSCW__)
    << true // metaDataAvailable
#else
    << false // metaDataAvailable
#endif // !defined(__WINS__) || defined(__WINSCW__)
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_wav.wav")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_wav.wav")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_wmv9.wmv")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_wmv9.wmv")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString


    QTest::newRow("test youtube stream")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("rtsp://v3.cache4.c.youtube.com/CkgLENy73wIaPwlU2rm7yu8PFhMYESARFEIJbXYtZ29vZ2xlSARSB3JlbGF0ZWRaDkNsaWNrVGh1bWJuYWlsYPi6_IXT2rvpSgw=/0/0/0/video.3gp")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

}

void tst_QMediaObject::initTestCase()
{
}

void tst_QMediaObject::cleanupTestCase()
{
}

void tst_QMediaObject::init()
{
    qRegisterMetaType<QMediaContent>("QMediaContent");
}

void tst_QMediaObject::cleanup()
{
}

void tst_QMediaObject::isMetaDataAvailable()
{
    QFETCH_GLOBAL(bool, metaDataAvailable);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700); 
    QVERIFY(player.isMetaDataAvailable() == metaDataAvailable);
}

void tst_QMediaObject::metaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(bool, metaDataAvailable);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    const QString artist(QLatin1String("Artist"));
    const QString title(QLatin1String("Title"));

    if (player.isMetaDataAvailable()) {
        QEXPECT_FAIL("", "player.metaData(QtMultimediaKit::AlbumArtist) failed: ", Continue);
        QCOMPARE(player.metaData(QtMultimediaKit::AlbumArtist).toString(), artist);
        QEXPECT_FAIL("", "player.metaData(QtMultimediaKit::Title) failed: ", Continue);
        QCOMPARE(player.metaData(QtMultimediaKit::Title).toString(), title);
    }
}

void tst_QMediaObject::availableMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(bool, metaDataAvailable);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);    

    if (player.isMetaDataAvailable()) {
        QList<QtMultimediaKit::MetaData> metaDataKeys = player.availableMetaData();
        QEXPECT_FAIL("", "metaDataKeys.count() failed: ", Continue);
        QVERIFY(metaDataKeys.count() > 0);
//        qWarning() << "metaDataKeys.count: " << metaDataKeys.count();
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMultimediaKit::AlbumArtist) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(QtMultimediaKit::AlbumArtist));
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMultimediaKit::Title) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(QtMultimediaKit::Title));
    }
}

void tst_QMediaObject::extendedMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    const QString artist(QLatin1String("Artist"));
    const QString title(QLatin1String("Title"));

    if (player.isMetaDataAvailable()) {  
        QEXPECT_FAIL("", "player.extendedMetaData(QtMultimediaKit::AlbumArtist) failed: ", Continue);
        QCOMPARE(player.extendedMetaData(metaDataKeyAsString(QtMultimediaKit::AlbumArtist)).toString(), artist);
        QEXPECT_FAIL("", "player.extendedMetaData(QtMultimediaKit::Title) failed: ", Continue);
        QCOMPARE(player.extendedMetaData(metaDataKeyAsString(QtMultimediaKit::Title)).toString(), title);
    }
}

void tst_QMediaObject::availableExtendedMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    const QString artist(QLatin1String("Artist"));
    const QString title(QLatin1String("Title"));
    
    if (player.isMetaDataAvailable()) {
        QStringList metaDataKeys = player.availableExtendedMetaData();
        QVERIFY(metaDataKeys.count() > 0);
/*        qWarning() << "metaDataKeys.count: " << metaDataKeys.count();
        int count = metaDataKeys.count();
        count = count-1;
        int i = 0;
        while(count >= i)
            {
            qWarning() << "metaDataKeys " << i <<". " << metaDataKeys.at(i);
            i++;
            }*/
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMultimediaKit::AlbumArtist) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(metaDataKeyAsString(QtMultimediaKit::AlbumArtist)));
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMultimediaKit::AlbumArtist) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(metaDataKeyAsString(QtMultimediaKit::Title)));
    }
}

QString tst_QMediaObject::metaDataKeyAsString(QtMultimediaKit::MetaData key) const
{
    switch(key) {
        case QtMultimediaKit::Title: return "title";
        case QtMultimediaKit::AlbumArtist: return "artist";
        case QtMultimediaKit::Comment: return "comment";
        case QtMultimediaKit::Genre: return "genre";
        case QtMultimediaKit::Year: return "year";
        case QtMultimediaKit::Copyright: return "copyright";
        case QtMultimediaKit::AlbumTitle: return "album";
        case QtMultimediaKit::Composer: return "composer";
        case QtMultimediaKit::TrackNumber: return "albumtrack";
        case QtMultimediaKit::AudioBitRate: return "audiobitrate";
        case QtMultimediaKit::VideoBitRate: return "videobitrate";
        case QtMultimediaKit::Duration: return "duration";
        case QtMultimediaKit::MediaType: return "contenttype";
        case QtMultimediaKit::SubTitle: // TODO: Find the matching metadata keys
        case QtMultimediaKit::Description:
        case QtMultimediaKit::Category:
        case QtMultimediaKit::Date:
        case QtMultimediaKit::UserRating:
        case QtMultimediaKit::Keywords:
        case QtMultimediaKit::Language:
        case QtMultimediaKit::Publisher:
        case QtMultimediaKit::ParentalRating:
        case QtMultimediaKit::RatingOrganisation:
        case QtMultimediaKit::Size:
        case QtMultimediaKit::AudioCodec:
        case QtMultimediaKit::AverageLevel:
        case QtMultimediaKit::ChannelCount:
        case QtMultimediaKit::PeakValue:
        case QtMultimediaKit::SampleRate:
        case QtMultimediaKit::Author:
        case QtMultimediaKit::ContributingArtist:
        case QtMultimediaKit::Conductor:
        case QtMultimediaKit::Lyrics:
        case QtMultimediaKit::Mood:
        case QtMultimediaKit::TrackCount:
        case QtMultimediaKit::CoverArtUrlSmall:
        case QtMultimediaKit::CoverArtUrlLarge:
        case QtMultimediaKit::Resolution:
        case QtMultimediaKit::PixelAspectRatio:
        case QtMultimediaKit::VideoFrameRate:
        case QtMultimediaKit::VideoCodec:
        case QtMultimediaKit::PosterUrl:
        case QtMultimediaKit::ChapterNumber:
        case QtMultimediaKit::Director:
        case QtMultimediaKit::LeadPerformer:
        case QtMultimediaKit::Writer:
        case QtMultimediaKit::CameraManufacturer:
        case QtMultimediaKit::CameraModel:
        case QtMultimediaKit::Event:
        case QtMultimediaKit::Subject:
        default:
            break;
    }

    return QString();
}

QTEST_MAIN(tst_QMediaObject)

#include "tst_qmediaobject_s60.moc"
