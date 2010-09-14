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

#include "CheckExpression.h"
#include "Semantic.h"
#include "TranslationUnit.h"
#include "AST.h"
#include "Scope.h"
#include "Literals.h"
#include "CoreTypes.h"
#include "Symbols.h"
#include "Control.h"

using namespace CPlusPlus;

CheckExpression::CheckExpression(Semantic *semantic)
    : SemanticCheck(semantic),
      _expression(0),
      _scope(0),
      _checkOldStyleCasts(false)
{ }

CheckExpression::~CheckExpression()
{ }

FullySpecifiedType CheckExpression::check(ExpressionAST *expression, Scope *scope)
{
    FullySpecifiedType previousType = switchFullySpecifiedType(FullySpecifiedType());
    Scope *previousScope = switchScope(scope);
    ExpressionAST *previousExpression = switchExpression(expression);
    accept(expression);
    (void) switchExpression(previousExpression);
    (void) switchScope(previousScope);
    return switchFullySpecifiedType(previousType);
}

ExpressionAST *CheckExpression::switchExpression(ExpressionAST *expression)
{
    ExpressionAST *previousExpression = _expression;
    _expression = expression;
    return previousExpression;
}

FullySpecifiedType CheckExpression::switchFullySpecifiedType(const FullySpecifiedType &type)
{
    FullySpecifiedType previousType = _fullySpecifiedType;
    _fullySpecifiedType = type;
    return previousType;
}

Scope *CheckExpression::switchScope(Scope *scope)
{
    Scope *previousScope = _scope;
    _scope = scope;
    return previousScope;
}

bool CheckExpression::visit(BinaryExpressionAST *ast)
{
    FullySpecifiedType leftExprTy = semantic()->check(ast->left_expression, _scope);
    FullySpecifiedType rightExprTy = semantic()->check(ast->right_expression, _scope);
    return false;
}

bool CheckExpression::visit(CastExpressionAST *ast)
{
    FullySpecifiedType castTy = semantic()->check(ast->type_id, _scope);
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    if (_checkOldStyleCasts && ! castTy->isVoidType())
        translationUnit()->warning(ast->firstToken(),
                                   "ugly old style cast");
    return false;
}

bool CheckExpression::visit(ConditionAST *ast)
{
    FullySpecifiedType typeSpecTy = semantic()->check(ast->type_specifier_list, _scope);
    const Name *name = 0;
    FullySpecifiedType declTy = semantic()->check(ast->declarator, typeSpecTy.qualifiedType(),
                                                  _scope, &name);
    Declaration *decl = control()->newDeclaration(ast->declarator->firstToken(), name);
    decl->setType(declTy);
    _scope->enterSymbol(decl);
    return false;
}

bool CheckExpression::visit(ConditionalExpressionAST *ast)
{
    FullySpecifiedType condTy = semantic()->check(ast->condition, _scope);
    FullySpecifiedType leftExprTy = semantic()->check(ast->left_expression, _scope);
    FullySpecifiedType rightExprTy = semantic()->check(ast->right_expression, _scope);
    return false;
}

