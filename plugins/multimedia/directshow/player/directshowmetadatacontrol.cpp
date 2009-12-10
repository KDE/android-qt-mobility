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

#include "directshowmetadatacontrol.h"

#include "directshowplayerservice.h"

#include <initguid.h>
#include <qnetwork.h>
#include <wmsdk.h>


namespace
{
    struct QWMMetaDataKeyLookup
    {
        QtMedia::MetaData key;
        const wchar_t *token;
    };
}

static const QWMMetaDataKeyLookup qt_wmMetaDataKeys[] =
{
    { QtMedia::Title, L"Title" },
    { QtMedia::SubTitle, L"WM/SubTitle" },
    { QtMedia::Author, L"Author" },
    { QtMedia::Comment, L"Comment" },
    { QtMedia::Description, L"Description" },
    { QtMedia::Category, L"WM/Category" },
    { QtMedia::Genre, L"WM/Genre" },
    //{ QtMedia::Date, 0 },
    { QtMedia::Year, L"WM/Year" },
    { QtMedia::UserRating, L"UserRating" },
    //{ QtMedia::MetaDatawords, 0 },
    { QtMedia::Language, L"Language" },
    { QtMedia::Publisher, L"WM/Publisher" },
    { QtMedia::Copyright, L"Copyright" },
    { QtMedia::ParentalRating, L"ParentalRating" },
    { QtMedia::RatingOrganisation, L"RatingOrganisation" },

    // Media
    { QtMedia::Size, L"FileSize" },
    { QtMedia::MediaType, L"MediaType" },
    { QtMedia::Duration, L"Duration" },

    // Audio
    { QtMedia::AudioBitRate, L"AudioBitRate" },
    { QtMedia::AudioCodec, L"AudioCodec" },
    { QtMedia::ChannelCount, L"ChannelCount" },
    { QtMedia::Frequency, L"Frequency" },

    // Music
    { QtMedia::AlbumTitle, L"WM/AlbumTitle" },
    { QtMedia::AlbumArtist, L"WM/AlbumArtist" },
    { QtMedia::ContributingArtist, L"Author" },
    { QtMedia::Composer, L"WM/Composer" },
    { QtMedia::Conductor, L"WM/Conductor" },
    { QtMedia::Lyrics, L"WM/Lyrics" },
    { QtMedia::Mood, L"WM/Mood" },
    { QtMedia::TrackNumber, L"WM/TrackNumber" },
    //{ QtMedia::TrackCount, 0 },
    //{ QtMedia::CoverArtUriSmall, 0 },
    //{ QtMedia::CoverArtUriLarge, 0 },

    // Image/Video
    //{ QtMedia::Resolution, 0 },
    //{ QtMedia::PixelAspectRatio, 0 },

    // Video
    //{ QtMedia::FrameRate, 0 },
    { QtMedia::VideoBitRate, L"VideoBitRate" },
    { QtMedia::VideoCodec, L"VideoCodec" },

    //{ QtMedia::PosterUri, 0 },

    // Movie
    { QtMedia::ChapterNumber, L"ChapterNumber" },
    { QtMedia::Director, L"WM/Director" },
    { QtMedia::LeadPerformer, L"LeadPerformer" },
    { QtMedia::Writer, L"WM/Writer" },

    // Photos
    { QtMedia::CameraManufacturer, L"CameraManufacturer" },
    { QtMedia::CameraModel, L"CameraModel" },
    { QtMedia::Event, L"Event" },
    { QtMedia::Subject, L"Subject" }
};

