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

#include "UnaryExpression.h"


namespace caramel::ast {

UnaryExpression::UnaryExpression(
        std::shared_ptr<caramel::ast::Expression> const &innerExpression,
        std::shared_ptr<caramel::ast::UnaryOperator> const &unaryOperator,
        antlr4::Token *startToken
) : Expression(startToken, unaryOperator->getExpressionType()),
    mInnerExpression{innerExpression},
    mUnaryOperator{unaryOperator} {}

std::shared_ptr<caramel::ir::IR>
UnaryExpression::getIR(
        std::shared_ptr<ir::BasicBlock> &currentBasicBlock
) {
    return mUnaryOperator->buildIR(currentBasicBlock, mInnerExpression);
}

void UnaryExpression::acceptAstDotVisit() {
    addNode(thisId(), "UnaryExpression: " + mUnaryOperator->getToken());
    visitChildrenAstDot();
}

void UnaryExpression::visitChildrenAstDot() {
    addEdge(thisId(), mInnerExpression->thisId());
    mInnerExpression->acceptAstDotVisit();
}

PrimaryType::Ptr UnaryExpression::getPrimaryType() const {
    return mInnerExpression->getPrimaryType();
}

bool UnaryExpression::shouldReturnAnIR() const {
    return true;
}

} // namespace caramel::ast




