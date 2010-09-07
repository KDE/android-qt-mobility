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

#ifndef CPLUSPLUS_SYMBOL_H
#define CPLUSPLUS_SYMBOL_H

#include "CPlusPlusForwardDeclarations.h"


namespace CPlusPlus {

class CPLUSPLUS_EXPORT Symbol
{
    Symbol(const Symbol &other);
    void operator =(const Symbol &other);

public:
    /// Storage class specifier
    enum Storage {
        NoStorage = 0,
        Friend,
        Register,
        Static,
        Extern,
        Mutable,
        Typedef
    };

    /// Access specifier.
    enum Visibility {
        Public,
        Protected,
        Private,
        Package
    };

public:
    /// Constructs a Symbol with the given source location, name and translation unit.
    Symbol(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);

    /// Destroy this Symbol.
    virtual ~Symbol();

    /// Returns this Symbol's Control object.
    Control *control() const;

    /// Returns this Symbol's source location.
    unsigned sourceLocation() const;

    /// Returns this Symbol's source offset.
    unsigned sourceOffset() const;

    /// Returns this Symbol's line number.
    unsigned line() const;

    /// Returns this Symbol's column number.
    unsigned column() const;

    /// Returns this Symbol's file name.
    const StringLiteral *fileId() const;

    /// Returns this Symbol's file name.
    const char *fileName() const;

    /// Returns this Symbol's file name length.
    unsigned fileNameLength() const;

    unsigned startOffset() const;
    void setStartOffset(unsigned offset);

    unsigned endOffset() const;
    void setEndOffset(unsigned offset);

    void getPosition(unsigned *line, unsigned *column = 0, const StringLiteral **fileId = 0) const;
    void getStartPosition(unsigned *line, unsigned *column = 0, const StringLiteral **fileId = 0) const;
    void getEndPosition(unsigned *line, unsigned *column = 0, const StringLiteral **fileId = 0) const;

    /// Returns this Symbol's name.
    const Name *name() const;

    /// Sets this Symbol's name.
    void setName(const Name *name); // ### dangerous

    /// Returns this Symbol's (optional) identifier
    const Identifier *identifier() const;

    /// Returns this Symbol's storage class specifier.
    int storage() const;

    /// Sets this Symbol's storage class specifier.
    void setStorage(int storage);

    /// Returns this Symbol's visibility.
    int visibility() const;

    /// Sets this Symbol's visibility.
    void setVisibility(int visibility);

    /// Returns this Symbol's scope.
    Scope *scope() const;

    /// Returns the next chained Symbol.
    Symbol *next() const;

    /// Returns true if this Symbol has friend storage specifier.
    bool isFriend() const;

    /// Returns true if this Symbol has register storage specifier.
    bool isRegister() const;

    /// Returns true if this Symbol has static storage specifier.
    bool isStatic() const;

    /// Returns true if this Symbol has extern storage specifier.
    bool isExtern() const;

    /// Returns true if this Symbol has mutable storage specifier.
    bool isMutable() const;

    /// Returns true if this Symbol has typedef storage specifier.
    bool isTypedef() const;

    /// Returns true if this Symbol's visibility is public.
    bool isPublic() const;

    /// Returns true if this Symbol's visibility is protected.
    bool isProtected() const;

    /// Returns true if this Symbol's visibility is private.
    bool isPrivate() const;

    /// Returns true if this Symbol is a ScopedSymbol.
    bool isScopedSymbol() const;

    /// Returns true if this Symbol is an Enum.
    bool isEnum() const;

    /// Returns true if this Symbol is an Function.
    bool isFunction() const;

    /// Returns true if this Symbol is a Namespace.
    bool isNamespace() const;

    /// Returns true if this Symbol is a Class.
    bool isClass() const;

    /// Returns true if this Symbol is a Block.
    bool isBlock() const;

    /// Returns true if this Symbol is a UsingNamespaceDirective.
    bool isUsingNamespaceDirective() const;

    /// Returns true if this Symbol is a UsingDeclaration.
    bool isUsingDeclaration() const;

    /// Returns true if this Symbol is a Declaration.
    bool isDeclaration() const;

    /// Returns true if this Symbol is an Argument.
    bool isArgument() const;

    /// Returns true if this Symbol is a Typename argument.
    bool isTypenameArgument() const;

    /// Returns true if this Symbol is a BaseClass.
    bool isBaseClass() const;

    /// Returns true if this Symbol is a ForwardClassDeclaration.
    bool isForwardClassDeclaration() const;

    bool isObjCBaseClass() const;
    bool isObjCBaseProtocol() const;

    /// Returns true if this Symbol is an Objective-C Class declaration.
    bool isObjCClass() const;

    /// Returns true if this Symbol is an Objective-C Class forward declaration.
    bool isObjCForwardClassDeclaration() const;

    /// Returns true if this Symbol is an Objective-C Protocol declaration.
    bool isObjCProtocol() const;

