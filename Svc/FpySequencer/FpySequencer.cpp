// ======================================================================
// \title  FpySequencer.cpp
// \author zimri.leisher
// \brief  cpp file for FpySequencer component implementation class
// ======================================================================

#include <Svc/FpySequencer/FpySequenceTypes.hpp>
#include <Svc/FpySequencer/FpySequencer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

FpySequencer ::FpySequencer(const char* const compName)
    : FpySequencerComponentBase(compName) {}

FpySequencer ::~FpySequencer() {}

void FpySequencer::allocateBuffer(NATIVE_INT_TYPE identifier,
                                  Fw::MemAllocator& allocator,
                                  NATIVE_UINT_TYPE bytes) {
  // has to be at least as big as a header
  FW_ASSERT(bytes >= Fpy::Header::SERIALIZED_SIZE);
  bool recoverable;
  this->m_allocatorId = identifier;
  this->m_sequenceBuffer.setExtBuffer(
      static_cast<U8*>(allocator.allocate(
          static_cast<NATIVE_UINT_TYPE>(identifier), bytes, recoverable)),
      bytes);
}

void FpySequencer::deallocateBuffer(Fw::MemAllocator& allocator) {
  allocator.deallocate(static_cast<NATIVE_UINT_TYPE>(this->m_allocatorId),
                       this->m_sequenceBuffer.getBuffAddr());
  this->m_sequenceBuffer.clear();
}
//! Handler for command RUN
//!
//! Loads, validates and runs a sequence
void FpySequencer::RUN_cmdHandler(
    FwOpcodeType opCode,               //!< The opcode
    U32 cmdSeq,                        //!< The command sequence number
    const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
    Fw::Wait block  //!< Return command status when complete or not
) {
  this->sequencer_sendSignal_cmd_RUN(FpySequencer_SequenceExecutionArgs(fileName, block));
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for command VALIDATE
//!
//! Loads and validates a sequence
void FpySequencer::VALIDATE_cmdHandler(
    FwOpcodeType opCode,              //!< The opcode
    U32 cmdSeq,                       //!< The command sequence number
    const Fw::CmdStringArg& fileName  //!< The name of the sequence file
) {
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for command RUN_VALIDATED
//!
//! Runs a previously validated sequence
void FpySequencer::RUN_VALIDATED_cmdHandler(
    FwOpcodeType opCode,  //!< The opcode
    U32 cmdSeq            //!< The command sequence number
) {
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
