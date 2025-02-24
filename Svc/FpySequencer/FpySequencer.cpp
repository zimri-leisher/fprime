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

FpySequencer ::FpySequencer(const char* const compName)
    : FpySequencerComponentBase(compName) {}

FpySequencer ::~FpySequencer() {}

void FpySequencer::allocateBuffer(NATIVE_INT_TYPE identifier,
                                  Fw::MemAllocator& allocator,
                                  NATIVE_UINT_TYPE bytes) {
  // if this assertion fails, you aren't allocating enough bytes for the
  // FpySequencer this is because you must have a buffer big enough to fit the
  // header of a sequence
  FW_ASSERT(bytes >= Fpy::Header::SERIALIZED_SIZE);
  bool recoverable = false;
  this->m_allocatorId = identifier;
  U8* allocatedMemory = static_cast<U8*>(allocator.allocate(
      static_cast<NATIVE_UINT_TYPE>(identifier), bytes, recoverable));
  // if this fails, unable to allocate the memory
  FW_ASSERT(bytes > 0);
  this->m_sequenceBuffer.setExtBuffer(allocatedMemory, bytes);
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
    FpySequencer_BlockState
        block  //!< Return command status when complete or not
) {
  if (block == FpySequencer_BlockState::BLOCK) {
    // save the opCode and cmdSeq so we can respond later
    m_savedOpCode = opCode;
    m_savedCmdSeq = cmdSeq;
  }

  this->sequencer_sendSignal_cmd_RUN(
      FpySequencer_SequenceExecutionArgs(fileName, block));

  // only respond if the user doesn't want us to block further execution
  if (block == FpySequencer_BlockState::NO_BLOCK) {
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }
}

//! Handler for command VALIDATE
//!
//! Loads and validates a sequence
void FpySequencer::VALIDATE_cmdHandler(
    FwOpcodeType opCode,              //!< The opcode
    U32 cmdSeq,                       //!< The command sequence number
    const Fw::CmdStringArg& fileName  //!< The name of the sequence file
) {
  m_savedOpCode = opCode;
  m_savedCmdSeq = cmdSeq;

  // validate always blocks until finished
  this->sequencer_sendSignal_cmd_VALIDATE(FpySequencer_SequenceExecutionArgs(
      fileName, FpySequencer_BlockState::BLOCK));
}

//! Handler for command RUN_VALIDATED
//!
//! Runs a previously validated sequence
void FpySequencer::RUN_VALIDATED_cmdHandler(
    FwOpcodeType opCode,  //!< The opcode
    U32 cmdSeq,           //!< The command sequence number
    FpySequencer_BlockState
        block  //!< Return command status when complete or not
) {
  if (block == FpySequencer_BlockState::BLOCK) {
    // save the opCode and cmdSeq so we can respond later
    m_savedOpCode = opCode;
    m_savedCmdSeq = cmdSeq;
  }

  this->sequencer_sendSignal_cmd_RUN_VALIDATED(
      FpySequencer_SequenceExecutionArgs(m_sequenceFilePath, block));

  // only respond if the user doesn't want us to block further execution
  if (block == FpySequencer_BlockState::NO_BLOCK) {
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }
}

//! Handler for command CANCEL
//!
//! Cancels a running or validated sequence
void FpySequencer::CANCEL_cmdHandler(
    FwOpcodeType opCode,  //!< The opcode
    U32 cmdSeq            //!< The command sequence number
) {
  // cancel always blocks, so save the cmdSeq and opcode so we can respond ltr
  m_savedOpCode = opCode;
  m_savedCmdSeq = cmdSeq;

  this->sequencer_sendSignal_cmd_CANCEL();
}

//! opens the sequence file based on the sequence file path member var
void FpySequencer::openSequenceFile() {
  FW_ASSERT(m_sequenceFilePath.length() > 0);
  // make sure not already open
  FW_ASSERT(!m_sequenceFileObj.isOpen());
  Os::File::Status openStatus =
      m_sequenceFileObj.open(m_sequenceFilePath.toChar(), Os::File::OPEN_READ);

  if (openStatus == Os::File::Status::OP_OK) {
    this->sequencer_sendSignal_result_success();
    return;
  }

  this->log_WARNING_HI_FileOpenError(m_sequenceFilePath,
                                     static_cast<I32>(openStatus));
  this->sequencer_sendSignal_result_failure();
}

