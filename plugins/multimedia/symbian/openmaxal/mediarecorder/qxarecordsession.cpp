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

#include <QVariant>
#include <qtmedianamespace.h>
#include "qxarecordsession.h"
#include "xarecordsessionimpl.h"
#include "qxacommon.h"

/* The following declaration is required to allow QList<int> to be added to
 * QVariant
 */
Q_DECLARE_METATYPE(QList<int>)

/* This macro checks for m_impl null pointer. If it is, emits an error signal
 * error(QMediaRecorder::ResourceError, tr("Service has not been started"));
 * and returns from function immediately with value 's'.
 */

#define RETURN_s_IF_m_impl_IS_NULL(s) \
    if (!m_impl) { \
        emit error(QMediaRecorder::ResourceError, tr("Service has not been started")); \
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Service has not been started\"))"); \
        return s; \
        }

/* This macro checks for m_impl null pointer. If it is, emits an error signal
 * error(QMediaRecorder::ResourceError, tr("Service has not been started"));
 * and returns from function immediately.
 */
#define RETURN_IF_m_impl_IS_NULL \
    if (!m_impl) { \
        emit error(QMediaRecorder::ResourceError, tr("Service has not been started")); \
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Service has not been started\"))"); \
        return; \
        }

QXARecordSession::QXARecordSession(QObject *parent)
:QObject(parent),
m_state(QMediaRecorder::StoppedState)
{
    QT_TRACE_FUNCTION_ENTRY;
    m_impl = NULL;
    m_impl = new XARecordSessionImpl(*this);
    if (m_impl) {
        if (m_impl->postConstruct() == KErrNone) {
            initCodecsList();
            initContainersList();
            m_containerMimeType = QString();
            m_audioencodersettings.setBitRate(0);
            m_audioencodersettings.setChannelCount(-1);
            m_audioencodersettings.setEncodingMode(QtMedia::ConstantQualityEncoding);
            m_audioencodersettings.setQuality(QtMedia::NormalQuality);
            m_audioencodersettings.setSampleRate(-1);
            QT_TRACE1("Initialized implementation");
        }
        else {
            delete m_impl;
            m_impl = NULL;
            QT_TRACE1("Error initializing implementation");
        }
    }
    else {
        emit error(QMediaRecorder::ResourceError, tr("Unable to start Service"));
    }
    QT_TRACE_FUNCTION_EXIT;
}

QXARecordSession::~QXARecordSession()
{
    QT_TRACE_FUNCTION_ENTRY;
    delete m_impl;
    QT_TRACE_FUNCTION_EXIT;
}

QUrl QXARecordSession::outputLocation()
{
    return m_outputLocation;
}

bool QXARecordSession::setOutputLocation(const QUrl &location)
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_s_IF_m_impl_IS_NULL(false);

    // Location can be set only when recorder is in stopped state.
    if (state() != QMediaRecorder::StoppedState )
        return false;

    // Validate URL
    if (!location.isValid())
        return false;

    // If old and new locations are same, do nothing.
    QString newUrlStr = (QUrl::fromUserInput(location.toString().toLower())).toString();
    QString curUrlStr = (QUrl::fromUserInput(m_outputLocation.toString().toLower())).toString();
    if (curUrlStr.compare(newUrlStr) == KErrNone)
        return true;

    bool retVal = false;
    TPtrC16 tempPtr(reinterpret_cast<const TUint16 *>(newUrlStr.utf16()));
    if (m_impl->setURI(tempPtr) == 0) {
        m_outputLocation = location;
        retVal = true;
    }
    else {
        emit error(QMediaRecorder::ResourceError, tr("Generic error"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Generic error\"))");
    }

    QT_TRACE_FUNCTION_EXIT;
    return retVal;
}

QMediaRecorder::State QXARecordSession::state()
{
    return m_state;
}

qint64 QXARecordSession::duration()
{
    TInt64 dur(0);

    QT_TRACE_FUNCTION_ENTRY;

    RETURN_s_IF_m_impl_IS_NULL(dur);

    m_impl->duration(dur);

    QT_TRACE_FUNCTION_EXIT;
    return (qint64)dur;
}

void QXARecordSession::applySettings()
{
    if (m_appliedaudioencodersettings != m_audioencodersettings)
        setEncoderSettingsToImpl();
}

void QXARecordSession::record()
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_IF_m_impl_IS_NULL;

    /* 1. Set encoder settings here */
    if (m_appliedaudioencodersettings != m_audioencodersettings)
        RET_IF_FALSE(setEncoderSettingsToImpl());

    /* 2. Start recording...
     * If successful, setRecorderState(QMediaRecorder::RecordingState);
     * will be called from the callback cbRecordingStarted()
     */
    if (m_impl->record() != KErrNone) {
        emit error(QMediaRecorder::ResourceError, tr("Generic error"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Generic error\"))");
        }

    QT_TRACE_FUNCTION_EXIT;
}

