/*
 * MIT License
 *
 * Copyright (c) 2018 insa.4if.hexanome_kalate
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
#include "../Logger.h"
#include "../datastructure/statements/expressions/atomicexpression/Constant.h"
#include "../datastructure/symboltable/VariableSymbol.h"
#include "../datastructure/symboltable/FunctionSymbol.h"
#include "../datastructure/statements/declaration/VariableDeclaration.h"
#include "../datastructure/statements/declaration/FunctionDeclaration.h"
#include "../datastructure/statements/declaration/ArrayDeclaration.h"
#include "../datastructure/statements/definition/VariableDefinition.h"
#include "../datastructure/statements/definition/FunctionDefinition.h"
#include "../datastructure/statements/definition/ArrayDefinition.h"
#include "../datastructure/symboltable/ArraySymbol.h"
#include "../datastructure/statements/expressions/binaryexpression/BinaryExpression.h"
#include "../exceptions/ArraySizeNonConstantException.h"
#include "../datastructure/statements/jumps/Jump.h"
#include "../datastructure/statements/controlblocks/ControlBlock.h"
#include "../exceptions/SemanticError.h"
#include "../exceptions/FunctionDefinitionParameterNameMismatchError.h"
#include "../datastructure/statements/jumps/ReturnStatement.h"
#include "../datastructure/statements/expressions/atomicexpression/Identifier.h"


using namespace caramel::visitors;
using namespace caramel::ast;
using namespace caramel::util;

AbstractSyntaxTreeVisitor::AbstractSyntaxTreeVisitor(std::string const &sourceFileName)
        : mSourceFileUtil{sourceFileName},
          mBitwiseShiftOperator(std::make_shared<BitwiseShiftOperator>()),
          mPlusOperator(std::make_shared<PlusOperator>()),
          mMultOperator(std::make_shared<MultOperator>()) {
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitR(CaramelParser::RContext *ctx) {
    ContextPusher contextPusher(*this);
    Context::Ptr context = contextPusher.getContext();

    SymbolTable::Ptr symbolTable = context->getSymbolTable();

    PrimaryType::Ptr void_t = Void_t::Create();
    PrimaryType::Ptr char_t = Char::Create();
    PrimaryType::Ptr int8_t = Int8_t::Create();
    PrimaryType::Ptr int16_t = Int16_t::Create();
    PrimaryType::Ptr int32_t = Int32_t::Create();
    PrimaryType::Ptr int64_t = Int64_t::Create();

    symbolTable->addPrimaryType(void_t, void_t->getIdentifier());
    symbolTable->addPrimaryType(char_t, char_t->getIdentifier());
    symbolTable->addPrimaryType(int8_t, int8_t->getIdentifier());
    symbolTable->addPrimaryType(int16_t, int16_t->getIdentifier());
    symbolTable->addPrimaryType(int32_t, int32_t->getIdentifier());
    symbolTable->addPrimaryType(int64_t, int64_t->getIdentifier());

    context->addStatements(visitStatements(ctx->statements()));
    return context;
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitStatements(CaramelParser::StatementsContext *ctx) {
    logger.trace() << "visit statements: " << ctx->getText();
    using namespace caramel::colors;
    using caramel::ast::Statement;

    std::vector<Statement::Ptr> statements;
    for (auto statement : ctx->statement()) {
        logger.debug() << "Visiting: " << mSourceFileUtil.getLine(statement->start->getLine());

        antlrcpp::Any r = visitStatement(statement);
        if (r.is<Statement::Ptr>()) {
            statements.push_back(r.as<Statement::Ptr>());
            logger.info() << green << "Yay statement:\n" << statement->getText();
        } else if (r.is<std::vector<Statement::Ptr>>()) {
            auto statementVector = r.as<std::vector<Statement::Ptr>>();
            std::copy(statementVector.begin(), statementVector.end(), std::back_inserter(statements));
            logger.info() << green << "Yay statement:\n" << statement->getText();
        } else {
            logger.warning() << "Skipping unhandled statement:\n" << statement->getText();
        }
    }
    return statements;
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitBlock(CaramelParser::BlockContext *ctx) {
    logger.trace() << "visit block: " << ctx->getText();

    using caramel::ast::Statement;

    std::vector<Statement::Ptr> returnStatements;

    if (ctx->declarations()) {
        std::vector<Statement::Ptr> declarations = visitDeclarations(ctx->declarations());
        //currentContext()->addStatements(std::move(declarations));
        std::move(declarations.begin(),declarations.end(),std::back_inserter(returnStatements));
    }
    if (ctx->instructions()) {
        std::vector<Statement::Ptr> instructions = visitInstructions(ctx->instructions());
        //currentContext()->addStatements(std::move(instructions));
        std::move(instructions.begin(),instructions.end(),std::back_inserter(returnStatements));
    }

    return returnStatements;
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitDeclarations(CaramelParser::DeclarationsContext *ctx) {
    logger.trace() << "visit declarations: " << ctx->getText();

    using caramel::ast::Statement;

    std::vector<Statement::Ptr> declarations;
    for (auto declaration : ctx->declaration()) {
        antlrcpp::Any r = visitDeclaration(declaration);
        if (r.is<Statement::Ptr>()) {
            declarations.push_back(r.as<Statement::Ptr>());
        } else if (r.is<std::vector<Statement::Ptr>>()) {
            auto declarationVector = r.as<std::vector<Statement::Ptr>>();
            std::copy(declarationVector.begin(), declarationVector.end(), std::back_inserter(declarations));
        } else {
            logger.warning() << "Skipping unhandled declaration:\n" << declaration->getText();
        }
    }
    return declarations;
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitInstructions(CaramelParser::InstructionsContext *ctx) {
    logger.trace() << "visit instructions: " << ctx->getText();

    using namespace caramel::ast;

    std::vector<Statement::Ptr> instructions;
    for (auto instructionCtx : ctx->instruction()) {
        Statement::Ptr instructionAsStatement = visitInstruction(instructionCtx);
        instructions.push_back(instructionAsStatement);
    }
    return instructions;
}

// Return std::string
antlrcpp::Any AbstractSyntaxTreeVisitor::visitValidIdentifier(CaramelParser::ValidIdentifierContext *ctx) {
    return ctx->getText();
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitTypeParameter(CaramelParser::TypeParameterContext *ctx) {
    logger.trace() << "type parameter: " << ctx->getText();

    using namespace caramel::ast;

    std::string symbolName = ctx->getText();
    if (currentContext()->getSymbolTable()->hasSymbol(symbolName)) {
        Symbol::Ptr symbol = currentContext()->getSymbolTable()->getSymbol(ctx, symbolName);
        return castTo<TypeSymbol::Ptr>(symbol);
    } else {
        logger.warning() << "Default symbol " << symbolName << " is created with void_t as return type";
        return std::make_shared<TypeSymbol>( symbolName, Void_t::Create() );
    }
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitVariableDeclaration(CaramelParser::VariableDeclarationContext *ctx) {

    using namespace caramel::ast;

    logger.trace() << "Visiting variable declaration: " << ctx->getText();
    TypeSymbol::Ptr typeSymbol = visitTypeParameter(ctx->typeParameter()).as<TypeSymbol::Ptr>();
    std::vector<Statement::Ptr> variables;
    for (auto validIdentifierCtx : ctx->validIdentifier()) {
        std::string name = visitValidIdentifier(validIdentifierCtx);
        VariableSymbol::Ptr variableSymbol = std::make_shared<VariableSymbol>(name, typeSymbol);

        VariableDeclaration::Ptr variableDeclaration = std::make_shared<VariableDeclaration>(variableSymbol,
                                                                                             validIdentifierCtx->getStart());
        variables.push_back(variableDeclaration);

        currentContext()->getSymbolTable()->addVariableDeclaration(ctx, typeSymbol->getType(), name,
                                                                   variableDeclaration);

        logger.trace() << "New variable declared " << name << " of type " << typeSymbol->getType()->getIdentifier();
    }

    return variables;
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitVariableDefinition(CaramelParser::VariableDefinitionContext *ctx) {

    using namespace caramel::ast;

    logger.trace() << "Visiting variable definition: " << ctx->getText();

    TypeSymbol::Ptr typeSymbol = visitTypeParameter(ctx->typeParameter()).as<TypeSymbol::Ptr>();
    std::vector<Statement::Ptr> variables;
    for (auto varDefValue : ctx->validIdentifier()) {
        std::string name = visitValidIdentifier(varDefValue);
        logger.trace() << "New variable declared: '" << name << "' with default value";

        VariableDefinition::Ptr variableDef = std::make_shared<VariableDefinition>(varDefValue->getStart());
        variables.push_back(variableDef);

        VariableSymbol::Ptr variableSymbol = currentContext()->getSymbolTable()->addVariableDefinition(
                ctx, typeSymbol->getType(), name, variableDef);
        variableDef->setVariableSymbol(variableSymbol);
    }
    for (auto *varWithValue : ctx->variableDefinitionAssignment()) {
        std::string name = visitValidIdentifier(varWithValue->validIdentifier());

        Expression::Ptr expression = visitExpression(varWithValue->expression());
        logger.trace() << "New variable declared: '" << name << "' with value "
                       << varWithValue->expression()->getText();

        VariableDefinition::Ptr variableDef = std::make_shared<VariableDefinition>(
                expression, varWithValue->getStart());
        variables.push_back(variableDef);

        VariableSymbol::Ptr variableSymbol = currentContext()->getSymbolTable()->addVariableDefinition(
                ctx, typeSymbol->getType(), name, variableDef);
        variableDef->setVariableSymbol(variableSymbol);
    }

    return variables;
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitFunctionDeclaration(CaramelParser::FunctionDeclarationContext *ctx) {

    using namespace caramel::ast;

    logger.trace() << "Visiting function declaration: " << ctx->getText() << ' ' << ctx->getStart();

    auto innerCtx = ctx->functionDeclarationInner();

    PrimaryType::Ptr returnType = visitTypeParameter(innerCtx->typeParameter()).as<TypeSymbol::Ptr>()->getType();
    std::string name = visitValidIdentifier(innerCtx->validIdentifier());
    std::vector<Symbol::Ptr> params = visitFunctionArguments(innerCtx->functionArguments());
    FunctionDeclaration::Ptr functionDeclaration = std::make_shared<FunctionDeclaration>(innerCtx->start);
    FunctionSymbol::Ptr functionSymbol = currentContext()->getSymbolTable()->addFunctionDeclaration(
            innerCtx, returnType, name, params, functionDeclaration);
    functionDeclaration->setFunctionSymbol(functionSymbol);

    auto traceLogger = logger.trace();
    traceLogger << "New function declared " << name << " with return type " << returnType->getIdentifier();
    for (Symbol::Ptr const &param : params) {
        traceLogger << "\n\t- param: " << param->getName() << " as " << param->getType()->getIdentifier();
    }
    traceLogger.show();

    return castTo<Statement::Ptr>(functionDeclaration);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitFunctionDefinition(CaramelParser::FunctionDefinitionContext *ctx) {
    logger.trace() << "Visiting function definition: " << ctx->getText() << ' ' << ctx->getStart();

    using namespace caramel::ast;

    Context::Ptr parentContext = currentContext();
    ContextPusher contextPusher(*this);

    Context::Ptr functionContext = currentContext();

    CaramelParser::FunctionDeclarationInnerContext *innerCtx = ctx->functionDeclarationInner();

    PrimaryType::Ptr returnType = visitTypeParameter(innerCtx->typeParameter()).as<TypeSymbol::Ptr>()->getType();
    std::string name = visitValidIdentifier(innerCtx->validIdentifier());
    std::vector<Symbol::Ptr> params = visitFunctionArguments(innerCtx->functionArguments());

    FunctionDefinition::Ptr functionDefinition = std::make_shared<FunctionDefinition>(functionContext, innerCtx->start);
    FunctionSymbol::Ptr functionSymbol = parentContext->getSymbolTable()->addFunctionDefinition(
            innerCtx, returnType, name, params, functionDefinition
    );
    functionDefinition->setSymbol(functionSymbol);

    functionContext->addStatements(visitBlock(ctx->block()));

    return castTo<Statement::Ptr>(functionDefinition);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitFunctionArguments(CaramelParser::FunctionArgumentsContext *ctx) {

    using namespace caramel::ast;

    logger.debug() << "Visiting named arguments: " << ctx->getText();

    std::vector<Symbol::Ptr> params;
    for (auto argument : ctx->functionArgument()) {
        Symbol::Ptr symbol = visitFunctionArgument(argument);
        params.push_back(symbol);
    }
    return params;
}

// Return Symbol::Ptr
antlrcpp::Any AbstractSyntaxTreeVisitor::visitFunctionArgument(CaramelParser::FunctionArgumentContext *ctx) {

    using namespace caramel::ast;

    logger.trace() << "Visiting function argument: " << ctx->getText();

    // Get the optional name, or generate a unique one
    std::string name;
    if (ctx->validIdentifier()) {
        name = visitValidIdentifier(ctx->validIdentifier()).as<std::string>();
    } else {
        std::stringstream nameSS;
        nameSS << "__unnamed_argument_" << ctx->start->getLine() << "_" << ctx->start->getCharPositionInLine() << "__";
        name = nameSS.str();
    }

    // Get the type
    TypeSymbol::Ptr type;
    type = visitTypeParameter(ctx->typeParameter());

    // The argument is an array
    if (ctx->functionArgumentArraySuffix()) {
        return castTo<Symbol::Ptr>(std::make_shared<ArraySymbol>(name, type));
    } else {
        return castTo<Symbol::Ptr>(std::make_shared<VariableSymbol>(name, type));
    }
}

// return Expression::Ptr | Symbol::Ptr
antlrcpp::Any AbstractSyntaxTreeVisitor::visitAtomicExpression(CaramelParser::AtomicExpressionContext *ctx) {

    using namespace caramel::ast;

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

antlrcpp::Any AbstractSyntaxTreeVisitor::visitNumberConstant(CaramelParser::NumberConstantContext *ctx) {

    using namespace caramel::ast;

    long long value = std::stoll(ctx->getText());
    return castTo<AtomicExpression::Ptr>(std::make_shared<Constant>(value, ctx->start));
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitCharConstant(CaramelParser::CharConstantContext *ctx) {

    using namespace caramel::ast;

    char value = ctx->getText().at(0);
    return castTo<AtomicExpression::Ptr>(std::make_shared<Constant>(value, ctx->start));
}

// Return Statement::Ptr
antlrcpp::Any AbstractSyntaxTreeVisitor::visitInstruction(CaramelParser::InstructionContext *ctx) {
    using namespace caramel::ast;
    if (ctx->jump()) { // pour kévin : != nullptr
        return castAnyTo<Jump::Ptr, Statement::Ptr>(visitJump(ctx->jump()));
    } else if (ctx->controlBlock()) {
        return castAnyTo<ControlBlock::Ptr, Statement::Ptr>(visitControlBlock(ctx->controlBlock()));
    }

    return castAnyTo<Expression::Ptr, Statement::Ptr>(visitExpression(ctx->expression()));
}

// Return Statement::Ptr
antlrcpp::Any AbstractSyntaxTreeVisitor::visitArrayDefinition(CaramelParser::ArrayDefinitionContext *ctx) {

    using namespace caramel::ast;

    ArraySymbol::Ptr arraySymbol;
    ArrayDeclaration::Ptr arrayDeclaration;
    ArrayDefinition::Ptr arrayDefinition;

    if (ctx->arrayDeclarationVoidInner()) {
        arraySymbol = visitArrayDeclarationVoidInner(ctx->arrayDeclarationVoidInner()).as<ArraySymbol::Ptr>();

        std::vector<Expression::Ptr> expressions = visitArrayBlock(ctx->arrayBlock());
        long arraySize = expressions.size();
        arraySymbol->setSize(arraySize);

        arrayDeclaration = std::make_shared<ArrayDeclaration>(
                arraySymbol, ctx->arrayDeclarationVoidInner()->validIdentifier()->getStart());

        arrayDefinition = std::make_shared<ArrayDefinition>(expressions, ctx->start);
        arrayDefinition->setArraySymbol(arraySymbol);

    } else if (ctx->arrayDeclarationInner()) {
        arraySymbol = visitArrayDeclarationInner(ctx->arrayDeclarationInner()).as<ArraySymbol::Ptr>();

        arrayDeclaration = std::make_shared<ArrayDeclaration>(
                arraySymbol, ctx->arrayDeclarationInner()->validIdentifier()->getStart());

        if (ctx->arrayBlock()) {
            std::vector<Expression::Ptr> expressions = visitArrayBlock(ctx->arrayBlock());
            arrayDefinition = std::make_shared<ArrayDefinition>(expressions, ctx->start);
        } else {
            arrayDefinition = std::make_shared<ArrayDefinition>(std::vector<Expression::Ptr>(), ctx->start);
        }
        arrayDefinition->setArraySymbol(arraySymbol);
    }

    currentContext()->getSymbolTable()->addVariableDeclaration(
            ctx, arraySymbol->getType(), arraySymbol->getName(), arrayDeclaration);
    currentContext()->getSymbolTable()->addVariableDefinition(
            ctx, arraySymbol->getType(), arraySymbol->getName(), arrayDefinition);

    logger.trace() << "New array declared : '" << arraySymbol->getName() << "' with return type "
                   << arraySymbol->getType()->getIdentifier()
                   << ", size " << arraySymbol->getSize();

    //TODO : Cas d'erreur
    return castTo<Statement::Ptr>(arrayDeclaration);
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitArrayDeclarationVoidInner(CaramelParser::ArrayDeclarationVoidInnerContext *ctx) {

    using namespace caramel::ast;

    logger.trace() << "Visiting array declaration void: " << ctx->getText();

    auto typeSymbol = visitTypeParameter(ctx->typeParameter()).as<TypeSymbol::Ptr>();
    std::string name = visitValidIdentifier(ctx->validIdentifier());
    auto arraySymbol{std::make_shared<ArraySymbol>(name, typeSymbol, 0)};

    return arraySymbol;
}

// Return std::vector<Expression::Ptr>
antlrcpp::Any AbstractSyntaxTreeVisitor::visitArrayBlock(CaramelParser::ArrayBlockContext *ctx) {
    using namespace caramel::ast;
    std::vector<Expression::Ptr> expressions;
    for (auto expression : ctx->expression()) {
        Expression::Ptr exp = visitExpression(expression);
        expressions.push_back(exp);
    }
    return expressions;
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitArrayDeclarationInner(CaramelParser::ArrayDeclarationInnerContext *ctx) {
    logger.trace() << "Visiting array declaration : " << ctx->getText();

    using namespace caramel::ast;
    using namespace caramel::exceptions;

    auto typeSymbol = visitTypeParameter(ctx->typeParameter()).as<TypeSymbol::Ptr>();
    std::string name = visitValidIdentifier(ctx->validIdentifier());
    antlrcpp::Any arraySizeAny = visitArraySizeDeclaration(ctx->arraySizeDeclaration());
    Constant::Ptr arraySize = castAnyTo<AtomicExpression::Ptr, Constant::Ptr>(arraySizeAny);
//    if (!arraySizeAny.is<Constant::Ptr>()) {
//        throw ArraySizeNonConstantException("Non constant expression not handled for array sizes.");
//    }
    ArraySymbol::Ptr arraySymbol = std::make_shared<ArraySymbol>(name, typeSymbol,
                                                                 arraySize->getValue());

    return arraySymbol;
}

//
antlrcpp::Any AbstractSyntaxTreeVisitor::visitArraySizeDeclaration(CaramelParser::ArraySizeDeclarationContext *ctx) {
    return visitPositiveConstant(ctx->positiveConstant());
}

// Return Atomic::Ptr
antlrcpp::Any AbstractSyntaxTreeVisitor::visitPositiveConstant(CaramelParser::PositiveConstantContext *ctx) {
    using namespace caramel::ast;

    long long value = std::stoll(ctx->getText());
    return castTo<AtomicExpression::Ptr>(std::make_shared<Constant>(value, ctx->getStart()));
}

// Return Statement::Ptr
antlrcpp::Any AbstractSyntaxTreeVisitor::visitTypeDefinition(CaramelParser::TypeDefinitionContext *ctx) {
    logger.trace() << "Visiting type definition " << ctx->getText();

    using namespace caramel::ast;

    TypeSymbol::Ptr primaryTypeSymbol = visitTypeParameter(ctx->typeParameter()[0]);
    TypeSymbol::Ptr typeAliasDefault = visitTypeParameter(ctx->typeParameter()[1]);

    TypeDefinition::Ptr typeDefinition = std::make_shared<TypeDefinition>( ctx->getStart(), typeAliasDefault->getName(), primaryTypeSymbol );
    currentContext()->getSymbolTable()->addType(ctx, typeDefinition);

    return castTo<Statement::Ptr>(typeDefinition);
}

// Return <Jump::Ptr>
antlrcpp::Any AbstractSyntaxTreeVisitor::visitReturnJump(CaramelParser::ReturnJumpContext *ctx) {
    using namespace caramel::ast;
    // Fixme : return true value

    ReturnStatement::Ptr returnStatement;
    Expression::Ptr returnedExpression;
    if (ctx->expression()) {
        returnedExpression = visitExpression(ctx->expression());
    } else {
        returnedExpression = castTo<Expression::Ptr>(Constant::defaultConstant(ctx->getStart()));
    }

    return castTo<Jump::Ptr>(std::make_shared<ReturnStatement>(returnedExpression, ctx->getStart()));
}

antlrcpp::Any
AbstractSyntaxTreeVisitor::visitPostfixUnaryExpression(CaramelParser::PostfixUnaryExpressionContext *ctx) {
    using namespace caramel::ast;

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

    } else {
        return atomicExpression;
    }
}

std::shared_ptr<Context> AbstractSyntaxTreeVisitor::currentContext() {
    return mContextStack.top();
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitChildren(antlr4::tree::ParseTree *node) {
    size_t n = node->children.size();
    size_t i = 0;
    while (i < n && nullptr == node->children[i]) { i++; }
    antlrcpp::Any childResult;
    if (i < n) {
        childResult = node->children[i]->accept(this);
    } else {
        childResult = defaultResult();
    }
    return childResult;
}

ContextPusher::ContextPusher(AbstractSyntaxTreeVisitor &abstractSyntaxTreeVisitor)
        : mAbstractSyntaxTreeVisitor(abstractSyntaxTreeVisitor) {
    logger.trace() << "ContextPusher: Trying to push a new context.";

    using namespace caramel::ast;

    auto &contextStack = mAbstractSyntaxTreeVisitor.mContextStack;
    SymbolTable::Ptr parentTable;
    if (not contextStack.empty()) {
        Context::Ptr parent = contextStack.top();
        contextStack.push(std::make_shared<Context>(parent));
    } else {
        contextStack.push(std::make_shared<Context>());
    }

    logger.debug() << "Pushed a new context.";
}

ContextPusher::~ContextPusher() {
    logger.debug() << "Pop context.";
    mAbstractSyntaxTreeVisitor.mContextStack.pop();
}

Context::Ptr ContextPusher::getContext() {
    return mAbstractSyntaxTreeVisitor.currentContext();
}