    /// Returns true if this Symbol is an Objective-C Protocol forward declaration.
    bool isObjCForwardProtocolDeclaration() const;

    /// Returns true if this Symbol is an Objective-C method declaration.
    bool isObjCMethod() const;

    /// Returns true if this Symbol is an Objective-C @property declaration.
    bool isObjCPropertyDeclaration() const;

    virtual const ScopedSymbol *asScopedSymbol() const { return 0; }
    virtual const Enum *asEnum() const { return 0; }
    virtual const Function *asFunction() const { return 0; }
    virtual const Namespace *asNamespace() const { return 0; }
    virtual const Class *asClass() const { return 0; }
    virtual const Block *asBlock() const { return 0; }
    virtual const UsingNamespaceDirective *asUsingNamespaceDirective() const { return 0; }
    virtual const UsingDeclaration *asUsingDeclaration() const { return 0; }
    virtual const Declaration *asDeclaration() const { return 0; }
    virtual const Argument *asArgument() const { return 0; }
    virtual const TypenameArgument *asTypenameArgument() const { return 0; }
    virtual const BaseClass *asBaseClass() const { return 0; }
    virtual const ForwardClassDeclaration *asForwardClassDeclaration() const { return 0; }
    virtual const ObjCBaseClass *asObjCBaseClass() const { return 0; }
    virtual const ObjCBaseProtocol *asObjCBaseProtocol() const { return 0; }
    virtual const ObjCClass *asObjCClass() const { return 0; }
    virtual const ObjCForwardClassDeclaration *asObjCForwardClassDeclaration() const { return 0; }
    virtual const ObjCProtocol *asObjCProtocol() const { return 0; }
    virtual const ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclaration() const { return 0; }
    virtual const ObjCMethod *asObjCMethod() const { return 0; }
    virtual const ObjCPropertyDeclaration *asObjCPropertyDeclaration() const { return 0; }

    virtual ScopedSymbol *asScopedSymbol() { return 0; }
    virtual Enum *asEnum() { return 0; }
    virtual Function *asFunction() { return 0; }
    virtual Namespace *asNamespace() { return 0; }
    virtual Class *asClass() { return 0; }
    virtual Block *asBlock() { return 0; }
    virtual UsingNamespaceDirective *asUsingNamespaceDirective() { return 0; }
    virtual UsingDeclaration *asUsingDeclaration() { return 0; }
    virtual Declaration *asDeclaration() { return 0; }
    virtual Argument *asArgument() { return 0; }
    virtual TypenameArgument *asTypenameArgument() { return 0; }
    virtual BaseClass *asBaseClass() { return 0; }
    virtual ForwardClassDeclaration *asForwardClassDeclaration() { return 0; }
    virtual ObjCBaseClass *asObjCBaseClass() { return 0; }
    virtual ObjCBaseProtocol *asObjCBaseProtocol() { return 0; }
    virtual ObjCClass *asObjCClass() { return 0; }
    virtual ObjCForwardClassDeclaration *asObjCForwardClassDeclaration() { return 0; }
    virtual ObjCProtocol *asObjCProtocol() { return 0; }
    virtual ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclaration() { return 0; }
    virtual ObjCMethod *asObjCMethod() { return 0; }
    virtual ObjCPropertyDeclaration *asObjCPropertyDeclaration() { return 0; }

    /// Returns this Symbol's type.
    virtual FullySpecifiedType type() const = 0;

    /// Returns this Symbol's hash value.
    unsigned hashCode() const;

    /// Returns this Symbol's index.
    unsigned index() const;

    const Name *identity() const;

    bool isGenerated() const;

    Symbol *enclosingSymbol() const;

    /// Returns the eclosing namespace scope.
    Scope *enclosingNamespaceScope() const;

    /// Returns the enclosing class scope.
    Scope *enclosingClassScope() const;

    /// Returns the enclosing enum scope.
    Scope *enclosingEnumScope() const;

    /// Returns the enclosing function scope.
    Scope *enclosingFunctionScope() const;

    /// Returns the enclosing Block scope.
    Scope *enclosingBlockScope() const;

    void setScope(Scope *scope); // ### make me private
    void setSourceLocation(unsigned sourceLocation); // ### make me private

    void visitSymbol(SymbolVisitor *visitor);
    static void visitSymbol(Symbol *symbol, SymbolVisitor *visitor);

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor) = 0;

    TranslationUnit *translationUnit() const;

private:
    Control *_control;
    unsigned _sourceLocation;
    unsigned _sourceOffset;
    unsigned _startOffset;
    unsigned _endOffset;
    const Name *_name;
    unsigned _hashCode;
    int _storage;
    int _visibility;
    Scope *_scope;
    unsigned _index;
    Symbol *_next;

    bool _isGenerated: 1;

    class IdentityForName;
    class HashCode;

    friend class Scope;
};

} // end of namespace CPlusPlus


#endif // CPLUSPLUS_SYMBOL_H