//! loads from disk and formats the header into memory representation
void FpySequencer::readHeader() {
  FW_ASSERT(m_sequenceFileObj.isOpen());
  FwSignedSizeType readLen = Fpy::Header::SERIALIZED_SIZE;

  const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
  FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen),
            static_cast<FwAssertArgType>(capacity),
            static_cast<FwAssertArgType>(readLen));
  Os::File::Status fileStatus =
      m_sequenceFileObj.read(m_sequenceBuffer.getBuffAddr(), readLen);

  if (fileStatus != Os::File::OP_OK) {
    this->log_WARNING_HI_FileReadError(m_sequenceFilePath,
                                       static_cast<I32>(fileStatus));
    this->sequencer_sendSignal_result_failure();
    return;
  }

  if (readLen != Fpy::Header::SERIALIZED_SIZE) {
    this->log_WARNING_HI_EndOfFileError(m_sequenceFilePath);
    this->sequencer_sendSignal_result_failure();
    return;
  }

  Fw::SerializeStatus serializeStatus = m_sequenceBuffer.setBuffLen(
      static_cast<Fw::Serializable::SizeType>(readLen));
  FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

  // Major version
  serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_majorVersion);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }

  // Minor version
  serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_minorVersion);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }

  // Patch version
  serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_patchVersion);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }

  // Schema version
  serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_schemaVersion);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }

  // Argument count
  serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_argumentCount);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }

  // Statement count
  serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_statementCount);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }

  // File size
  serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_bodySize);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }

  this->log_DIAGNOSTIC_ReadHeaderSuccess(m_sequenceFilePath);
  this->sequencer_sendSignal_result_success();
}

//! loads from disk and formats the body into memory representation
void FpySequencer::readBody() {
  FW_ASSERT(m_sequenceFileObj.isOpen());
  FwSignedSizeType readLen = m_sequenceObj.m_header.m_bodySize;

  const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
  FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen),
            static_cast<FwAssertArgType>(capacity),
            static_cast<FwAssertArgType>(readLen));
  Os::File::Status fileStatus =
      m_sequenceFileObj.read(m_sequenceBuffer.getBuffAddr(), readLen);

  if (fileStatus != Os::File::OP_OK) {
    this->log_WARNING_HI_FileReadError(m_sequenceFilePath,
                                       static_cast<I32>(fileStatus));
    this->sequencer_sendSignal_result_failure();
    return;
  }

  if (readLen != Fpy::Header::SERIALIZED_SIZE) {
    this->log_WARNING_HI_EndOfFileError(m_sequenceFilePath);
    this->sequencer_sendSignal_result_failure();
    return;
  }

  Fw::SerializeStatus serializeStatus;

  U8 remainingArgMappings = m_sequenceObj.m_header.m_argumentCount;
  while (remainingArgMappings > 0) {
    // local variable index of arg $remainingArgMappings - 1
    serializeStatus = m_sequenceBuffer.deserialize(
        m_sequenceObj.m_argArray[remainingArgMappings - 1]);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->log_WARNING_HI_DeserializeError(
          m_sequenceFilePath, static_cast<I32>(serializeStatus),
          m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
      this->sequencer_sendSignal_result_failure();
      return;
    }
    remainingArgMappings--;
  }
  U16 remainingStatements = m_sequenceObj.m_header.m_statementCount;
  while (remainingStatements > 0) {
    // statement at index $remainingStatements - 1
    Fpy::Statement& statement =
        m_sequenceObj.m_statementArray[remainingStatements - 1];
    // opcode
    serializeStatus = m_sequenceBuffer.deserialize(statement.m_opcode);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->log_WARNING_HI_DeserializeError(
          m_sequenceFilePath, static_cast<I32>(serializeStatus),
          m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
      this->sequencer_sendSignal_result_failure();
      return;
    }

    // arg buf
    serializeStatus = m_sequenceBuffer.deserialize(statement.m_args);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->log_WARNING_HI_DeserializeError(
          m_sequenceFilePath, static_cast<I32>(serializeStatus),
          m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
      this->sequencer_sendSignal_result_failure();
      return;
    }
    remainingStatements--;
  }

  this->log_DIAGNOSTIC_ReadBodySuccess(m_sequenceFilePath);
  this->sequencer_sendSignal_result_success();
}

//! loads from disk and formats the footer into memory representation
void FpySequencer::readFooter() {
  Fw::SerializeStatus serializeStatus =
      m_sequenceBuffer.deserialize(m_sequenceObj.m_footer.m_crc);
  if (serializeStatus != Fw::FW_SERIALIZE_OK) {
    this->log_WARNING_HI_DeserializeError(
        m_sequenceFilePath, static_cast<I32>(serializeStatus),
        m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
    this->sequencer_sendSignal_result_failure();
    return;
  }
  this->log_DIAGNOSTIC_ReadFooterSuccess(m_sequenceFilePath);
  this->sequencer_sendSignal_result_success();
}

}  // namespace Svc
