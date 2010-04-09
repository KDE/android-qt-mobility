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

#include "s60audiocapturesession.h"
#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QDir>

#include <Mda\Common\Audio.h>
#include <Mda\Common\Resource.h>
#include <Mda\Client\Utility.h>
#include <MdaAudioSampleEditor.h>
#include <mmf\common\mmfcontrollerpluginresolver.h>
#include <mmf\common\mmfcontroller.h>
#include <BADESCA.H>
#include <BAUTILS.H>
#include <coemain.h>  

_LIT(KAudioDummyFile, "c:\\data\\temp\\temp.wav");

S60AudioCaptureSession::S60AudioCaptureSession(QObject *parent):
    QObject(parent)
    , m_recorderUtility(NULL)    
    , m_captureState(ENotInitialized)    
    , m_controllerIdMap(QHash<QString, ControllerData>())    
    , m_audioCodeclist(QHash<QString, CodecData>())    
{
    TRAPD(err, initializeSessionL());
    qWarning()<<err;
}

void S60AudioCaptureSession::initializeSessionL()
{    
    m_recorderUtility = CMdaAudioRecorderUtility::NewL(*this, 0, 80,(TMdaPriorityPreference) 0x5210001);        
    updateAudioContainersL();    
    populateAudioCodecsDataL();
    setDefaultSettings();
}

S60AudioCaptureSession::~S60AudioCaptureSession()
{       
    delete m_recorderUtility;    
}

QAudioFormat S60AudioCaptureSession::format() const
{
    return m_format;
}

bool S60AudioCaptureSession::setFormat(const QAudioFormat &format)
{		
    m_format = format;
    return true;
}

QStringList S60AudioCaptureSession::supportedAudioCodecs() const
{    
	return m_audioCodeclist.keys();
}

QStringList S60AudioCaptureSession::supportedAudioContainers() const
{
    return m_controllerIdMap.keys();
}

QString S60AudioCaptureSession::codecDescription(const QString &codecName)
{
    if (m_audioCodeclist.keys().contains(codecName))
        return m_audioCodeclist[codecName].codecDescription;
    else
        return QString();
}

QString S60AudioCaptureSession::audioContainerDescription(const QString &containerName)
{   
    if (m_controllerIdMap.keys().contains(containerName))
        return m_controllerIdMap[containerName].destinationFormatDescription;
    else
        return QString();
}

bool S60AudioCaptureSession::setAudioCodec(const QString &codecName)
{    
    QStringList codecs = supportedAudioCodecs();
    if(codecs.contains(codecName)) {
        m_format.setCodec(codecName);
        return true;
    }    
    return false;
}

bool S60AudioCaptureSession::setAudioContainer(const QString &containerMimeType) 
{    
    QStringList containers = supportedAudioContainers();
    if(containers.contains(containerMimeType)) { 
        m_container = containerMimeType;
        return true;
    }
    return false;
}

QString S60AudioCaptureSession::audioCodec() const
{   
    return m_format.codec();
}

QString S60AudioCaptureSession::audioContainer() const
{
    return m_container;
}

QUrl S60AudioCaptureSession::outputLocation() const
{
    return m_sink;
}

bool S60AudioCaptureSession::setOutputLocation(const QUrl& sink)
{
    QString filename = QDir::toNativeSeparators(m_sink.toString());
    TPtrC16 path(reinterpret_cast<const TUint16*>(filename.utf16()));         
    TRAPD(err, BaflUtils::EnsurePathExistsL(CCoeEnv::Static()->FsSession(),path));
    if (err==KErrNone) {
        m_sink = sink;
        return true;
    }else 
        return false;    
}

qint64 S60AudioCaptureSession::position() const
{    
    if ((m_captureState != ERecording) || !m_recorderUtility) 
        return 0;
        
    return m_recorderUtility->Duration().Int64() / 1000;        
}

