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

#ifndef CPLUSPLUS_LOOKUPCONTEXT_H
#define CPLUSPLUS_LOOKUPCONTEXT_H

#include "CppDocument.h"
#include <FullySpecifiedType.h>

namespace CPlusPlus {

class CPLUSPLUS_EXPORT LookupItem
{
public:
    LookupItem()
        : _lastVisibleSymbol(0) {}

    LookupItem(const FullySpecifiedType &type, Symbol *lastVisibleSymbol)
        : _type(type), _lastVisibleSymbol(lastVisibleSymbol) {}

    FullySpecifiedType type() const { return _type; }
    void setType(const FullySpecifiedType &type) { _type = type; }

    Symbol *lastVisibleSymbol() const { return _lastVisibleSymbol; }
    void setLastVisibleSymbol(Symbol *symbol) { _lastVisibleSymbol = symbol; }

    bool operator == (const LookupItem &other) const
    {
        if (_type == other._type)
            return _lastVisibleSymbol == other._lastVisibleSymbol;

        return false;
    }

    bool operator != (const LookupItem &result) const
    { return ! operator == (result); }

private:
    FullySpecifiedType _type;
    Symbol *_lastVisibleSymbol;
};

class CPLUSPLUS_EXPORT LookupContext
{
public:
    LookupContext(Control *control = 0);

    LookupContext(Symbol *symbol,
                  Document::Ptr expressionDocument,
                  Document::Ptr thisDocument,
                  const Snapshot &snapshot);

    bool isValid() const;

    Control *control() const;
    Symbol *symbol() const;
    Document::Ptr expressionDocument() const;
    Document::Ptr thisDocument() const;
    Document::Ptr document(const QString &fileName) const;
    Snapshot snapshot() const;

    static Symbol *canonicalSymbol(const QList<Symbol *> &candidates,
                                   NamespaceBinding *globalNamespaceBinding);

    static Symbol *canonicalSymbol(Symbol *symbol,
                                   NamespaceBinding *globalNamespaceBinding);

    static Symbol *canonicalSymbol(const QList<LookupItem> &candidates,
                                   NamespaceBinding *globalNamespaceBinding);

    QList<Symbol *> resolve(const Name *name) const
    { return resolve(name, visibleScopes()); }

    QList<Symbol *> resolveNamespace(const Name *name) const
    { return resolveNamespace(name, visibleScopes()); }

    QList<Symbol *> resolveClass(const Name *name) const
    { return resolveClass(name, visibleScopes()); }

    QList<Symbol *> resolveClassOrNamespace(const Name *name) const
    { return resolveClassOrNamespace(name, visibleScopes()); }

    QList<Symbol *> resolveObjCClass(const Name *name) const
    { return resolveObjCClass(name, visibleScopes()); }

    QList<Symbol *> resolveObjCProtocol(const Name *name) const
    { return resolveObjCProtocol(name, visibleScopes()); }

    enum ResolveMode {
        ResolveSymbol           = 0x01,
        ResolveClass            = 0x02,
        ResolveNamespace        = 0x04,
        ResolveClassOrNamespace = ResolveClass  | ResolveNamespace,
        ResolveObjCClass        = 0x08,
        ResolveObjCProtocol     = 0x10,
        ResolveAll              = ResolveSymbol | ResolveClassOrNamespace | ResolveObjCClass | ResolveObjCProtocol
    };

    QList<Symbol *> resolve(const Name *name, const QList<Scope *> &visibleScopes,
                            ResolveMode mode = ResolveAll) const;

    QList<Symbol *> resolveNamespace(const Name *name, const QList<Scope *> &visibleScopes) const
    { return resolve(name, visibleScopes, ResolveNamespace); }

    QList<Symbol *> resolveClass(const Name *name, const QList<Scope *> &visibleScopes) const
    { return resolve(name, visibleScopes, ResolveClass); }

