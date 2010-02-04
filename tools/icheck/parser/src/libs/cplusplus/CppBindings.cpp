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

#include "CppBindings.h"
#include "CppDocument.h"
#include "Overview.h"

#include <CoreTypes.h>
#include <Symbols.h>
#include <Literals.h>
#include <Names.h>
#include <Scope.h>
#include <Control.h>
#include <SymbolVisitor.h>

#include <QtDebug>

using namespace CPlusPlus;


////////////////////////////////////////////////////////////////////////////////
// Location
////////////////////////////////////////////////////////////////////////////////
Location::Location()
    : _fileId(0),
      _sourceLocation(0)
{ }

Location::Location(Symbol *symbol)
    : _fileId(symbol->fileId()),
      _sourceLocation(symbol->sourceLocation())
{ }

Location::Location(const StringLiteral *fileId, unsigned sourceLocation)
    : _fileId(fileId), _sourceLocation(sourceLocation)
{ }

////////////////////////////////////////////////////////////////////////////////
// NamespaceBinding
////////////////////////////////////////////////////////////////////////////////

NamespaceBinding::NamespaceBinding(NamespaceBinding *parent)
    : parent(parent),
      anonymousNamespaceBinding(0)
{
    if (parent)
        parent->children.append(this);
}

NamespaceBinding::~NamespaceBinding()
{
    qDeleteAll(children);
    qDeleteAll(classBindings);
}

const NameId *NamespaceBinding::name() const
{
    if (symbols.size()) {
        if (const Name *name = symbols.first()->name()) {
            const NameId *nameId = name->asNameId();
            Q_ASSERT(nameId != 0);

            return nameId;
        }
    }

    return 0;
}

const Identifier *NamespaceBinding::identifier() const
{
    if (const NameId *nameId = name())
        return nameId->identifier();

    return 0;
}

NamespaceBinding *NamespaceBinding::globalNamespaceBinding()
{
    NamespaceBinding *it = this;

    for (; it; it = it->parent) {
        if (! it->parent)
            break;
    }

    return it;
}

Binding *NamespaceBinding::findClassOrNamespaceBinding(const Identifier *id, QSet<Binding *> *processed)
{
    if (processed->contains(this))
        return 0;

    processed->insert(this);

    if (id->isEqualTo(identifier()))
        return const_cast<NamespaceBinding *>(this);

    foreach (NamespaceBinding *nestedNamespaceBinding, children) {
        if (id->isEqualTo(nestedNamespaceBinding->identifier()))
            return nestedNamespaceBinding;
    }

    foreach (ClassBinding *classBinding, classBindings) {
        if (id->isEqualTo(classBinding->identifier()))
            return classBinding;
    }

    foreach (NamespaceBinding *u, usings) {
        if (Binding *b = u->findClassOrNamespaceBinding(id, processed))
            return b;
    }

    if (parent)
        return parent->findClassOrNamespaceBinding(id, processed);

    return 0;
}

ClassBinding *NamespaceBinding::findClassBinding(const Name *name, QSet<Binding *> *processed)
{
    if (! name)
        return 0;

    if (processed->contains(this))
        return 0;

    if (const QualifiedNameId *q = name->asQualifiedNameId()) {
        Binding *current = this;

        for (unsigned i = 0; i < q->nameCount(); ++i) {
            const Identifier *nameId = q->nameAt(i)->identifier();
            if (! nameId)
                return 0;

            QSet<Binding *> visited;
            Binding *binding = current->findClassOrNamespaceBinding(nameId, &visited); // ### TODO: check recursion.
            if (! binding)
                return 0;

            current = binding;
        }

        return current->asClassBinding();
    }

    processed->insert(this);

    const Identifier *id = name->identifier();

    foreach (ClassBinding *classBinding, classBindings) {
        if (id->isEqualTo(classBinding->identifier()))
            return classBinding;
    }

    if (parent)
        return parent->findClassBinding(name, processed);

    foreach (NamespaceBinding *u, usings) {
        if (ClassBinding *classBinding = u->findClassBinding(name, processed))
            return classBinding;
    }

    return 0;
}

