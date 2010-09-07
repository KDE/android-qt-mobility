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

#include "CheckDeclarator.h"
#include "Semantic.h"
#include "AST.h"
#include "Control.h"
#include "TranslationUnit.h"
#include "Literals.h"
#include "CoreTypes.h"
#include "Symbols.h"

using namespace CPlusPlus;

CheckDeclarator::CheckDeclarator(Semantic *semantic)
    : SemanticCheck(semantic),
      _declarator(0),
      _scope(0),
      _name(0)
{ }

CheckDeclarator::~CheckDeclarator()
{ }

FullySpecifiedType CheckDeclarator::check(DeclaratorAST *declarator,
                                          const FullySpecifiedType &type,
                                          Scope *scope,
                                          const Name **name)
{
    FullySpecifiedType previousType = switchFullySpecifiedType(type);
    Scope *previousScope = switchScope(scope);
    DeclaratorAST *previousDeclarator = switchDeclarator(declarator);
    const Name **previousName = switchName(name);
    accept(declarator);
    (void) switchName(previousName);
    (void) switchDeclarator(previousDeclarator);
    (void) switchScope(previousScope);
    return switchFullySpecifiedType(previousType);
}

FullySpecifiedType CheckDeclarator::check(PtrOperatorListAST *ptrOperators,
                                          const FullySpecifiedType &type,
                                          Scope *scope)
{
    FullySpecifiedType previousType = switchFullySpecifiedType(type);
    Scope *previousScope = switchScope(scope);
    accept(ptrOperators);
    (void) switchScope(previousScope);
    return switchFullySpecifiedType(previousType);
}

FullySpecifiedType CheckDeclarator::check(ObjCMethodPrototypeAST *methodPrototype,
                                          Scope *scope)
{
    FullySpecifiedType previousType = switchFullySpecifiedType(FullySpecifiedType());
    Scope *previousScope = switchScope(scope);
    accept(methodPrototype);
    (void) switchScope(previousScope);
    return switchFullySpecifiedType(previousType);
}

DeclaratorAST *CheckDeclarator::switchDeclarator(DeclaratorAST *declarator)
{
    DeclaratorAST *previousDeclarator = _declarator;
    _declarator = declarator;
    return previousDeclarator;
}

FullySpecifiedType CheckDeclarator::switchFullySpecifiedType(const FullySpecifiedType &type)
{
    FullySpecifiedType previousType = _fullySpecifiedType;
    _fullySpecifiedType = type;
    return previousType;
}

Scope *CheckDeclarator::switchScope(Scope *scope)
{
    Scope *previousScope = _scope;
    _scope = scope;
    return previousScope;
}

const Name **CheckDeclarator::switchName(const Name **name)
{
    const Name **previousName = _name;
    _name = name;
    return previousName;
}

bool CheckDeclarator::visit(DeclaratorAST *ast)
{
    accept(ast->ptr_operator_list);
    accept(ast->postfix_declarator_list);
    accept(ast->core_declarator);

    if (ast->initializer) {
        FullySpecifiedType exprTy = semantic()->check(ast->initializer, _scope);

        if (Function *funTy = _fullySpecifiedType->asFunctionType())
            funTy->setPureVirtual(true);
    }

    return false;
}

bool CheckDeclarator::visit(DeclaratorIdAST *ast)
{
    const Name *name = semantic()->check(ast->name, _scope);
    if (_name)
        *_name = name;
    return false;
}

bool CheckDeclarator::visit(NestedDeclaratorAST *ast)
{
    accept(ast->declarator);
    return false;
}

