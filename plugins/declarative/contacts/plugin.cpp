/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QApplication>
#include <QtDeclarative>
#include <QDeclarativeExtensionPlugin>
#include <QDebug>
#include "qmlcontactmodel.h"
#include "qmlcontact.h"
#include "qmlcontactdetail.h"
#include "qmlcontactdetailfield.h"
// Qt 4.7.0b2 and rc1 have a source break - turn off for now
// #include "imageprovider.h"

QT_USE_NAMESPACE


class QContactQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.contacts"));
        qmlRegisterType<QMLContactModel>(uri, 1, 0, "QmlContactModel");
        qmlRegisterType<QMLContact>(uri, 1, 0, "QmlContact");
        qmlRegisterType<QMLContactDetail>(uri, 1, 0, "QmlContactDetail");
        qmlRegisterType<QMLContactDetailField>(uri, 1, 0, "QmlContactDetailField");
    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri) {
        Q_UNUSED(uri);
        //engine->addImageProvider("thumbnail", new ContactThumbnailImageProvider);//due to source break
    }
};

#include "plugin.moc"

Q_EXPORT_PLUGIN2(qcontactqmlplugin, QContactQmlPlugin);

