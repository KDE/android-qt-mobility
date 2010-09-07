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

#include "CheckDeclaration.h"
#include "Semantic.h"
#include "AST.h"
#include "TranslationUnit.h"
#include "Scope.h"
#include "Names.h"
#include "CoreTypes.h"
#include "Symbols.h"
#include "Control.h"
#include "Literals.h"
#include <string>
#include <cassert>

using namespace CPlusPlus;

CheckDeclaration::CheckDeclaration(Semantic *semantic)
    : SemanticCheck(semantic),
      _declaration(0),
      _scope(0),
      _templateParameters(0),
      _checkAnonymousArguments(false)
{ }

CheckDeclaration::~CheckDeclaration()
{ }

void CheckDeclaration::check(DeclarationAST *declaration,
                             Scope *scope, TemplateParameters *templateParameters)
{
    Scope *previousScope = switchScope(scope);
    TemplateParameters *previousTemplateParameters = switchTemplateParameters(templateParameters);
    DeclarationAST *previousDeclaration = switchDeclaration(declaration);
    accept(declaration);
    (void) switchDeclaration(previousDeclaration);
    (void) switchTemplateParameters(previousTemplateParameters);
    (void) switchScope(previousScope);
}

DeclarationAST *CheckDeclaration::switchDeclaration(DeclarationAST *declaration)
{
    DeclarationAST *previousDeclaration = _declaration;
    _declaration = declaration;
    return previousDeclaration;
}

Scope *CheckDeclaration::switchScope(Scope *scope)
{
    Scope *previousScope = _scope;
    _scope = scope;
    return previousScope;
}

TemplateParameters *CheckDeclaration::switchTemplateParameters(TemplateParameters *templateParameters)
{
    TemplateParameters *previousTemplateParameters = _templateParameters;
    _templateParameters = templateParameters;
    return previousTemplateParameters;
}

void CheckDeclaration::checkFunctionArguments(Function *fun)
{
    if (! _checkAnonymousArguments)
        return;

    if (_scope->isClassScope() && fun->isPublic()) {
        for (unsigned argc = 0; argc < fun->argumentCount(); ++argc) {
            Argument *arg = fun->argumentAt(argc)->asArgument();
            assert(arg != 0);

            if (! arg->name()) {
                translationUnit()->warning(arg->sourceLocation(),
                                           "anonymous argument");
            }
        }
    }
}

unsigned CheckDeclaration::locationOfDeclaratorId(DeclaratorAST *declarator) const
{
    if (declarator && declarator->core_declarator) {
        if (DeclaratorIdAST *declaratorId = declarator->core_declarator->asDeclaratorId())
            return declaratorId->firstToken();
        else if (NestedDeclaratorAST *nested = declarator->core_declarator->asNestedDeclarator())
            return locationOfDeclaratorId(nested->declarator);
    }

    return 0;
}

