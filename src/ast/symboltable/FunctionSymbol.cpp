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

#include "FunctionSymbol.h"


namespace caramel::ast {

FunctionSymbol::FunctionSymbol(
        const std::string &mName,
        const std::shared_ptr<caramel::ast::PrimaryType> &mType
) : Symbol(mName, mType, SymbolType::FunctionSymbol),
    mParameters() {
}

std::vector<std::shared_ptr<Symbol>>
FunctionSymbol::getParameters() const {
    return mParameters;
}

void FunctionSymbol::setParameters(
        const std::vector<std::shared_ptr<caramel::ast::Symbol>> &namedParameters
) {
    mParameters.clear();
    for (const auto &parameter : namedParameters) {
        mParameters.push_back(parameter);
    }
}

void FunctionSymbol::acceptAstDotVisit() {
    addNode(thisId(), "FunctionSymbol: " + getName() + " as " + getType()->getIdentifier(), "note", "orange");
    visitChildrenAstDot();
}

void FunctionSymbol::visitChildrenAstDot() {
    size_t i = 0;
    for (auto const& parameter : mParameters) {
        addEdge(thisId(), parameter->thisId(), std::to_string(i++));
        parameter->acceptAstDotVisit();
    }
}

} // namespace caramel::ast
