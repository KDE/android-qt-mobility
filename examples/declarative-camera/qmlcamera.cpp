/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeEngine>

#if !defined(QT_NO_OPENGL)
#include <QtOpenGL/QGLWidget>
#endif

int main(int argc, char *argv[])
{

#if defined (Q_WS_X11) || defined (Q_WS_MAC)
    //### default to using raster graphics backend for now
    bool gsSpecified = false;
    for (int i = 0; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-graphicssystem") {
            gsSpecified = true;
            break;
        }
    }

    if (!gsSpecified)
        QApplication::setGraphicsSystem("raster");
#endif

    QApplication application(argc, argv);
    const QString mainQmlApp = QLatin1String("qrc:/declarative-camera.qml");
    QDeclarativeView view;
#if !defined(QT_NO_OPENGL) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    view.setViewport(new QGLWidget);
#endif
    view.setSource(QUrl(mainQmlApp));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    // Qt.quit() called in embedded .qml by default only emits
    // quit() signal, so do this (optionally use Qt.exit()).
    QObject::connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    view.showFullScreen();
#else
    view.setGeometry(QRect(100, 100, 800, 480));
    view.show();
#endif
    return application.exec();
}

