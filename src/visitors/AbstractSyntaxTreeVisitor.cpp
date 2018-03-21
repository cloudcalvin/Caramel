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
#include "../datastructure/Constant.h"
#include "../datastructure/PrimaryType.h"
#include "../datastructure/VariableSymbol.h"

using namespace Caramel::Visitors;

antlrcpp::Any AbstractSyntaxTreeVisitor::visitR(CaramelParser::RContext *ctx) {
    pushNewContext();
    return visitChildren(ctx);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitStatement(CaramelParser::StatementContext *ctx) {

    using namespace Caramel::Colors;

//    std::cout << "Visited statement: " << yellow << ctx->getText() << reset << std::endl;

    return visitChildren(ctx);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitNumberConstant(CaramelParser::NumberConstantContext *ctx) {
    long long value = std::stoll(ctx->getText());

    return Constant::Create(value);
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitCharConstant(CaramelParser::CharConstantContext *ctx) {
    return CaramelBaseVisitor::visitCharConstant(ctx);
}

antlrcpp::Any Caramel::Visitors::AbstractSyntaxTreeVisitor::visitValidIdentifier(CaramelParser::ValidIdentifierContext *ctx) {
    return ctx->getText();
}

antlrcpp::Any Caramel::Visitors::AbstractSyntaxTreeVisitor::visitTypeParameter(CaramelParser::TypeParameterContext *ctx) {
    std::string type = ctx->getText();
    // TODO: Checker in the SymbolTable

    if (type == "int8_t") {
        return (Int8_t::Create());
    } else if (type == "int16_t") {
        return (Int16_t::Create());
    } else if (type == "int32_t") {
        return (Int32_t::Create());
    } else if (type == "int64_t") {
        return (Int64_t::Create());
    } else {
        return (Int8_t::Create());
        //TODO: Trouver le return
    }
}

antlrcpp::Any Caramel::Visitors::AbstractSyntaxTreeVisitor::visitVariableDeclaration(CaramelParser::VariableDeclarationContext *ctx) {
    PrimaryType::Ptr typeName = visitTypeParameter(ctx->typeParameter());
    for (auto validIdentifierCtx : ctx->validIdentifier()) {
        std::string name = visitValidIdentifier(validIdentifierCtx);
        currentContext()->getSymbolTable()->addVariableDeclaration(typeName, name, nullptr); // FIXME: Occurrence
        logger.trace() <<  "New variable declared " << name << " of type " << typeName;
    }
    // TODO: Return SVariable

    return CaramelBaseVisitor::visitVariableDeclaration(ctx);
}


antlrcpp::Any
AbstractSyntaxTreeVisitor::visitFunctionDeclaration(CaramelParser::FunctionDeclarationContext *ctx) {
    PrimaryType::Ptr returnType = visitTypeParameter(ctx->typeParameter());
    std::string name = visitValidIdentifier(ctx->validIdentifier());
    std::vector<Symbol::Ptr> params = visitNamedArguments(ctx->namedArguments());
    currentContext()->getSymbolTable()->addFunctionDeclaration(returnType, name, params, nullptr); // FIXME: Occurrence
    logger.trace() <<  "New function declared " << name << " with return type " << returnType;
    visitNamedArguments(ctx->namedArguments());

    // TODO: Return SFunction
    return antlrcpp::Any();
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitNamedArguments(CaramelParser::NamedArgumentsContext *ctx) {
    std::vector<Symbol::Ptr> params;
    for (auto argument : ctx->namedArgument()){
        params.push_back(visitNamedArgument(argument).as<Symbol::Ptr>());
    }
    return params;
}

antlrcpp::Any AbstractSyntaxTreeVisitor::visitNamedArgument(CaramelParser::NamedArgumentContext *ctx) {
    if (nullptr != ctx->variableDeclaration()) {
        return visitVariableDeclaration(ctx->variableDeclaration()).as<Symbol::Ptr>();
    } else if (nullptr != ctx->arrayDeclarationVoid()) {
        return visitArrayDeclarationVoid(ctx->arrayDeclarationVoid()).as<Symbol::Ptr>();
    } else if (nullptr != ctx->typeParameter()) {
        return visitTypeParameter(ctx->typeParameter()).as<Symbol::Ptr>();
    }

    // TODO: Throw an error
    return nullptr;
}



void AbstractSyntaxTreeVisitor::pushNewContext() {
    mContextStack.push(Context::Create());
}

Context::Ptr AbstractSyntaxTreeVisitor::currentContext() {
    return mContextStack.top();
}


