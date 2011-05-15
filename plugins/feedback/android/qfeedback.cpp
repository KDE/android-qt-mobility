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

#include <qfeedbackactuator.h>
#include "qfeedback.h"
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>
#include <QtCore/QFile>


Q_EXPORT_PLUGIN2(feedback_android, QFeedbackAndroid)



#define VIBRA_DEVICE 0
#define TOUCH_DEVICE 1

static QtFeedbackJNI::AndroidThemeEffect convertToAndroid(QFeedbackEffect::ThemeEffect effect)
{
    QtFeedbackJNI::AndroidThemeEffect themeEffect;
    switch (effect) {
    case QFeedbackEffect::ThemeBasic:
    case QFeedbackEffect::ThemeSensitive:
    case QFeedbackEffect::ThemeBasicButton:
    case QFeedbackEffect::ThemeSensitiveButton:
    case QFeedbackEffect::ThemeBasicItem:
    case QFeedbackEffect::ThemeSensitiveItem:
        themeEffect = QtFeedbackJNI::Click;
        break;
    case QFeedbackEffect::ThemeBounceEffect:
    case QFeedbackEffect::ThemePopupOpen:
    case QFeedbackEffect::ThemePopupClose:
    case QFeedbackEffect::ThemeBasicSlider:
    case QFeedbackEffect::ThemeSensitiveSlider:
    case QFeedbackEffect::ThemeStopFlick:
    case QFeedbackEffect::ThemeFlick:
        themeEffect= QtFeedbackJNI::KeypressStandard;
        break;
    case QFeedbackEffect::ThemeEditor:
    case QFeedbackEffect::ThemeTextSelection:
    case QFeedbackEffect::ThemeBlankSelection:
    case QFeedbackEffect::ThemeLineSelection:
    case QFeedbackEffect::ThemeEmptyLineSelection:
        themeEffect= QtFeedbackJNI::KeypressSpacebar;
        break;
    case QFeedbackEffect::ThemeCheckBox:
    case QFeedbackEffect::ThemeMultipleCheckBox:
    case QFeedbackEffect::ThemeSensitiveKeypad:
    case QFeedbackEffect::ThemeBasicKeypad:
    case QFeedbackEffect::ThemeRotateStep:
        themeEffect= QtFeedbackJNI::KeypressReturn;
        break;
    case QFeedbackEffect::ThemeItemDrop:
    case QFeedbackEffect::ThemeItemMoveOver:
    case QFeedbackEffect::ThemeItemPick:
    case QFeedbackEffect::ThemeItemScroll:
    case QFeedbackEffect::ThemePopUp:
    case QFeedbackEffect::ThemeLongPress:
    default:
        themeEffect= QtFeedbackJNI::KeypressDelete;
        break;
    }
    return themeEffect;
}




bool QFeedbackAndroid::play(QFeedbackEffect::ThemeEffect effect)
{
    QtFeedbackJNI::AndroidThemeEffect themeConst=convertToAndroid(effect);
    QtFeedbackJNI::playThemeEffect(themeConst);
    return true;
}



QFeedbackInterface::PluginPriority QFeedbackAndroid::pluginPriority()
{
    return PluginLowPriority;
}

QFeedbackAndroid::QFeedbackAndroid() : QObject(qApp)
{
    m_hapticEnabled=false;
    m_vibraEnabled=false;
    m_fileResumeTime=0;
    m_hapticResumeTime=0;
    m_fileDuration=0;
    m_vibraName = QLatin1String("VIBRA");
    m_touchName = QLatin1String("TOUCH");
    m_actuatorList << createFeedbackActuator(this,TOUCH_DEVICE);
    m_actuatorList << createFeedbackActuator(this,VIBRA_DEVICE);

}

QFeedbackAndroid::~QFeedbackAndroid()
{
    m_hapticTimer.pause();
    m_fileTimer.pause();
}



