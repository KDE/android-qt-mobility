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

#ifndef ANDROIDCAMEXPOSURECONTROL_H
#define ANDROIDCAMEXPOSURECONTROL_H


#include <qcameraexposurecontrol.h>
#include <androidcamjni.h>

QT_USE_NAMESPACE
class AndroidCamExposureControl : public QCameraExposureControl
{
    Q_OBJECT

public: // Constructors & Destructor
    AndroidCamExposureControl(QObject *parent = 0);
    ~AndroidCamExposureControl();

public: // QCameraExposureControl

    // Exposure Mode
    virtual QCameraExposure::ExposureMode exposureMode() const;
    virtual void setExposureMode(QCameraExposure::ExposureMode mode);
    virtual bool isExposureModeSupported(QCameraExposure::ExposureMode mode) const;

    // Metering Mode
    virtual QCameraExposure::MeteringMode meteringMode() const;
    virtual void setMeteringMode(QCameraExposure::MeteringMode mode);
    virtual bool isMeteringModeSupported(QCameraExposure::MeteringMode mode) const;

    // Exposure Parameter
    virtual bool isParameterSupported(ExposureParameter parameter) const;
    virtual QVariant exposureParameter(ExposureParameter parameter) const;
    virtual QCameraExposureControl::ParameterFlags exposureParameterFlags(ExposureParameter parameter) const;
    virtual QVariantList supportedParameterRange(ExposureParameter parameter) const;
    virtual bool setExposureParameter(ExposureParameter parameter, const QVariant& value);

    virtual QString extendedParameterName(ExposureParameter parameter);
    QString m_scene;
    int m_range[2];
private:
    QCameraExposure::ExposureMode m_mode;
    QVariant m_exposureParameter;


};


#endif // ANDROIDCAMEXPOSURECONTROL_H
