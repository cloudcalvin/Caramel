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

#pragma once
#include "../../ir/BasicBlock.h"

#define GET_MAX_TYPE(leftExpr, rightExpr) PrimaryType::max(leftExpr->getPrimaryType(), rightExpr->getPrimaryType());

#define SAFE_ADD_INSTRUCTION(expr, block)                                                           \
([&] () { auto __tmp_instr__ = expr->getIR(block);                                                  \
          return block->addInstruction(__tmp_instr__);})()

#define MOVE_TO(var, to, max)                                                                       \
currentBasicBlock->addInstruction(std::make_shared<ir::CopyInstruction>(                            \
    currentBasicBlock,                                                                              \
    max,                                                                                            \
    to,                                                                                             \
    var                                                                                             \
))