QList<QFeedbackActuator*> QFeedbackAndroid::actuators()
{
    return m_actuatorList;
}
//to set the priority of the two vibartors.
void QFeedbackAndroid::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    switch(prop)
    {
    case Enabled:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            m_vibraEnabled = value.toBool();
            break;

        case TOUCH_DEVICE:
            m_hapticEnabled = value.toBool();
            QtFeedbackJNI::setHapticFeedback(m_hapticEnabled);

            break;

        default:
            break;

        }
        break;

    case Name:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            m_vibraName = value.toString();
            break;

        case TOUCH_DEVICE:
            m_touchName = value.toString();
            break;

        default:
            break;

        }
        break;
    default:
        return;
    }
}

QVariant QFeedbackAndroid::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    switch(prop)
    {
    case Name:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            return m_vibraName;
        case TOUCH_DEVICE:
            return m_touchName;

        default:
            return QString();
        }
        break;
    case State:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            {

                switch (m_hapticState)
                {
                case QtFeedbackJNI::Start:
                    return QFeedbackActuator::Busy;
                case QtFeedbackJNI::Stop:
                    return QFeedbackActuator::Ready;
                default:
                    return QFeedbackActuator::Unknown;
                }
            }
        case TOUCH_DEVICE:
            //there is no way of getting the state of the device!
        default:
            return QFeedbackActuator::Unknown;
        }
        break;

    case Enabled:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            return m_vibraEnabled;

        case TOUCH_DEVICE:
            return m_hapticEnabled;
        default:
            return false;
        }
    default:
        return QVariant();
    }

}

bool QFeedbackAndroid::isActuatorCapabilitySupported(const QFeedbackActuator &actuator, QFeedbackActuator::Capability capability)
{
    if(actuator.id() == VIBRA_DEVICE)
    {
        switch (capability)
        {
        case QFeedbackActuator::Period:
            return true;
        case QFeedbackActuator::Envelope:
            return false;
        }

    }
    return false;
}


void QFeedbackAndroid::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty prop)
{

    QtFeedbackJNI::FeedbackIntensity intensity;
    if(prop == Intensity)
    {
        if(effect->actuator()->id() == TOUCH_DEVICE)
        {

            if(effect->intensity() >= 0 && effect->intensity() < 0.5)
                intensity = QtFeedbackJNI::Low;
            else if(effect->intensity() == 0.5)
                intensity = QtFeedbackJNI::Medium;
            else if(effect->intensity() > 0.5 && effect->intensity() <= 1)
                intensity = QtFeedbackJNI::High;
            QtFeedbackJNI::setHapticIntensity(intensity);

        }
    }


}

void QFeedbackAndroid::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State newState)
{
    switch(effect->actuator()->id())
    {
    case VIBRA_DEVICE:

        switch(newState)
        {
        case QFeedbackEffect::Stopped:
            m_hapticState= QtFeedbackJNI::Stop;
            m_hapticTimer.pause();
            break;
        case QFeedbackEffect::Paused:
            m_hapticState = QtFeedbackJNI::Pause;
            m_hapticResumeTime = effect->duration()- m_hapticTimer.elapsed();
            m_hapticTimer.pause();
            break;
        case QFeedbackEffect::Running:
            if(m_hapticState == QtFeedbackJNI::Pause)
            {
                m_hapticState = QtFeedbackJNI::Resume;
                m_hapticTimer.start();
            }
            else
            {
                m_hapticState = QtFeedbackJNI::Start;
                m_hapticTimer.start();
                m_hapticResumeTime = 0;
            }
            break;
                case QFeedbackEffect::Loading:
            break;

                default:
            break;
        }
        QtFeedbackJNI::setVibraDurPer(effect->duration(),effect->period(),m_hapticResumeTime);
        QtFeedbackJNI::setVibraState(m_hapticState);
        break;


       case TOUCH_DEVICE:
        if(newState == QFeedbackEffect::Running)
        {
            QtFeedbackJNI::performHapticFeedback();
        }
        break;
         default:
        break;

        reportError(effect, QFeedbackEffect::UnknownError);
    }
}