void QXARecordSession::pause()
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_IF_m_impl_IS_NULL;

    if (m_impl->pause() == KErrNone) {
        setRecorderState(QMediaRecorder::PausedState);
    }
    else {
        emit error(QMediaRecorder::ResourceError, tr("Unable to pause"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Unable to pause\"))");
    }

    QT_TRACE_FUNCTION_EXIT;
}

void QXARecordSession::stop()
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_IF_m_impl_IS_NULL;

    if ((m_impl->stop() == KErrNone)) {
        setRecorderState(QMediaRecorder::StoppedState);
    }
    else {
        emit error(QMediaRecorder::ResourceError, tr("Unable to stop"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Unable to stop\"))");
    }

    QT_TRACE_FUNCTION_EXIT;
}

void QXARecordSession::cbDurationChanged(TInt64 new_pos)
{
    emit durationChanged((qint64)new_pos);
    SIGNAL_EMIT_TRACE1("emit durationChanged((qint64)new_pos);");
}

void QXARecordSession::cbAvailableAudioInputsChanged()
{
    emit availableAudioInputsChanged();
    SIGNAL_EMIT_TRACE1("emit availableAudioInputsChanged();");
}

void QXARecordSession::cbRecordingStarted()
{
    setRecorderState(QMediaRecorder::RecordingState);
}

void QXARecordSession::cbRecordingStopped()
{
    emit error(QMediaRecorder::ResourceError, tr("Resources Unavailable"));
    SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Resources Unavailable\"))");
    setRecorderState(QMediaRecorder::StoppedState);
}

/* For QAudioEndpointSelector begin */
QList<QString> QXARecordSession::availableEndpoints()
{
    QT_TRACE_FUNCTION_ENTRY;

    QList<QString> strList;

    RETURN_s_IF_m_impl_IS_NULL(strList);

    QString str;
    RArray<TPtrC> names;
    m_impl->getAudioInputDeviceNames(names);
    for (TInt index = 0; index < names.Count(); index++) {
        str = QString((QChar*)names[index].Ptr(), names[index].Length());
        strList.append(str);
    }

    QT_TRACE_FUNCTION_EXIT;
    return strList;
}

QString QXARecordSession::endpointDescription(const QString &name)
{
    /* From AL we get only device name */
    return name;
}

QString QXARecordSession::defaultEndpoint()
{
    QT_TRACE_FUNCTION_ENTRY;

    QString str;

    RETURN_s_IF_m_impl_IS_NULL(str);

    TPtrC name;
    if(m_impl->defaultAudioInputDevice(name) == KErrNone)
        str = QString((QChar*)name.Ptr(), name.Length());

    QT_TRACE_FUNCTION_EXIT;
    return str;
}

QString QXARecordSession::activeEndpoint()
{
    QT_TRACE_FUNCTION_ENTRY;

    QString str;

    RETURN_s_IF_m_impl_IS_NULL(str);

    TPtrC name;
    if(m_impl->activeAudioInputDevice(name) == KErrNone)
        str = QString((QChar*)name.Ptr(), name.Length());

    QT_TRACE_FUNCTION_EXIT;
    return str;
}

void QXARecordSession::setActiveEndpoint(const QString &name)
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_IF_m_impl_IS_NULL;

    TPtrC16 tempPtr(reinterpret_cast<const TUint16*>(name.utf16()));
    if (m_impl->setAudioInputDevice(tempPtr) == true) {
        emit activeEndpointChanged(name);
        SIGNAL_EMIT_TRACE1("emit activeEndpointChanged(name)");
    }
    else {
        emit error(QMediaRecorder::ResourceError, tr("Invalid endpoint"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Invalid endpoint\"))");
        }

    QT_TRACE_FUNCTION_EXIT;
}
/* For QAudioEndpointSelector end */

/* For QAudioEncoderControl begin */
QStringList QXARecordSession::supportedAudioCodecs()
{
    return m_codecs;
}

QString QXARecordSession::codecDescription(const QString &codecName)
{
    if (m_codecs.contains(codecName.toLower()))
        return QString(codecName);
    return QString();
}