bool CheckDeclaration::visit(SimpleDeclarationAST *ast)
{
    FullySpecifiedType ty = semantic()->check(ast->decl_specifier_list, _scope);
    FullySpecifiedType qualTy = ty.qualifiedType();

    if (_templateParameters && ty) {
        if (Class *klass = ty->asClassType()) {
            klass->setTemplateParameters(_templateParameters);
        }
    }

    if (! ast->declarator_list && ast->decl_specifier_list && ! ast->decl_specifier_list->next) {
        if (ElaboratedTypeSpecifierAST *elab_type_spec = ast->decl_specifier_list->value->asElaboratedTypeSpecifier()) {

            unsigned sourceLocation = elab_type_spec->firstToken();

            if (elab_type_spec->name)
                sourceLocation = elab_type_spec->name->firstToken();

            const Name *name = semantic()->check(elab_type_spec->name, _scope);
            ForwardClassDeclaration *symbol =
                    control()->newForwardClassDeclaration(sourceLocation, name);

            if (_templateParameters) {
                symbol->setTemplateParameters(_templateParameters);
                _templateParameters = 0;
            }

            _scope->enterSymbol(symbol);
            return false;
        }
    }

    const bool isQ_SLOT   = ast->qt_invokable_token && tokenKind(ast->qt_invokable_token) == T_Q_SLOT;
    const bool isQ_SIGNAL = ast->qt_invokable_token && tokenKind(ast->qt_invokable_token) == T_Q_SIGNAL;
#ifdef ICHECK_BUILD
    const bool isQ_INVOKABLE = (ast->invoke_token > 0);
#endif

    List<Declaration *> **decl_it = &ast->symbols;
    for (DeclaratorListAST *it = ast->declarator_list; it; it = it->next) {
        const Name *name = 0;
        FullySpecifiedType declTy = semantic()->check(it->value, qualTy,
                                                      _scope, &name);

        unsigned location = locationOfDeclaratorId(it->value);
        if (! location) {
            if (it->value)
                location = it->value->firstToken();
            else
                location = ast->firstToken();
        }

        Function *fun = 0;
        if (declTy && 0 != (fun = declTy->asFunctionType())) {
            fun->setSourceLocation(location);
            fun->setScope(_scope);
            fun->setName(name);
            fun->setMethodKey(semantic()->currentMethodKey());
            fun->setVirtual(ty.isVirtual());
            if (isQ_SIGNAL)
                fun->setMethodKey(Function::SignalMethod);
            else if (isQ_SLOT)
                fun->setMethodKey(Function::SlotMethod);
#ifdef ICHECK_BUILD
            else if (isQ_INVOKABLE)
                fun->setInvokable(true);
#endif
            fun->setVisibility(semantic()->currentVisibility());
        } else if (semantic()->currentMethodKey() != Function::NormalMethod) {
            translationUnit()->warning(ast->firstToken(),
                                       "expected a function declaration");
        }

        Declaration *symbol = control()->newDeclaration(location, name);
        symbol->setStartOffset(tokenAt(ast->firstToken()).offset);
        symbol->setEndOffset(tokenAt(ast->lastToken()).offset);

        symbol->setType(control()->integerType(IntegerType::Int));
        symbol->setType(declTy);

        if (_templateParameters && it == ast->declarator_list && ty && ! ty->isClassType())
            symbol->setTemplateParameters(_templateParameters);

        symbol->setVisibility(semantic()->currentVisibility());

        if (ty.isFriend())
            symbol->setStorage(Symbol::Friend);
        else if (ty.isRegister())
            symbol->setStorage(Symbol::Register);
        else if (ty.isStatic())
            symbol->setStorage(Symbol::Static);
        else if (ty.isExtern())
            symbol->setStorage(Symbol::Extern);
        else if (ty.isMutable())
            symbol->setStorage(Symbol::Mutable);
        else if (ty.isTypedef())
            symbol->setStorage(Symbol::Typedef);

        if (it->value && it->value->initializer) {
            FullySpecifiedType initTy = semantic()->check(it->value->initializer, _scope);
        }

        *decl_it = new (translationUnit()->memoryPool()) List<Declaration *>();
        (*decl_it)->value = symbol;
        decl_it = &(*decl_it)->next;

        _scope->enterSymbol(symbol);
    }
    return false;
}

bool CheckDeclaration::visit(EmptyDeclarationAST *)
{
    return false;
}

bool CheckDeclaration::visit(AccessDeclarationAST *ast)
{
    int accessSpecifier = tokenKind(ast->access_specifier_token);
    int visibility = semantic()->visibilityForAccessSpecifier(accessSpecifier);
    semantic()->switchVisibility(visibility);
    if (ast->slots_token)
        semantic()->switchMethodKey(Function::SlotMethod);
    else if (accessSpecifier == T_Q_SIGNALS)
        semantic()->switchMethodKey(Function::SignalMethod);
    else
        semantic()->switchMethodKey(Function::NormalMethod);
    return false;
}

#ifdef ICHECK_BUILD
bool CheckDeclaration::visit(QPropertyDeclarationAST *)
{
    return false;
}

bool CheckDeclaration::visit(QEnumDeclarationAST *)
{
    return false;
}

bool CheckDeclaration::visit(QFlagsDeclarationAST *)
{
    return false;
}

