/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnetworksession_p.h"
#include "qnetworksession.h"
#include "qnetworksessionengine_win_p.h"
#include "qnlaengine_win_p.h"

#ifndef Q_OS_WINCE
#include "qnativewifiengine_win_p.h"
#include "qioctlwifiengine_win_p.h"
#endif

#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>
#include <QtCore/qmutex.h>

#include <QtNetwork/qnetworkinterface.h>

QT_BEGIN_NAMESPACE

static QNetworkSessionEngine *getEngineFromId(const QString &id)
{
    QNlaEngine *nla = QNlaEngine::instance();
    if (nla && nla->hasIdentifier(id))
        return nla;

#ifndef Q_OS_WINCE
    QNativeWifiEngine *nativeWifi = QNativeWifiEngine::instance();
    if (nativeWifi && nativeWifi->hasIdentifier(id))
        return nativeWifi;

    QIoctlWifiEngine *ioctlWifi = QIoctlWifiEngine::instance();
    if (ioctlWifi && ioctlWifi->hasIdentifier(id))
        return ioctlWifi;
#endif

    return 0;
}

class QNetworkSessionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    QNetworkSessionManagerPrivate(QObject *parent = 0);
    ~QNetworkSessionManagerPrivate();

    void forceSessionClose(const QNetworkConfiguration &config);

Q_SIGNALS:
    void forcedSessionClose(const QNetworkConfiguration &config);
};

#include "qnetworksession_win.moc"

Q_GLOBAL_STATIC(QNetworkSessionManagerPrivate, sessionManager);

QNetworkSessionManagerPrivate::QNetworkSessionManagerPrivate(QObject *parent)
:   QObject(parent)
{
}

QNetworkSessionManagerPrivate::~QNetworkSessionManagerPrivate()
{
}

void QNetworkSessionManagerPrivate::forceSessionClose(const QNetworkConfiguration &config)
{
    emit forcedSessionClose(config);
}

void QNetworkSessionPrivate::syncStateWithInterface()
{
    connect(&manager, SIGNAL(updateCompleted()), this, SLOT(networkConfigurationsChanged()));
    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(configurationChanged(QNetworkConfiguration)));
    connect(sessionManager(), SIGNAL(forcedSessionClose(QNetworkConfiguration)),
            this, SLOT(forcedSessionClose(QNetworkConfiguration)));

    opened = false;
    state = QNetworkSession::Invalid;
    lastError = QNetworkSession::UnknownSessionError;

    qRegisterMetaType<QNetworkSessionEngine::ConnectionError>
        ("QNetworkSessionEngine::ConnectionError");

    switch (publicConfig.type()) {
    case QNetworkConfiguration::InternetAccessPoint:
        activeConfig = publicConfig;
        engine = getEngineFromId(activeConfig.identifier());
        connect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                Qt::QueuedConnection);
        break;
    case QNetworkConfiguration::ServiceNetwork:
        serviceConfig = publicConfig;
        // Defer setting engine and signals until open().
        // fall through
    case QNetworkConfiguration::UserChoice:
        // Defer setting serviceConfig and activeConfig until open().
        // fall through
    default:
        engine = 0;
    }

    networkConfigurationsChanged();
}

