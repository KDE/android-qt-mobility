/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its
contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit
(EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef QANDROIDMETADATACONTROL_H
#define QANDROIDMETADATACONTROL_H

#include <QtCore/qobject.h>
#include <qmetadatareadercontrol.h>
#include <qmediaplayer.h>

#include <limits.h>

#include "qandroidplayersession.h"

QT_USE_NAMESPACE

class QAndroidMetaDataControl : public QMetaDataReaderControl
{
    Q_OBJECT
public :
        QAndroidMetaDataControl(QAndroidPlayerSession *session,QObject *parent);
virtual ~QAndroidMetaDataControl();

bool isMetaDataAvailable() const;
bool isWritable() const;

QVariant metaData(QtMultimediaKit::MetaData key) const;
QList<QtMultimediaKit::MetaData> availableMetaData() const;

QVariant extendedMetaData(const QString &key) const ;
QStringList availableExtendedMetaData() const;

private slots:
void updateTags();
private:
QAndroidPlayerSession *m_session;
};

#endif // QANDROIDMETADATACONTROL_H
