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

#pragma once

#include "Common.h"
#include "../../exceptions/NotImplementedException.h"
#include "Symbol.h"
#include <CaramelBaseVisitor.h>
#include <string>
#include <map>

#include <memory>


namespace Caramel::DataStructure {

class SymbolTable {
public:
    using Ptr = std::shared_ptr<SymbolTable>;

    static Ptr Create() {
        return Ptr(new SymbolTable);
    }

    static Ptr Create(SymbolTable::Ptr const & parentTable) {
        return Ptr(new SymbolTable(parentTable));
    }

    void addVariableDeclaration(const PrimaryType::Ptr &primaryType, const std::string &name, const Declaration::Ptr &declaration);
    void addVariableDefinition(const PrimaryType::Ptr &primaryType, const std::string &name, const Definition::Ptr &definition);
    void addVariableUsage(const std::string &name, const Expression::Ptr &expression);

    void addFunctionDeclaration(const PrimaryType::Ptr &returnType, const std::string &name, std::vector<Symbol::Ptr> namedParameters, const Declaration::Ptr &declaration);
    void addFunctionDefinition(const PrimaryType::Ptr &returnType, const std::string &name, std::vector<Symbol::Ptr> namedParameters, const Definition::Ptr &definition);
    void addFunctionCall(const std::string &name, const std::vector<Symbol::Ptr> &valueParameters, const Expression::Ptr &expression);

    void addType(const PrimaryType::Ptr &primaryType, const std::string &name, const Definition::Ptr &definition);

    bool hasSymbol(std::string const& name);
    Symbol::Ptr getSymbol(std::string const& name);

    SymbolTable::Ptr getParentTable();

private:
    std::map<std::string, Symbol::Ptr> mSymbolMap;

private:
    SymbolTable() = default;
    explicit SymbolTable (SymbolTable::Ptr const &parentTable);

    SymbolTable::Ptr mParentTable;

    bool isDeclared(const std::string &name);
    bool isDefined(const std::string &name);

    inline bool isNotDeclared(const std::string &name) { return !isDeclared(name); }
    inline bool isNotDefined(const std::string &name) { return !isDefined(name); }

    std::string buildAlreadyDefinedErrorMessage(std::string const &variableName);
    std::string buildAlreadyDeclaredErrorMessage(std::string const &variableName);
    std::string buildMismatchSymbolTypeErrorMessage(std::string const &variableName, SymbolType requiredSymbolType);
    std::string buildMismatchTypeErrorMessage(std::string const &variableName, PrimaryType::Ptr const &requiredType);
    std::string buildUndefinedSymbolErrorMessage(std::string const &name, SymbolType symbolType);

};

} // namespace Caramel::DataStructure