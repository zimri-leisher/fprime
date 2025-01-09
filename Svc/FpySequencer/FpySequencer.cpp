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

void FpySequencer::initializeComputedCRC() {
  this->m_computedCRC = CRC_INITIAL_VALUE;
}

// Update computed CRC
void FpySequencer::updateComputedCRC(
    const BYTE* buffer,          //!< The buffer
    NATIVE_UINT_TYPE bufferSize  //!< The buffer size
) {
  FW_ASSERT(buffer);
  for (NATIVE_UINT_TYPE index = 0; index < bufferSize; index++) {
    // this->m_computedCRC = static_cast<U32>(
    // update_crc_32(this->m_computedCRC, static_cast<char>(buffer[index])));
  }
}

// Finalize computed CRC
void FpySequencer::finalizeComputedCRC() {
  this->m_computedCRC = ~this->m_computedCRC;
}
//! Handler for command RUN
//!
//! Dispatches a sequence to the first available sequencer
void RUN_cmdHandler(
    FwOpcodeType opCode,               //!< The opcode
    U32 cmdSeq,                        //!< The command sequence number
    const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
    Fw::Wait block  //!< Return command status when complete or not
) {}

//! Handler for command OPEN
void FpySequencer::OPEN_cmdHandler(
    FwOpcodeType opCode,              //!< The opcode
    U32 cmdSeq,                       //!< The command sequence number
    const Fw::CmdStringArg& fileName  //!< The name of the sequence file
) {
  // this->sequencer_sendSignal_cmd_LOAD(fileName);
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for command LOG_STATUS
//!
//! Logs via Events the state of each connected command sequencer
void LOG_STATUS_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                           U32 cmdSeq  //!< The command sequence number
);
// ----------------------------------------------------------------------
// Functions to implement for internal state machine actions
// ----------------------------------------------------------------------

//! Implementation for action setSequenceFilePath of state machine
//! Svc_FpySequencer_StateMachine
//!
//! sets the current sequence file path member
// void FpySequencer::Svc_FpySequencer_StateMachine_action_setSequenceFilePath(
//     SmId smId,                                     //!< The state machine id
//     Svc_FpySequencer_StateMachine::Signal signal,  //!< The signal
//     const Fw::StringBase& value                    //!< The value
// ) {
//   this->m_sequenceFilePath = value;
// }

//! Implementation for action resetSequenceFilePath of state machine
//! Svc_FpySequencer_StateMachine
//!
//! resets the sequence file path member
// void FpySequencer::Svc_FpySequencer_StateMachine_action_resetSequenceFilePath(
//     SmId smId,                                    //!< The state machine id
//     Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
// ) {
//   this->m_sequenceFilePath = "";
// }

// //! Implementation for action openSequenceFile of state machine
// //! Svc_FpySequencer_StateMachine
// //!
// //! opens the file descriptor
// void FpySequencer::Svc_FpySequencer_StateMachine_action_openSequenceFile(
//     SmId smId,                                    //!< The state machine id
//     Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
// ) {
//   Os::File::Status status = this->m_sequenceFileObj.open(
//       this->m_sequenceFilePath.toChar(), Os::File::OPEN_READ);

//   if (status != Os::File::OP_OK) {
//     if (status == Os::File::DOESNT_EXIST) {
//       // this->m_events.fileNotFound();
//     } else {
//       // this->m_events.fileReadError();
//     }
//     this->m_sequenceFileObj.close();
//   }
// }

  // ----------------------------------------------------------------------
  // Functions to implement for internal state machine actions
  // ----------------------------------------------------------------------

  // //! sets the current sequence file path member
  // void FpySequencer::action_setSequenceFilePath(Signal signal,               //!< The signal
  //                                 const Fw::StringBase& value  //!< The value
  // );

  // void FpySequencer::action_validateSequence(Signal signal  //!< The signal
  // );

  // void FpySequencer::action_reportValidationError(
  //     Signal signal,                                  //!< The signal
  //     const Svc::FpySequencer_ValidationError& value  //!< The value
  // );

  // void FpySequencer::action_stepStatement(Signal signal  //!< The signal
  // );

  //     // ----------------------------------------------------------------------
  //     // Guards
  //     // ----------------------------------------------------------------------

  //     //! check whether the sequence file exists
  //     bool
  //     FpySequencer::guard_sequenceFileExists(Signal signal  //!< The signal
  //     ) const;

  // //! check whether the sequence file is open
  // bool FpySequencer::guard_sequenceFileOpen(Signal signal  //!< The signal
  // ) const;

  // //! check whether the header is valid
  // bool FpySequencer::guard_headerValid(Signal signal  //!< The signal
  // ) const;

  // bool FpySequencer::guard_bodyValid(Signal signal  //!< The signal
  // ) const;

  // bool FpySequencer::guard_footerValid(Signal signal  //!< The signal
  // ) const;