void S60AudioCaptureSession::record()
{   
    if (!m_recorderUtility)
        return;
    
    if (m_captureState == EInitialized || m_captureState == ERecordComplete) {
        QString filename = QDir::toNativeSeparators(m_sink.toString());        
        TPtrC16 sink(reinterpret_cast<const TUint16*>(filename.utf16()));         
        TUid controllerUid(TUid::Uid(m_controllerIdMap[m_container].controllerUid));
        TUid formatUid(TUid::Uid(m_controllerIdMap[m_container].destinationFormatUid));
                
        TRAPD(err, 
        if (m_container != "audio/amr")   
            m_recorderUtility->OpenFileL(sink, controllerUid, KNullUid, formatUid);
        else   
            m_recorderUtility->OpenFileL(sink);
        );       
        qWarning() << err;
    }else if (m_captureState == EPaused) {        
        m_recorderUtility->SetPosition(m_pausedPosition);
        TRAPD(error, m_recorderUtility->RecordL());
        qWarning() << error;
        m_captureState = ERecording;
        emit stateChanged(m_captureState);
    }        
}

void S60AudioCaptureSession::setDefaultSettings()
{
    // Setting AMR to default format if supported
    if (m_controllerIdMap.count() > 0) {
        if ( m_controllerIdMap.contains("audio/amr"))
            m_container = QString("audio/amr");
        else
            m_container = m_controllerIdMap.keys()[0];
    }
    if (m_audioCodeclist.keys().count() > 0) {
        if (m_audioCodeclist.keys().contains("AMR"))
            m_format.setCodec("AMR");
        else
            m_format.setCodec(m_audioCodeclist.keys()[0]);
    }
}

void S60AudioCaptureSession::pause()
{  
    if (!m_recorderUtility)
        return;
    
    m_pausedPosition = m_recorderUtility->Position();
    m_recorderUtility->Stop();
    m_captureState = EPaused;
    emit stateChanged(m_captureState);    
}

void S60AudioCaptureSession::stop()
{
    if (!m_recorderUtility)
        return;
    
    m_recorderUtility->Stop();
    m_recorderUtility->Close();        
    m_captureState = ERecordComplete;
    emit stateChanged(m_captureState);
}

void S60AudioCaptureSession::setCaptureDevice(const QString &deviceName)
{
    m_captureDevice = deviceName;
}

