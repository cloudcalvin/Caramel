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

#include "../IR.h"
#include "../IRVisitor.h"
#include "../BasicBlock.h"

#include <memory>
#include <ostream>


namespace caramel::ir::Pdf {

class PdfIRVisitor : public IRVisitor {
public:
    using Ptr = std::shared_ptr<PdfIRVisitor>;
    using WeakPtr = std::shared_ptr<PdfIRVisitor>;

public:
    explicit PdfIRVisitor() = default;
    virtual ~PdfIRVisitor() = default;

    void visitCopy(caramel::ir::CopyInstruction *instruction, std::ostream &os) override;

    void visitCopyAddr(CopyAddrInstruction *instruction, std::ostream &os) override;

    void visitArrayAccess(ArrayAccessInstruction *instruction, std::ostream &os) override;

    void visitEmpty(caramel::ir::EmptyInstruction *instruction, std::ostream &os) override;

    void visitProlog(caramel::ir::PrologInstruction *instruction, std::ostream &os) override;

    void visitEpilog(caramel::ir::EpilogInstruction *instruction, std::ostream &os) override;

    void visitAddition(caramel::ir::AdditionInstruction *instruction, std::ostream &os) override;

    void visitLdConst(caramel::ir::LDConstInstruction *instruction, std::ostream &os) override;

    void visitNope(caramel::ir::NopInstruction *instruction, std::ostream &os) override;

    void visitFunctionCall(caramel::ir::FunctionCallInstruction *instruction, std::ostream &os) override;

    void visitBreak(caramel::ir::BreakInstruction *instruction, std::ostream &os) override;

    void visitReturn(caramel::ir::ReturnInstruction *instruction, std::ostream &os) override;

    void visitCallParameter(CallParameterInstruction *instruction, std::ostream &os) override;

    void visitSubtraction(SubtractionInstruction *instruction, std::ostream &os) override;

    void visitPush(PushInstruction *instruction, std::ostream &os) override;

    void visitPop(PopInstruction *instruction, std::ostream &os) override;

    void visitMultiplication(MultiplicationInstruction *instruction, std::ostream &os) override;

    void visitMod(ModInstruction *instruction, std::ostream &os) override;

    void visitDivision(DivInstruction *instruction, std::ostream &os) override;

    void visitFlagToReg(FlagToRegInstruction *instruction, std::ostream &os) override;

    void visitLeftShift(LeftShiftInstruction *instruction, std::ostream &os) override;

    void visitRightShift(RightShiftInstruction *instruction, std::ostream &os) override;

    void visitBitwiseAnd(BitwiseAndInstruction *instruction, std::ostream &os) override;

    void visitBitwiseOr(BitwiseOrInstruction *instruction, std::ostream &os) override;

    void visitBitwiseXor(BitwiseXorInstruction *instruction, std::ostream &os) override;
};

} // namespace caramel::ir::Pdf