QList<int> QXARecordSession::supportedSampleRates(
            const QAudioEncoderSettings &settings,
            bool *continuous)
{
    QT_TRACE_FUNCTION_ENTRY;

    QList<int> srList;

    RETURN_s_IF_m_impl_IS_NULL(srList);

    QString selectedCodec = settings.codec().toLower();

    if (m_codecs.indexOf(selectedCodec) >= 0) {
        RArray<TInt32> sampleRates;
        TBool isContinuous;
        TPtrC16 tempPtr(reinterpret_cast<const TUint16*>(selectedCodec.utf16()));
        if (m_impl->getSampleRates(tempPtr, sampleRates, isContinuous) == KErrNone) {
            for (TInt index = 0; index < sampleRates.Count(); index++)
                srList.append(sampleRates[index]);
            sampleRates.Close();
            *continuous = false;
            if (isContinuous == true)
                *continuous = true;
        }
    }

    QT_TRACE_FUNCTION_EXIT;
    return srList;
}

QAudioEncoderSettings QXARecordSession::audioSettings()
{
    return m_audioencodersettings;
}

void QXARecordSession::setAudioSettings(const QAudioEncoderSettings &settings)
{
    m_audioencodersettings = settings;
}

QStringList QXARecordSession::supportedEncodingOptions(const QString &codec)
{
    QT_TRACE_FUNCTION_ENTRY;
    Q_UNUSED(codec);
    QStringList options;
    options << "bitrate";

    QT_TRACE_FUNCTION_EXIT;
    return options;
}

QVariant QXARecordSession::encodingOption(const QString &codec, const QString &name)
{
    QT_TRACE_FUNCTION_ENTRY;

    QVariant encodingOption;

    RETURN_s_IF_m_impl_IS_NULL(encodingOption);

    if (name.toLower().compare("bitrate") == 0) {
        TPtrC16 tempPtr(reinterpret_cast<const TUint16*>(codec.toLower().utf16()));
        QList<int> bitrateList;
        RArray<TUint32> bitrates;
        if (m_impl->getBitrates(tempPtr, bitrates) == KErrNone) {
            for (TInt index = 0; index < bitrates.Count(); index++)
                bitrateList.append(bitrates[index]);
            bitrates.Close();
        }
        encodingOption.setValue(bitrateList);
    }

    QT_TRACE_FUNCTION_EXIT;
    return encodingOption;
}

void QXARecordSession::setEncodingOption(
                                    const QString &codec,
                                    const QString &name,
                                    const QVariant &value)
{
    /*
     * Currently nothing can be set via this function.
     * Bitrate is set via QAudioEncoderSettings::setBitrate().
     */
    Q_UNUSED(codec);
    Q_UNUSED(name);
    Q_UNUSED(value);
}
/* For QAudioEncoderControl end */

QStringList QXARecordSession::supportedContainers()
{
    return m_containers;
}

QString QXARecordSession::containerMimeType()
{
    return m_containerMimeType;
}

void QXARecordSession::setContainerMimeType(const QString &formatMimeType)
{
    if (m_containers.indexOf(formatMimeType.toLower()) >= 0 )
        m_containerMimeType = formatMimeType;
    else {
        emit error(QMediaRecorder::FormatError, tr("Invalid container"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid container\"))");
        }
}

QString QXARecordSession::containerDescription(const QString &formatMimeType)
{
    int index = m_containers.indexOf(formatMimeType.toLower());
    if (index >= 0) {
        return m_containersDesc.at(index);
        }
    else {
        emit error(QMediaRecorder::FormatError, tr("Invalid container"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid container\"))");
    }
    return QString();
}

void QXARecordSession::setRecorderState(QMediaRecorder::State state)
{
    if (state != m_state) {
        m_state = state;
        emit stateChanged(m_state);
        SIGNAL_EMIT_TRACE1("emit stateChanged(m_state);");
    }
}

void QXARecordSession::initCodecsList()
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_IF_m_impl_IS_NULL;

    m_codecs.clear();

    const RArray<TPtrC>& names = m_impl->getAudioEncoderNames();
    QString str;

    for (TInt index = 0; index < names.Count(); index++) {
        str = QString((QChar*)names[index].Ptr(), names[index].Length());
        m_codecs.append(str);
    }
    QT_TRACE_FUNCTION_EXIT;
}

void QXARecordSession::initContainersList()
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_IF_m_impl_IS_NULL;

    m_containers.clear();
    m_containersDesc.clear();

    const RArray<TPtrC>& names = m_impl->getContainerNames();
    const RArray<TPtrC>& descs = m_impl->getContainerDescs();
    QString str;

    for (TInt32 index = 0; index < names.Count(); index++) {
        str = QString((QChar*)names[index].Ptr(), names[index].Length());
        m_containers.append(str);
        str = QString((QChar*)descs[index].Ptr(), descs[index].Length());
        m_containersDesc.append(str);
    }
    QT_TRACE_FUNCTION_EXIT;
}

