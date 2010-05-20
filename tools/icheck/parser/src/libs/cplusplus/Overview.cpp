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

#include "Overview.h"
#include "NamePrettyPrinter.h"
#include "TypePrettyPrinter.h"
#include <FullySpecifiedType.h>

using namespace CPlusPlus;

Overview::Overview()
    : _markedArgument(0),
      _markedArgumentBegin(0),
      _markedArgumentEnd(0),
      _showArgumentNames(false),
      _showReturnTypes(false),
      _showFunctionSignatures(true),
      _showFullyQualifiedNames(false)
{ }

Overview::~Overview()
{ }

bool Overview::showArgumentNames() const
{
    return _showArgumentNames;
}

void Overview::setShowArgumentNames(bool showArgumentNames)
{
    _showArgumentNames = showArgumentNames;
}

void Overview::setShowReturnTypes(bool showReturnTypes)
{
    _showReturnTypes = showReturnTypes;
}

bool Overview::showReturnTypes() const
{
    return _showReturnTypes;
}

unsigned Overview::markedArgument() const
{
    return _markedArgument;
}

void Overview::setMarkedArgument(unsigned position)
{
    _markedArgument = position;
}

int Overview::markedArgumentBegin() const
{
    return _markedArgumentBegin;
}

void Overview::setMarkedArgumentBegin(int begin)
{
    _markedArgumentBegin = begin;
}

int Overview::markedArgumentEnd() const
{
    return _markedArgumentEnd;
}

void Overview::setMarkedArgumentEnd(int end)
{
    _markedArgumentEnd = end;
}

bool Overview::showFunctionSignatures() const
{
    return _showFunctionSignatures;
}

void Overview::setShowFunctionSignatures(bool showFunctionSignatures)
{
    _showFunctionSignatures = showFunctionSignatures;
}

bool Overview::showFullyQualifiedNames() const
{
    return _showFullyQualifiedNames;
}

void Overview::setShowFullyQualifiedNamed(bool showFullyQualifiedNames)
{
    _showFullyQualifiedNames = showFullyQualifiedNames;
}

QString Overview::prettyName(const Name *name) const
{
    NamePrettyPrinter pp(this);
    return pp(name);
}

QString Overview::prettyType(const FullySpecifiedType &ty, const Name *name) const
{
    return prettyType(ty, prettyName(name));
}

QString Overview::prettyType(const FullySpecifiedType &ty,
                             const QString &name) const
{
    TypePrettyPrinter pp(this);
    return pp(ty, name);
}
