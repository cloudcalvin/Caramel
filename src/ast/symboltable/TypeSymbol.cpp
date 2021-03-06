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

#include "TypeSymbol.h"
#include "Common.h"


namespace caramel::ast {

TypeSymbol::TypeSymbol(const std::string &mName,
                       std::shared_ptr<PrimaryType> mType)
        : Symbol(mName, std::move(mType), SymbolType::TypeSymbol) {
}

bool TypeSymbol::isDeclared() {
    return mDefined;
}

bool TypeSymbol::isDefined() {
    return mDefined;
}

void TypeSymbol::onDeclaration(caramel_unused const std::shared_ptr<caramel::ast::Declaration> &declaration) {
    mDefined = true;
}

void TypeSymbol::onDefinition(caramel_unused const std::shared_ptr<caramel::ast::Definition> &definition) {
    mDefined = true;
}

void TypeSymbol::acceptAstDotVisit() {
    addNode(thisId(), "TypeSymbol: " + getName() + " as " + getType()->getIdentifier(), "note", "orange");
}

} // namespace caramel::dataStructure::symbolTable