//! Implementation for action readHeader of state machine
//! Svc_FpySequencer_StateMachine
// void FpySequencer::Svc_FpySequencer_StateMachine_action_readHeader(
//     SmId smId,                                    //!< The state machine id
//     Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
// ) {
//   U8* const buffAddr = this->m_sequenceBuffer.getBuffAddr();
//   this->initializeComputedCRC();
//   bool status = this->readHeader();
//   if (not status) {
//     stateTo_INVALID();
//     return;
//   }
//   if (status) {
//     this->updateComputedCRC(buffAddr, Svc::Fpy::Header::SERIALIZED_SIZE);
//     status = this->deserializeHeader() and this->readRecordsAndCRC() and
//              this->extractCRC();
//   }
//   if (status) {
//     const NATIVE_UINT_TYPE buffLen = this->m_buffer.getBuffLength();
//     this->m_crc.update(buffAddr, buffLen);
//     this->m_crc.finalize();
//   } else {
//   }
// }


//// called when transitioned to READING_HEADER
// void FpySequencer::transtionCallback_READING_HEADER() {
//}

//// called when transitioned to READING_BODY
// bool FpySequencer::transitionCallback_READING_BODY() {}
//// called when transitioned to READING_FOOTER
// bool FpySequencer::transitionCallback_READING_FOOTER() {}

//// called when transitioned to RUNNING
// void FpySequencer::transtionCallback_RUNNING() {}
//// called when transitioned to INVALID
// void FpySequencer::transtionCallback_INVALID() {}

// bool FpySequencer::readHeader() {
// Os::File& file = this->m_sequenceFile;
// Fw::SerializeBufferBase& buffer = this->m_sequenceBuffer;
// bool status = true;

// FwSignedSizeType readLen = Fpy::Header::SERIALIZED_SIZE;
// FW_ASSERT(readLen >= 0, static_cast<FwAssertArgType>(readLen));

// const NATIVE_UINT_TYPE capacity = buffer.getBuffCapacity();
// FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen),
// static_cast<FwAssertArgType>(capacity),
// static_cast<FwAssertArgType>(readLen));
// Os::File::Status fileStatus = file.read(buffer.getBuffAddr(), readLen);

// if (fileStatus != Os::File::OP_OK) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::READ_HEADER,
// fileStatus);
// status = false;
//}

// if (status and readLen != Sequence::Header::SERIALIZED_SIZE) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::READ_HEADER_SIZE,
// static_cast<I32>(readLen));
// status = false;
//}

// if (status) {
// const Fw::SerializeStatus serializeStatus =
// buffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
// FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);
//}

// return status;
//}

// bool FpySequencer::deserializeHeader() {
// Fw::SerializeBufferBase& buffer = this->m_buffer;
// Header& header = this->m_header;

//// File size
// Fw::SerializeStatus serializeStatus = buffer.deserialize(header.m_fileSize);
// if (serializeStatus != Fw::FW_SERIALIZE_OK) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::DESER_SIZE,
// serializeStatus);
// return false;
//}
// if (header.m_fileSize > buffer.getBuffCapacity()) {
// this->m_events.fileSizeError(header.m_fileSize);
// return false;
//}
//// Number of records
// serializeStatus = buffer.deserialize(header.m_numRecords);
// if (serializeStatus != Fw::FW_SERIALIZE_OK) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::DESER_NUM_RECORDS,
// serializeStatus);
// return false;
//}
//// Time base
// FwTimeBaseStoreType tbase;
// serializeStatus = buffer.deserialize(tbase);
// if (serializeStatus != Fw::FW_SERIALIZE_OK) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::DESER_TIME_BASE,
// serializeStatus);
// return false;
//}
// header.m_timeBase = static_cast<TimeBase>(tbase);
//// Time context
// serializeStatus = buffer.deserialize(header.m_timeContext);
// if (serializeStatus != Fw::FW_SERIALIZE_OK) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::DESER_TIME_CONTEXT,
// serializeStatus);
// return false;
//}
// return true;
//}

// bool FpySequencer::readRecordsAndCRC() {
// Os::File& file = this->m_sequenceFile;
// const NATIVE_UINT_TYPE size = this->m_header.m_fileSize;
// Fw::SerializeBufferBase& buffer = this->m_buffer;

// FwSignedSizeType readLen = size;
// Os::File::Status fileStatus = file.read(buffer.getBuffAddr(), readLen);
//// check read status
// if (fileStatus != Os::File::OP_OK) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::READ_SEQ_DATA,
// fileStatus);
// return false;
//}
//// check read size
// if (static_cast<NATIVE_INT_TYPE>(size) != readLen) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::READ_SEQ_DATA_SIZE,
// static_cast<I32>(readLen));
// return false;
//}
//// set buffer size
// Fw::SerializeStatus serializeStatus = buffer.setBuffLen(size);
// FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);
// return true;
//}

// bool FpySequencer::extractCRC() {
// Fw::SerializeBufferBase& buffer = this->m_buffer;
// U32& crc = this->m_crc.m_stored;

