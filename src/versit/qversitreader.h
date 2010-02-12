/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QVERSITREADER_H
#define QVERSITREADER_H

#include "qversitdocument.h"
#include "qmobilityglobal.h"

#include <QObject>

class QIODevice;
class QTextCodec;

QTM_BEGIN_NAMESPACE

class QVersitReaderPrivate;

// reads a QVersitDocument from i/o device
class Q_VERSIT_EXPORT QVersitReader : public QObject
{
    Q_OBJECT  
    
public:
    enum Error {
        NoError = 0,
        UnspecifiedError,
        IOError,
        OutOfMemoryError,
        NotReadyError,
        ParseError
    };

    enum State {
        InactiveState = 0,
        ActiveState,
        CanceledState,
        FinishedState
    };

    QVersitReader();
    ~QVersitReader();

    // input:
    void setDevice(QIODevice* device);
    QIODevice* device() const;

    void setDefaultCodec(QTextCodec* codec);
    QTextCodec* defaultCodec() const;

    // reading:
    bool startReading();
    void cancel();
    bool waitForFinished(int msec = -1);

    // output:
    QList<QVersitDocument> results() const;

    State state() const;
    Error error() const;

    // Deprecated
    bool Q_DECL_DEPRECATED readAll();
    QList<QVersitDocument> Q_DECL_DEPRECATED result() const;

signals:
    void stateChanged(QVersitReader::State state);
    void resultsAvailable(QList<QVersitDocument>& results);
    void resultsAvailable();
    
private: // data
    QVersitReaderPrivate* d;   
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QVersitReader::State))

#endif // QVERSITREADER_H
