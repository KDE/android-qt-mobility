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

#include "qaudiosystemplugin.h"
#include "qaudiopluginloader_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtGui/qapplication.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/qfactoryinterface.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QAudioPluginLoader::QAudioPluginLoader(const char *iid, const QString &location, Qt::CaseSensitivity):
    m_iid(iid)
{
    m_location = location + "/";
    load();
}

QAudioPluginLoader::~QAudioPluginLoader()
{
    for (int i = 0; i < m_plugins.count(); i++ ) {
        delete m_plugins.at(i);
    }
}

QStringList QAudioPluginLoader::pluginList() const
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    QStringList paths = QApplication::libraryPaths();
#ifdef QTM_PLUGIN_PATH
    paths << QLatin1String(QTM_PLUGIN_PATH);
#endif
#if !defined QT_NO_DEBUG
    if (showDebug)
        qDebug() << "Plugin paths:" << paths;
#endif

    //temp variable to avoid multiple identic path
    QSet<QString> processed;

    /* Discover a bunch o plugins */
    QStringList plugins;

    /* Enumerate our plugin paths */
    for (int i=0; i < paths.count(); i++) {
        if (processed.contains(paths.at(i)))
            continue;
        processed.insert(paths.at(i));
        QDir pluginsDir(paths.at(i)+m_location);
        if (!pluginsDir.exists())
            continue;

        QStringList files = pluginsDir.entryList(QDir::Files);
#if !defined QT_NO_DEBUG
        if (showDebug)
            qDebug()<<"Looking for plugins in "<<pluginsDir.path()<<files;
#endif
        for (int j=0; j < files.count(); j++) {
            plugins <<  pluginsDir.absoluteFilePath(files.at(j));
        }
    }
    return  plugins;
}

QStringList QAudioPluginLoader::keys() const
{
    QMutexLocker(m_mutex());

    QStringList list;
    for (int i = 0; i < m_plugins.count(); i++) {
        QAudioSystemPlugin* p = qobject_cast<QAudioSystemPlugin*>(m_plugins.at(i)->instance());
        if (p) list << p->keys();
    }

    return list;
}

QObject* QAudioPluginLoader::instance(QString const &key)
{
    QMutexLocker(mutex());

    for (int i = 0; i < m_plugins.count(); i++) {
        QAudioSystemPlugin* p = qobject_cast<QAudioSystemPlugin*>(m_plugins.at(i)->instance());
        if (p && p->keys().contains(key))
            return m_plugins.at(i)->instance();
    }
    return 0;
}

QList<QObject*> QAudioPluginLoader::instances(QString const &key)
{
    QMutexLocker(mutex());

    QList<QObject*> list;
    for (int i = 0; i < m_plugins.count(); i++) {
        QAudioSystemPlugin* p = qobject_cast<QAudioSystemPlugin*>(m_plugins.at(i)->instance());
        if (p && p->keys().contains(key))
            list << m_plugins.at(i)->instance();
    }
    return list;
}

void QAudioPluginLoader::load()
{
    if (!m_plugins.isEmpty())
        return;

    QStringList plugins = pluginList();
    for (int i=0; i < plugins.count(); i++) {
        QPluginLoader* loader = new QPluginLoader(plugins.at(i));
        QObject *o = loader->instance();
        if (o != 0 && o->qt_metacast(m_iid) != 0)
            m_plugins.append(loader);
        else {
            qWarning() << "QAudioPluginLoader: Failed to load plugin: "
                << plugins.at(i) << loader->errorString();
        }
    }
}
QT_END_NAMESPACE