NamespaceBinding *NamespaceBinding::findNamespaceBinding(const Name *name)
{
    if (! name)
        return anonymousNamespaceBinding;

    else if (const NameId *nameId = name->asNameId())
        return findNamespaceBindingForNameId(nameId, /*lookAtParent = */ true);

    else if (const QualifiedNameId *q = name->asQualifiedNameId()) {
        NamespaceBinding *current = this;

        for (unsigned i = 0; i < q->nameCount(); ++i) {
            const NameId *namespaceName = q->nameAt(i)->asNameId();
            if (! namespaceName)
                return 0;

            bool lookAtParent = false;
            if (i == 0)
                lookAtParent = true;

            NamespaceBinding *binding = current->findNamespaceBindingForNameId(namespaceName, lookAtParent);
            if (! binding)
                return 0;

            current = binding;
        }

        return current;
    }

    // invalid binding
    return 0;
}

NamespaceBinding *NamespaceBinding::findNamespaceBindingForNameId(const NameId *name,
                                                                  bool lookAtParentNamespace)
{
    QSet<NamespaceBinding *> processed;
    return findNamespaceBindingForNameId_helper(name, lookAtParentNamespace, &processed);
}

NamespaceBinding *NamespaceBinding::findNamespaceBindingForNameId_helper(const NameId *name,
                                                                         bool lookAtParentNamespace,
                                                                         QSet<NamespaceBinding *> *processed)
{
    if (processed->contains(this))
        return 0;

    processed->insert(this);

    foreach (NamespaceBinding *binding, children) {
        const Name *bindingName = binding->name();

        if (! bindingName)
            continue;

        if (const NameId *bindingNameId = bindingName->asNameId()) {
            if (name->isEqualTo(bindingNameId))
                return binding;
        }
    }

    foreach (NamespaceBinding *u, usings) {
        if (NamespaceBinding *b = u->findNamespaceBindingForNameId_helper(name, lookAtParentNamespace, processed)) {
            return b;
        }
    }

    if (lookAtParentNamespace && parent)
        return parent->findNamespaceBindingForNameId_helper(name, lookAtParentNamespace, processed);

    return 0;
}

NamespaceBinding *NamespaceBinding::findOrCreateNamespaceBinding(Namespace *symbol)
{
    if (NamespaceBinding *binding = findNamespaceBinding(symbol->name())) {
        int index = 0;

        for (; index < binding->symbols.size(); ++index) {
            Namespace *ns = binding->symbols.at(index);

            if (ns == symbol)
                break;
        }

        if (index == binding->symbols.size())
            binding->symbols.append(symbol);

        return binding;
    }

    NamespaceBinding *binding = new NamespaceBinding(this);
    binding->symbols.append(symbol);

    if (! symbol->name()) {
        Q_ASSERT(! anonymousNamespaceBinding);

        anonymousNamespaceBinding = binding;
    }

    return binding;
}

static void closure(const Location &loc,
                    NamespaceBinding *binding, const Name *name,
                    QList<NamespaceBinding *> *bindings)
{
    if (bindings->contains(binding))
        return;

    bindings->append(binding);

    Q_ASSERT(name->isNameId());

    const Identifier *id = name->asNameId()->identifier();
    bool ignoreUsingDirectives = false;

    foreach (Namespace *symbol, binding->symbols) {
        Scope *scope = symbol->members();

        for (Symbol *symbol = scope->lookat(id); symbol; symbol = symbol->next()) {
            if (symbol->name() != name || ! symbol->isNamespace())
                continue;

            const Location l(symbol);

            if (l.fileId() == loc.fileId() && l.sourceLocation() < loc.sourceLocation()) {
                ignoreUsingDirectives = true;
                break;
            }
        }
    }

    if (ignoreUsingDirectives)
        return;

    foreach (NamespaceBinding *u, binding->usings)
        closure(loc, u, name, bindings);
}


NamespaceBinding *NamespaceBinding::resolveNamespace(const Location &loc,
                                                     const Name *name,
                                                     bool lookAtParent)
{
    if (! name)
        return 0;

    else if (const NameId *nameId = name->asNameId()) {
        QList<NamespaceBinding *> bindings;
        closure(loc, this, nameId, &bindings);

        QList<NamespaceBinding *> results;

        foreach (NamespaceBinding *binding, bindings) {
            if (NamespaceBinding *b = binding->findNamespaceBinding(nameId))
                results.append(b);
        }

        if (results.size() == 1)
            return results.at(0);

        else if (results.size() > 1) {
            // ### FIXME: return 0;
            return results.at(0);
        }

        else if (parent && lookAtParent)
            return parent->resolveNamespace(loc, name);

    } else if (const QualifiedNameId *q = name->asQualifiedNameId()) {
        if (q->nameCount() == 1) {
            Q_ASSERT(q->isGlobal());

            return globalNamespaceBinding()->resolveNamespace(loc, q->nameAt(0));
        }

        NamespaceBinding *current = this;
        if (q->isGlobal())
            current = globalNamespaceBinding();

        current = current->resolveNamespace(loc, q->nameAt(0));
        for (unsigned i = 1; current && i < q->nameCount(); ++i)
            current = current->resolveNamespace(loc, q->nameAt(i), false);

        return current;
    }

    return 0;
}

