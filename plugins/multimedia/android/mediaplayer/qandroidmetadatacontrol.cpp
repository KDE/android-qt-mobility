/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its
contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit
(EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QtCore/qdebug.h>

#include <media/mediametadataretriever.h>

#include "qandroidmetadatacontrol.h"
#include "qandroidplayersession.h"
#include "mediaPlayerJNI.h"

QAndroidMetaDataControl::QAndroidMetaDataControl(QAndroidPlayerSession *session, QObject *parent)
    :QMetaDataReaderControl(parent),m_session(session)
{
    connect(m_session, SIGNAL(tagsChanged()), SLOT(updateTags()));
}

QAndroidMetaDataControl::~QAndroidMetaDataControl()
{

}

bool QAndroidMetaDataControl::isMetaDataAvailable() const
{
    return m_session->isMetaDataAvailable();
}

bool QAndroidMetaDataControl::isWritable() const
{
    return false;
}

// Currently not supporting Date
QVariant QAndroidMetaDataControl::metaData(QtMultimediaKit::MetaData key) const
{
    switch(key)
    {
    case QtMultimediaKit::Title:
        return m_session->getMetaData(android::METADATA_KEY_TITLE);
    case QtMultimediaKit::Author:
        return m_session->getMetaData(android::METADATA_KEY_AUTHOR);
    case QtMultimediaKit::Comment:
        return m_session->getMetaData(android::METADATA_KEY_COMMENT);
    case QtMultimediaKit::Genre:
        return m_session->getMetaData(android::METADATA_KEY_GENRE);
    case QtMultimediaKit::Year:
        return m_session->getMetaData(android::METADATA_KEY_YEAR);
    case QtMultimediaKit::MediaType:
        return m_session->getMetaData(android::METADATA_KEY_MIMETYPE);
    case QtMultimediaKit::UserRating:
        return m_session->getMetaData(android::METADATA_KEY_RATING);
    case QtMultimediaKit::Duration:
        return m_session->getMetaData(android::METADATA_KEY_DURATION);
    case QtMultimediaKit::AlbumArtist:
        return m_session->getMetaData(android::METADATA_KEY_ALBUMARTIST);
    case QtMultimediaKit::AlbumTitle:
        return m_session->getMetaData(android::METADATA_KEY_ALBUM);
    case QtMultimediaKit::AudioBitRate:
        return m_session->getMetaData(android::METADATA_KEY_BIT_RATE);
    case QtMultimediaKit::Composer:
        return m_session->getMetaData(android::METADATA_KEY_COMPOSER);
    case QtMultimediaKit::TrackNumber:
        return m_session->getMetaData(android::METADATA_KEY_CD_TRACK_NUMBER);
    case QtMultimediaKit::Writer:
        return m_session->getMetaData(android::METADATA_KEY_WRITER);
    case QtMultimediaKit::AudioCodec:
        return m_session->getMetaData(android::METADATA_KEY_CODEC);
    case QtMultimediaKit::VideoFrameRate:
        return m_session->getMetaData(android::METADATA_KEY_FRAME_RATE);
    case QtMultimediaKit::Copyright:
        return m_session->getMetaData(android::METADATA_KEY_COPYRIGHT);
    case QtMultimediaKit::TrackCount:
        return m_session->getMetaData(android::METADATA_KEY_NUM_TRACKS);
    default:
        return QVariant();
    }
}

QList<QtMultimediaKit::MetaData> QAndroidMetaDataControl::availableMetaData() const
{
    return m_session->availableMetaData();
}
//We are not supporting extended meta data
QVariant QAndroidMetaDataControl::extendedMetaData(const QString &key) const
{
    Q_UNUSED(key);
    return QVariant();
}

//We are not supporting extended meta data
QStringList QAndroidMetaDataControl::availableExtendedMetaData() const
{
    QStringList res;
    return res;
}

void QAndroidMetaDataControl::updateTags()
{
    emit metaDataChanged();
}
