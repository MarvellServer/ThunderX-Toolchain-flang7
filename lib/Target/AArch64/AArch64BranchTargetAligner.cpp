//==--- AArch64BranchTargetAligner.cpp - Branch target alignments on T99 ---==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// \file Determine special treatment for branch targets on ThunderX2T99.
//===----------------------------------------------------------------------===//

#include "AArch64.h"
#include "AArch64RegisterInfo.h"
#include "AArch64Subtarget.h"
#include "AArch64BranchTargetAligner.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "MCTargetDesc/AArch64FixupKinds.h"
#include "AArch64GenInstrInfo.inc"
using namespace llvm;

namespace llvm {

#ifndef _NDEBUG
static const char* getOpcodeName(unsigned Opcode) {
  const char *Name = "<unknown>";

  switch (Opcode) {
  default:
    break;
  case AArch64::B:
    Name = "AArch64::B";
    break;
  case AArch64::Bcc:
    Name = "AArch64::Bcc";
    break;
  case AArch64::BR:
    Name = "AArch64::BR";
    break;
  case AArch64::BRAA:
    Name = "AArch64::BRAA";
    break;
  case AArch64::BRAAZ:
    Name = "AArch64::BRAAZ";
    break;
  case AArch64::BRAB:
    Name = "AArch64::BRAB";
    break;
  case AArch64::BRABZ:
    Name = "AArch64::BRABZ";
    break;
  case AArch64::BL:
    Name = "AArch64::BL";
    break;
  case AArch64::BLR:
    Name = "AArch64::BLR";
    break;
  case AArch64::BLRAA:
    Name = "AArch64::BLRAA";
    break;
  case AArch64::BLRAAZ:
    Name = "AArch64::BLRAAZ";
    break;
  case AArch64::BLRAB:
    Name = "AArch64::BLRAB";
    break;
  case AArch64::BLRABZ:
    Name = "AArch64::BLRABZ";
    break;
  case AArch64::CBZW:
    Name = "AArch64::CBZW";
    break;
  case AArch64::CBZX:
    Name = "AArch64::CBZX";
    break;
  case AArch64::CBNZW:
    Name = "AArch64::CBNZW";
    break;
  case AArch64::CBNZX:
    Name = "AArch64::CBNZX";
    break;
  case AArch64::CCMPWr:
    Name = "AArch64::CCMPWr";
    break;
  case AArch64::CCMPXr:
    Name = "AArch64::CCMPXr";
    break;
  case AArch64::G_FCMP:
    Name = "AArch64::G_FCMP";
    break;
  case AArch64::G_ICMP:
    Name = "AArch64::G_ICMP";
    break;
  case AArch64::TBZW:
    Name = "AArch64::TBZW";
    break;
  case AArch64::TBZX:
    Name = "AArch64::TBZX";
    break;
  case AArch64::TBNZW:
    Name = "AArch64::TBNZW";
    break;
  case AArch64::TBNZX:
    Name = "AArch64::TBNZX";
    break;
  case AArch64::RET:
    Name = "AArch64::RET";
    break;
  case AArch64::RETAA:
    Name = "AArch64::RETAA";
    break;
  case AArch64::RETAB:
    Name = "AArch64::RETAB";
    break;
  case AArch64::SVC:
    Name = "AArch64::SVC";
    break;
  }

  return Name;
}
#endif

bool AArch64BranchTargetAligner::needsSpecialAlignment(StringRef CPU,
                                                       unsigned Opcode) {
  if (CPU != "thunderx2t99")
    return false;

  switch (Opcode) {
  default:
    return false;
    break;
  case AArch64::B:
  case AArch64::Bcc:
  case AArch64::BR:
  case AArch64::BRAA:
  case AArch64::BRAAZ:
  case AArch64::BRAB:
  case AArch64::BRABZ:
  case AArch64::BL:
  case AArch64::BLR:
  case AArch64::BLRAA:
  case AArch64::BLRAAZ:
  case AArch64::BLRAB:
  case AArch64::BLRABZ:
  case AArch64::CBZW:
  case AArch64::CBZX:
  case AArch64::CBNZW:
  case AArch64::CBNZX:
  case AArch64::CCMPWr:
  case AArch64::CCMPXr:
  case AArch64::G_FCMP:
  case AArch64::G_ICMP:
  case AArch64::TBZW:
  case AArch64::TBZX:
  case AArch64::TBNZW:
  case AArch64::TBNZX:
  case AArch64::RET:
  case AArch64::RETAA:
  case AArch64::RETAB:
  case AArch64::SVC:
    return true;
    break;
  }

  return false;
}

static bool isFixup(unsigned Opcode) {
  switch (Opcode) {
  case AArch64::fixup_aarch64_add_imm12:
  case AArch64::fixup_aarch64_ldr_pcrel_imm19:
  case AArch64::fixup_aarch64_ldst_imm12_scale1:
  case AArch64::fixup_aarch64_ldst_imm12_scale16:
  case AArch64::fixup_aarch64_ldst_imm12_scale2:
  case AArch64::fixup_aarch64_ldst_imm12_scale4:
  case AArch64::fixup_aarch64_ldst_imm12_scale8:
  case AArch64::fixup_aarch64_movw:
  case AArch64::fixup_aarch64_pcrel_adr_imm21:
  case AArch64::fixup_aarch64_pcrel_adrp_imm21:
  case AArch64::fixup_aarch64_pcrel_branch14:
  case AArch64::fixup_aarch64_pcrel_branch19:
  case AArch64::fixup_aarch64_pcrel_branch26:
  case AArch64::fixup_aarch64_pcrel_call26:
  case AArch64::fixup_aarch64_tlsdesc_call:
    return true;
    break;
  default:
    return false;
    break;
  }
}

unsigned AArch64BranchTargetAligner::getLoopIndexForNoOps(const MCInst &Inst) {
  unsigned Opcode = Inst.getOpcode();
  unsigned NumOperands = Inst.getNumOperands();

  if (isFixup(Opcode) || Opcode == AArch64::NOP || Opcode == AArch64::ADRP)
    return 0;

  if (NumOperands == 0)
    return 15;

  bool CanAlign = false;
  unsigned R = 15;

  for (unsigned I = 0; I < NumOperands; ++I) {
    if (Inst.getOperand(I).isExpr()) {
      const MCExpr *MCE = Inst.getOperand(I).getExpr();
      if (MCE && (MCE->getKind() == MCExpr::Binary ||
                  MCE->getKind() == MCExpr::Constant ||
                  MCE->getKind() == MCExpr::Unary)) {
        CanAlign = true;
      } else {
        CanAlign = false;
        break;
      }
    } else if (Inst.getOperand(I).isInst()) {
      const MCInst *MCI = Inst.getOperand(I).getInst();
      if (MCI && isFixup(MCI->getOpcode())) {
        CanAlign = false;
        break;
      } else {
        CanAlign = true;
      }
    } else if (Inst.getOperand(I).isImm()) {
      switch (Opcode) {
      case AArch64::B:
      case AArch64::Bcc:
      case AArch64::BL:
      case AArch64::BLR:
      case AArch64::BR:
      case AArch64::BLRAA:
      case AArch64::BLRAAZ:
      case AArch64::BLRAB:
      case AArch64::BLRABZ:
        CanAlign = false;
        break;
      default:
        CanAlign = true;
        break;
      }

      if (!CanAlign)
        break;
    } else if (Inst.getOperand(I).isFPImm()) {
      CanAlign = false;
      break;
    } else if (Inst.getOperand(I).isReg()) {
      switch (Opcode) {
      case AArch64::B:
      case AArch64::Bcc:
      case AArch64::BL:
      case AArch64::BLR:
      case AArch64::BR:
      case AArch64::BLRAA:
      case AArch64::BLRAAZ:
      case AArch64::BLRAB:
      case AArch64::BLRABZ:
        CanAlign = false;
        break;
      default:
        CanAlign = true;
        break;
      }

      if (!CanAlign)
        break;
    } else if (!Inst.getOperand(I).isValid()) {
      CanAlign = false;
      break;
    }
  }

  return CanAlign ? R : 0;
}

MCInst AArch64BranchTargetAligner::createNopInstruction() {
  MCInst Inst;
  Inst.setOpcode(AArch64::NOP);
  return Inst;
}

} // namespace llvm