// ### rewrite me
QByteArray NamespaceBinding::qualifiedId() const
{
    if (! parent)
        return "<root>";

    QByteArray s;

    s.append(parent->qualifiedId());
    s.append("::");

    if (const Identifier *id = identifier())
        s.append(id->chars(), id->size());

    else
        s.append("<anonymous>");

    return s;
}

// ### rewrite me
QByteArray ClassBinding::qualifiedId() const
{
    QByteArray s = parent->qualifiedId();
    s += "::";

    if (const Identifier *id = identifier())
        s.append(id->chars(), id->size());

    else
        s.append("<anonymous>");

    return s;
}

Binding *ClassBinding::findClassOrNamespaceBinding(const Identifier *id, QSet<Binding *> *processed)
{
    if (id->isEqualTo(identifier()))
        return this;

    if (processed->contains(this))
        return 0;

    processed->insert(this);

    foreach (ClassBinding *nestedClassBinding, children) {
        if (id->isEqualTo(nestedClassBinding->identifier()))
            return nestedClassBinding;
    }

    foreach (ClassBinding *baseClassBinding, baseClassBindings) {
        if (! baseClassBinding)
            continue;

        else if (Binding *b = baseClassBinding->findClassOrNamespaceBinding(id, processed))
            return b;
    }

    if (parent)
        return parent->findClassOrNamespaceBinding(id, processed);

    return 0;
}

ClassBinding *ClassBinding::findClassBinding(const Name *name, QSet<Binding *> *processed)
{
    if (! name)
        return 0;

    if (processed->contains(this))
        return 0;

    processed->insert(this);

    if (const QualifiedNameId *q = name->asQualifiedNameId()) {
        Binding *currentBinding = this;

        for (unsigned i = 0; i < q->nameCount() - 1; ++i) {
            const Identifier *id = q->nameAt(i)->identifier();
            if (! id)
                return 0;

            Binding *classOrNamespaceBinding = currentBinding->findClassOrNamespaceBinding(id, processed);

            if (! classOrNamespaceBinding)
                return 0;

            currentBinding = classOrNamespaceBinding;
        }

        if (currentBinding)
            return currentBinding->findClassBinding(q->unqualifiedNameId(), processed);

        return 0;
    }

    if (const Identifier *id = name->identifier()) {
        if (id->isEqualTo(identifier()))
            return this;

        foreach (ClassBinding *nestedClassBinding, children) {
            if (const Identifier *nestedClassId = nestedClassBinding->identifier()) {
                if (nestedClassId->isEqualTo(id))
                    return nestedClassBinding;
            }
        }

        if (parent)
            return parent->findClassBinding(name, processed);
    }

    return 0;
}

static int depth;

void NamespaceBinding::dump()
{
    qDebug() << QByteArray(depth, ' ').constData() << "namespace" << qualifiedId().constData()
              << " # " << symbols.size();

    ++depth;

    foreach (ClassBinding *classBinding, classBindings) {
        classBinding->dump();
    }

    foreach (NamespaceBinding *child, children) {
        child->dump();
    }

    --depth;
}

void ClassBinding::dump()
{
    qDebug() << QByteArray(depth, ' ').constData() << "class" << qualifiedId().constData()
              << " # " << symbols.size();

    ++depth;

    foreach (ClassBinding *classBinding, children) {
        classBinding->dump();
    }

    --depth;
}

////////////////////////////////////////////////////////////////////////////////
// ClassBinding
////////////////////////////////////////////////////////////////////////////////
ClassBinding::ClassBinding(NamespaceBinding *parent)
    : parent(parent)
{
    parent->classBindings.append(this);
}

