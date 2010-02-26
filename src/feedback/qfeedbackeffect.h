/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFEEDBACKEFFECT_H
#define QFEEDBACKEFFECT_H

#include <qmobilityglobal.h>
#include <QtCore/qscopedpointer.h>

QTM_BEGIN_NAMESPACE

//TODO: should those classes be QObject subclasses or even QAbstractAnimation subclasses?

//continous
enum ContinuousEffect {
    ContinuousNone, ContinuousSmooth, ContinuousSlider, ContinuousPopup,
    ContinuousInput, ContinuousPinch, NumberOfContinuousFeedbacks, NoContinuousOverride,
    ContinuousUser = 1000, ContinuousMaxUser = 65535
};


//instant
enum InstantEffect {
  InstantNone, InstantBasic, InstantSensitive, InstantBasicButton,
  InstantSensitiveButton, InstantBasicKeypad, InstantSensitiveKeypad, InstantBasicSlider,
  InstantSensitiveSlider, InstantBasicItem, InstantSensitiveItem, InstantItemScroll,
  InstantItemPick, InstantItemDrop, InstantItemMoveOver, InstantBounceEffect,
  InstantCheckbox, InstantMultipleCheckbox, InstantEditor, InstantTextSelection,
  InstantBlankSelection, InstantLineSelection, InstantEmptyLineSelection, InstantPopUp,
  InstantPopupOpen, InstantPopupClose, InstantFlick, InstantStopFlick,
  InstantMultiInstantActivate, InstantRotateStep, InstantNumberOfInstantFeedbacks,
  InstantNoOverride, InstantUser = 65535, InstantMaxUser = 262140
};

QT_FORWARD_DECLARE_CLASS(QWidget)
class QFeedbackEffectPrivate;
class QVibraEffectPrivate;
class QTouchEffectPrivate;

class Q_FEEDBACK_EXPORT QFeedbackEffect
{
public:
    virtual ~QFeedbackEffect();

    void setDuration(int msecs);
    int duration() const;

    void setIntensity(qreal intensity);
    qreal intensity() const;

    virtual void play() = 0;

protected:
    QFeedbackEffect(QFeedbackEffectPrivate &dd);
    QScopedPointer<QFeedbackEffectPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(QFeedbackEffect)
};


//
// feedback used when the program need to decide dynamically the duration
// and intensity this one is using the Continuous motor
//
class Q_FEEDBACK_EXPORT QVibraEffect : public QFeedbackEffect
{
public:
    QVibraEffect();
    ~QVibraEffect();

    void play();

private:
    Q_DISABLE_COPY(QVibraEffect)
    Q_DECLARE_PRIVATE(QVibraEffect)

};



//
// feedback used when the program need to decide dynamically the duration
// and intensity this one is using the Continuous touch feedback
//
class Q_FEEDBACK_EXPORT QTouchEffect : public QFeedbackEffect
{
public:
    QTouchEffect();
    ~QTouchEffect();

    void play();

    void setContinuousEffect(ContinuousEffect);
    ContinuousEffect continuousEffect() const;

    static void play(InstantEffect effect);

    void setWidget(QWidget *);
    QWidget *widget() const;

private:
    Q_DISABLE_COPY(QTouchEffect)
    Q_DECLARE_PRIVATE(QTouchEffect)

};

QTM_END_NAMESPACE

#endif // QFEEDBACKEFFECT_H