void S60AudioCaptureSession::MoscoStateChangeEvent(CBase* aObject,
        TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
{    
	TRAPD(err, MoscoStateChangeEventL(aObject, aPreviousState, aCurrentState, NULL));    
	qWarning() << err;
}

void S60AudioCaptureSession::MoscoStateChangeEventL(CBase* aObject,
        TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
{    
	if (aObject != m_recorderUtility)
	    return;
	
		switch(aCurrentState) {
        case CMdaAudioClipUtility::EOpen: {            
            if(aPreviousState == CMdaAudioClipUtility::ENotReady) {                
                if (m_captureState == EInitializing) {                    
                    retrieveSupportedAudioSampleRatesL();                   
                    m_recorderUtility->Close();                    
                    m_captureState = EInitialized;
                    emit stateChanged(m_captureState);
                }else {         
                    applyAudioSettingsL();                            
                    m_recorderUtility->SetAudioDeviceMode(CMdaAudioRecorderUtility::ELocal);
                    m_recorderUtility->SetGain(m_recorderUtility->MaxGain());                    
                    m_recorderUtility->SetPosition(TTimeIntervalMicroSeconds(0));
                    m_recorderUtility->CropL();
                    m_recorderUtility->RecordL();
                    m_captureState = EOpenCompelete;                    
                    emit stateChanged(m_captureState);
                    }
                break;
            }
        }		
        case CMdaAudioClipUtility::ENotReady: {            
            m_captureState = EInitialized;            
            emit stateChanged(m_captureState);
            break;
        }
        case CMdaAudioClipUtility::ERecording: {            
            m_captureState = ERecording;            
            emit stateChanged(m_captureState);
            break;
        }
        default: {            
            break;
        }   
		}    
}

void S60AudioCaptureSession::updateAudioContainersL()
{
    CMMFControllerPluginSelectionParameters* pluginParameters = 
    	CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParameters = 
		CMMFFormatSelectionParameters::NewLC();
	 
	pluginParameters->SetRequiredRecordFormatSupportL(*formatParameters);
	 
	RArray<TUid> ids;
	CleanupClosePushL(ids);
	User::LeaveIfError(ids.Append(KUidMediaTypeAudio));  

	pluginParameters->SetMediaIdsL(ids, 
		CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);
	 
	RMMFControllerImplInfoArray controllers; 
	CleanupResetAndDestroyPushL(controllers);
	 
	//Get all audio record controllers/formats that are supported
	pluginParameters->ListImplementationsL(controllers);
	 
	for (TInt index=0; index<controllers.Count(); index++) {
		const RMMFFormatImplInfoArray& recordFormats = 
			controllers[index]->RecordFormats();
		for (TInt j=0; j<recordFormats.Count(); j++) {
			const CDesC8Array& mimeTypes = recordFormats[j]->SupportedMimeTypes();
			TInt count = mimeTypes.Count();
			if (count > 0) {
				TPtrC8 mimeType = mimeTypes[0];
				QString type = QString::fromUtf8((char *)mimeType.Ptr(), mimeType.Length());
				if (type != "audio/mp4") {
                    ControllerData data;
                    data.controllerUid = controllers[index]->Uid().iUid;
                    data.destinationFormatUid = recordFormats[j]->Uid().iUid;
                    data.destinationFormatDescription = QString::fromUtf16(
                            recordFormats[j]->DisplayName().Ptr(),
                            recordFormats[j]->DisplayName().Length());                
                    m_controllerIdMap[type] = data;
				}
			}
		}
	}	
	CleanupStack::PopAndDestroy(4);//controllers, ids, formatParameters, pluginParameters
}

void S60AudioCaptureSession::retrieveSupportedAudioSampleRatesL()
{       
    if (!m_recorderUtility || m_captureState != EInitializing) 
        return;
    
    RArray<TUint> supportedSampleRates;
    CleanupClosePushL(supportedSampleRates);
    m_recorderUtility->GetSupportedSampleRatesL(supportedSampleRates);        
    for (TInt j = 0; j < supportedSampleRates.Count(); j++ ) {            
        m_supportedSampleRates.append(supportedSampleRates[j]);
        //qDebug()<<"S60AudioCaptureSession::doPopulateAudioCodecsDataL, samplerate: "<<m_supportedSampleRates[j];                        
    }            
    CleanupStack::PopAndDestroy(&supportedSampleRates);    
}

QList<int> S60AudioCaptureSession::supportedAudioSampleRates() const
{
    return m_supportedSampleRates;
}

void S60AudioCaptureSession::populateAudioCodecsDataL()
{
    //qDebug() << "S60AudioCaptureSession::doInitializeAudioRecorderL START";    
    if (!m_recorderUtility)  
        return;
        
    if (m_controllerIdMap.contains("audio/amr")) {       
        CodecData data;              
        data.codecDescription = QString("GSM AMR Codec");
        m_audioCodeclist[QString("AMR")]=data;
    }   
    if (m_controllerIdMap.contains("audio/basic")) {
        CodecData data;           
        data.fourCC = KMMFFourCCCodeALAW;                   
        data.codecDescription = QString("Sun/Next ""Au"" audio codec");
        m_audioCodeclist[QString("AULAW")]=data;            
    }
    if (m_controllerIdMap.contains("audio/wav")) {            
        CodecData data;           
        data.fourCC = KMMFFourCCCodePCM16;                   
        data.codecDescription = QString("Pulse code modulation");
        m_audioCodeclist[QString("PCM")]=data;
    }    
    if (m_controllerIdMap.contains("audio/mp4")) {            
        CodecData data;           
        data.fourCC = KMMFFourCCCodeAAC;                   
        data.codecDescription = QString("Advanced Audio Codec");
        m_audioCodeclist[QString("AAC")]=data;
    }        
    
    if (m_controllerIdMap.contains("audio/wav")) {            
        TMdaFileClipLocation location;   
        location.iName = KAudioDummyFile();
        TMdaWavClipFormat format;
        m_captureState = EInitializing;
        m_recorderUtility->OpenL(&location, &format);             
    }    
}

void S60AudioCaptureSession::applyAudioSettingsL()
{
    //qDebug() << "S60AudioCaptureSession::applyAudioSettings START";  
    if (!m_recorderUtility)
        return;
    
    TFourCC fourCC = m_audioCodeclist[m_format.codec()].fourCC;    
    
    //set destination datatype
    RArray<TFourCC> supportedDataTypes;
    CleanupClosePushL(supportedDataTypes);
    m_recorderUtility->GetSupportedDestinationDataTypesL(supportedDataTypes);
    //qDebug() << "S60AudioCaptureSession::applyAudioSettingsL, datatype count"<<supportedDataTypes.Count();

    for (TInt k = 0; k < supportedDataTypes.Count(); k++ ) {
        qDebug() << "S60AudioCaptureSession::applyAudioSettingsL, codec"<<m_format.codec()<<"datatype"<<supportedDataTypes[k].FourCC();
        if (supportedDataTypes[k].FourCC() == fourCC.FourCC()) {
            //qDebug() << "S60AudioCaptureSession::applyAudioSettingsL, codec"<<m_format.codec()<<"matched datatype"<<supportedDataTypes[k].FourCC();
            m_recorderUtility->SetDestinationDataTypeL(supportedDataTypes[k]);
            break;
        }
    }
    CleanupStack::PopAndDestroy(&supportedDataTypes);
    
    RArray<TUint> supportedSampleRates;
    CleanupClosePushL(supportedSampleRates);
    m_recorderUtility->GetSupportedSampleRatesL(supportedSampleRates);    
    for (TInt i = 0; i < supportedSampleRates.Count(); i++ ) {
        TUint supportedSampleRate = supportedSampleRates[i];
        if (supportedSampleRate == m_format.frequency()) { 
            m_recorderUtility->SetDestinationSampleRateL(m_format.frequency());
            break;
        }        
    }
    CleanupStack::PopAndDestroy(&supportedSampleRates);
 
    RArray<TUint> supportedChannels;
    CleanupClosePushL(supportedChannels);
    m_recorderUtility->GetSupportedNumberOfChannelsL(supportedChannels);
    for (TInt l = 0; l < supportedChannels.Count(); l++ ) {
        if (supportedChannels[l] == m_format.channels()) {
            m_recorderUtility->SetDestinationNumberOfChannelsL(m_format.channels());
            break;
        }
    }
    CleanupStack::PopAndDestroy(&supportedChannels);
    //qDebug() << "S60AudioCaptureSession::applyAudioSettings END";        
}

TFourCC S60AudioCaptureSession::determinePCMFormat()
{
    //qDebug() << "S60AudioCaptureSession::determinePCMSampleSize START";    
    TFourCC fourCC;    
    
    if (m_format.sampleSize() == 8) {
        // 8 bit
        switch (m_format.sampleType()) {
        case QAudioFormat::SignedInt: {
            fourCC.Set(KMMFFourCCCodePCM8);
            break;
        }
        case QAudioFormat::UnSignedInt: {
            fourCC.Set(KMMFFourCCCodePCMU8);
            break;
        }
        case QAudioFormat::Float: 
        case QAudioFormat::Unknown:
        default: {
            fourCC.Set(KMMFFourCCCodePCM8);
            break;
        }
        }
    } else if (m_format.sampleSize() == 16) {
        // 16 bit 
        switch (m_format.sampleType()) {
        case QAudioFormat::SignedInt: {
            fourCC.Set(m_format.byteOrder()==QAudioFormat::BigEndian?
                KMMFFourCCCodePCM16B:KMMFFourCCCodePCM16);
            break;
        }
        case QAudioFormat::UnSignedInt: {
            fourCC.Set(m_format.byteOrder()==QAudioFormat::BigEndian?
                KMMFFourCCCodePCMU16B:KMMFFourCCCodePCMU16);
            break;
        }
        default: {
            fourCC.Set(KMMFFourCCCodePCM16);
            break;
        }
        }    
    }    
    //qDebug() << "S60AudioCaptureSession::determinePCMSampleSize END";
    return fourCC;
}
