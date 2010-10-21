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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef PATHMAPPER_PROXY_SYMBIAN_P_H
#define PATHMAPPER_PROXY_SYMBIAN_P_H

#include <QStringList>
#include <QHash>

#include <qmobilityglobal.h>
#include <e32base.h>

QTM_BEGIN_NAMESPACE

class PathMapper : public QObject
{
    Q_OBJECT

public:
    PathMapper();
    ~PathMapper();

    enum Target {TargetCRepository, TargetRPropery};

    bool getChildren(const QString &path, QSet<QString> &children) const;
    QStringList childPaths(const QString &path) const;
    bool resolvePath(const QString &path, Target &target, quint32 &category, quint32 &key) const;

private:
    class PathData
    {
    public:
        PathData() : m_target(TargetRPropery), m_category(0), m_key(0) {}
        PathData(Target target, quint32 category, quint32 key) :
            m_target(target), m_category(category), m_key(key) {}

    public:
        Target m_target;
        quint32 m_category;
        quint32 m_key;
    };

    class RPathMapperServerSession : public RSessionBase
    {
    public:
        RPathMapperServerSession();

    public:
        TInt Connect();
        TVersion Version() const;

        bool getChildren(const QString &path, QSet<QString> &children) const;
        QStringList childPaths(const QString &path) const;
        bool resolvePath(const QString &path, Target &target, quint32 &category, quint32 &key) const;

    private:
        TInt StartServer();
    };

private:
    RPathMapperServerSession m_pathMapperServerSession;
};

QTM_END_NAMESPACE

#endif //PATHMAPPER_PROXY_SYMBIAN_P_H
