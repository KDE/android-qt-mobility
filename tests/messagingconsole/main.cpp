#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <stdio.h>
#include "testsms.h"

QTM_USE_NAMESPACE

#define COMMAND_SEND_SMS            "send"
#define COMMAND_SEND_EMAIL          "send-email"
#define COMMAND_FOLDERS             "folders"
#define COMMAND_MESSAGES_COUNT      "mcount"
#define COMMAND_MESSAGES_QUERY_SMS      "query-sms"
#define COMMAND_MESSAGES_QUERY_EMAIL    "query-email"
#define COMMAND_MESSAGE             "message"
#define COMMAND_GET_MESSAGE         "message-get"
#define COMMAND_COMPOSE             "compose"
#define COMMAND_COMPOSE_QMF         "compose-qmf"
#define COMMAND_SHOW                "show"
#define COMMAND_SHOW_QMF            "show-qmf"
#define COMMAND_CONSTRUCT           "construct"
#define COMMAND_COUNT_ACCOUNTS      "count-accounts"
#define COMMAND_QUERY_ACCOUNTS      "query-accounts"
#define COMMAND_REMOVE_QMF_ACCOUNT  "remove-qmf-account"



void printUsage(const char *progname)
{
    qWarning() << "usage: " << progname << " < " << COMMAND_SEND_SMS
            << "|" << COMMAND_FOLDERS
            << "|" << COMMAND_MESSAGES_COUNT
            << "|" << COMMAND_MESSAGES_QUERY_SMS
            << "|" << COMMAND_COMPOSE
            << " >";
}

int main(int argc, char *argv[])
{
    qDebug() << "hello";
    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
    TestSms test;

    qDebug() << argc << " " << argv[0] <<  " " << argv[1];

    if (argc < 2) {
        printUsage(argv[0]);
        return -1;
     }

    if (QString(argv[1]) == QString(COMMAND_SEND_SMS))
    {
        test.startSmsSending();
    } else if (QString(argv[1]) == QString(COMMAND_SEND_EMAIL))
    {
        test.startEmailSending();
    } else if (QString(argv[1]) == QString(COMMAND_FOLDERS))
    {
        test.testQueryFolders();
    } else if (QString(argv[1]) == QString(COMMAND_MESSAGES_QUERY_SMS))
    {
        test.testMessageManager_queryMessage(QMessageFilter::byType(QMessage::Sms)/* & QMessageFilter::byStandardFolder(QMessage::InboxFolder)*/);
    } else if (QString(argv[1]) == QString(COMMAND_MESSAGES_QUERY_EMAIL))
    {
        test.testMessageManager_queryMessage(QMessageFilter::byType(QMessage::Email)/* & QMessageFilter::byStandardFolder(QMessage::InboxFolder)*/);
    } else if (QString(argv[1]) == QString(COMMAND_MESSAGES_COUNT))
    {
        test.testMessageService_countMessages();
    } else if (QString(argv[1]) == QString(COMMAND_MESSAGE))
    {
        test.testMessage();
    } else if (QString(argv[1]) == QString(COMMAND_GET_MESSAGE))
    {
        test.getMessage(QString(argv[2]));
    } else if (QString(argv[1]) == QString(COMMAND_COMPOSE))
    {
        test.compose();
    } else if (QString(argv[1]) == QString(COMMAND_COMPOSE_QMF))
    {
        test.qmf_compose();
    }else if (QString(argv[1]) == QString(COMMAND_SHOW))
    {
        test.show(QString(argv[2]));
    } else if (QString(argv[1]) == QString(COMMAND_SHOW_QMF))
    {
        test.qmf_show(QString(argv[2]));
    } else if (QString(argv[1]) == QString(COMMAND_CONSTRUCT))
    {
        test.construct();
    } else if (QString(argv[1]) == QString(COMMAND_COUNT_ACCOUNTS))
    {
        test.testCountAccounts();
    } else if (QString(argv[1]) == QString(COMMAND_QUERY_ACCOUNTS))
    {
        test.testQueryAccounts();
    } else if (QString(argv[1]) == QString(COMMAND_REMOVE_QMF_ACCOUNT))
    {
        test.testRemoveQMFAccount(QString(argv[2]));
    } else {
        printUsage(argv[0]);
        return -1;
    }



    return a.exec();
}
