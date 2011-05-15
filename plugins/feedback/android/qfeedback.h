/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit (EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef QFEEDBACK_H
#define QFEEDBACK_H

#include <QtCore/QHash>
#include <QtGui/QWidget>

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
#include <QtCore/QElapsedTimer>
#else
#include <QtCore/QTime>
#endif


#include "globalfeedbackconstants.h"
#include "jnifeedback.h"

#include "qmobilityglobal.h"

#include <qfeedbackplugininterfaces.h>
#include <qfeedbackeffect.h>




QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QFeedbackAndroid : public QObject, public QFeedbackHapticsInterface

, public QFeedbackThemeInterface , public QFeedbackFileInterface

{
    Q_OBJECT
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackHapticsInterface)
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackFileInterface)
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackThemeInterface)

public:
    QFeedbackAndroid();
    virtual ~QFeedbackAndroid();
    virtual PluginPriority pluginPriority();
    virtual QList<QFeedbackActuator*> actuators();

    //for device handling
    virtual void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &);
    virtual QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty);
    virtual bool isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability);
    //haptic effect
    virtual void updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty);
    virtual void setEffectState(const QFeedbackHapticsEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *);

    //theme effect
    virtual bool play(QFeedbackEffect::ThemeEffect);

    //file effect
    virtual void setLoaded(QFeedbackFileEffect*, bool);
    virtual void setEffectState(QFeedbackFileEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackFileEffect *);
    virtual int effectDuration(const QFeedbackFileEffect *);
    virtual QStringList supportedMimeTypes();


private:

    class PausableElapsedTimer
    {
    public:
        PausableElapsedTimer() : pausedTime(0)
        {
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
           m_elapsedTimer.invalidate();
#else
            m_elapsedTimer = QTime();
#endif
        }

        void start()
        {
            m_elapsedTimer.restart();
        }

        void pause()
        {
            if (m_elapsedTimer.isValid())
                pausedTime += m_elapsedTimer.elapsed();
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
            m_elapsedTimer.invalidate();
#else
            m_elapsedTimer = QTime();
#endif
        }

        bool isValid() const
        {
            return m_elapsedTimer.isValid();

        }


        int elapsed() const
        {
            return m_elapsedTimer.elapsed();
        }


        void clear()
        {
            m_elapsedTimer.invalidate();
        }



    private:
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        QElapsedTimer m_elapsedTimer;
#else
        QTime m_elapsedTimer;
#endif
        int pausedTime;

    };


    QtFeedbackJNI::State m_hapticState;
    QtFeedbackJNI::State m_fileState;
    PausableElapsedTimer m_hapticTimer;
    PausableElapsedTimer m_fileTimer;
    bool m_hapticEnabled;
    bool m_vibraEnabled;
    int m_fileDuration;
    int m_hapticResumeTime;
    int m_fileResumeTime;
    QList<QFeedbackActuator*> m_actuatorList;
    QString m_vibraName;
    QString m_touchName;

};


QT_END_HEADER

#endif // QFEEDBACK_H