bool CheckDeclarator::visit(FunctionDeclaratorAST *ast)
{
    Function *fun = control()->newFunction(ast->firstToken());
    fun->setAmbiguous(ast->as_cpp_initializer != 0);
    ast->symbol = fun;
    fun->setReturnType(_fullySpecifiedType);

    if (_fullySpecifiedType.isVirtual())
        fun->setVirtual(true);

    if (ast->parameters) {
        DeclarationListAST *parameter_declarations = ast->parameters->parameter_declaration_list;
        for (DeclarationListAST *decl = parameter_declarations; decl; decl = decl->next) {
            semantic()->check(decl->value, fun->arguments());
        }

        if (ast->parameters->dot_dot_dot_token)
            fun->setVariadic(true);
    }

    // check the arguments
    bool hasDefaultArguments = false;
    for (unsigned i = 0; i < fun->argumentCount(); ++i) {
        Argument *arg = fun->argumentAt(i)->asArgument();
        if (hasDefaultArguments && ! arg->hasInitializer()) {
            translationUnit()->error(ast->firstToken(),
                "default argument missing for parameter at position %d", i + 1);
        } else if (! hasDefaultArguments) {
            hasDefaultArguments = arg->hasInitializer();
        }
    }

    FullySpecifiedType funTy(fun);
    _fullySpecifiedType = funTy;

    for (SpecifierListAST *it = ast->cv_qualifier_list; it; it = it->next) {
        SimpleSpecifierAST *cv = static_cast<SimpleSpecifierAST *>(it->value);
        const int k = tokenKind(cv->specifier_token);
        if (k == T_CONST)
            fun->setConst(true);
        else if (k == T_VOLATILE)
            fun->setVolatile(true);
    }

    return false;
}

bool CheckDeclarator::visit(ArrayDeclaratorAST *ast)
{
    ArrayType *ty = control()->arrayType(_fullySpecifiedType); // ### set the dimension
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    FullySpecifiedType arrTy(ty);
    _fullySpecifiedType = ty;
    return false;
}

bool CheckDeclarator::visit(PointerToMemberAST *ast)
{
    const Name *memberName = semantic()->check(ast->nested_name_specifier_list, _scope);
    PointerToMemberType *ptrTy = control()->pointerToMemberType(memberName, _fullySpecifiedType);
    FullySpecifiedType ty(ptrTy);
    _fullySpecifiedType = ty;
    applyCvQualifiers(ast->cv_qualifier_list);
    return false;
}

bool CheckDeclarator::visit(PointerAST *ast)
{
    PointerType *ptrTy = control()->pointerType(_fullySpecifiedType);
    FullySpecifiedType ty(ptrTy);
    _fullySpecifiedType = ty;
    applyCvQualifiers(ast->cv_qualifier_list);
    return false;
}

bool CheckDeclarator::visit(ReferenceAST *)
{
    ReferenceType *refTy = control()->referenceType(_fullySpecifiedType);
    FullySpecifiedType ty(refTy);
    _fullySpecifiedType = ty;
    return false;
}

bool CheckDeclarator::visit(ObjCMethodPrototypeAST *ast)
{
    if (!ast)
        return false;

    if (!ast->selector) {
        // TODO: (EV) this currently happens when parsing:
        //   + (id<NSSomeProtocol>) zoo;
        // where the parser will start doing template magic. We'll need to disambiguate this case.
        return false;
    }

    FullySpecifiedType returnType = semantic()->check(ast->type_name, _scope);

    unsigned location = ast->firstToken();

    semantic()->check(ast->selector, _scope);

    ObjCMethod *method = control()->newObjCMethod(location, ast->selector->selector_name);
    ast->symbol = method;
    method->setSourceLocation(location);
    method->setScope(_scope);
    method->setVisibility(semantic()->currentVisibility());
    method->setReturnType(returnType);
    if (semantic()->isObjCClassMethod(tokenKind(ast->method_type_token)))
        method->setStorage(Symbol::Static);

    if (ast->selector && ast->selector->asObjCSelectorWithArguments()) {
        for (ObjCMessageArgumentDeclarationListAST *it = ast->argument_list; it; it = it->next) {
            ObjCMessageArgumentDeclarationAST *argDecl = it->value;

            semantic()->check(argDecl, method->arguments());
        }

        if (ast->dot_dot_dot_token)
            method->setVariadic(true);
    }

    _fullySpecifiedType = FullySpecifiedType(method);

    return false;
}

void CheckDeclarator::applyCvQualifiers(SpecifierListAST *it)
{
    for (; it; it = it->next) {
        SpecifierAST *cv = it->value;
        SimpleSpecifierAST *spec = static_cast<SimpleSpecifierAST *>(cv);
        switch (translationUnit()->tokenKind(spec->specifier_token)) {
        case T_VOLATILE:
            _fullySpecifiedType.setVolatile(true);
            break;
        case T_CONST:
            _fullySpecifiedType.setConst(true);
            break;
        default:
            break;
        } // switch
    }
}