bool CheckDeclaration::visit(QDeclareFlagsDeclarationAST *)
{
    return false;
}
#endif

bool CheckDeclaration::visit(AsmDefinitionAST *)
{
    return false;
}

bool CheckDeclaration::visit(ExceptionDeclarationAST *ast)
{
    FullySpecifiedType ty = semantic()->check(ast->type_specifier_list, _scope);
    FullySpecifiedType qualTy = ty.qualifiedType();

    const Name *name = 0;
    FullySpecifiedType declTy = semantic()->check(ast->declarator, qualTy,
                                                  _scope, &name);

    unsigned location = locationOfDeclaratorId(ast->declarator);
    if (! location) {
        if (ast->declarator)
            location = ast->declarator->firstToken();
        else
            location = ast->firstToken();
    }

    Declaration *symbol = control()->newDeclaration(location, name);
    symbol->setStartOffset(tokenAt(ast->firstToken()).offset);
    symbol->setEndOffset(tokenAt(ast->lastToken()).offset);
    symbol->setType(declTy);
    _scope->enterSymbol(symbol);

    return false;
}

bool CheckDeclaration::visit(FunctionDefinitionAST *ast)
{
    FullySpecifiedType ty = semantic()->check(ast->decl_specifier_list, _scope);
    FullySpecifiedType qualTy = ty.qualifiedType();
    const Name *name = 0;
    FullySpecifiedType funTy = semantic()->check(ast->declarator, qualTy,
                                                 _scope, &name);
    if (! (funTy && funTy->isFunctionType())) {
        translationUnit()->error(ast->firstToken(),
                                 "expected a function prototype");
        return false;
    }

    Function *fun = funTy->asFunctionType();
    fun->setVirtual(ty.isVirtual());
    fun->setStartOffset(tokenAt(ast->firstToken()).offset);
    fun->setEndOffset(tokenAt(ast->lastToken()).offset);
    if (ast->declarator)
        fun->setSourceLocation(ast->declarator->firstToken());
    fun->setName(name);
    fun->setTemplateParameters(_templateParameters);
    fun->setVisibility(semantic()->currentVisibility());
    fun->setMethodKey(semantic()->currentMethodKey());

    const bool isQ_SLOT   = ast->qt_invokable_token && tokenKind(ast->qt_invokable_token) == T_Q_SLOT;
    const bool isQ_SIGNAL = ast->qt_invokable_token && tokenKind(ast->qt_invokable_token) == T_Q_SIGNAL;
#ifdef ICHECK_BUILD
    const bool isQ_INVOKABLE = (ast->invoke_token > 0);
#endif

    if (isQ_SIGNAL)
        fun->setMethodKey(Function::SignalMethod);
    else if (isQ_SLOT)
        fun->setMethodKey(Function::SlotMethod);
#ifdef ICHECK_BUILD
    else if (isQ_INVOKABLE)
        fun->setInvokable(true);
#endif

    checkFunctionArguments(fun);

    ast->symbol = fun;
    _scope->enterSymbol(fun);

    if (! semantic()->skipFunctionBodies()) {
        if (ast->ctor_initializer) {
            bool looksLikeCtor = false;
            if (ty.isValid() || ! fun->identity())
                looksLikeCtor = false;
            else if (fun->identity()->isNameId() || fun->identity()->isTemplateNameId())
                looksLikeCtor = true;

            if (! looksLikeCtor) {
                translationUnit()->error(ast->ctor_initializer->firstToken(),
                                         "only constructors take base initializers");
            }
            accept(ast->ctor_initializer);
        }

        const int previousVisibility = semantic()->switchVisibility(Symbol::Public);
        const int previousMethodKey = semantic()->switchMethodKey(Function::NormalMethod);

        semantic()->check(ast->function_body, fun->members());

        semantic()->switchMethodKey(previousMethodKey);
        semantic()->switchVisibility(previousVisibility);
    }

    return false;
}

bool CheckDeclaration::visit(MemInitializerAST *ast)
{
    (void) semantic()->check(ast->name, _scope);
    for (ExpressionListAST *it = ast->expression_list; it; it = it->next) {
        FullySpecifiedType ty = semantic()->check(it->value, _scope);
    }
    return false;
}