ClassBinding::ClassBinding(ClassBinding *parentClass)
    : parent(parentClass)
{
    parentClass->children.append(this);
}

ClassBinding::~ClassBinding()
{ qDeleteAll(children); }

const Name *ClassBinding::name() const
{
    if (symbols.isEmpty())
        return 0;

    return symbols.first()->name();
}

const Identifier *ClassBinding::identifier() const
{
    if (const Name *n = name())
        return n->identifier();

    return 0;
}

namespace {

////////////////////////////////////////////////////////////////////////////////
// Binder
////////////////////////////////////////////////////////////////////////////////

class Binder: protected SymbolVisitor
{
public:
    Binder(NamespaceBinding *globals);
    virtual ~Binder();

    NamespaceBinding *operator()(Document::Ptr doc, const Snapshot &snapshot)
    {
        namespaceBinding = _globals;
        const Snapshot previousSnapshot = _snapshot;

        _snapshot = snapshot;
        (void) bind(doc);
        _snapshot = previousSnapshot;

        return _globals;
    }

    Snapshot _snapshot;

protected:
    NamespaceBinding *bind(Document::Ptr doc)
    {
        QSet<QString> processed;
        return bind(doc, &processed);
    }

    NamespaceBinding *bind(Document::Ptr doc, QSet<QString> *processed)
    {
        if (processed->contains(doc->fileName()))
            return 0;

        processed->insert(doc->fileName());

        foreach (const Document::Include &i, doc->includes()) {
            if (Document::Ptr includedDoc = _snapshot.document(i.fileName())) {
                /*NamepaceBinding *binding = */ bind(includedDoc, processed);
            }
        }

        Namespace *ns = doc->globalNamespace();
        _globals->symbols.append(ns);

        for (unsigned i = 0; i < ns->memberCount(); ++i) {
            (void) bind(ns->memberAt(i), _globals);
        }

        return _globals;
    }

    NamespaceBinding *bind(Symbol *symbol, NamespaceBinding *binding);
    NamespaceBinding *findOrCreateNamespaceBinding(Namespace *symbol);
    NamespaceBinding *resolveNamespace(const Location &loc, const Name *name);

    NamespaceBinding *switchNamespaceBinding(NamespaceBinding *binding);

    ClassBinding *findOrCreateClassBinding(Class *classSymbol);
    ClassBinding *findClassBinding(const Name *name);

    ClassBinding *switchClassBinding(ClassBinding *binding);

    using SymbolVisitor::visit;

    virtual bool visit(Namespace *);
    virtual bool visit(UsingNamespaceDirective *);
    virtual bool visit(Class *);
    virtual bool visit(Function *);
    virtual bool visit(Block *);

private:
    NamespaceBinding *_globals;
    NamespaceBinding *namespaceBinding;
    ClassBinding *classBinding;
};

Binder::Binder(NamespaceBinding *globals)
    : _globals(globals),
      namespaceBinding(0),
      classBinding(0)
{ }

Binder::~Binder()
{ }

NamespaceBinding *Binder::bind(Symbol *symbol, NamespaceBinding *binding)
{
    NamespaceBinding *previousBinding = switchNamespaceBinding(binding);
    accept(symbol);
    return switchNamespaceBinding(previousBinding);
}

NamespaceBinding *Binder::findOrCreateNamespaceBinding(Namespace *symbol)
{ return namespaceBinding->findOrCreateNamespaceBinding(symbol); }

NamespaceBinding *Binder::resolveNamespace(const Location &loc, const Name *name)
{
    if (! namespaceBinding)
        return 0;

    return namespaceBinding->resolveNamespace(loc, name);
}

NamespaceBinding *Binder::switchNamespaceBinding(NamespaceBinding *binding)
{
    NamespaceBinding *previousBinding = namespaceBinding;
    namespaceBinding = binding;
    return previousBinding;
}

ClassBinding *Binder::findOrCreateClassBinding(Class *classSymbol)
{
    // ### FINISH ME
    ClassBinding *binding = 0;

    if (classBinding)
        binding = new ClassBinding(classBinding);
    else
        binding = new ClassBinding(namespaceBinding);

    binding->symbols.append(classSymbol);
    return binding;
}

ClassBinding *Binder::findClassBinding(const Name *name)
{
    QSet<Binding *> processed;

    if (classBinding) {
        if (ClassBinding *k = classBinding->findClassBinding(name, &processed))
            return k;

        processed.clear();
    }

    if (namespaceBinding)
        return namespaceBinding->findClassBinding(name, &processed);

    return 0;
}

ClassBinding *Binder::switchClassBinding(ClassBinding *binding)
{
    ClassBinding *previousClassBinding = classBinding;
    classBinding = binding;
    return previousClassBinding;
}

bool Binder::visit(Namespace *symbol)
{
    NamespaceBinding *binding = findOrCreateNamespaceBinding(symbol);

    for (unsigned i = 0; i < symbol->memberCount(); ++i) {
        Symbol *member = symbol->memberAt(i);

        bind(member, binding);
    }

    return false;
}

bool Binder::visit(UsingNamespaceDirective *u)
{
    NamespaceBinding *resolved = resolveNamespace(Location(u), u->name());

    if (! resolved)
        return false;

    namespaceBinding->usings.append(resolved);

    return false;
}

bool Binder::visit(Class *classSymbol)
{
    ClassBinding *binding = findOrCreateClassBinding(classSymbol);
    ClassBinding *previousClassBinding = switchClassBinding(binding);

    for (unsigned i = 0; i < classSymbol->baseClassCount(); ++i) {
        BaseClass *baseClass = classSymbol->baseClassAt(i);
        ClassBinding *baseClassBinding = findClassBinding(baseClass->name());
        binding->baseClassBindings.append(baseClassBinding);
    }

    for (unsigned i = 0; i < classSymbol->memberCount(); ++i)
        accept(classSymbol->memberAt(i));

    (void) switchClassBinding(previousClassBinding);

    return false;
}

bool Binder::visit(Function *)
{ return false; }

bool Binder::visit(Block *)
{ return false; }

} // end of anonymous namespace

