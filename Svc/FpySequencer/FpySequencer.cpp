// ======================================================================
// \title  FpySequencer.cpp
// \author zimri.leisher
// \brief  cpp file for FpySequencer component implementation class
// ======================================================================

#include <Svc/FpySequencer/FpySequencer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

FpySequencer ::FpySequencer(const char* const compName) : FpySequencerComponentBase(compName) {}

FpySequencer ::~FpySequencer() {}

void FpySequencer::allocateBuffer(NATIVE_INT_TYPE identifier, Fw::MemAllocator& allocator, NATIVE_UINT_TYPE bytes) {
    // if this assertion fails, you aren't allocating enough bytes for the FpySequencer
    // this is because you must have a buffer big enough to fit the header of a sequence
    FW_ASSERT(bytes >= Fpy::Header::SERIALIZED_SIZE);
    bool recoverable = false;
    this->m_allocatorId = identifier;
    U8* allocatedMemory =
        static_cast<U8*>(allocator.allocate(static_cast<NATIVE_UINT_TYPE>(identifier), bytes, recoverable));
    // if this fails, unable to allocate the memory
    FW_ASSERT(bytes > 0);
    this->m_sequenceBuffer.setExtBuffer(allocatedMemory, bytes);
}

void FpySequencer::deallocateBuffer(Fw::MemAllocator& allocator) {
    allocator.deallocate(static_cast<NATIVE_UINT_TYPE>(this->m_allocatorId), this->m_sequenceBuffer.getBuffAddr());
    this->m_sequenceBuffer.clear();
}

//! Handler for command RUN
//!
//! Loads, validates and runs a sequence
void FpySequencer::RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                                  U32 cmdSeq,                        //!< The command sequence number
                                  const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                                  FpySequencer_BlockState block      //!< Return command status when complete or not
) {
    this->sequencer_sendSignal_cmd_RUN(FpySequencer_SequenceExecutionArgs(fileName, block));
    // only respond if the user doesn't want us to block further execution
    if (block == FpySequencer_BlockState::NO_BLOCK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        // otherwise, save the opCode and cmdSeq so we can respond later
        m_savedOpCode = opCode;
        m_savedCmdSeq = cmdSeq;
    }
}

//! Handler for command VALIDATE
//!
//! Loads and validates a sequence
void FpySequencer::VALIDATE_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                                       U32 cmdSeq,                       //!< The command sequence number
                                       const Fw::CmdStringArg& fileName  //!< The name of the sequence file
) {
    // validate always blocks until finished
    this->sequencer_sendSignal_cmd_VALIDATE(
        FpySequencer_SequenceExecutionArgs(fileName, FpySequencer_BlockState::BLOCK));
}

//! Handler for command RUN_VALIDATED
//!
//! Runs a previously validated sequence
void FpySequencer::RUN_VALIDATED_cmdHandler(
    FwOpcodeType opCode,           //!< The opcode
    U32 cmdSeq,                    //!< The command sequence number
    FpySequencer_BlockState block  //!< Return command status when complete or not
) {
    this->sequencer_sendSignal_cmd_RUN_VALIDATED(FpySequencer_SequenceExecutionArgs(m_sequenceFilePath, block));
    // only respond if the user doesn't want us to block further execution
    if (block == FpySequencer_BlockState::NO_BLOCK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        // otherwise, save the opCode and cmdSeq so we can respond later
        m_savedOpCode = opCode;
        m_savedCmdSeq = cmdSeq;
    }
}

}  // namespace Svc