bool CheckExpression::visit(CppCastExpressionAST *ast)
{
    FullySpecifiedType typeIdTy = semantic()->check(ast->type_id, _scope);
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(DeleteExpressionAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(ArrayInitializerAST *ast)
{
    for (ExpressionListAST *it = ast->expression_list; it; it = it->next) {
        FullySpecifiedType exprTy = semantic()->check(it->value, _scope);
    }
    return false;
}

bool CheckExpression::visit(QualifiedNameAST *ast)
{
    (void) semantic()->check(ast, _scope);
    return false;
}

bool CheckExpression::visit(OperatorFunctionIdAST *ast)
{
    (void) semantic()->check(ast, _scope);
    return false;
}

bool CheckExpression::visit(ConversionFunctionIdAST *ast)
{
    (void) semantic()->check(ast, _scope);
    return false;
}

bool CheckExpression::visit(SimpleNameAST *ast)
{
    (void) semantic()->check(ast, _scope);
    return false;
}

bool CheckExpression::visit(DestructorNameAST *ast)
{
    (void) semantic()->check(ast, _scope);
    return false;
}

bool CheckExpression::visit(TemplateIdAST *ast)
{
    (void) semantic()->check(ast, _scope);
    return false;
}

bool CheckExpression::visit(NewExpressionAST *ast)
{
    if (ast->new_placement) {
        for (ExpressionListAST *it = ast->new_placement->expression_list; it; it = it->next) {
            FullySpecifiedType exprTy = semantic()->check(it->value, _scope);
        }
    }

    FullySpecifiedType typeIdTy = semantic()->check(ast->type_id, _scope);

    if (ast->new_type_id) {
        FullySpecifiedType ty = semantic()->check(ast->new_type_id->type_specifier_list, _scope);

        for (NewArrayDeclaratorListAST *it = ast->new_type_id->new_array_declarator_list; it; it = it->next) {
            if (NewArrayDeclaratorAST *declarator = it->value) {
                FullySpecifiedType exprTy = semantic()->check(declarator->expression, _scope);
            }
        }
    }

    // ### process new-initializer
    if (ast->new_initializer) {
        FullySpecifiedType exprTy = semantic()->check(ast->new_initializer->expression, _scope);
    }

    return false;
}

bool CheckExpression::visit(TypeidExpressionAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(TypenameCallExpressionAST *ast)
{
    (void) semantic()->check(ast->name, _scope);

    for (ExpressionListAST *it = ast->expression_list; it; it = it->next) {
        FullySpecifiedType exprTy = semantic()->check(it->value, _scope);
        (void) exprTy;
    }
    return false;
}

bool CheckExpression::visit(TypeConstructorCallAST *ast)
{
    FullySpecifiedType typeSpecTy = semantic()->check(ast->type_specifier_list, _scope);
    for (ExpressionListAST *it = ast->expression_list; it; it = it->next) {
        FullySpecifiedType exprTy = semantic()->check(it->value, _scope);
    }
    return false;
}

bool CheckExpression::visit(PostfixExpressionAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->base_expression, _scope);
    for (PostfixListAST *it = ast->postfix_expression_list; it; it = it->next) {
        accept(it->value); // ### not exactly.
    }
    return false;
}

bool CheckExpression::visit(SizeofExpressionAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(NumericLiteralAST *)
{
    _fullySpecifiedType.setType(control()->integerType(IntegerType::Int));
    return false;
}

bool CheckExpression::visit(BoolLiteralAST *)
{
    _fullySpecifiedType.setType(control()->integerType(IntegerType::Bool));
    return false;
}

bool CheckExpression::visit(StringLiteralAST *)
{
    IntegerType *charTy = control()->integerType(IntegerType::Char);
    _fullySpecifiedType.setType(control()->pointerType(charTy));
    return false;
}

bool CheckExpression::visit(ThisExpressionAST *)
{
    return false;
}

bool CheckExpression::visit(NestedExpressionAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(ThrowExpressionAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(TypeIdAST *ast)
{
    FullySpecifiedType typeSpecTy = semantic()->check(ast->type_specifier_list, _scope);
    FullySpecifiedType declTy = semantic()->check(ast->declarator, typeSpecTy.qualifiedType(), _scope);
    _fullySpecifiedType = declTy;
    return false;
}

bool CheckExpression::visit(UnaryExpressionAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(QtMethodAST *ast)
{
    const Name *name = 0;
    Scope dummy;
    FullySpecifiedType methTy = semantic()->check(ast->declarator, FullySpecifiedType(),
                                                  &dummy, &name);
    Function *fty = methTy->asFunctionType();
    if (! fty)
        translationUnit()->warning(ast->firstToken(), "expected a function declarator");
    else {
        for (unsigned i = 0; i < fty->argumentCount(); ++i) {
            Symbol *arg = fty->argumentAt(i);
            if (arg->name())
                translationUnit()->warning(arg->sourceLocation(),
                                           "argument should be anonymous");
        }
    }
    return false;
}

bool CheckExpression::visit(CompoundLiteralAST *ast)
{
    /*FullySpecifiedType exprTy = */ semantic()->check(ast->type_id, _scope);
    /*FullySpecifiedType initTy = */ semantic()->check(ast->initializer, _scope);
    return false;
}

bool CheckExpression::visit(CallAST *ast)
{
    for (ExpressionListAST *it = ast->expression_list; it; it = it->next) {
        FullySpecifiedType exprTy = semantic()->check(it->value, _scope);
    }
    return false;
}

bool CheckExpression::visit(ArrayAccessAST *ast)
{
    FullySpecifiedType exprTy = semantic()->check(ast->expression, _scope);
    return false;
}

bool CheckExpression::visit(PostIncrDecrAST *)
{
    return false;
}

bool CheckExpression::visit(MemberAccessAST *ast)
{
    (void) semantic()->check(ast->member_name, _scope);
    return false;
}

bool CheckExpression::visit(ObjCMessageExpressionAST *ast)
{
    (void) semantic()->check(ast->receiver_expression, _scope);
    (void) semantic()->check(ast->selector, _scope);

    accept(ast->argument_list); // ### not necessary.
    return false;
}

bool CheckExpression::visit(ObjCEncodeExpressionAST * /*ast*/)
{
    // TODO: visit the type name, but store the type here? (EV)
    return true;
}

bool CheckExpression::visit(ObjCSelectorExpressionAST *ast)
{
    (void) semantic()->check(ast->selector, _scope);
    return false;
}