//// Compute the data size
// const U32 buffSize = buffer.getBuffLength();
// const U32 crcSize = sizeof(crc);
// U8* const buffAddr = buffer.getBuffAddr();
// if (buffSize < crcSize) {
// this->m_events.fileInvalid(CmdSequencer_FileReadStage::READ_SEQ_CRC,
// static_cast<I32>(buffSize));
// return false;
//}
// FW_ASSERT(buffSize >= crcSize, static_cast<FwAssertArgType>(buffSize),
// crcSize);
// const NATIVE_UINT_TYPE dataSize = buffSize - crcSize;
//// Create a CRC buffer pointing at the CRC in the main buffer, after the data
// Fw::ExternalSerializeBuffer crcBuff(&buffAddr[dataSize], crcSize);
// Fw::SerializeStatus status = crcBuff.setBuffLen(crcSize);
// FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
//// Deserialize the CRC from the CRC buffer
// status = crcBuff.deserialize(crc);
// FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
//// Set the main buffer size to the data size
// status = buffer.setBuffLen(dataSize);
// FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
// return true;
//}

// Fw::SerializeStatus
// CmdSequencerComponentImpl::FPrimeSequence ::deserializeRecord(Record& record)
// { U32 recordSize;

// Fw::SerializeStatus status =
// this->deserializeDescriptor(record.m_descriptor);

// if (status == Fw::FW_SERIALIZE_OK and
// record.m_descriptor == Record::END_OF_SEQUENCE) {
// return Fw::FW_SERIALIZE_OK;
//}

// if (status == Fw::FW_SERIALIZE_OK) {
// status = this->deserializeTimeTag(record.m_timeTag);
//}
// if (status == Fw::FW_SERIALIZE_OK) {
// status = this->deserializeRecordSize(recordSize);
//}
// if (status == Fw::FW_SERIALIZE_OK) {
// status = this->copyCommand(record.m_command, recordSize);
//}

// return status;
//}

// Fw::SerializeStatus
// CmdSequencerComponentImpl::FPrimeSequence ::deserializeDescriptor(
// Record::Descriptor& descriptor) {
// Fw::SerializeBufferBase& buffer = this->m_buffer;
// U8 descEntry;

// Fw::SerializeStatus status = buffer.deserialize(descEntry);
// if (status != Fw::FW_SERIALIZE_OK) {
// return status;
//}

// if (descEntry > Sequence::Record::END_OF_SEQUENCE) {
// return Fw::FW_DESERIALIZE_FORMAT_ERROR;
//}

// descriptor = static_cast<Record::Descriptor>(descEntry);
// return Fw::FW_SERIALIZE_OK;
//}

// Fw::SerializeStatus
// CmdSequencerComponentImpl::FPrimeSequence ::deserializeTimeTag(
// Fw::Time& timeTag) {
// Fw::SerializeBufferBase& buffer = this->m_buffer;
// U32 seconds, useconds;
// Fw::SerializeStatus status = buffer.deserialize(seconds);
// if (status == Fw::FW_SERIALIZE_OK) {
// status = buffer.deserialize(useconds);
//}
// if (status == Fw::FW_SERIALIZE_OK) {
// timeTag.set(seconds, useconds);
//}
// return status;
//}

// Fw::SerializeStatus
// CmdSequencerComponentImpl::FPrimeSequence ::deserializeRecordSize(
// U32& recordSize) {
// Fw::SerializeBufferBase& buffer = this->m_buffer;
// Fw::SerializeStatus status = buffer.deserialize(recordSize);
// if (status == Fw::FW_SERIALIZE_OK and recordSize > buffer.getBuffLeft()) {
//// Not enough data left
// status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
//}
// if (status == Fw::FW_SERIALIZE_OK and
// recordSize + sizeof(FwPacketDescriptorType) >
// Fw::ComBuffer::SERIALIZED_SIZE) {
//// Record size is too big for com buffer
// status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
//}
// return status;
//}

// Fw::SerializeStatus CmdSequencerComponentImpl::FPrimeSequence ::copyCommand(
// Fw::ComBuffer& comBuffer,
// const U32 recordSize) {
// Fw::SerializeBufferBase& buffer = this->m_buffer;
// comBuffer.resetSer();
// NATIVE_UINT_TYPE size = recordSize;
// Fw::SerializeStatus status = comBuffer.setBuffLen(recordSize);
// FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
// status = buffer.deserialize(comBuffer.getBuffAddr(), size, true);
// return status;
//}

// bool CmdSequencerComponentImpl::FPrimeSequence ::validateRecords() {
// Fw::SerializeBufferBase& buffer = this->m_buffer;
// const U32 numRecords = this->m_header.m_numRecords;
// Sequence::Record record;

//// Deserialize all records
// for (NATIVE_UINT_TYPE recordNumber = 0; recordNumber < numRecords;
// recordNumber++) {
// Fw::SerializeStatus status = this->deserializeRecord(record);
// if (status != Fw::FW_SERIALIZE_OK) {
// this->m_events.recordInvalid(recordNumber, status);
// return false;
//}
//}
//// Check there is no data left
// const U32 buffLeftSize = buffer.getBuffLeft();
// if (buffLeftSize > 0) {
// this->m_events.recordMismatch(numRecords, buffLeftSize);
// return false;
//}
//// Rewind deserialization
// buffer.resetDeser();

// return true;
//}
}  // namespace Svc
