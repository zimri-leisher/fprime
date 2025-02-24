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

//! Implementation for action setSequenceBlockState of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the block state of the sequence to be run
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setSequenceBlockState(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
    ) {
  m_sequenceBlockState = value.getblock();
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

//! Implementation for action report_seqSucceeded of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was completed
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_report_seqSucceeded(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  this->log_ACTIVITY_HI_SequenceDone(m_sequenceFilePath);
}

//! Implementation for action report_seqCancelled of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was cancelled
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  this->log_ACTIVITY_HI_SequenceCancelled(m_sequenceFilePath);
}

//! Implementation for action report_invalidCmd of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! warns that the user cmd was invalid
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_report_invalidCmd(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {}

//! Implementation for action report_invalidSeq of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! warns that the sequence failed validation
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_report_invalidSeq(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {}

//! Implementation for action report_seqFailed of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence failed
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_report_seqFailed(
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
    ) {
  m_cancelNextStatement = true;
}

//! Implementation for action setGoalState_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to RUNNING
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  m_goalState = FpySequencer_GoalState::RUNNING;
}

//! Implementation for action setGoalState_VALID of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to VALID
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  m_goalState = FpySequencer_GoalState::VALID;
}

//! Implementation for action setGoalState_IDLE of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to IDLE
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_setGoalState_IDLE(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  m_goalState = FpySequencer_GoalState::IDLE;
}

//! Implementation for action cmdResponse_OK of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with OK
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_cmdResponse_OK(
    SmId smId,  //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
  if (this->m_sequenceBlockState == FpySequencer_BlockState::BLOCK) {
    // respond if we were waiting on a response
    this->cmdResponse_out(m_savedOpCode, m_savedCmdSeq, Fw::CmdResponse::OK);
  }
}

//! Implementation for action cmdResponse_EXECUTION_ERROR of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with EXECUTION_ERROR
void FpySequencer::
    Svc_FpySequencer_SequencerStateMachine_action_cmdResponse_EXECUTION_ERROR(
        SmId smId,  //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) {
  if (this->m_sequenceBlockState == FpySequencer_BlockState::BLOCK) {
    // respond if we were waiting on a response
    this->cmdResponse_out(m_savedOpCode, m_savedCmdSeq,
                          Fw::CmdResponse::EXECUTION_ERROR);
  }
}

//! Implementation for action resetSequence of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! resets the sequence execution arguments, memory object, CRC, saved
//! opcode/cmdseq
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_resetSequence(
    SmId smId,  //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {}

//! Implementation for action validate of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! performs all steps necessary for sequence validation, and raises a signal
//! result_success or result_failure
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_validate(
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
  return m_goalState == FpySequencer_GoalState::RUNNING;
}

}  // namespace Svc