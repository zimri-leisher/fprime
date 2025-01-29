#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

// ----------------------------------------------------------------------
// Functions to implement for internal state machine actions
// ----------------------------------------------------------------------

//! Implementation for action setSequenceFilePath of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the current sequence file path member var
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setSequenceFilePath(
    SmId smId,                                              //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
    const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
) {
    m_sequenceFilePath = value.getfilePath();
}

//! Implementation for action setSequenceShouldBlock of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the block state of the sequence to be run
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setSequenceShouldBlock(
    SmId smId,                                              //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
    const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
) {
    m_blockState = value.getblock();
}

//! Implementation for action resetSequenceExecutionArgs of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! resets the sequence file member var
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_resetSequenceExecutionArgs(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_sequenceFilePath = "";
    m_blockState = FpySequencer_BlockState::UNSPECIFIED;
}

//! Implementation for action openSequenceFile of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! opens the sequence file based on the sequence file path member var
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_openSequenceFile(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    FW_ASSERT(m_sequenceFilePath.length() > 0);
    // @FPRIME guys -- how do I check that the file is not open already?
    Os::File::Status openStatus = m_sequenceFileObj.open(m_sequenceFilePath.toChar(), Os::File::OPEN_READ);

    if (openStatus == Os::File::Status::OP_OK) {
        this->sequencer_sendSignal_openSequenceFile_success();
        return;
    }

    this->log_WARNING_HI_FileOpenError(m_sequenceFilePath, static_cast<I32>(openStatus));
    this->sequencer_sendSignal_openSequenceFile_failure();
}

//! Implementation for action closeSequenceFile of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! closes the open sequence file
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_closeSequenceFile(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_sequenceFileObj.close();
}

//! Implementation for action readHeader of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! loads from disk and formats the header into memory representation
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_readHeader(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    // @FPRIME guys-- again, want to check that file is open
    FwSignedSizeType readLen = Fpy::Header::SERIALIZED_SIZE;

    const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
    FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen), static_cast<FwAssertArgType>(capacity),
              static_cast<FwAssertArgType>(readLen));
    Os::File::Status fileStatus = m_sequenceFileObj.read(m_sequenceBuffer.getBuffAddr(), readLen);

    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(m_sequenceFilePath, static_cast<I32>(fileStatus));
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    if (readLen != Fpy::Header::SERIALIZED_SIZE) {
        this->log_WARNING_HI_EndOfFileError(m_sequenceFilePath);
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    Fw::SerializeStatus serializeStatus = m_sequenceBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    // Major version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_majorVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    // Minor version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_minorVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    // Patch version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_patchVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    // Schema version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_schemaVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    // Argument count
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_argumentCount);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    // Statement count
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_statementCount);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    // File size
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_header.m_bodySize);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readHeader_failure();
        return;
    }

    this->log_DIAGNOSTIC_ReadHeaderSuccess(m_sequenceFilePath);
    this->sequencer_sendSignal_readHeader_success();
}

//! Implementation for action readBody of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! loads from disk and formats the body into memory representation
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_readBody(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    FwSignedSizeType readLen = m_sequenceObj.m_header.m_bodySize;

    const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
    FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen), static_cast<FwAssertArgType>(capacity),
              static_cast<FwAssertArgType>(readLen));
    Os::File::Status fileStatus = m_sequenceFileObj.read(m_sequenceBuffer.getBuffAddr(), readLen);

    Fw::SerializeStatus serializeStatus;

    U8 remainingArgMappings = m_sequenceObj.m_header.m_argumentCount;
    while (remainingArgMappings > 0) {
        // local variable index of arg $remainingArgMappings - 1
        serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_argArray[remainingArgMappings - 1]);
        if (serializeStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                  m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
            this->sequencer_sendSignal_readBody_failure();
            return;
        }
        remainingArgMappings--;
    }
    U16 remainingStatements = m_sequenceObj.m_header.m_statementCount;
    while (remainingStatements > 0) {
        // statement at index $remainingStatements - 1
        Fpy::Statement& statement = m_sequenceObj.m_statementArray[remainingStatements - 1];
        // opcode
        serializeStatus = m_sequenceBuffer.deserialize(statement.m_opcode);
        if (serializeStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                  m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
            this->sequencer_sendSignal_readBody_failure();
            return;
        }

        // arg buf
        serializeStatus = m_sequenceBuffer.deserialize(statement.m_args);
        if (serializeStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                  m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
            this->sequencer_sendSignal_readBody_failure();
            return;
        }
        remainingStatements--;
    }

    this->log_DIAGNOSTIC_ReadBodySuccess(m_sequenceFilePath);
    this->sequencer_sendSignal_readBody_success();
}

//! Implementation for action readFooter of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! loads from disk and formats the footer into memory representation
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_readFooter(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    Fw::SerializeStatus serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.m_footer.m_crc);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                              m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        this->sequencer_sendSignal_readFooter_failure();
        return;
    }
    this->log_DIAGNOSTIC_ReadFooterSuccess(m_sequenceFilePath);
    this->sequencer_sendSignal_readFooter_success();
}

//! Implementation for action report_seqSucceeded of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was completed
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqSucceeded(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->log_ACTIVITY_HI_SequenceDone(m_sequenceFilePath);
}

//! Implementation for action report_seqCancelled of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was cancelled
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->log_ACTIVITY_HI_SequenceCancelled(m_sequenceFilePath);
}

//! Implementation for action warn_invalidCmd of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! warns that the user cmd was invalid
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_warn_invalidCmd(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action stepStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! iterates to the next statement and dispatches it
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_stepStatement(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action cancelNextStepStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! indicates to the component that the next call to stepStatement should
//! cancel
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_cancelNextStepStatement(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_cancelAfterNextStep = true;
}

//! Implementation for action setGoalState_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to RUNNING
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_goalState = FpySequencer_GoalState::RUNNING;
}

//! Implementation for action setGoalState_VALID of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to VALID
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_goalState = FpySequencer_GoalState::VALID;
}

//! Implementation for action setGoalState_IDLE of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to IDLE
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_IDLE(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_goalState = FpySequencer_GoalState::IDLE;
}

//! Implementation for action cmdResponse_OK of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with OK
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_cmdResponse_OK(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->m_blockState == FpySequencer_BlockState::BLOCK) {
        // respond if necessary
        this->cmdResponse_out(m_savedOpCode, m_savedCmdSeq, Fw::CmdResponse::OK);
    }
}

//! Implementation for action cmdResponse_EXECUTION_ERROR of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with EXECUTION_ERROR
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_cmdResponse_EXECUTION_ERROR(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->m_blockState == FpySequencer_BlockState::BLOCK) {
        // respond if necessary
        this->cmdResponse_out(m_savedOpCode, m_savedCmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}
// ----------------------------------------------------------------------
// Functions to implement for internal state machine guards
// ----------------------------------------------------------------------

//! Implementation for guard goalStateIs_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! return true if the goal state is RUNNING
bool FpySequencer::Svc_FpySequencer_SequencerStateMachine_guard_goalStateIs_RUNNING(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) const {
    return false;
}

}  // namespace Svc