/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This device is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This device contains pre-release code and may not be distributed.
** You may use this device in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this device may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the device LICENSE.LGPL included in the
** packaging of this device.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the device LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this device, please contact
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

#include <QtCore/qendian.h>
#include "wavheader.h"


struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
    DATAHeader  data;
};

static const int HeaderLength = sizeof(CombinedHeader);


WavHeader::WavHeader(const QAudioFormat &format, qint64 dataLength)
    :   m_format(format)
    ,   m_dataLength(dataLength)
{

}

bool WavHeader::read(QIODevice &device)
{
    bool result = true;

    if (!device.isSequential())
        result = device.seek(0);
    // else, assume that current position is the start of the header

    if (result) {
        CombinedHeader header;
        result = (device.read(reinterpret_cast<char *>(&header), HeaderLength) == HeaderLength);
        if (result) {
            if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
                || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
                && memcmp(&header.riff.type, "WAVE", 4) == 0
                && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
                && header.wave.audioFormat == 1 // PCM
            ) {
                if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                    m_format.setByteOrder(QAudioFormat::LittleEndian);
                else
                    m_format.setByteOrder(QAudioFormat::BigEndian);

                m_format.setChannels(qFromLittleEndian<quint16>(header.wave.numChannels));
                m_format.setCodec("audio/pcm");
                m_format.setFrequency(qFromLittleEndian<quint32>(header.wave.sampleRate));
                m_format.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));

                switch(header.wave.bitsPerSample) {
                case 8:
                    m_format.setSampleType(QAudioFormat::UnSignedInt);
                    break;
                case 16:
                    m_format.setSampleType(QAudioFormat::SignedInt);
                    break;
                default:
                    result = false;
                }

                m_dataLength = device.size() - HeaderLength;
            } else {
                result = false;
            }
        }
    }

    return result;
}

bool WavHeader::write(QIODevice &device)
{
    CombinedHeader header;

    memset(&header, 0, HeaderLength);

    // RIFF header
    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
        memcpy(header.riff.descriptor.id,"RIFF",4);
    else
        memcpy(header.riff.descriptor.id,"RIFX",4);
    qToLittleEndian<quint32>(quint32(m_dataLength + HeaderLength - 8),
                             reinterpret_cast<unsigned char*>(&header.riff.descriptor.size));
    memcpy(header.riff.type, "WAVE",4);

    // WAVE header
    memcpy(header.wave.descriptor.id,"fmt ",4);
    qToLittleEndian<quint32>(quint32(16),
                             reinterpret_cast<unsigned char*>(&header.wave.descriptor.size));
    qToLittleEndian<quint16>(quint16(1),
                             reinterpret_cast<unsigned char*>(&header.wave.audioFormat));
    qToLittleEndian<quint16>(quint16(m_format.channels()),
                             reinterpret_cast<unsigned char*>(&header.wave.numChannels));
    qToLittleEndian<quint32>(quint32(m_format.frequency()),
                             reinterpret_cast<unsigned char*>(&header.wave.sampleRate));
    qToLittleEndian<quint32>(quint32(m_format.frequency() * m_format.channels() * m_format.sampleSize() / 8),
                             reinterpret_cast<unsigned char*>(&header.wave.byteRate));
    qToLittleEndian<quint16>(quint16(m_format.channels() * m_format.sampleSize() / 8),
                             reinterpret_cast<unsigned char*>(&header.wave.blockAlign));
    qToLittleEndian<quint16>(quint16(m_format.sampleSize()),
                             reinterpret_cast<unsigned char*>(&header.wave.bitsPerSample));

    // DATA header
    memcpy(header.data.descriptor.id,"data",4);
    qToLittleEndian<quint32>(quint32(m_dataLength),
                             reinterpret_cast<unsigned char*>(&header.data.descriptor.size));

    return (device.write(reinterpret_cast<const char *>(&header), HeaderLength) == HeaderLength);
}

const QAudioFormat& WavHeader::format() const
{
    return m_format;
}

qint64 WavHeader::dataLength() const
{
    return m_dataLength;
}

qint64 WavHeader::headerLength()
{
    return HeaderLength;
}

bool WavHeader::writeDataLength(QIODevice &device, qint64 dataLength)
{
    bool result = false;
    if (!device.isSequential()) {
        device.seek(40);
        unsigned char dataLengthLE[4];
        qToLittleEndian<quint32>(quint32(dataLength), dataLengthLE);
        result = (device.write(reinterpret_cast<const char *>(dataLengthLE), 4) == 4);
    }
    return result;
}
