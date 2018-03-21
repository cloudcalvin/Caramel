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

#include "Declaration.h"
#include "TypeSymbol.h"
#include "VariableSymbol.h"

#include <memory>


namespace Caramel::DataStructure {

class VariableDeclaration : public Declaration {
public:
    static Ptr Create(VariableSymbol::WeakPtr symbol, antlr4::Token *startToken) {
        return Ptr(new VariableDeclaration(std::forward<VariableSymbol::WeakPtr>(symbol), startToken));
    }

private:
    VariableDeclaration(VariableSymbol::WeakPtr symbol, antlr4::Token *startToken);

private:
    VariableSymbol::WeakPtr mSymbol;
};

} // namespace Caramel::DataStructure
