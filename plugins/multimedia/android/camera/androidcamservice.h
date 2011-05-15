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

#ifndef ANDROIDCAMSERVICE_H
#define ANDROIDCAMSERVICE_H

#include <QtCore/qobject.h>
#include <qmediaservice.h>
#include <androidcamcontrol.h>
#include <androidcamimagecapturecontrol.h>
#include <androidcamexposurecontrol.h>
#include <androidcamfocuscontrol.h>
#include <androidcamflashcontrol.h>
#include <androidcamimageprocessingcontrol.h>
#include <androidcamimageencodercontrol.h>
#include <androidvideowidgetcontrol.h>
#include <androidcammediacapturecontrol.h>
#include <androidmediacontainercontrol.h>
#include <androidvideoencodercontrol.h>
#include <androidaudioencodercontrol.h>

QT_USE_NAMESPACE

class AndroidCamControl;
class AndroidCamImageCaptureControl;
class AndroidCamExposureControl;
class AndroidCamFocusControl;
class AndroidCamFlashControl;
class AndroidCamImageProcessingControl;
class AndroidCamImageEncoderControl;
class AndroidVideoWidgetControl;
class AndroidCamMediaCaptureControl;
class AndroidMediaContainerControl;
class AndroidVideoEncoderControl;
class AndroidAudioEncoderControl;

class AndroidCamService : public QMediaService
{
    Q_OBJECT

public: // Contructor & Destructor

    AndroidCamService(QObject *parent = 0);
    ~AndroidCamService();

public: // QMediaService

    QMediaControl *requestControl(const char *name);
    void releaseControl(QMediaControl *control);

public: // Static Device Info

    static int deviceCount();
    static QString deviceName(const int index);
    static QString deviceDescription(const int index);



public: // Data

    AndroidCamControl *m_control;
    AndroidCamImageCaptureControl *m_imageCaptureControl;
    AndroidCamExposureControl * m_exposureControl;
    AndroidCamFocusControl * m_focusControl;
    AndroidCamFlashControl * m_flashControl;
    AndroidCamImageProcessingControl * m_imageProcessingControl;
    AndroidCamImageEncoderControl * m_imageEncoderControl;
    AndroidVideoWidgetControl * m_videoWidgetControl;
    AndroidCamMediaCaptureControl *m_mediaCaptureControl;
    AndroidVideoEncoderControl *m_videoEncoder;
    AndroidAudioEncoderControl *m_audioEncoder;
    AndroidMediaContainerControl *m_mediaFormat;
};


#endif // ANDROIDCAMSERVICE_H