bool CheckDeclaration::visit(LinkageBodyAST *ast)
{
    for (DeclarationListAST *decl = ast->declaration_list; decl; decl = decl->next) {
       semantic()->check(decl->value, _scope);
    }
    return false;
}

bool CheckDeclaration::visit(LinkageSpecificationAST *ast)
{
    semantic()->check(ast->declaration, _scope);
    return false;
}

bool CheckDeclaration::visit(NamespaceAST *ast)
{
    const Name *namespaceName = 0;
    if (const Identifier *id = identifier(ast->identifier_token))
        namespaceName = control()->nameId(id);

    unsigned sourceLocation = ast->firstToken();

    if (ast->identifier_token)
        sourceLocation = ast->identifier_token;

    Namespace *ns = control()->newNamespace(sourceLocation, namespaceName);
    ns->setStartOffset(tokenAt(ast->firstToken()).offset);
    ns->setEndOffset(tokenAt(ast->lastToken()).offset);
    ast->symbol = ns;
    _scope->enterSymbol(ns);
    semantic()->check(ast->linkage_body, ns->members()); // ### we'll do the merge later.

    return false;
}

bool CheckDeclaration::visit(NamespaceAliasDefinitionAST *)
{
    return false;
}

bool CheckDeclaration::visit(ParameterDeclarationAST *ast)
{
    unsigned sourceLocation = locationOfDeclaratorId(ast->declarator);
    if (! sourceLocation) {
        if (ast->declarator)
            sourceLocation = ast->declarator->firstToken();
        else
            sourceLocation = ast->firstToken();
    }

    const Name *argName = 0;
    FullySpecifiedType ty = semantic()->check(ast->type_specifier_list, _scope);
    FullySpecifiedType argTy = semantic()->check(ast->declarator, ty.qualifiedType(),
                                                 _scope, &argName);
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    Argument *arg = control()->newArgument(sourceLocation, argName);
    ast->symbol = arg;
    if (ast->expression) {
        unsigned startOfExpression = ast->expression->firstToken();
        unsigned endOfExpression = ast->expression->lastToken();
        std::string buffer;
        for (unsigned index = startOfExpression; index != endOfExpression; ++index) {
            const Token &tk = tokenAt(index);
            if (tk.whitespace() || tk.newline())
                buffer += ' ';
            buffer += tk.spell();
        }
        const StringLiteral *initializer = control()->findOrInsertStringLiteral(buffer.c_str(), buffer.size());
        arg->setInitializer(initializer);
    }
    arg->setType(argTy);
    _scope->enterSymbol(arg);
    return false;
}

bool CheckDeclaration::visit(TemplateDeclarationAST *ast)
{
    Scope *scope = new Scope(_scope->owner());

    for (DeclarationListAST *param = ast->template_parameter_list; param; param = param->next) {
       semantic()->check(param->value, scope);
    }

    semantic()->check(ast->declaration, _scope,
                      new TemplateParameters(_templateParameters, scope));

    return false;
}

bool CheckDeclaration::visit(TypenameTypeParameterAST *ast)
{
    unsigned sourceLocation = ast->firstToken();
    if (ast->name)
        sourceLocation = ast->name->firstToken();

    const Name *name = semantic()->check(ast->name, _scope);
    TypenameArgument *arg = control()->newTypenameArgument(sourceLocation, name);
    FullySpecifiedType ty = semantic()->check(ast->type_id, _scope);
    arg->setType(ty);
    ast->symbol = arg;
    _scope->enterSymbol(arg);
    return false;
}

bool CheckDeclaration::visit(TemplateTypeParameterAST *ast)
{
    unsigned sourceLocation = ast->firstToken();
    if (ast->name)
        sourceLocation = ast->name->firstToken();

    const Name *name = semantic()->check(ast->name, _scope);
    TypenameArgument *arg = control()->newTypenameArgument(sourceLocation, name);
    FullySpecifiedType ty = semantic()->check(ast->type_id, _scope);
    arg->setType(ty);
    ast->symbol = arg;
    _scope->enterSymbol(arg);
    return false;
}

