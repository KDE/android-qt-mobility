/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qmediapluginloader_p.h>
#include <qmediaserviceproviderplugin.h>

#include <QtTest/QtTest>
#include <QDebug>

QT_USE_NAMESPACE

class tst_QMediaPluginLoader : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testInstance();
    void testInstances();
    void testInvalidKey();

private:
    QMediaPluginLoader *loader;
};

void tst_QMediaPluginLoader::initTestCase()
{
    loader = new QMediaPluginLoader(QMediaServiceProviderFactoryInterface_iid,
                                QLatin1String("/mediaservice"),
                                Qt::CaseInsensitive);
}

void tst_QMediaPluginLoader::cleanupTestCase()
{
    delete loader;
}

void tst_QMediaPluginLoader::testInstance()
{
    const QStringList keys = loader->keys();

    if (keys.isEmpty()) // Test is invalidated, skip.
        QSKIP("No plug-ins available", SkipAll);

    foreach (const QString &key, keys)
        QVERIFY(loader->instance(key) != 0);
}

void tst_QMediaPluginLoader::testInstances()
{
    const QStringList keys = loader->keys();

    if (keys.isEmpty()) // Test is invalidated, skip.
        QSKIP("No plug-ins available", SkipAll);

    foreach (const QString &key, keys)
        QVERIFY(loader->instances(key).size() > 0);
}

// Last so as to not interfere with the other tests if there is a failure.
void tst_QMediaPluginLoader::testInvalidKey()
{
    const QString key(QLatin1String("invalid-key"));

    // This test assumes there is no 'invalid-key' in the key list, verify that.
    if (loader->keys().contains(key))
        QSKIP("a plug-in includes the invalid key", SkipAll);

    QVERIFY(loader->instance(key) == 0);

    // Test looking up the key hasn't inserted it into the list. See QMap::operator[].
    QVERIFY(!loader->keys().contains(key));

    QVERIFY(loader->instances(key).isEmpty());
    QVERIFY(!loader->keys().contains(key));
}

QTEST_MAIN(tst_QMediaPluginLoader)

#include "tst_qmediapluginloader.moc"
