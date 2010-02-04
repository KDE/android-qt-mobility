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

#ifndef CPPBINDINGS_H
#define CPPBINDINGS_H

#include "CppDocument.h"

#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QByteArray>

namespace CPlusPlus {

class Location;
class Binding;
class NamespaceBinding;
class ClassBinding;

typedef QSharedPointer<Binding> BindingPtr;
typedef QSharedPointer<ClassBinding> ClassBindingPtr;
typedef QSharedPointer<NamespaceBinding> NamespaceBindingPtr;

class CPLUSPLUS_EXPORT Location
{
public:
    Location();
    Location(Symbol *symbol);
    Location(const StringLiteral *fileId, unsigned sourceLocation);

    inline bool isValid() const
    { return _fileId != 0; }

    inline operator bool() const
    { return _fileId != 0; }

    inline const StringLiteral *fileId() const
    { return _fileId; }

    inline unsigned sourceLocation() const
    { return _sourceLocation; }

private:
    const StringLiteral *_fileId;
    unsigned _sourceLocation;
};

class CPLUSPLUS_EXPORT Binding
{
    Q_DISABLE_COPY(Binding)

public:
    Binding() {}
    virtual ~Binding() {}

    virtual QByteArray qualifiedId() const = 0;
    virtual NamespaceBinding *asNamespaceBinding() { return 0; }
    virtual ClassBinding *asClassBinding() { return 0; }

    virtual ClassBinding *findClassBinding(const Name *name, QSet<Binding *> *processed) = 0;
    virtual Binding *findClassOrNamespaceBinding(const Identifier *id, QSet<Binding *> *processed) = 0;
};

class CPLUSPLUS_EXPORT NamespaceBinding: public Binding
{
public:
    /// Constructs a binding with the given parent.
    NamespaceBinding(NamespaceBinding *parent = 0);

    /// Destroys the binding.
    virtual ~NamespaceBinding();

    /// Returns this binding's name.
    const NameId *name() const;

    /// Returns this binding's identifier.
    const Identifier *identifier() const;

    /// Returns the binding for the global namespace (aka ::).
    NamespaceBinding *globalNamespaceBinding();

    /// Returns the binding for the given namespace symbol.
    NamespaceBinding *findNamespaceBinding(const Name *name);

    /// Returns the binding associated with the given symbol.
    NamespaceBinding *findOrCreateNamespaceBinding(Namespace *symbol);

    NamespaceBinding *resolveNamespace(const Location &loc,
                                       const Name *name,
                                       bool lookAtParent = true);

    virtual ClassBinding *findClassBinding(const Name *name, QSet<Binding *> *processed);
    virtual Binding *findClassOrNamespaceBinding(const Identifier *id, QSet<Binding *> *processed);

    /// Helpers.
    virtual QByteArray qualifiedId() const;
    void dump();

    virtual NamespaceBinding *asNamespaceBinding() { return this; }

    static NamespaceBinding *find(Namespace *symbol, NamespaceBinding *binding);
    static ClassBinding *find(Class *symbol, NamespaceBinding *binding);

private:
    NamespaceBinding *findNamespaceBindingForNameId(const NameId *name,
                                                    bool lookAtParentNamespace);

    NamespaceBinding *findNamespaceBindingForNameId_helper(const NameId *name,
                                                           bool lookAtParentNamespace,
                                                           QSet<NamespaceBinding *> *processed);

public: // attributes
    /// This binding's parent.
    NamespaceBinding *parent;

    /// Binding for anonymous namespace symbols.
    NamespaceBinding *anonymousNamespaceBinding;

    /// This binding's connections.
    QList<NamespaceBinding *> children;

    /// This binding's list of using namespaces.
    QList<NamespaceBinding *> usings;

    /// This binding's namespace symbols.
    QList<Namespace *> symbols;

    QList<ClassBinding *> classBindings;
};

class CPLUSPLUS_EXPORT ClassBinding: public Binding
{
public:
    ClassBinding(NamespaceBinding *parent);
    ClassBinding(ClassBinding *parentClass);
    virtual ~ClassBinding();

    virtual ClassBinding *asClassBinding() { return this; }

    /// Returns this binding's name.
    const Name *name() const;

    /// Returns this binding's identifier.
    const Identifier *identifier() const;
    virtual QByteArray qualifiedId() const;

    virtual ClassBinding *findClassBinding(const Name *name, QSet<Binding *> *processed);
    virtual Binding *findClassOrNamespaceBinding(const Identifier *id, QSet<Binding *> *processed);

    void dump();

public: // attributes
    Binding *parent;

    QList<ClassBinding *> children;

    /// This binding's class symbols.
    QList<Class *> symbols;

    /// Bindings for the base classes.
    QList<ClassBinding *> baseClassBindings;
};

CPLUSPLUS_EXPORT NamespaceBindingPtr bind(Document::Ptr doc, Snapshot snapshot);

} // end of namespace CPlusPlus

#endif // CPPBINDINGS_H
