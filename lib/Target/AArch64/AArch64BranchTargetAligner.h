//==---- AArch64BranchTargetAligner.h - Branch target alignments on T99 ----==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// \file Determine special treatment for branch targets on ThunderX2T99.
//===----------------------------------------------------------------------===//
//
#include "AArch64.h"
#include "AArch64Subtarget.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
class AArch64BranchTargetAligner
{
public:
  AArch64BranchTargetAligner() = default;
  ~AArch64BranchTargetAligner() = default;
  bool needsSpecialAlignment(StringRef CPU, unsigned Opcode);
  unsigned getBranchTargetAlignment(const MCInst &Inst);
  MCInst createNopInstruction();
};
}