static NamespaceBinding *find_helper(Namespace *symbol, NamespaceBinding *binding,
                                     QSet<NamespaceBinding *> *processed)
{
    if (binding && ! processed->contains(binding)) {
        processed->insert(binding);

        if (binding->symbols.contains(symbol))
            return binding;

        foreach (NamespaceBinding *nestedBinding, binding->children) {
            if (NamespaceBinding *ns = find_helper(symbol, nestedBinding, processed))
                return ns;
        }

        if (NamespaceBinding *a = find_helper(symbol, binding->anonymousNamespaceBinding, processed))
            return a;
    }

    return 0;
}

static ClassBinding *find_helper(Class *symbol, Binding *binding,
                                 QSet<Binding *> *processed)
{
    if (binding && ! processed->contains(binding)) {
        processed->insert(binding);

        if (NamespaceBinding *namespaceBinding = binding->asNamespaceBinding()) {
            foreach (ClassBinding *classBinding, namespaceBinding->classBindings) {
                if (ClassBinding *c = find_helper(symbol, classBinding, processed))
                    return c;
            }

            foreach (NamespaceBinding *nestedBinding, namespaceBinding->children) {
                if (ClassBinding *c = find_helper(symbol, nestedBinding, processed))
                    return c;
            }

            if (ClassBinding *a = find_helper(symbol, namespaceBinding->anonymousNamespaceBinding, processed))
                return a;

        } else if (ClassBinding *classBinding = binding->asClassBinding()) {
            foreach (Class *klass, classBinding->symbols) {
                if (klass == symbol)
                    return classBinding;
            }

            foreach (ClassBinding *nestedClassBinding, classBinding->children) {
                if (ClassBinding *c = find_helper(symbol, nestedClassBinding, processed))
                    return c;
            }

#if 0 // ### FIXME
            if (ClassBinding *a = find_helper(symbol, classBinding->anonymousClassBinding, processed))
                return a;
#endif
        }
    }

    return 0;
}

NamespaceBinding *NamespaceBinding::find(Namespace *symbol, NamespaceBinding *binding)
{
    QSet<NamespaceBinding *> processed;
    return find_helper(symbol, binding, &processed);
}

ClassBinding *NamespaceBinding::find(Class *symbol, NamespaceBinding *binding)
{
    QSet<Binding *> processed;
    return find_helper(symbol, binding, &processed);
}

NamespaceBindingPtr CPlusPlus::bind(Document::Ptr doc, Snapshot snapshot)
{
    NamespaceBindingPtr global(new NamespaceBinding());

    Binder bind(global.data());
    bind(doc, snapshot);
    return global;
}

