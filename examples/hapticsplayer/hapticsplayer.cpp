/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "hapticsplayer.h"

#include <QtCore/qmetaobject.h>

#include <QtGui/QFileDialog>
#include <QDebug>

static const char ENUM_THEME_EFFECT[] = "ThemeEffect";
static const char ENUM_ANIMATION_STATE[] = "State";

HapticsPlayer::HapticsPlayer()
{
    ui.setupUi(this);

#if defined(Q_WS_MAEMO_5)
    // maemo5 style problem: title of groupboxes is rendered badly if there isn't enough space
    // that is, the sizehint of the title area is Preferred rather than Minimum.
    // to fix that, we manually tweak some spacers.
    ui.verticalSpacer_2->changeSize(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.verticalSpacer_3->changeSize(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.verticalSpacer_4->changeSize(20, 30, QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.verticalSpacer_5->changeSize(20, 30, QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.verticalSpacer_6->changeSize(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.verticalSpacer_7->changeSize(20, 30, QSizePolicy::Expanding, QSizePolicy::Fixed);
#endif

    connect(ui.actuators, SIGNAL(currentIndexChanged(int)), SLOT(actuatorChanged()));
    connect(ui.enabled, SIGNAL(toggled(bool)), SLOT(enabledChanged(bool)));
    connect(ui.playPause, SIGNAL(pressed()), SLOT(playPauseClicked()));
    connect(ui.stop, SIGNAL(pressed()), &effect, SLOT(stop()));
    connect(ui.duration, SIGNAL(valueChanged(int)), SLOT(durationChanged(int)));
    connect(ui.intensity, SIGNAL(valueChanged(int)), SLOT(intensityChanged(int)));

    //for the envelope
    connect(ui.attackTime, SIGNAL(valueChanged(int)), SLOT(attackTimeChanged(int)));
    connect(ui.attackIntensity, SIGNAL(valueChanged(int)), SLOT(attackIntensityChanged(int)));
    connect(ui.fadeTime, SIGNAL(valueChanged(int)), SLOT(fadeTimeChanged(int)));
    connect(ui.fadeIntensity, SIGNAL(valueChanged(int)), SLOT(fadeIntensityChanged(int)));

    //for the period
    connect(ui.grpPeriod, SIGNAL(toggled(bool)), SLOT(periodToggled(bool)));
    connect(ui.period, SIGNAL(valueChanged(int)), SLOT(periodChanged(int)));

    connect(ui.instantPlay, SIGNAL(pressed()), SLOT(instantPlayClicked()));

    //file API
    connect(ui.browse, SIGNAL(pressed()), SLOT(browseClicked()));
    connect(ui.filePlayPause, SIGNAL(pressed()), SLOT(filePlayPauseClicked()));
    connect(ui.fileStop, SIGNAL(pressed()), &fileEffect, SLOT(stop()));

    foreach(const QFeedbackActuator &dev, QFeedbackActuator::actuators()) {
        ui.actuators->addItem(dev.name());
    }

    //adding the instant effects
    const QMetaObject &mo = QFeedbackEffect::staticMetaObject;
    const QMetaEnum &me = mo.enumerator(mo.indexOfEnumerator(ENUM_THEME_EFFECT));
    Q_ASSERT(me.keyCount());
    for (int i = 0 ; i < me.keyCount(); ++i) {
        ui.instantEffect->addItem(me.key(i));
    }

    //initialization
    durationChanged(effect.duration());
    intensityChanged(ui.intensity->value());
    attackTimeChanged(ui.attackTime->value());
    attackIntensityChanged(ui.attackIntensity->value());
    fadeTimeChanged(ui.fadeTime->value());
    fadeIntensityChanged(ui.fadeIntensity->value());

    ui.tabWidget->setTabEnabled(1, QFeedbackEffect::supportsThemeEffect());
    ui.tabWidget->setTabEnabled(2, !QFeedbackFileEffect::supportedMimeTypes().isEmpty());

    //that is a hackish way of updating the info concerning the effects
    startTimer(50);
}

QFeedbackActuator HapticsPlayer::currentActuator() const
{
    QList<QFeedbackActuator> devs = QFeedbackActuator::actuators();
    int index = ui.actuators->currentIndex();
    if (index == -1 || devs.count() == 0 || index > devs.count())
        return QFeedbackActuator();
    return devs.at(index);
}

void HapticsPlayer::actuatorChanged()
{
    QFeedbackActuator dev = currentActuator();
    enabledChanged(dev.isEnabled());
    effect.setActuator(dev);
}

void HapticsPlayer::enabledChanged(bool on)
{
    if (!on)
        effect.stop();
    QFeedbackActuator dev = currentActuator();
    dev.setEnabled(on);
    ui.enabled->setChecked(dev.isEnabled());

    if (dev.isEnabled() && (dev.isCapabilitySupported(QFeedbackActuator::Envelope))) {
        ui.envelope->setEnabled(true);
        ui.envelope->show();
    } else {
        ui.envelope->setEnabled(true);
        ui.envelope->hide();
    }
    if (dev.isEnabled() && (dev.isCapabilitySupported(QFeedbackActuator::Period))) {
        ui.grpPeriod->setEnabled(true);
        ui.grpPeriod->show();
    } else {
        ui.grpPeriod->setEnabled(false);
        ui.grpPeriod->hide();
    }
}

void HapticsPlayer::playPauseClicked()
{
    if (effect.state() == QFeedbackEffect::Running) {
        effect.pause();
    } else {
        effect.start();
    }
}

void HapticsPlayer::durationChanged(int duration)
{
    effect.setDuration(duration);
    ui.attackTime->setMaximum(duration);
    ui.fadeTime->setMaximum(duration);
    attackTimeChanged(ui.attackTime->value());
}

void HapticsPlayer::intensityChanged(int value)
{
    effect.setIntensity(qreal(value) / ui.intensity->maximum());
    ui.lblIntensity->setText(QString::number(effect.intensity()));
}

void HapticsPlayer::timerEvent(QTimerEvent *e)
{
    //update the display for effect
    {
        QFeedbackEffect::State newState = effect.state();
        const QMetaObject *mo = effect.metaObject();
        ui.effectState->setText(mo->enumerator(mo->indexOfEnumerator(ENUM_ANIMATION_STATE)).key(newState));
        ui.stop->setEnabled(newState != QFeedbackEffect::Stopped);
        if (effect.state() == QFeedbackEffect::Paused || effect.state() == QFeedbackEffect::Stopped)
            ui.playPause->setText(tr("Play"));
        else
            ui.playPause->setText(tr("Pause"));
    }

    //update the display for effect
    {
        QFeedbackEffect::State newState = fileEffect.state();
        const QMetaObject *mo = fileEffect.metaObject();
        ui.fileEffectState->setText(mo->enumerator(mo->indexOfEnumerator(ENUM_ANIMATION_STATE)).key(newState));
        ui.fileStop->setEnabled(newState != QFeedbackEffect::Stopped);
        ui.filePlayPause->setEnabled(fileEffect.isLoaded());
        ui.browse->setEnabled(newState == QFeedbackEffect::Stopped);
        ui.fileStatus->setText( fileEffect.isLoaded() ? QString::fromLatin1("%1 : %2 ms").arg(tr("Loaded")).arg(fileEffect.duration()) : tr("Not Loaded") );
    }
    QWidget::timerEvent(e);
}

void HapticsPlayer::attackTimeChanged(int attackTime)
{
    effect.setAttackTime(attackTime);
    //let's check the boundaries
    if (attackTime + ui.fadeTime->value() > ui.duration->value())
        ui.fadeTime->setValue(ui.duration->value() - attackTime);
}

void HapticsPlayer::attackIntensityChanged(int attackIntensity)
{
    effect.setAttackIntensity(qreal(attackIntensity) / ui.attackIntensity->maximum());
    ui.lblAttackIntensity->setText(QString::number(effect.attackIntensity()));
}

void HapticsPlayer::fadeTimeChanged(int fadeTime)
{
    effect.setFadeTime(fadeTime);
    //let's check the boundaries
    if (fadeTime + ui.attackTime->value() > ui.duration->value())
        ui.attackTime->setValue(ui.duration->value() - fadeTime);
}

void HapticsPlayer::fadeIntensityChanged(int fadeIntensity)
{
    effect.setFadeIntensity(qreal(fadeIntensity) / ui.fadeIntensity->maximum());
    ui.lblFadeIntensity->setText(QString::number(effect.fadeIntensity()));
}

void HapticsPlayer::periodToggled(bool on)
{
    effect.setPeriod(on ? ui.period->value() : 0);
}

void HapticsPlayer::periodChanged(int value)
{
    effect.setPeriod(value);
}

void HapticsPlayer::instantPlayClicked()
{
    const QMetaObject &mo = QFeedbackEffect::staticMetaObject;
    const QMetaEnum &me = mo.enumerator(mo.indexOfEnumerator(ENUM_THEME_EFFECT));
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeEffect(me.keyToValue(ui.instantEffect->currentText().toLatin1())));
}

void HapticsPlayer::browseClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Feedback file"));
    if (!filename.isEmpty()) {
        ui.filename->setText(QDir::toNativeSeparators(filename));
        fileEffect.setFileName(filename);
    }
}

void HapticsPlayer::filePlayPauseClicked()
{
    if (fileEffect.state() == QFeedbackEffect::Running)
        fileEffect.pause();
    else
        fileEffect.start();
}

#include "moc_hapticsplayer.cpp"