bool CheckDeclaration::visit(UsingAST *ast)
{
    const Name *name = semantic()->check(ast->name, _scope);

    unsigned sourceLocation = ast->firstToken();
    if (ast->name)
        sourceLocation = ast->name->firstToken();

    UsingDeclaration *u = control()->newUsingDeclaration(sourceLocation, name);
    ast->symbol = u;
    _scope->enterSymbol(u);
    return false;
}

bool CheckDeclaration::visit(UsingDirectiveAST *ast)
{
    const Name *name = semantic()->check(ast->name, _scope);

    unsigned sourceLocation = ast->firstToken();
    if (ast->name)
        sourceLocation = ast->name->firstToken();

    UsingNamespaceDirective *u = control()->newUsingNamespaceDirective(sourceLocation, name);
    ast->symbol = u;
    _scope->enterSymbol(u);

    if (! (_scope->isBlockScope() || _scope->isNamespaceScope()))
        translationUnit()->error(ast->firstToken(),
                                 "using-directive not within namespace or block scope");

    return false;
}

bool CheckDeclaration::visit(ObjCProtocolForwardDeclarationAST *ast)
{
    const unsigned sourceLocation = ast->firstToken();

    List<ObjCForwardProtocolDeclaration *> **symbolIter = &ast->symbols;
    for (ObjCIdentifierListAST *it = ast->identifier_list; it; it = it->next) {
        unsigned declarationLocation;
        if (it->value)
            declarationLocation = it->value->firstToken();
        else
            declarationLocation = sourceLocation;

        const Name *protocolName = semantic()->check(it->value, _scope);
        ObjCForwardProtocolDeclaration *fwdProtocol = control()->newObjCForwardProtocolDeclaration(sourceLocation, protocolName);
        fwdProtocol->setStartOffset(tokenAt(ast->firstToken()).offset);
        fwdProtocol->setEndOffset(tokenAt(ast->lastToken()).offset);

        _scope->enterSymbol(fwdProtocol);

        *symbolIter = new (translationUnit()->memoryPool()) List<ObjCForwardProtocolDeclaration *>();
        (*symbolIter)->value = fwdProtocol;
        symbolIter = &(*symbolIter)->next;
    }

    return false;
}

bool CheckDeclaration::visit(ObjCProtocolDeclarationAST *ast)
{
    unsigned sourceLocation;
    if (ast->name)
        sourceLocation = ast->name->firstToken();
    else
        sourceLocation = ast->firstToken();

    const Name *protocolName = semantic()->check(ast->name, _scope);
    ObjCProtocol *protocol = control()->newObjCProtocol(sourceLocation, protocolName);
    protocol->setStartOffset(tokenAt(ast->firstToken()).offset);
    protocol->setEndOffset(tokenAt(ast->lastToken()).offset);

    if (ast->protocol_refs && ast->protocol_refs->identifier_list) {
        for (ObjCIdentifierListAST *iter = ast->protocol_refs->identifier_list; iter; iter = iter->next) {
            NameAST* name = iter->value;
            const Name *protocolName = semantic()->check(name, _scope);
            ObjCBaseProtocol *baseProtocol = control()->newObjCBaseProtocol(name->firstToken(), protocolName);
            protocol->addProtocol(baseProtocol);
        }
    }

    int previousObjCVisibility = semantic()->switchObjCVisibility(Function::Public);
    for (DeclarationListAST *it = ast->member_declaration_list; it; it = it->next) {
        semantic()->check(it->value, protocol->members());
    }
    (void) semantic()->switchObjCVisibility(previousObjCVisibility);

    ast->symbol = protocol;
    _scope->enterSymbol(protocol);

    return false;
}