    QList<Symbol *> resolveClassOrNamespace(const Name *name, const QList<Scope *> &visibleScopes) const
    { return resolve(name, visibleScopes, ResolveClassOrNamespace); }

    QList<Symbol *> resolveObjCClass(const Name *name, const QList<Scope *> &visibleScopes) const
    { return resolve(name, visibleScopes, ResolveObjCClass); }

    QList<Symbol *> resolveObjCProtocol(const Name *name, const QList<Scope *> &visibleScopes) const
    { return resolve(name, visibleScopes, ResolveObjCProtocol); }

    QList<Scope *> visibleScopes() const
    { return _visibleScopes; }

    QList<Scope *> visibleScopes(Symbol *symbol) const;
    QList<Scope *> visibleScopes(const LookupItem &result) const;

    QList<Scope *> expand(const QList<Scope *> &scopes) const;

    void expand(const QList<Scope *> &scopes, QList<Scope *> *expandedScopes) const;

    void expand(Scope *scope, const QList<Scope *> &visibleScopes,
                QList<Scope *> *expandedScopes) const;

    void expandNamespace(Namespace *namespaceSymbol,
                         const QList<Scope *> &visibleScopes,
                         QList<Scope *> *expandedScopes) const;

    void expandClass(Class *classSymbol,
                     const QList<Scope *> &visibleScopes,
                     QList<Scope *> *expandedScopes) const;

    void expandBlock(Block *blockSymbol,
                     const QList<Scope *> &visibleScopes,
                     QList<Scope *> *expandedScopes) const;

    void expandFunction(Function *functionSymbol,
                        const QList<Scope *> &visibleScopes,
                        QList<Scope *> *expandedScopes) const;

    void expandObjCMethod(ObjCMethod *method,
                          const QList<Scope *> &visibleScopes,
                          QList<Scope *> *expandedScopes) const;

    void expandObjCClass(ObjCClass *klass,
                         const QList<Scope *> &visibleScopes,
                         QList<Scope *> *expandedScopes) const;

    void expandObjCProtocol(ObjCProtocol *protocol,
                            const QList<Scope *> &visibleScopes,
                            QList<Scope *> *expandedScopes) const;

    void expandEnumOrAnonymousSymbol(ScopedSymbol *scopedSymbol,
                                     QList<Scope *> *expandedScopes) const;

private:
    static Symbol *canonicalSymbol(Symbol *symbol);

    QList<Symbol *> resolveQualifiedNameId(const QualifiedNameId *q,
                                           const QList<Scope *> &visibleScopes,
                                           ResolveMode mode) const;

    QList<Symbol *> resolveOperatorNameId(const OperatorNameId *opId,
                                          const QList<Scope *> &visibleScopes,
                                          ResolveMode mode) const;

    QList<Scope *> resolveNestedNameSpecifier(const QualifiedNameId *q,
                                               const QList<Scope *> &visibleScopes) const;

    const Identifier *identifier(const Name *name) const;

    QList<Scope *> buildVisibleScopes();

    void buildVisibleScopes_helper(Document::Ptr doc, QList<Scope *> *scopes,
                                   QSet<QString> *processed);

    static bool maybeValidSymbol(Symbol *symbol,
                                 ResolveMode mode,
                                 const QList<Symbol *> &candidates);

private:
    Control *_control;

    // The current symbol.
    Symbol *_symbol;

    // The current expression.
    Document::Ptr _expressionDocument;

    // The current document.
    Document::Ptr _thisDocument;

    // All documents.
    Snapshot _snapshot;

    // Visible scopes.
    QList<Scope *> _visibleScopes;
};

uint qHash(const CPlusPlus::LookupItem &result);

} // end of namespace CPlusPlus

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300
//this ensures that code outside QmlJS can use the hash function
//it also a workaround for some compilers
inline uint qHash(const CPlusPlus::LookupItem &item) { return CPlusPlus::qHash(item); }
#endif

#endif // CPLUSPLUS_LOOKUPCONTEXT_H
