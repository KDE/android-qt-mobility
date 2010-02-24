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

class IMyInterface 
{
    
public:
    //successful part
    Q_SIGNAL void signalFct1();
    Q_SIGNAL void signalFct2();
    Q_SIGNAL void signalFct3(long arg1, double arg2, QObject arg3);
    Q_SIGNAL void signalFct4(bool arg1, QString arg2, int arg3);

    Q_SIGNAL void signalFct5(const long& arg1, const double& arg2, const QObject& arg3);
    Q_SIGNAL void signalFct6(const bool& arg1, const QString& arg2, const int& arg3);
    Q_SIGNAL void signalFct7(const long* arg1, const double* arg2, const QObject* arg3);
    Q_SIGNAL void signalFct8(const bool* arg1, const QString* arg2, const int* arg3);
signals:
    void signalFct9(long& arg1, double& arg2, QObject& arg3);
    void signalFct10(bool& arg1, QString& arg2, int& arg3);
    void signalFct11(long* arg1, double* arg2, QObject* arg3);
    void signalFct12(bool* arg1, QString* arg2, int* arg3);

    void signalFct13(long arg1, double arg2, QObject arg3);
    void signalFct14(bool arg1, QString arg2, int arg3);
    void signalFct15(long arg1, double arg2, QObject arg3);
    void signalFct16(bool arg1, QString arg2, int arg3);

    //this is implemented in the base class
    void implementedInBase(bool arg1, QString arg2, int arg3);

    //unsuccessful  part
    Q_SIGNAL int signalFct17(bool arg1, QString arg2, int arg3); //return type
    Q_SIGNAL void diffnames1(bool arg1, QString arg2, int arg3); //name
    Q_SIGNAL void overloadedDiffFct(bool arg1, QString arg2, int arg3);//first parameter

signals;
    void noSingnal();//signal
};
