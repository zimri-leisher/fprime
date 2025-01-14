#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

// ----------------------------------------------------------------------
// Functions to implement for internal state machine actions
// ----------------------------------------------------------------------

//! Implementation for action setSequenceFilePath of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the current sequence file path member var
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setSequenceFilePath(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
    ) {
  m_sequenceFilePath = value.getfilePath();
}

//! Implementation for action setSequenceShouldBlock of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the block state of the sequence to be run
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setSequenceShouldBlock(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
    ) {
  m_sequenceShouldBlock = value.getblock();
}

//! Implementation for action resetSequenceFile of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! resets the sequence file member var
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_resetSequenceFile(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  m_sequenceFilePath = "";
  m_sequenceShouldBlock = Fw::Wait::WAIT;
}

//! Implementation for action openSequenceFile of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! opens the sequence file based on the sequence file path member var
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_openSequenceFile(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  Os::File::Status openStatus =
      m_sequenceFileObj.open(m_sequenceFilePath.toChar(), Os::File::OPEN_READ);

  if (openStatus == Os::File::Status::OP_OK) {
    this->sequencer_sendSignal_openSequenceFile_success();
    return;
  }

  FpySequencer_OpenSequenceFileFailure failure;
  switch (openStatus) {
    case Os::File::DOESNT_EXIST: {
      failure = FpySequencer_OpenSequenceFileFailure::DOESNT_EXIST;
      break;
    }
    case Os::File::NO_SPACE: {
      failure = FpySequencer_OpenSequenceFileFailure::NO_SPACE;
      break;
    }
    case Os::File::NO_PERMISSION: {
      failure = FpySequencer_OpenSequenceFileFailure::NO_PERMISSION;
      break;
    }
    case Os::File::BAD_SIZE: {
      failure = FpySequencer_OpenSequenceFileFailure::BAD_SIZE;
      break;
    }
    case Os::File::NOT_OPENED: {
      failure = FpySequencer_OpenSequenceFileFailure::NOT_OPENED;
      break;
    }
    case Os::File::FILE_EXISTS: {
      failure = FpySequencer_OpenSequenceFileFailure::FILE_EXISTS;
      break;
    }
    case Os::File::NOT_SUPPORTED: {
      failure = FpySequencer_OpenSequenceFileFailure::NOT_SUPPORTED;
      break;
    }
    case Os::File::INVALID_MODE: {
      failure = FpySequencer_OpenSequenceFileFailure::INVALID_MODE;
      break;
    }
    case Os::File::INVALID_ARGUMENT: {
      failure = FpySequencer_OpenSequenceFileFailure::INVALID_ARGUMENT;
      break;
    }
    case Os::File::OTHER_ERROR: {
      failure = FpySequencer_OpenSequenceFileFailure::OTHER_ERROR;
      break;
    }
    default: {
      failure = FpySequencer_OpenSequenceFileFailure::UNKNOWN;
      break;
    }
  }

  this->sequencer_sendSignal_openSequenceFile_failure(failure);
}

//! Implementation for action closeSequenceFile of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! closes the open sequence file
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_closeSequenceFile(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  m_sequenceFileObj.close();
}

//! Implementation for action readHeader of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! loads from disk and formats the header into memory representation
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_readHeader(
    SmId smId,  //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action readBody of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! loads from disk and formats the body into memory representation
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_readBody(
    SmId smId,  //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action readFooter of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! loads from disk and formats the footer into memory representation
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_readFooter(
    SmId smId,  //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action reportError_stepStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports an error that occurred in the stepStatement method
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_reportError_stepStatement(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_StepStatementFailure& value     //!< The value
    ) {}

//! Implementation for action reportError_openSequenceFile of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports an error that occurred in the openSequenceFile method
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_reportError_openSequenceFile(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_OpenSequenceFileFailure& value  //!< The value
    ) {}

//! Implementation for action reportError_readHeader of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports an error that occurred in the readHeader method
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_reportError_readHeader(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_ReadHeaderFailure& value        //!< The value
    ) {}

//! Implementation for action reportError_readBody of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports an error that occurred in the readBody method
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_reportError_readBody(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_ReadBodyFailure& value          //!< The value
    ) {}

//! Implementation for action reportError_readFooter of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports an error that occurred in the readFooter method
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_reportError_readFooter(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_ReadFooterFailure& value        //!< The value
    ) {}

//! Implementation for action reportError_statementFinished of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports an error that occurred in the statementFinished method
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_reportError_statementFinished(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,   //!< The signal
        const Svc::FpySequencer_StatementFinishedFailure& value  //!< The value
    ) {}

//! Implementation for action report_seqDone of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was completed
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqDone(
    SmId smId,  //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action report_seqCancelled of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was cancelled
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {}

//! Implementation for action stepStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! iterates to the next statement and dispatches it
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_stepStatement(
    SmId smId,  //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action cancelNextStepStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! indicates to the component that the next call to stepStatement should
//! cancel
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_cancelNextStepStatement(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {}

//! Implementation for action setGoalState_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to RUNNING
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {}

//! Implementation for action setGoalState_VALID of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to VALID
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {}

//! Implementation for action warn_noSequenceToCancel of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! warns that the user said to cancel a sequence but there is no sequence to
//! cancel
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_warn_noSequenceToCancel(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {}

// ----------------------------------------------------------------------
// Functions to implement for internal state machine guards
// ----------------------------------------------------------------------

//! Implementation for guard goalStateIs_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! return true if the goal state is RUNNING
bool FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_guard_goalStateIs_RUNNING(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const {
  return false;
}

}  // namespace Svc