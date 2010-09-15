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

#ifndef QGALLERYABSTRACTREQUEST_H
#define QGALLERYABSTRACTREQUEST_H

#include <qmobilityglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

class QAbstractGallery;
class QGalleryAbstractResponse;

class QGalleryAbstractRequestPrivate;

class Q_GALLERY_EXPORT QGalleryAbstractRequest : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryAbstractRequest)
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery)
    Q_PROPERTY(bool supported READ isSupported NOTIFY supportedChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(int currentProgress READ currentProgress NOTIFY progressChanged)
    Q_PROPERTY(int maximumProgress READ maximumProgress NOTIFY progressChanged)
    Q_ENUMS(Status)
    Q_ENUMS(RequestType)
public:
    enum Status
    {
        Inactive,
        Active,
        Cancelling,
        Cancelled,
        Idle,
        Finished,
        Error
    };

    enum RequestError
    {
        NoError,
        NoGallery,
        NotSupported,
        GalleryError = 100
    };

    enum RequestType
    {
        QueryRequest,
        ItemRequest,
        TypeRequest,
        RemoveRequest
    };

    explicit QGalleryAbstractRequest(RequestType type, QObject *parent = 0);
    explicit QGalleryAbstractRequest(
            QAbstractGallery *gallery, RequestType type, QObject *parent = 0);
    ~QGalleryAbstractRequest();

    QAbstractGallery *gallery() const;
    void setGallery(QAbstractGallery *gallery);

    bool isSupported() const;

    RequestType type() const;
    Status status() const;

    int error() const;
    QString errorString() const;

    int currentProgress() const;
    int maximumProgress() const;

    bool waitForFinished(int msecs);

public Q_SLOTS:
    void execute();
    void cancel();
    void clear();

Q_SIGNALS:
    void supportedChanged();
    void finished();
    void cancelled();
    void error(int error, const QString &errorString);
    void statusChanged(QGalleryAbstractRequest::Status status);
    void errorChanged();
    void progressChanged(int current, int maximum);

protected:
    virtual void setResponse(QGalleryAbstractResponse *response) = 0;

    explicit QGalleryAbstractRequest(QGalleryAbstractRequestPrivate &dd, QObject *parent);

    QScopedPointer<QGalleryAbstractRequestPrivate> d_ptr;

private:
    Q_PRIVATE_SLOT(d_ptr, void _q_finished())
    Q_PRIVATE_SLOT(d_ptr, void _q_cancelled())
    Q_PRIVATE_SLOT(d_ptr, void _q_resumed())
    Q_PRIVATE_SLOT(d_ptr, void _q_progressChanged(int, int))
};

QTM_END_NAMESPACE

#endif
