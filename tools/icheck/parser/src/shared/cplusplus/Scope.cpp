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
// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Scope.h"
#include "Symbols.h"
#include "Names.h"
#include "Literals.h"
#include <cassert>
#include <cstring>

using namespace CPlusPlus;

Scope::Scope(ScopedSymbol *owner)
    : _owner(owner),
      _symbols(0),
      _allocatedSymbols(0),
      _symbolCount(-1),
      _hash(0),
      _hashSize(0)
{ }

Scope::~Scope()
{
    if (_symbols)
        free(_symbols);
    if (_hash)
        free(_hash);
}

ScopedSymbol *Scope::owner() const
{ return _owner; }

void Scope::setOwner(ScopedSymbol *owner)
{ _owner = owner; }

Scope *Scope::enclosingScope() const
{
    if (! _owner)
        return 0;

    return _owner->scope();
}

Scope *Scope::enclosingNamespaceScope() const
{
    Scope *scope = enclosingScope();
    for (; scope; scope = scope->enclosingScope()) {
        if (scope->owner()->isNamespace())
            break;
    }
    return scope;
}

Scope *Scope::enclosingClassScope() const
{
    Scope *scope = enclosingScope();
    for (; scope; scope = scope->enclosingScope()) {
        if (scope->owner()->isClass())
            break;
    }
    return scope;
}

Scope *Scope::enclosingEnumScope() const
{
    Scope *scope = enclosingScope();
    for (; scope; scope = scope->enclosingScope()) {
        if (scope->owner()->isEnum())
            break;
    }
    return scope;
}

Scope *Scope::enclosingFunctionScope() const
{
    Scope *scope = enclosingScope();
    for (; scope; scope = scope->enclosingScope()) {
        if (scope->owner()->isFunction())
            break;
    }
    return scope;
}

Scope *Scope::enclosingBlockScope() const
{
    Scope *scope = enclosingScope();
    for (; scope; scope = scope->enclosingScope()) {
        if (scope->owner()->isBlock())
            break;
    }
    return scope;
}

bool Scope::isNamespaceScope() const
{
    if (_owner)
        return _owner->isNamespace();
    return false;
}

bool Scope::isClassScope() const
{
    if (_owner)
        return _owner->isClass();
    return false;
}

bool Scope::isEnumScope() const
{
    if (_owner)
        return _owner->isEnum();
    return false;
}

bool Scope::isBlockScope() const
{
    if (_owner)
        return _owner->isBlock();
    return false;
}

bool Scope::isPrototypeScope() const
{
    Function *f = 0;
    if (_owner && 0 != (f = _owner->asFunction()))
        return f->arguments() == this;
    return false;
}

bool Scope::isObjCClassScope() const
{
    if (_owner)
        return _owner->isObjCClass();
    return false;
}

bool Scope::isFunctionScope() const
{
    Function *f = 0;
    if (_owner && 0 != (f = _owner->asFunction()))
        return f->arguments() != this;
    return false;
}

bool Scope::isObjCMethodScope() const
{
    ObjCMethod *m = 0;
    if (_owner && 0 != (m = _owner->asObjCMethod()))
        return m->arguments() != this;
    return false;
}

void Scope::enterSymbol(Symbol *symbol)
{
    if (++_symbolCount == _allocatedSymbols) {
        _allocatedSymbols <<= 1;
        if (! _allocatedSymbols)
            _allocatedSymbols = DefaultInitialSize;

        _symbols = reinterpret_cast<Symbol **>(realloc(_symbols, sizeof(Symbol *) * _allocatedSymbols));
    }

    assert(! symbol->_scope || symbol->scope() == this);
    symbol->_index = _symbolCount;
    symbol->_scope = this;
    _symbols[_symbolCount] = symbol;

    if (_symbolCount >= _hashSize * 0.6)
        rehash();
    else {
        const unsigned h = hashValue(symbol);
        symbol->_next = _hash[h];
        _hash[h] = symbol;
    }
}

Symbol *Scope::lookat(const Name *name) const
{
    if (! name)
        return 0;

    else if (const OperatorNameId *opId = name->asOperatorNameId())
        return lookat(opId->kind());

    else if (const Identifier *id = name->identifier())
        return lookat(id);

    else
        return 0;
}

Symbol *Scope::lookat(const Identifier *id) const
{
    if (! _hash || ! id)
        return 0;

    const unsigned h = id->hashCode() % _hashSize;
    Symbol *symbol = _hash[h];
    for (; symbol; symbol = symbol->_next) {
        const Name *identity = symbol->identity();
        if (! identity) {
            continue;
        } else if (const NameId *nameId = identity->asNameId()) {
            if (nameId->identifier()->isEqualTo(id))
                break;
        } else if (const TemplateNameId *t = identity->asTemplateNameId()) {
            if (t->identifier()->isEqualTo(id))
                break;
        } else if (const DestructorNameId *d = identity->asDestructorNameId()) {
            if (d->identifier()->isEqualTo(id))
                break;
        } else if (identity->isQualifiedNameId()) {
            return 0;
        } else if (const SelectorNameId *selectorNameId = identity->asSelectorNameId()) {
            if (selectorNameId->identifier()->isEqualTo(id))
                break;
        }
    }
    return symbol;
}

Symbol *Scope::lookat(int operatorId) const
{
    if (! _hash)
        return 0;

    const unsigned h = operatorId % _hashSize;
    Symbol *symbol = _hash[h];
    for (; symbol; symbol = symbol->_next) {
        const Name *identity = symbol->identity();
        if (const OperatorNameId *op = identity->asOperatorNameId()) {
            if (op->kind() == operatorId)
                break;
        }
    }
    return symbol;
}

void Scope::rehash()
{
    _hashSize <<= 1;

    if (! _hashSize)
        _hashSize = DefaultInitialSize;

    _hash = reinterpret_cast<Symbol **>(realloc(_hash, sizeof(Symbol *) * _hashSize));
    std::memset(_hash, 0, sizeof(Symbol *) * _hashSize);

    for (int index = 0; index < _symbolCount + 1; ++index) {
        Symbol *symbol = _symbols[index];
        const unsigned h = hashValue(symbol);
        symbol->_next = _hash[h];
        _hash[h] = symbol;
    }
}

unsigned Scope::hashValue(Symbol *symbol) const
{
    if (! symbol)
        return 0;

    return symbol->hashCode() % _hashSize;
}

bool Scope::isEmpty() const
{ return _symbolCount == -1; }

unsigned Scope::symbolCount() const
{ return _symbolCount + 1; }

Symbol *Scope::symbolAt(unsigned index) const
{
    if (! _symbols)
        return 0;
    return _symbols[index];
}

Scope::iterator Scope::firstSymbol() const
{ return _symbols; }

Scope::iterator Scope::lastSymbol() const
{ return _symbols + _symbolCount + 1; }


