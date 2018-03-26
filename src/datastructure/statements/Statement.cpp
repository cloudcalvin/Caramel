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

#include "Statement.h"


caramel::dataStructure::statements::Statement::Statement(antlr4::Token *startToken, StatementType type)
        : mLine{startToken->getLine()},
          mColumn{startToken->getCharPositionInLine()},
          mLength{startToken->getText().length()},
          mType{type} {
}

size_t caramel::dataStructure::statements::Statement::getLine() const {
    return mLine;
}

size_t caramel::dataStructure::statements::Statement::getColumn() const {
    return mColumn;
}

size_t caramel::dataStructure::statements::Statement::getLength() const {
    return mLength;
}

caramel::dataStructure::statements::StatementType caramel::dataStructure::statements::Statement::getType() const {
    return mType;
}

std::ostream &caramel::dataStructure::statements::operator<<(std::ostream &os, const StatementType &type) {
    switch (type) {
        case StatementType::Unknown: return os << "Unknown";
        case StatementType::Declaration: return os << "Declaration";
        case StatementType::VariableDeclaration: return os << "VariableDeclaration";
        case StatementType::FunctionDeclaration: return os << "FunctionDeclaration";
        case StatementType::ArrayDeclaration: return os << "ArrayDeclaration";
        case StatementType::Definition: return os << "Definition";
    }
}