void QNetworkSessionPrivate::open()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else if (!isActive) {
        if ((activeConfig.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            lastError =QNetworkSession::InvalidConfigurationError;
            emit q->error(lastError);
            return;
        }
        opened = true;

        if ((activeConfig.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active &&
            (activeConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
            state = QNetworkSession::Connecting;
            emit q->stateChanged(state);

            engine->connectToId(activeConfig.identifier());
        }

        isActive = (activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
        if (isActive)
            emit quitPendingWaitsForOpened();
    }
}

void QNetworkSessionPrivate::close()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else if (isActive) {
        opened = false;
        isActive = false;
        emit q->sessionClosed();
    }
}

void QNetworkSessionPrivate::stop()
{
    if (serviceConfig.isValid()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else {
        if ((activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            state = QNetworkSession::Closing;
            emit q->stateChanged(state);

            engine->disconnectFromId(activeConfig.identifier());

            sessionManager()->forceSessionClose(activeConfig);
        }

        opened = false;
        isActive = false;
        emit q->sessionClosed();
    }
}

void QNetworkSessionPrivate::migrate()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::accept()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::ignore()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::reject()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
    if (!publicConfig.isValid() || !engine || state != QNetworkSession::Connected)
        return QNetworkInterface();

    QString interface = engine->getInterfaceFromId(activeConfig.identifier());

    if (interface.isEmpty())
        return QNetworkInterface();

    return QNetworkInterface::interfaceFromName(interface);
}

QVariant QNetworkSessionPrivate::property(const QString&)
{
    return QVariant();
}

QString QNetworkSessionPrivate::bearerName() const
{
    if (!publicConfig.isValid() || !engine)
        return QString();

    return engine->bearerName(activeConfig.identifier());
}

QString QNetworkSessionPrivate::errorString() const
{
    switch (lastError) {
    case QNetworkSession::UnknownSessionError:
        return tr("Unknown session error.");
    case QNetworkSession::SessionAbortedError:
        return tr("The session was aborted by the user or system.");
    case QNetworkSession::OperationNotSupportedError:
        return tr("The requested operation is not supported by the system.");
    case QNetworkSession::InvalidConfigurationError:
        return tr("The specified configuration cannot be used.");
    }

    return QString();
}

QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return lastError;
}

quint64 QNetworkSessionPrivate::sentData() const
{
    return tx_data;
}

quint64 QNetworkSessionPrivate::receivedData() const
{
    return rx_data;
}

quint64 QNetworkSessionPrivate::activeTime() const
{
    return m_activeTime;
}

void QNetworkSessionPrivate::updateStateFromServiceNetwork()
{
    QNetworkSession::State oldState = state;

    foreach (const QNetworkConfiguration &config, serviceConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active)
            continue;

        if (activeConfig != config) {
            if (engine) {
                disconnect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                           this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)));
            }

            activeConfig = config;
            engine = getEngineFromId(activeConfig.identifier());
            connect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                    this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                    Qt::QueuedConnection);
            emit q->newConfigurationActivated();
        }

        state = QNetworkSession::Connected;
        if (state != oldState)
            emit q->stateChanged(state);

        return;
    }

    if (serviceConfig.children().isEmpty())
        state = QNetworkSession::NotAvailable;
    else
        state = QNetworkSession::Disconnected;

    if (state != oldState)
        emit q->stateChanged(state);
}

void QNetworkSessionPrivate::updateStateFromActiveConfig()
{
    QNetworkSession::State oldState = state;

    bool newActive = false;

    if (!activeConfig.isValid()) {
        state = QNetworkSession::Invalid;
    } else if ((activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
        state = QNetworkSession::Connected;
        newActive = opened;
    } else if ((activeConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
        state = QNetworkSession::Disconnected;
    } else if ((activeConfig.state() & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined) {
        state = QNetworkSession::NotAvailable;
    } else if ((activeConfig.state() & QNetworkConfiguration::Undefined) == QNetworkConfiguration::Undefined) {
        state = QNetworkSession::NotAvailable;
    }

    bool oldActive = isActive;
    isActive = newActive;

    if (!oldActive && isActive)
        emit quitPendingWaitsForOpened();
    if (oldActive && !isActive)
        emit q->sessionClosed();

    if (oldState != state)
        emit q->stateChanged(state);
}

void QNetworkSessionPrivate::networkConfigurationsChanged()
{
    if (serviceConfig.isValid())
        updateStateFromServiceNetwork();
    else
        updateStateFromActiveConfig();
}

void QNetworkSessionPrivate::configurationChanged(const QNetworkConfiguration &config)
{
    if (serviceConfig.isValid() && (config == serviceConfig || config == activeConfig))
        updateStateFromServiceNetwork();
    else if (config == activeConfig)
        updateStateFromActiveConfig();
}

void QNetworkSessionPrivate::forcedSessionClose(const QNetworkConfiguration &config)
{
    if (activeConfig == config) {
        opened = false;
        isActive = false;

        emit q->sessionClosed();

        lastError = QNetworkSession::SessionAbortedError;
        emit q->error(lastError);
    }
}

void QNetworkSessionPrivate::connectionError(const QString &id, QNetworkSessionEngine::ConnectionError error)
{
    if (activeConfig.identifier() == id) {
        switch (error) {
        case QNetworkSessionEngine::OperationNotSupported:
            lastError = QNetworkSession::OperationNotSupportedError;
            opened = false;
            break;
        case QNetworkSessionEngine::InterfaceLookupError:
        case QNetworkSessionEngine::ConnectError:
        case QNetworkSessionEngine::DisconnectionError:
        default:
            lastError = QNetworkSession::UnknownSessionError;
        }

        emit q->error(lastError);
    }
}

QT_END_NAMESPACE

