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

#include "PreNegOperator.h"
#include "../../../utils/Common.h"
#include "../../../ir/BasicBlock.h"
#include "../../../ir/helpers/IROperatorHelper.h"
#include "../../../ir/instructions/CopyInstruction.h"
#include "../../../ir/instructions/SubtractionInstruction.h"
#include "../../statements/expressions/atomicexpression/LValue.h"


using namespace caramel::utils;

std::shared_ptr<caramel::ir::IR> caramel::ast::PreNegOperator::buildIR(
        std::shared_ptr<ir::BasicBlock> &currentBasicBlock,
        std::shared_ptr<caramel::ast::Expression> const &expression
) {
    std::string before = SAFE_ADD_INSTRUCTION(expression, currentBasicBlock);
    return castTo<ir::IR::Ptr>(std::make_shared<ir::SubtractionInstruction>(
            Statement::createVarName(),
            currentBasicBlock,
            expression->getPrimaryType(),
            "0",
            before
    ));
}

caramel::ast::StatementType caramel::ast::PreNegOperator::getExpressionType() const {
    return StatementType::NegativeExpression;
}

std::string caramel::ast::PreNegOperator::getToken() const {
    return SYMBOL;
}
