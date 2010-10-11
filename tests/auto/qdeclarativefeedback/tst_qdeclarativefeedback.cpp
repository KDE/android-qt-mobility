/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qtest.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <qfeedbackeffect.h>
#include <qfeedbackactuator.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

QTM_USE_NAMESPACE

class tst_qdeclarativefeedback : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativefeedback();

private slots:
    void hapticsEffect();
    void fileEffect();
    void actuator();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativefeedback::tst_qdeclarativefeedback()
{
}

void tst_qdeclarativefeedback::hapticsEffect()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/hapticseffect.qml"));

    QFeedbackHapticsEffect *hapticsEffect = qobject_cast<QFeedbackHapticsEffect*>(component.create());
    QVERIFY(hapticsEffect != 0);

    QCOMPARE(hapticsEffect->attackIntensity(), 0.0);
    QCOMPARE(hapticsEffect->attackTime(), 250);
    QCOMPARE(hapticsEffect->intensity(), 1.0);
    QCOMPARE(hapticsEffect->duration(), 100);
    QCOMPARE(hapticsEffect->fadeTime(), 250);
    QCOMPARE(hapticsEffect->fadeIntensity(), 0.0);
    QVERIFY(hapticsEffect->actuator() != 0);
    QCOMPARE(hapticsEffect->state(), QFeedbackEffect::Stopped);

    QCOMPARE(hapticsEffect->property("supportsThemeEffect").toBool(), false);

    QCOMPARE(hapticsEffect->property("running").toBool(), false);
    QCOMPARE(hapticsEffect->property("paused").toBool(), false);
    hapticsEffect->setProperty("running", true);
    hapticsEffect->setProperty("paused", true);

    // dummy backend
    QCOMPARE(hapticsEffect->property("running").toBool(), false);
    QCOMPARE(hapticsEffect->property("paused").toBool(), false);

    delete hapticsEffect;
}

void tst_qdeclarativefeedback::fileEffect()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/fileeffect.qml"));

    QFeedbackFileEffect *fileEffect = qobject_cast<QFeedbackFileEffect*>(component.create());
    QVERIFY(fileEffect != 0);

    QCOMPARE(fileEffect->fileName(), QString("nonexistingfile.haptic"));
    QCOMPARE(fileEffect->isLoaded(), false);
    QCOMPARE(fileEffect->state(), QFeedbackEffect::Stopped);

    QCOMPARE(fileEffect->property("running").toBool(), false);
    QCOMPARE(fileEffect->property("paused").toBool(), false);
    fileEffect->setProperty("running", true);
    fileEffect->setProperty("paused", true);

    // dummy backend
    QCOMPARE(fileEffect->property("running").toBool(), false);
    QCOMPARE(fileEffect->property("paused").toBool(), false);

    delete fileEffect;
}

void tst_qdeclarativefeedback::actuator()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/actuator.qml"));

    QFeedbackActuator *actuator = qobject_cast<QFeedbackActuator*>(component.create());
    QVERIFY(actuator != 0);
    QCOMPARE(actuator->isEnabled(), false);

    delete actuator;
}

QTEST_MAIN(tst_qdeclarativefeedback)

#include "tst_qdeclarativefeedback.moc"
