#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include <qllcpsocket.h>
#include <qnearfieldmanager.h>
#include <qnearfieldtarget.h>
#include "qnfctestcommon.h"
#include "qnfctestutil.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QLlcpSocket::Error);
Q_DECLARE_METATYPE(QLlcpSocket::State);

class tst_qllcpsocketlocal : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsocketlocal();
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    // ALERT�� Handshake required, do NOT��change the sequence of handshaking testcases.
    void testCase1();   // handshake 1
    void testCase2();   // handshake 2
    void testCase3();
    //void testCase3();

    void negTestCase1();
    void negTestCase1_data();
    void negTestCase2();
    void negTestCase3();
    void negTestCase4();
    void negTestCase5();


private:
     QNearFieldTarget *m_target; // not own
     quint8 m_port;
};

tst_qllcpsocketlocal::tst_qllcpsocketlocal()
{
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*");
    qRegisterMetaType<QNearFieldTarget *>("QLlcpSocket::Error");
    qRegisterMetaType<QNearFieldTarget *>("QLlcpSocket::State");
}


/*!
 Description: Init test case for NFC LLCP connection-less mode socket - local peer

 TestScenario:
     Touch a NFC device with LLCP connection-less service actived

 TestExpectedResults:
     Signal of target detected has been found.
*/
void tst_qllcpsocketlocal::initTestCase()
{
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::AnyTarget);

    QString message("Local Wait touch");
    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);

    m_port = 35;
}

void tst_qllcpsocketlocal::cleanupTestCase()
{
}

/*!
 Description: testCase 1 for NFC LLCP connection-less mode socket - local peer

 TestScenario:
               1. Local peer binds to the remote peer
               2. Local peer sends the "connect-less unit test string" message to the remote peer
               3. Local peer receives the above message sending from the remote peer

 TestExpectedResults:
               1. Local peer binds to local port successfully.
               2. The message has be sent to remote peer.
               3. The message has been received from remote peer.
*/
void tst_qllcpsocketlocal::testCase1()
{
    QString message("testcase1 string");
    QLlcpSocket socket(this);

    // STEP 1:  bind the local port for current socket
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(m_port);
    QVERIFY(ret);

    QString messageBox("handshake 1");
    QNfcTestUtil::ShowMessage(messageBox);

    // STEP 2: Local peer sends the  message to the remote peer
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    socket.writeDatagram(data,strSize,m_target, m_port);

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();
    QCOMPARE(writtenSize, strSize);

    // STEP 3: Receive data from remote peer
    QTRY_VERIFY(!readyReadSpy.isEmpty());
    QByteArray datagram;
    while (socket.hasPendingDatagrams())
    {
       datagram.resize(socket.pendingDatagramSize());
       qint64 readSize = socket.readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    // verify the echoed string is same as the original string
    QString receivedMessage = datagram.data();
    QVERIFY(message == receivedMessage);

    // make sure the no error signal emitted
    QVERIFY(errorSpy.isEmpty());
}


void tst_qllcpsocketlocal::testCase2()
{
    QString message("testcase2 string");
    QLlcpSocket socket(this);

    // STEP 1:  bind the local port for current socket
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(m_port);
    QVERIFY(ret);

    QString messageBox("handshake 2");
    QNfcTestUtil::ShowMessage(messageBox);

    // STEP 2: Local peer sends the  message to the remote peer
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    socket.writeDatagram(data,strSize,m_target, m_port);

    const int Timeout = 10 * 1000;
    ret = socket.waitForBytesWritten(Timeout);
    QVERIFY(ret);
}


/*!
 Description:  connection-less state change checking
*/
void tst_qllcpsocketlocal::testCase3()
{
    QString message("testcase3");
    QNfcTestUtil::ShowMessage(message);
    QLlcpSocket socket(this);
    QCOMPARE(socket.state(), QLlcpSocket::UnconnectedState);
    QSignalSpy stateChangedSpy(&socket, SIGNAL(stateChanged(QLlcpSocket::State)));
    bool ret = socket.bind(m_port);
    QVERIFY(ret);
    QVERIFY(!stateChangedSpy.isEmpty());
    QCOMPARE(socket.state(), QLlcpSocket::BoundState);
}



/*!
 Description: bind negative test -invalid port
*/
void tst_qllcpsocketlocal::negTestCase1()
{
    QLlcpSocket socket(this);
    bool ret = socket.bind(m_port);
    QVERIFY(ret == true);
    // bind again will cause failure
    ret = socket.bind(m_port);
    QVERIFY(ret == false);
    // bind invalid port cause failure
    quint8 unknownPort = -1;
}

void tst_qllcpsocketlocal::negTestCase1_data()
{
    QTest::addColumn<quint8>("port");

    quint8 sdpPort = 14;
    quint8 tooBigPort = 65;
    quint8 invalidPort = -1;

    QTest::newRow("row1") << sdpPort;
    QTest::newRow("row2") << tooBigPort;
    QTest::newRow("row3") << invalidPort;
}

/*!
 Description: bind negative test II - double bind
*/
void tst_qllcpsocketlocal::negTestCase2()
{
    QLlcpSocket socket(this);
    bool ret = socket.bind(m_port);
    QVERIFY(ret == true);

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    // bind again will cause failure
    ret = socket.bind(m_port);
    QVERIFY(ret == false);
    QVERIFY(!errorSpy.isEmpty());
}


/*!
 Description:  readDatagram negative testcase - invalid state machine
*/
void tst_qllcpsocketlocal::negTestCase3()
{
    QLlcpSocket socket(this);
    // readDatagram must be called before bind
    QByteArray datagram;
    datagram.resize(127);
    qint64 ret = socket.readDatagram(datagram.data(), datagram.size());
    QVERIFY(ret == -1);
}

/*!
 Description: writeDatagram negative testcase I - invalid port num
*/
void tst_qllcpsocketlocal::negTestCase4()
{
    QLlcpSocket socket(this);
    QString message = "Oops, Invalid port num for writeDatagram";
    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint8 invalidPort = -1;
    qint64 ret = socket.writeDatagram(data,strSize,m_target, invalidPort);
    QVERIFY(ret == -1);
}

/*!
 Description: negative testcase II - invalid usage of connection-oriented API
*/
void tst_qllcpsocketlocal::negTestCase5()
{
    QLlcpSocket socket(this);

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    socket.connectToService(m_target,"uri");
    QVERIFY(errorSpy.count() == 1);

    socket.disconnectFromService();
    QVERIFY(errorSpy.count() == 2);

    QVERIFY(socket.waitForConnected() == false);
    QVERIFY(socket.waitForDisconnected() == false);

    QString message = "Oops, must follow a port parameter";
    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint64 ret = socket.writeDatagram(data,strSize);
    QVERIFY(ret == -1);
}

QTEST_MAIN(tst_qllcpsocketlocal);

#include "tst_qllcpsocketlocal.moc"