QFeedbackEffect::State QFeedbackAndroid::effectState(const QFeedbackHapticsEffect *effect)
{

    switch(effect->actuator()->id())
    {
    case VIBRA_DEVICE:
        if(m_hapticResumeTime == 0)
        {
            if((m_hapticTimer.isValid()) && (m_hapticTimer.elapsed() >= (effect->duration())))
            {
                m_hapticTimer.pause();
                m_hapticState = QtFeedbackJNI::Stop;

            }

        }
        else
        {
            if((m_hapticTimer.isValid()) && (m_hapticTimer.elapsed() >= m_hapticResumeTime))
            {
                m_hapticTimer.pause();
                m_hapticState = QtFeedbackJNI::Stop;

            }

        }
        switch(m_hapticState)
        {
        case QtFeedbackJNI::Resume:
        case QtFeedbackJNI::Start:
            return QFeedbackEffect::Running;

        case QtFeedbackJNI::Stop:
            return QFeedbackEffect::Stopped;

        case QtFeedbackJNI::Pause:
            return QFeedbackEffect::Paused;

        default:
            return QFeedbackEffect::Stopped;
        }

        break;
    case TOUCH_DEVICE:
     default:
        return QFeedbackEffect::Stopped;
        break;
    }


}





QStringList QFeedbackAndroid::supportedMimeTypes()
{
    return QStringList() << QLatin1String("ogg") <<  QLatin1String("mp3") <<  QLatin1String("wav");
}




void QFeedbackAndroid::setLoaded(QFeedbackFileEffect *effect, bool load)
{
    QStringList fileNames;
    QString currentFile =((QUrl) effect->source()).path();
    fileNames << currentFile;
    if(fileNames.count() == 3)
    {
        fileNames.pop_front();
    }

    if( (fileNames.count() == 2) && ((((QString)fileNames.at(0)).compare(currentFile) == 0)))
    {
        return;
    }
    if(load)
    {
        QtFeedbackJNI::loadMusicFile(currentFile);
        m_fileDuration = (QtFeedbackJNI::getFileDur());
        reportLoadFinished(effect,true);
    }
    else
    {
        reportLoadFinished(effect,false);
    }

}



void QFeedbackAndroid::setEffectState(QFeedbackFileEffect *, QFeedbackEffect::State state)
{
    switch (state)
    {
    case QFeedbackEffect::Running:
        if(m_fileState == QtFeedbackJNI::Pause)
        {
            m_fileState = QtFeedbackJNI::Resume;
            m_fileTimer.start();
        }
        else
        {
            m_fileState = QtFeedbackJNI::Start;
            m_fileTimer.start();
            m_fileResumeTime = 0;
        }
        break;

    case QFeedbackEffect::Stopped:
        m_fileState = QtFeedbackJNI::Stop;
        m_fileTimer.pause();
        break;

    case QFeedbackEffect::Paused:
        m_fileState = QtFeedbackJNI::Pause;
        m_fileResumeTime = m_fileDuration - m_fileTimer.elapsed();
        m_fileTimer.pause();
        break;

     case QFeedbackEffect::Loading:
        break;

     default:
        break;

    }
    QtFeedbackJNI::setFileState(m_fileState);
}

QFeedbackEffect::State QFeedbackAndroid::effectState(const QFeedbackFileEffect *)//effect)
{
    if((m_fileTimer.isValid())  && (m_fileResumeTime == 0) && (m_fileTimer.elapsed() >= m_fileDuration))
    {
        m_fileTimer.pause();
        m_fileState = QtFeedbackJNI::Stop;
        QtFeedbackJNI::setFileState(m_fileState);

    }
    else if((m_fileTimer.isValid())  && (m_fileResumeTime != 0) && (m_fileTimer.elapsed() >= m_fileResumeTime))
    {
        m_fileTimer.pause();
        m_fileState = QtFeedbackJNI::Stop;
        QtFeedbackJNI::setFileState(m_fileState);

    }
    switch(m_fileState)
    {
    case QtFeedbackJNI::Resume:
    case QtFeedbackJNI::Start:
        return QFeedbackEffect::Running;

    case QtFeedbackJNI::Stop:
        return QFeedbackEffect::Stopped;

    case QtFeedbackJNI::Pause:
        return QFeedbackEffect::Paused;

    default:
        return QFeedbackEffect::Stopped;
    }


}

int QFeedbackAndroid::effectDuration(const QFeedbackFileEffect *effect)
{
    return effect->Infinite ;
}