bool CheckDeclaration::visit(ObjCClassForwardDeclarationAST *ast)
{
    const unsigned sourceLocation = ast->firstToken();

    List<ObjCForwardClassDeclaration *> **symbolIter = &ast->symbols;
    for (ObjCIdentifierListAST *it = ast->identifier_list; it; it = it->next) {
        unsigned declarationLocation;
        if (it->value)
            declarationLocation = it->value->firstToken();
        else
            declarationLocation = sourceLocation;

        const Name *className = semantic()->check(it->value, _scope);
        ObjCForwardClassDeclaration *fwdClass = control()->newObjCForwardClassDeclaration(sourceLocation, className);
        fwdClass->setStartOffset(tokenAt(ast->firstToken()).offset);
        fwdClass->setEndOffset(tokenAt(ast->lastToken()).offset);

        _scope->enterSymbol(fwdClass);

        *symbolIter = new (translationUnit()->memoryPool()) List<ObjCForwardClassDeclaration *>();
        (*symbolIter)->value = fwdClass;
        symbolIter = &(*symbolIter)->next;
    }

    return false;
}

bool CheckDeclaration::visit(ObjCClassDeclarationAST *ast)
{
    unsigned sourceLocation;
    if (ast->class_name)
        sourceLocation = ast->class_name->firstToken();
    else
        sourceLocation = ast->firstToken();

    const Name *className = semantic()->check(ast->class_name, _scope);
    ObjCClass *klass = control()->newObjCClass(sourceLocation, className);
    klass->setStartOffset(tokenAt(ast->firstToken()).offset);
    klass->setEndOffset(tokenAt(ast->lastToken()).offset);
    ast->symbol = klass;

    klass->setInterface(ast->interface_token != 0);

    if (ast->category_name) {
        const Name *categoryName = semantic()->check(ast->category_name, _scope);
        klass->setCategoryName(categoryName);
    }

    if (ast->superclass) {
        const Name *superClassName = semantic()->check(ast->superclass, _scope);
        ObjCBaseClass *superKlass = control()->newObjCBaseClass(ast->superclass->firstToken(), superClassName);
        klass->setBaseClass(superKlass);
    }

    if (ast->protocol_refs && ast->protocol_refs->identifier_list) {
        for (ObjCIdentifierListAST *iter = ast->protocol_refs->identifier_list; iter; iter = iter->next) {
            NameAST* name = iter->value;
            const Name *protocolName = semantic()->check(name, _scope);
            ObjCBaseProtocol *baseProtocol = control()->newObjCBaseProtocol(name->firstToken(), protocolName);
            klass->addProtocol(baseProtocol);
        }
    }

    _scope->enterSymbol(klass);

    int previousObjCVisibility = semantic()->switchObjCVisibility(Function::Protected);

    if (ast->inst_vars_decl) {
        for (DeclarationListAST *it = ast->inst_vars_decl->instance_variable_list; it; it = it->next) {
            semantic()->check(it->value, klass->members());
        }
    }

    (void) semantic()->switchObjCVisibility(Function::Public);

    for (DeclarationListAST *it = ast->member_declaration_list; it; it = it->next) {
        semantic()->check(it->value, klass->members());
    }

    (void) semantic()->switchObjCVisibility(previousObjCVisibility);

    return false;
}

bool CheckDeclaration::visit(ObjCMethodDeclarationAST *ast)
{
    if (!ast->method_prototype)
        return false;

    FullySpecifiedType ty = semantic()->check(ast->method_prototype, _scope);
    ObjCMethod *methodTy = ty.type()->asObjCMethodType();
    if (!methodTy)
        return false;

    Symbol *symbol;
    if (ast->function_body) {
        if (!semantic()->skipFunctionBodies()) {
            semantic()->check(ast->function_body, methodTy->members());
        }

        symbol = methodTy;
    } else {
        Declaration *decl = control()->newDeclaration(ast->firstToken(), methodTy->name());
        decl->setType(methodTy);
        symbol = decl;
        symbol->setStorage(methodTy->storage());
    }

    symbol->setStartOffset(tokenAt(ast->firstToken()).offset);
    symbol->setEndOffset(tokenAt(ast->lastToken()).offset);
    symbol->setVisibility(semantic()->currentVisibility());

    _scope->enterSymbol(symbol);

    return false;
}