static QVariant getValue(IWMHeaderInfo *header, const wchar_t *key)
{
    WORD streamNumber = 0;
    WMT_ATTR_DATATYPE type = WMT_TYPE_DWORD;
    WORD size = 0;

    if (header->GetAttributeByName(&streamNumber, key, &type, 0, &size) == S_OK) {
        switch (type) {
        case WMT_TYPE_DWORD:
            if (size == sizeof(DWORD)) {
                DWORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return int(word);
                }
            }
            break;
        case WMT_TYPE_STRING:
            {
                QString string;
                string.resize(size / 2 - 1);

                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(const_cast<ushort *>(string.utf16())),
                        &size) == S_OK) {
                    return string;
                }
            }
            break;
        case WMT_TYPE_BINARY:
            {
                QByteArray bytes;
                bytes.resize(size);
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(bytes.data()),
                        &size) == S_OK) {
                    return bytes;
                }
            }
            break;
        case WMT_TYPE_BOOL:
            if (size == sizeof(DWORD)) {
                DWORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return bool(word);
                }
            }
            break;
        case WMT_TYPE_QWORD:
            if (size == sizeof(QWORD)) {
                QWORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return qint64(word);
                }
            }
            break;
        case WMT_TYPE_WORD:
            if (size == sizeof(WORD)){
                WORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key, 
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return short(word);
                }
            }
            break;
        case WMT_TYPE_GUID:
            if (size == 16) {
            }
            break;
        default:
            break;
        }
    }
    return QVariant();
}

DirectShowMetaDataControl::DirectShowMetaDataControl(
        DirectShowPlayerService *service, QObject *parent)
    : QMetaDataControl(parent)
    , m_service(service)
{
}

DirectShowMetaDataControl::~DirectShowMetaDataControl()
{
}

bool DirectShowMetaDataControl::isWritable() const
{
    return false;
}

bool DirectShowMetaDataControl::isMetaDataAvailable() const
{
    IAMMediaContent *content = 0;
    if (m_service->graph() && m_service->graph()->QueryInterface(
            IID_IAMMediaContent, reinterpret_cast<void **>(&content)) == S_OK) {
        content->Release();
        return true;
    } else if (IWMHeaderInfo *header = com_cast<IWMHeaderInfo>(m_service->source())) {
        header->Release();
        return true;
    } else {
        return false;
    }
}

QVariant DirectShowMetaDataControl::metaData(QtMedia::MetaData key) const
{
    QVariant value;

    IAMMediaContent *content = 0;
    if (m_service->graph() && m_service->graph()->QueryInterface(
            IID_IAMMediaContent, reinterpret_cast<void **>(&content)) == S_OK) {
        BSTR string = 0;

        switch (key) {
        case QtMedia::Author:
            content->get_AuthorName(&string);
            break;
        case QtMedia::Title:
            content->get_Title(&string);
            break;
        case QtMedia::ParentalRating:
            content->get_Rating(&string);
            break;
        case QtMedia::Description:
            content->get_Description(&string);
            break;
        case QtMedia::Copyright:
            content->get_Copyright(&string);
            break;
        default:
            break;
        }

        if (string) {
            value = QString::fromUtf16(string, ::SysStringLen(string));

            ::SysFreeString(string);
        }
        content->Release();
    } else if (IWMHeaderInfo *header = com_cast<IWMHeaderInfo>(m_service->source())) {
        static const int  count = sizeof(qt_wmMetaDataKeys) / sizeof(QWMMetaDataKeyLookup);
        for (int i = 0; i < count; ++i) {
            if (qt_wmMetaDataKeys[i].key == key) {
                value = getValue(header, qt_wmMetaDataKeys[i].token);
                break;
            }
        }
    }
    return value;
}

void DirectShowMetaDataControl::setMetaData(QtMedia::MetaData, const QVariant &)
{
}

QList<QtMedia::MetaData> DirectShowMetaDataControl::availableMetaData() const
{
    return QList<QtMedia::MetaData>();
}

QVariant DirectShowMetaDataControl::extendedMetaData(const QString &) const
{
    return QVariant();
}

void DirectShowMetaDataControl::setExtendedMetaData(const QString &, const QVariant &)
{
}

QStringList DirectShowMetaDataControl::availableExtendedMetaData() const
{
    return QStringList();
}

