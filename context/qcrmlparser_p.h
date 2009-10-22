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

#ifndef QCRMLPARSER_H
#define QCRMLPARSER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QList>
#include <QString>
class QDomElement;

class KeyData{
public:
    KeyData():m_UID(0), m_bitIndex(0){}
    KeyData(const QString &path, quint64 UID, quint32 bitmask=0);
    quint64 UID() const {return m_UID;}
    quint32 bitIndex() const { return m_bitIndex;}
    QString path()const {return m_path;}

private:
    quint64 m_UID;
    quint32 m_bitIndex;
    QString m_path;
};

class QCRMLParser {
public:
    enum ErrorType{NoError, FileDoesNotExist, ParseError};
    QList<KeyData> parseQCRML(const QString &filePath);
    ErrorType error();
    QString errorString();
private:
    void setError(ErrorType type, const QString &errorString);
    QList<KeyData> parseKey(const QDomElement &element);
    bool parseRepository(const QDomElement &element);
    QList<KeyData> parseKeyRange(const QDomElement &element);
    uint uidStringToUInt32(const QString &uid, bool *ok=false);
    bool checkMandatoryAttributes(const QDomElement &element,
                                  const QStringList &mandatoryAttributes);

    ErrorType m_error;
    QString m_errorString;

    quint32 m_repoUID;
    QList<KeyData> m_keyData;
};
#endif