bool QXARecordSession::setEncoderSettingsToImpl()
{
    QT_TRACE_FUNCTION_ENTRY;

    RETURN_s_IF_m_impl_IS_NULL(false);

    m_impl->resetEncoderAttributes();

    QString tempStr = m_containerMimeType.toLower();
    TPtrC16 tempPtr(reinterpret_cast<const TUint16 *>(tempStr.utf16()));
    m_impl->setContainerType(tempPtr);

    /* Validate and set bitrate only if encoding mode is other than quality encoding */
    if (m_audioencodersettings.encodingMode() != QtMedia::ConstantQualityEncoding) {
        if (m_audioencodersettings.bitRate() < 0 ) {
            emit error(QMediaRecorder::FormatError, tr("Invalid bitrate"));
            SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid bitrate\"))");
            return false;
        }
        else {
            m_impl->setBitRate(m_audioencodersettings.bitRate());
        }
    }

    if (m_audioencodersettings.channelCount() == -1) {
        m_impl->setOptimalChannelCount();
    }
    else if (m_audioencodersettings.channelCount() <= 0) {
        emit error(QMediaRecorder::FormatError, tr("Invalid channel count"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid channel count\"));");
        return false;
    }
    else {
        m_impl->setChannels(m_audioencodersettings.channelCount());
    }

    tempStr = m_audioencodersettings.codec().toLower();
    if (m_codecs.indexOf(tempStr) >= 0) {
        tempPtr.Set(reinterpret_cast<const TUint16*>(tempStr.utf16()));
        /* We already did validation above, so function always returns true */
        m_impl->setCodec(tempPtr);
    }
    else {
        emit error(QMediaRecorder::FormatError, tr("Invalid codec"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid codec\"));");
        return false;
    }

    switch (m_audioencodersettings.encodingMode()) {
    case QtMedia::ConstantQualityEncoding: {
            switch (m_audioencodersettings.quality()) {
            case QtMedia::VeryLowQuality:
                m_impl->setVeryLowQuality();
                break;
            case QtMedia::LowQuality:
                m_impl->setLowQuality();
                break;
            case QtMedia::NormalQuality:
                m_impl->setNormalQuality();
                break;
            case QtMedia::HighQuality:
                m_impl->setHighQuality();
                break;
            case QtMedia::VeryHighQuality:
                m_impl->setVeryHighQuality();
                break;
            default:
                emit error(QMediaRecorder::FormatError, tr("Invalid encoding quality setting"));
                SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid encoding quality setting\"));");
                return false;
            }; /* end of switch (m_audioencodersettings.quality())*/
        }
        break;
    case QtMedia::ConstantBitRateEncoding: {
            TInt32 status = m_impl->setCBRMode();
            if (status == KErrNotSupported) {
                emit error(QMediaRecorder::FormatError, tr("Invalid encoding mode setting"));
                SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid encoding mode setting\"));");
                return false;
            }
            else if (status != KErrNone) {
                emit error(QMediaRecorder::ResourceError, tr("Internal error"));
                SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Internal error\"));");
                return false;
            }
        }
        break;
    case QtMedia::AverageBitRateEncoding: {
            TInt32 status = m_impl->setVBRMode();
            if (status == KErrNotSupported) {
                emit error(QMediaRecorder::FormatError, tr("Invalid encoding mode setting"));
                SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid encoding mode setting\"));");
                return false;
            }
            else if (status != KErrNone) {
                emit error(QMediaRecorder::ResourceError, tr("Internal error"));
                SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::ResourceError, tr(\"Internal error\"));");
                return false;
            }
        }
        break;
    case QtMedia::TwoPassEncoding:
        // fall through
    default: {
            emit error(QMediaRecorder::FormatError, tr("Invalid encoding mode setting"));
            SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid encoding mode setting\"));");
            return false;
        }
    }; /* switch (m_audioencodersettings.encodingMode()) */

    if (m_audioencodersettings.sampleRate() == -1) {
        m_impl->setOptimalSampleRate();
    }
    else if (m_audioencodersettings.sampleRate() <= 0) {
        emit error(QMediaRecorder::FormatError, tr("Invalid sample rate"));
        SIGNAL_EMIT_TRACE1("emit error(QMediaRecorder::FormatError, tr(\"Invalid sample rate\"));");
        return false;
    }
    else {
        m_impl->setSampleRate(m_audioencodersettings.sampleRate());
    }
    m_appliedaudioencodersettings = m_audioencodersettings;
    QT_TRACE_FUNCTION_EXIT;
    return true;
}
