/*
 * MIT License
 *
 * Copyright (c) 2018 Kalate Hexanome, 4IF, INSA Lyon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "AbstractSyntaxTreeVisitor.h"
#include "../ast/statements/expressions/atomicexpression/Constant.h"
#include "../ast/statements/expressions/binaryexpression/BinaryExpression.h"
#include "../ast/operators/binaryoperators/BitwiseAndOperator.h"
#include "../ast/operators/binaryoperators/BitwiseOrOperator.h"
#include "../ast/operators/binaryoperators/BitwiseXorOperator.h"
#include "../ast/operators/binaryoperators/DisjunctionOperator.h"
#include "../ast/operators/binaryoperators/ConjunctionOperator.h"

#define FIND_BINARY_OP(ctx) mBinaryOperatorIndex.getOpForToken((ctx)->getText())

using namespace caramel::ast;
using namespace caramel::utils;
using namespace caramel::colors;
using namespace caramel::visitors;

//--------------------------------------------------------------------------------------------------------
// Binary Expressions

antlrcpp::Any AbstractSyntaxTreeVisitor::visitExpression(CaramelParser::ExpressionContext *ctx) {
    logger.trace() << "visiting expression: " << grey <<ctx->getText();
    return CaramelBaseVisitor::visitExpression(ctx);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitAdditiveExpression(CaramelParser::AdditiveExpressionContext *ctx) {
    logger.trace() << "visiting additive expression: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitAdditiveExpression(ctx->additiveExpression(0)),
                visitAdditiveOperator(ctx->additiveOperator()),
                visitAdditiveExpression(ctx->additiveExpression(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitBitwiseShiftExpression(CaramelParser::BitwiseShiftExpressionContext *ctx) {
    logger.trace() << "visiting bitwise shift expression: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitBitwiseShiftExpression(ctx->bitwiseShiftExpression(0)),
                visitBitwiseShiftOperator(ctx->bitwiseShiftOperator()),
                visitBitwiseShiftExpression(ctx->bitwiseShiftExpression(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitComparison(CaramelParser::ComparisonContext *ctx) {
    logger.trace() << "visiting comparison: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitComparison(ctx->comparison(0)),
                visitComparativeOperator(ctx->comparativeOperator()),
                visitComparison(ctx->comparison(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitEqualityComparison(CaramelParser::EqualityComparisonContext *ctx) {
    logger.trace() << "visiting equality comparison: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitEqualityComparison(ctx->equalityComparison(0)),
                visitEqualityOperator(ctx->equalityOperator()),
                visitEqualityComparison(ctx->equalityComparison(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitMultiplicativeExpression(CaramelParser::MultiplicativeExpressionContext *ctx) {
    logger.trace() << "visiting multiplicative expression: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitMultiplicativeExpression(ctx->multiplicativeExpression(0)),
                visitMultiplicativeOperator(ctx->multiplicativeOperator()),
                visitMultiplicativeExpression(ctx->multiplicativeExpression(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitAtomicExpression(CaramelParser::AtomicExpressionContext *ctx) {
    logger.trace() << "visiting atomic expression: " << grey <<ctx->getText();

    Expression::Ptr result;
    if (ctx->validIdentifier()) {
        std::string varName = visitValidIdentifier(ctx->validIdentifier());
        Symbol::Ptr symbol = currentContext()->getSymbolTable()->addVariableUsage(ctx, varName, castTo<Statement::Ptr>(result));
    } else if (ctx->charConstant()) {
        result = castAnyTo<AtomicExpression::Ptr, Expression::Ptr>(visitCharConstant(ctx->charConstant()));
    } else if (ctx->numberConstant()) {
        result = castAnyTo<AtomicExpression::Ptr, Expression::Ptr>(visitNumberConstant(ctx->numberConstant()));
    } else if (ctx->expression()) {
        result = visitExpression(ctx->expression());
    }
    return castTo<Expression::Ptr>(result);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitAndBitwiseExpression(CaramelParser::AndBitwiseExpressionContext *ctx) {
    logger.trace() << "visiting bitwise AND expression: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitAndBitwiseExpression(ctx->andBitwiseExpression(0)),
                mBinaryOperatorIndex.getOpForToken(BitwiseAndOperator::SYMBOL),
                visitAndBitwiseExpression(ctx->andBitwiseExpression(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitOrBitwiseExpression(CaramelParser::OrBitwiseExpressionContext *ctx) {
    logger.trace() << "visiting bitwise OR expression: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitOrBitwiseExpression(ctx->orBitwiseExpression(0)),
                mBinaryOperatorIndex.getOpForToken(BitwiseOrOperator::SYMBOL),
                visitOrBitwiseExpression(ctx->orBitwiseExpression(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitXorBitwiseExpression(CaramelParser::XorBitwiseExpressionContext *ctx) {
    logger.trace() << "visiting bitwise XOR expression: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitXorBitwiseExpression(ctx->xorBitwiseExpression(0)),
                mBinaryOperatorIndex.getOpForToken(BitwiseXorOperator::SYMBOL),
                visitXorBitwiseExpression(ctx->xorBitwiseExpression(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitConjunction(CaramelParser::ConjunctionContext *ctx) {
    logger.trace() << "visiting conjuction: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitConjunction(ctx->conjunction(0)),
                mBinaryOperatorIndex.getOpForToken(ConjunctionOperator::SYMBOL),
                visitConjunction(ctx->conjunction(1)),
                ctx->getStart()
        ));
    }
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitDisjunction(CaramelParser::DisjunctionContext *ctx) {
    logger.trace() << "visiting disjunction: " << grey <<ctx->getText();
    if (ctx->children.size() == 1) {
        // One children = No BinaryExpression at this step.
        return visitChildren(ctx).as<Expression::Ptr>();
    } else {
        return std::dynamic_pointer_cast<Expression>(std::make_shared<BinaryExpression>(
                visitDisjunction(ctx->disjunction(0)),
                mBinaryOperatorIndex.getOpForToken(DisjunctionOperator::SYMBOL),
                visitDisjunction(ctx->disjunction(1)),
                ctx->getStart()
        ));
    }
}


//--------------------------------------------------------------------------------------------------------
// Unary Expressions

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitPostfixUnaryExpression(CaramelParser::PostfixUnaryExpressionContext *ctx) {
    logger.trace() << "visiting postfix unary expression: " << grey <<ctx->getText();

    antlrcpp::Any atomicExpression = visitAtomicExpression(ctx->atomicExpression());
    if (ctx->postfixUnaryOperation().size() > 0) {
        for (CaramelParser::PostfixUnaryOperationContext *postFixCtx : ctx->postfixUnaryOperation()) {
            if (postFixCtx->callSufix()) {
                if (!atomicExpression.is<FunctionSymbol::Ptr>()) {
                    throw std::runtime_error("Cannot use function call on non function symbol");
                }
            } else if (postFixCtx->arrayAccess()) {
                if (!atomicExpression.is<VariableSymbol::Ptr>()) {

                }
            } else if (postFixCtx->postfixUnaryOperator()) {
                if (!atomicExpression.is<VariableSymbol::Ptr>()) {
                    throw std::runtime_error("Cannot use postfix Unary Operator on non variable symbol");
                }
            }
        }
    }

    if (atomicExpression.is<Symbol::Ptr>()) {
        return castAnyTo<Symbol::Ptr, AtomicExpression::Ptr>(atomicExpression);
    } else {
        return atomicExpression;
    }
}

//--------------------------------------------------------------------------------------------------------
// Binary Operators

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitAdditiveOperator(caramel_unused CaramelParser::AdditiveOperatorContext *ctx) {
    logger.trace() << "visiting additive operator: " << grey <<ctx->getText();
    return FIND_BINARY_OP(ctx);
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitMultiplicativeOperator(CaramelParser::MultiplicativeOperatorContext *ctx) {
    logger.trace() << "visiting multiplicative operator: " << grey << ctx->getText();
    return FIND_BINARY_OP(ctx);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitBitwiseShiftOperator(CaramelParser::BitwiseShiftOperatorContext *ctx) {
    logger.trace() << "visiting bitwise shift operator: " << grey << ctx->getText();
    return FIND_BINARY_OP(ctx);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitComparativeOperator(CaramelParser::ComparativeOperatorContext *ctx) {
    logger.trace() << "visiting comparative operator: " << grey << ctx->getText();
    return FIND_BINARY_OP(ctx);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitEqualityOperator(CaramelParser::EqualityOperatorContext *ctx) {
    logger.trace() << "visiting equality operator: " << grey << ctx->getText();
    return FIND_BINARY_OP(ctx);
}

//--------------------------------------------------------------------------------------------------------
// Unary Operators

//--------------------------------------------------------------------------------------------------------
// LValue

antlrcpp::Any AbstractSyntaxTreeVisitor::visitNumberConstant(CaramelParser::NumberConstantContext *ctx) {
    logger.trace() << "visiting number constant: " << grey <<ctx->getText();

    long long value = std::stoll(ctx->getText());
    return castTo<AtomicExpression::Ptr>(std::make_shared<Constant>(value, ctx->start));
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitCharConstant(CaramelParser::CharConstantContext *ctx) {
    logger.trace() << "visiting char constant: " << grey <<ctx->getText();

    char value = ctx->getText().at(0);
    return castTo<AtomicExpression::Ptr>(std::make_shared<Constant>(value, ctx->start));
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitPositiveConstant(CaramelParser::PositiveConstantContext *ctx) {
    logger.trace() << "visiting positive constant: " << grey <<ctx->getText();

    long long value = std::stoll(ctx->getText());
    return castTo<AtomicExpression::Ptr>(std::make_shared<Constant>(value, ctx->getStart()));
}