bool CheckDeclaration::visit(ObjCVisibilityDeclarationAST *ast)
{
    int accessSpecifier = tokenKind(ast->visibility_token);
    int visibility = semantic()->visibilityForObjCAccessSpecifier(accessSpecifier);
    semantic()->switchObjCVisibility(visibility);
    return false;
}

bool CheckDeclaration::checkPropertyAttribute(ObjCPropertyAttributeAST *attrAst,
                                              int &flags,
                                              int attr)
{
    if (flags & attr) {
        translationUnit()->warning(attrAst->attribute_identifier_token,
                                   "duplicate property attribute \"%s\"",
                                   spell(attrAst->attribute_identifier_token));
        return false;
    } else {
        flags |= attr;
        return true;
    }
}

bool CheckDeclaration::visit(ObjCPropertyDeclarationAST *ast)
{
    semantic()->check(ast->simple_declaration, _scope);
    SimpleDeclarationAST *simpleDecl = ast->simple_declaration->asSimpleDeclaration();

    if (!simpleDecl) {
        translationUnit()->warning(ast->simple_declaration->firstToken(),
                                   "invalid type for property declaration");
        return false;
    }

    int propAttrs = ObjCPropertyDeclaration::None;
    const Name *getterName = 0, *setterName = 0;

    for (ObjCPropertyAttributeListAST *iter= ast->property_attribute_list; iter; iter = iter->next) {
        ObjCPropertyAttributeAST *attrAst = iter->value;
        if (!attrAst)
            continue;

        const Identifier *attrId = identifier(attrAst->attribute_identifier_token);
        if (attrId == control()->objcGetterId()) {
            if (checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::Getter)) {
                getterName = semantic()->check(attrAst->method_selector, _scope);
            }
        } else if (attrId == control()->objcSetterId()) {
            if (checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::Setter)) {
                setterName = semantic()->check(attrAst->method_selector, _scope);
            }
        } else if (attrId == control()->objcReadwriteId()) {
            checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::ReadWrite);
        } else if (attrId == control()->objcReadonlyId()) {
            checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::ReadOnly);
        } else if (attrId == control()->objcAssignId()) {
            checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::Assign);
        } else if (attrId == control()->objcRetainId()) {
            checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::Retain);
        } else if (attrId == control()->objcCopyId()) {
            checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::Copy);
        } else if (attrId == control()->objcNonatomicId()) {
            checkPropertyAttribute(attrAst, propAttrs, ObjCPropertyDeclaration::NonAtomic);
        }
    }

    if (propAttrs & ObjCPropertyDeclaration::ReadOnly &&
        propAttrs & ObjCPropertyDeclaration::ReadWrite)
        // Should this be an error instead of only a warning?
        translationUnit()->warning(ast->property_token,
                                   "property can have at most one attribute \"readonly\" or \"readwrite\" specified");
    int setterSemAttrs = propAttrs & ObjCPropertyDeclaration::SetterSemanticsMask;
    if (setterSemAttrs
            && setterSemAttrs != ObjCPropertyDeclaration::Assign
            && setterSemAttrs != ObjCPropertyDeclaration::Retain
            && setterSemAttrs != ObjCPropertyDeclaration::Copy) {
        // Should this be an error instead of only a warning?
        translationUnit()->warning(ast->property_token,
                                   "property can have at most one attribute \"assign\", \"retain\", or \"copy\" specified");
    }

    List<ObjCPropertyDeclaration *> **lastSymbols = &ast->symbols;
    for (List<Declaration*> *iter = simpleDecl->symbols; iter; iter = iter->next) {
        ObjCPropertyDeclaration *propDecl = control()->newObjCPropertyDeclaration(ast->firstToken(),
                                                                                  iter->value->name());
        propDecl->setType(iter->value->type());
        propDecl->setAttributes(propAttrs);
        propDecl->setGetterName(getterName);
        propDecl->setSetterName(setterName);
        _scope->enterSymbol(propDecl);

        *lastSymbols = new (translationUnit()->memoryPool()) List<ObjCPropertyDeclaration *>();
        (*lastSymbols)->value = propDecl;
        lastSymbols = &(*lastSymbols)->next;
    }

    return false;
}
