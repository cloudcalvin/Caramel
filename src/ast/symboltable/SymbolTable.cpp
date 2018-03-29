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

#include "SymbolTable.h"
#include "../../Logger.h"
#include "../../utils/Common.h"
#include "../statements/definition/TypeDefinition.h"
#include "../../exceptions/UndefinedSymbolError.h"
#include "../../exceptions/SymbolAlreadyDefinedError.h"
#include "../../exceptions/SymbolAlreadyDeclaredError.h"
#include "../../exceptions/DeclarationMismatchException.h"
#include "../../exceptions/FunctionCallArgumentsTypeMismatchException.h"
#include "../../exceptions/FunctionDefinitionParameterNameMismatchError.h"
#include "../../exceptions/FunctionDefinitionParameterTypeMismatchError.h"
#include "../../exceptions/FunctionCallArgumentsNumberMismatchException.h"
#include "../../exceptions/FunctionDefinitionNumberOfParametersMismatchError.h"


namespace caramel::ast {

using namespace utils;
using namespace exceptions;

SymbolTable::SymbolTable(SymbolTable::Ptr const &parentTable) : mParentTable(parentTable) {}

VariableSymbol::Ptr SymbolTable::addVariableDeclaration(
        antlr4::ParserRuleContext *antlrContext,
        std::shared_ptr<caramel::ast::PrimaryType> const &primaryType,
        std::string const &name,
        const std::shared_ptr<Declaration> &declaration
) {
    logger.trace() << "SymbolTable::addVariableDeclaration()";

    if (isDefined(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDefinedError(
                name,
                symbol,
                antlrContext,
                symbol->getDefinition(),
                declaration
        );
    } else if (isDeclared(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDeclaredError(
                name,
                symbol,
                antlrContext,
                symbol->getDeclaration(),
                declaration
        );
    } else {
        VariableSymbol::Ptr variableSymbol = std::make_shared<VariableSymbol>(name, primaryType);
        mSymbolMap[name] = variableSymbol;
        variableSymbol->addDeclaration(declaration);
        return variableSymbol;
    }
}

VariableSymbol::Ptr SymbolTable::addVariableDefinition(
        antlr4::ParserRuleContext *antlrContext,
        std::shared_ptr<caramel::ast::PrimaryType> const &primaryType,
        std::string const &name,
        const std::shared_ptr<Definition> &definition
) {
    logger.trace() << "SymbolTable::addVariableDefinition()";

    if (isDefined(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDefinedError(
                name,
                symbol,
                antlrContext,
                symbol->getDefinition(),
                definition
        );
    } else if (isDeclared(name)) {
        Symbol::Ptr recordedSymbol = getSymbol(antlrContext, name);
        if (recordedSymbol->getSymbolType() != SymbolType::VariableSymbol) {
            throw DeclarationMismatchException(
                    buildMismatchSymbolTypeErrorMessage(name, SymbolType::VariableSymbol)
            );
        }
        if (!recordedSymbol->getType()->equals(primaryType)) {
            throw DeclarationMismatchException(
                    buildMismatchTypeErrorMessage(name, primaryType)
            );
        }
        recordedSymbol->addDefinition(definition);
        return std::dynamic_pointer_cast<VariableSymbol>(recordedSymbol);
    } else {
        VariableSymbol::Ptr variableSymbol = std::make_shared<VariableSymbol>(name, primaryType);
        mSymbolMap[name] = variableSymbol;
        variableSymbol->addDefinition(definition);
        return variableSymbol;
    }
}

Symbol::Ptr SymbolTable::addVariableUsage(
        antlr4::ParserRuleContext *antlrContext,
        std::string const &name,
        const std::shared_ptr<Statement> &statement
) {
    logger.trace() << "SymbolTable::addVariableUsage()";

    if (isDefined(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        symbol->addUsage(statement);
        return symbol;
    } else {
        SymbolTable::Ptr parent = getParentTable();
        if (parent) {
            return parent->addVariableUsage(antlrContext, name, statement);
        } else {
            throw UndefinedSymbolError(
                    name,
                    SymbolType::VariableSymbol,
                    antlrContext
            );
        }
    }
}

ArraySymbol::Ptr SymbolTable::addArrayDeclaration(
        antlr4::ParserRuleContext *antlrContext,
        std::shared_ptr<caramel::ast::PrimaryType> const &primaryType,
        std::string const &name,
        bool sized, size_t size,
        const std::shared_ptr<Declaration> &declaration
) {
    logger.trace() << "SymbolTable::addArrayDeclaration()";

    if (isDefined(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDefinedError(
                name,
                symbol,
                antlrContext,
                symbol->getDefinition(),
                declaration
        );
    } else if (isDeclared(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDeclaredError(
                name,
                symbol,
                antlrContext,
                symbol->getDeclaration(),
                declaration
        );
    } else {
        ArraySymbol::Ptr arraySymbol;
        if (sized) {
            arraySymbol = std::make_shared<ArraySymbol>(name, primaryType, size);
        } else {
            arraySymbol = std::make_shared<ArraySymbol>(name, primaryType);
        }
        mSymbolMap[name] = arraySymbol;
        arraySymbol->addDeclaration(declaration);
        return arraySymbol;
    }
}

ArraySymbol::Ptr SymbolTable::addArrayDefinition(
        antlr4::ParserRuleContext *antlrContext,
        std::shared_ptr<PrimaryType> const &primaryType,
        std::string const &name,
        std::vector<std::shared_ptr<Expression>> &&content,
        const std::shared_ptr<Definition> &definition
) {
    logger.trace() << "SymbolTable::addArrayDefinition()";

    if (isDefined(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDefinedError(
                name,
                symbol,
                antlrContext,
                symbol->getDefinition(),
                definition
        );
    } else if (isDeclared(name)) {
        Symbol::Ptr recordedSymbol = getSymbol(antlrContext, name);
        if (recordedSymbol->getSymbolType() != SymbolType::ArraySymbol) {
            throw DeclarationMismatchException(
                    buildMismatchSymbolTypeErrorMessage(name, SymbolType::ArraySymbol)
            );
        }
        if (!recordedSymbol->getType()->equals(primaryType)) {
            throw DeclarationMismatchException(
                    buildMismatchTypeErrorMessage(name, primaryType)
            );
        }
        recordedSymbol->addDefinition(definition);
        return castTo<ArraySymbol::Ptr>(recordedSymbol);
    } else {
        ArraySymbol::Ptr arraySymbol = std::make_shared<ArraySymbol>(name, primaryType, std::move(content));
        mSymbolMap[name] = arraySymbol;
        arraySymbol->addDefinition(definition);
        return arraySymbol;
    }
}

Symbol::Ptr SymbolTable::addArrayAccess(
        antlr4::ParserRuleContext *antlrContext,
        std::string const &name,
        const std::shared_ptr<Statement> &statement
) {
    logger.trace() << "SymbolTable::addArrayAccess()";

    if (isDefined(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        symbol->addUsage(statement);
        return symbol;
    } else {
        SymbolTable::Ptr parent = getParentTable();
        if (parent) {
            return parent->addArrayAccess(antlrContext, name, statement);
        } else {
            throw UndefinedSymbolError(
                    name, SymbolType::ArraySymbol,
                    antlrContext
            );
        }
    }
}

FunctionSymbol::Ptr SymbolTable::addFunctionDeclaration(
        antlr4::ParserRuleContext *antlrContext,
        std::shared_ptr<caramel::ast::PrimaryType> const &returnType,
        std::string const &name,
        std::vector<std::shared_ptr<caramel::ast::Symbol>> namedParameters,
        const std::shared_ptr<Declaration> &declaration
) {
    logger.trace() << "SymbolTable::addFunctionDeclaration()";

    if (isNotDeclared(name)) {
        FunctionSymbol::Ptr functionSymbol = std::make_shared<FunctionSymbol>(name, returnType);
        mSymbolMap[name] = functionSymbol;
        functionSymbol->addDeclaration(declaration);
        functionSymbol->setParameters(namedParameters);
        return functionSymbol;
    } else {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDeclaredError(
                name,
                symbol,
                antlrContext,
                symbol->getDeclaration(),
                declaration
        );
    }
}

FunctionSymbol::Ptr SymbolTable::addFunctionDefinition(
        antlr4::ParserRuleContext *antlrContext,
        std::shared_ptr<caramel::ast::PrimaryType> const &returnType,
        std::string const &name,
        std::vector<std::shared_ptr<caramel::ast::Symbol>> namedParameters,
        const std::shared_ptr<Definition> &definition
) {
    logger.trace() << "SymbolTable::addFunctionDefinition()";

    if (isDefined(name)) {
        auto const &symbol = getSymbol(antlrContext, name);
        throw SymbolAlreadyDefinedError(
                name,
                symbol,
                antlrContext,
                symbol->getDefinition(),
                definition
        );
    } else if (!isDeclared(name)) {
        FunctionSymbol::Ptr functionSymbol = std::make_shared<FunctionSymbol>(name, returnType);
        functionSymbol->setParameters(namedParameters);
        mSymbolMap[name] = functionSymbol;
    } else {
        FunctionSymbol::Ptr declaredSymbol = std::dynamic_pointer_cast<FunctionSymbol>(getSymbol(antlrContext, name));

        std::vector<std::shared_ptr<caramel::ast::Symbol>> declaredParameters = declaredSymbol->getParameters();
        if (declaredParameters.size() == namedParameters.size()) {
            for (size_t i = 0; i < declaredSymbol->getParameters().size(); i++) {
                std::string declaredParameterName = declaredParameters.at(i)->getName();
                std::string const declaredParameterTypeIdentifier =
                        declaredParameters.at(i)->getType()->getIdentifier();

                std::string parameterName = namedParameters.at(i)->getName();
                std::string const parameterTypeIdentifier = namedParameters.at(i)->getType()->getIdentifier();

                if (declaredParameterName != parameterName) {
                    throw FunctionDefinitionParameterNameMismatchError(
                            buildFunctionDefinitionParameterNameMismatchErrorMessage(
                                    name, declaredParameterName, parameterName),
                            antlrContext,
                            declaredParameterName,
                            parameterName
                    );
                }
                if (declaredParameterTypeIdentifier != parameterTypeIdentifier) {
                    throw FunctionDefinitionParameterTypeMismatchError(
                            buildFunctionDefinitionParameterTypeMismatchErrorMessage(
                                    declaredParameters.at(i)->getType(), namedParameters.at(i)->getType()),
                            antlrContext,
                            declaredParameters.at(i)->getType(),
                            namedParameters.at(i)->getType()
                    );
                }
            }
        } else {
            throw FunctionDefinitionNumberOfParametersMismatchError(
                    buildFunctionDefinitionNumberOfParametersMismatchErrorMessage(name, declaredParameters.size(),
                                                                                  namedParameters.size()),
                    antlrContext,
                    declaredParameters.size(),
                    namedParameters.size()
            );
        }
    }
    FunctionSymbol::Ptr symbol = std::dynamic_pointer_cast<FunctionSymbol>(getSymbol(antlrContext, name));
    symbol->addDefinition(definition);
    return symbol;
}

FunctionSymbol::Ptr SymbolTable::addFunctionCall(
        antlr4::ParserRuleContext *antlrContext,
        std::string const &name,
        std::vector<std::shared_ptr<caramel::ast::Symbol>> const &arguments,
        const std::shared_ptr<Statement> &statement
) {
    logger.trace() << "SymbolTable::addFunctionCall()";

    if (isDeclared(name)) {
        Symbol::Ptr symbol = getSymbol(antlrContext, name);
        if (symbol->getSymbolType() != SymbolType::FunctionSymbol) {
            throw DeclarationMismatchException(
                    buildMismatchSymbolTypeErrorMessage(name, SymbolType::FunctionSymbol)
            );
        }
        auto functionSymbol = castTo<FunctionSymbol::Ptr>(symbol);

        // Check if the arguments match with the function parameters types
        std::vector<std::shared_ptr<Symbol>> const &parameters = functionSymbol->getParameters();
        if (arguments.size() != parameters.size()) {
            throw FunctionCallArgumentsNumberMismatchException(
                    "The function " + name + " takes " + std::to_string(parameters.size()) + " arguments, "
                    + "but only " + std::to_string(arguments.size()) + " were given."
            );
        }
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (!arguments[i]->getType()->equals(parameters[i]->getType())) {
                std::stringstream errorMessage;
                errorMessage
                        << "The function " << name << " " << i << " parameter is of type " << parameters[i]->getType()
                        << ", but got a " << arguments[i]->getType();
                throw FunctionCallArgumentsTypeMismatchException(errorMessage.str());
            }
        }

        functionSymbol->addUsage(statement);
        return functionSymbol;
    } else {
        throw UndefinedSymbolError(
                name,
                SymbolType::FunctionSymbol,
                antlrContext
        );
    }
}

void SymbolTable::addPrimaryType(std::shared_ptr<caramel::ast::PrimaryType> const &primaryType,
                                 std::string const &name) {
    logger.trace() << "SymbolTable::addPrimaryType()";

    // Not declared and not defined
    if (isNotDeclared(name) && isNotDefined(name)) {
        mSymbolMap[name] = std::make_shared<TypeSymbol>(name, primaryType);
        mSymbolMap.at(name)->addDeclaration(nullptr);
        mSymbolMap.at(name)->addDefinition(nullptr);
    } else {
        logger.fatal() << "Can't add " << name << " as a primary type, because a symbol named " << name
                << " already exists.";
        exit(1);
    }
}

TypeSymbol::Ptr SymbolTable::addType(
        antlr4::ParserRuleContext *antlrContext,
        std::shared_ptr<TypeDefinition> definition
) {
    logger.trace() << "SymbolTable::addType()";

    std::string typeAlias = definition->getTypeSymbol().lock()->getName();
    PrimaryType::Ptr primaryType = definition->getTypeSymbol().lock()->getType();

    // Not declared and not defined
    if (isNotDeclared(typeAlias)) {
        TypeSymbol::Ptr typeSymbol = std::make_shared<TypeSymbol>(typeAlias, primaryType);
        mSymbolMap[typeAlias] = typeSymbol;
        typeSymbol->addDefinition(definition);
        return typeSymbol;
    } else {
        throw caramel::exceptions::SymbolAlreadyDeclaredError(
                "Cannot execute typedef",
                mSymbolMap[typeAlias],
                antlrContext,
                mSymbolMap[typeAlias]->getDeclaration(),
                std::dynamic_pointer_cast<Declaration>(definition));
    }
}

bool SymbolTable::hasSymbol(std::string const &name) {
    return thisHasSymbol(name) || parentHasSymbol(name);
}

bool SymbolTable::thisHasSymbol(std::string const &name) {
    return mSymbolMap.find(name) != mSymbolMap.end();
}

bool SymbolTable::parentHasSymbol(std::string const &name) {
    return getParentTable() && getParentTable()->hasSymbol(name);
}

std::shared_ptr<Symbol>
SymbolTable::getSymbol(antlr4::ParserRuleContext *antlrContext, std::string const &name) {
    logger.trace() << "SymbolTable::getSymbol()";

    if (thisHasSymbol(name)) {
        return mSymbolMap.at(name);
    } else {
        SymbolTable::Ptr parent = getParentTable();
        if (parent) {
            return parent->getSymbol(antlrContext, name);
        } else {
            throw UndefinedSymbolError(name, antlrContext);
        }
    }
}

std::shared_ptr<SymbolTable> SymbolTable::getParentTable() {
    return mParentTable;
}

void SymbolTable::acceptAstDotVisit() {
    addNode(thisId(), "SymbolTable: " + std::to_string(mSymbolMap.size()) + " symbols", "cylinder", "darkorange");
    visitChildrenAstDot();
}

void SymbolTable::visitChildrenAstDot() {
    for (auto const& symbol : mSymbolMap) {
        addEdge(thisId(), symbol.second->thisId(), symbol.first);
        symbol.second->acceptAstDotVisit();
    }
}

bool SymbolTable::isDeclared(const std::string &name) {
    return (thisHasSymbol(name) && mSymbolMap.at(name)->isDeclared()) ||
           (getParentTable() && getParentTable()->isDeclared(name));
}

bool SymbolTable::isDefined(const std::string &name) {
    return (thisHasSymbol(name) && mSymbolMap.at(name)->isDefined()) ||
           (getParentTable() && getParentTable()->isDefined(name));
}

std::string SymbolTable::buildMismatchSymbolTypeErrorMessage(std::string const &variableName,
                                                             SymbolType requiredSymbolType) {
    std::stringstream res;
    res << "Cannot defined a ";
    switch (requiredSymbolType) {
        case SymbolType::VariableSymbol:
            res << "variable";
            break;
        case SymbolType::ArraySymbol:
            res << "array";
            break;
        case SymbolType::FunctionSymbol:
            res << "function";
            break;
        case SymbolType::TypeSymbol:
            res << "type";
            break;
    }
    res << " which has a previous occurrence as a " << mSymbolMap[variableName]->getSymbolType();
    return res.str();
}

std::string
SymbolTable::buildMismatchTypeErrorMessage(std::string const &variableName, PrimaryType::Ptr const &requiredType) {
    std::stringstream res;
    res << "Mismatch type for " << variableName << " between the type definition " << requiredType->getIdentifier()
        << " and declaration type " << mSymbolMap[variableName]->getType()->getIdentifier() << '.';
    return res.str();
}

std::string
SymbolTable::buildFunctionDefinitionNumberOfParametersMismatchErrorMessage(const std::string &name,
                                                                           unsigned long declaredSize,
                                                                           unsigned long definedSize) {
    std::stringstream res;
    res << "The function: "
        << name
        << " was previously declared with "
        << declaredSize
        << " parameter(s).\n"
        << "Actual definition has "
        << definedSize
        << " parameter(s).";
    return res.str();
}

std::string SymbolTable::buildFunctionDefinitionParameterNameMismatchErrorMessage(const std::string &name,
                                                                                  std::string declaredName,
                                                                                  std::string definedName) {
    std::stringstream res;
    res << name << "'s parameter name " << definedName
        << " mismatches with the previously declared parameter " << declaredName << ".";
    return res.str();
}

std::string SymbolTable::buildFunctionDefinitionParameterTypeMismatchErrorMessage(
        std::shared_ptr<PrimaryType> declaredType,
        std::shared_ptr<PrimaryType> declaredName) {
    std::stringstream res;
    res << "The function: "
        << declaredType->getIdentifier()
        << " was previously declared with a parameter of type "
        << declaredName
        << " .\n"
        << "Actual parameter type is "
        << declaredName->getIdentifier()
        << ".";
    return res.str();
}

} // namespace caramel::ast
