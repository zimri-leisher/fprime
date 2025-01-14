// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer component implementation class
// ======================================================================

#ifndef FpySequencer_HPP
#define FpySequencer_HPP

#include "FppConstantsAc.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Fw/Types/WaitEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/FpySequencer/FpySequencerComponentAc.hpp"

namespace Svc {

class FpySequencer : public FpySequencerComponentBase {
 public:
  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  //! Construct object FpySequencer
  //!
  FpySequencer(const char* const compName /*!< The component name*/
  );

  //! Destroy object FpySequencer
  //!
  ~FpySequencer();

  PRIVATE :

      //! Handler for command RUN
      //!
      //! Loads, validates and runs a sequence
      void
      RUN_cmdHandler(
          FwOpcodeType opCode,               //!< The opcode
          U32 cmdSeq,                        //!< The command sequence number
          const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
          Fw::Wait block  //!< Return command status when complete or not
          ) override;

  //! Handler for command VALIDATE
  //!
  //! Loads and validates a sequence
  void VALIDATE_cmdHandler(
      FwOpcodeType opCode,              //!< The opcode
      U32 cmdSeq,                       //!< The command sequence number
      const Fw::CmdStringArg& fileName  //!< The name of the sequence file
      ) override;

  //! Handler for command RUN_VALIDATED
  //!
  //! Runs a previously validated sequence
  void RUN_VALIDATED_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq  //!< The command sequence number
                                ) override;

  //! Handler for command CANCEL
  //!
  //! Cancels a running or validated sequence
  void CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;

  // ----------------------------------------------------------------------
  // Functions to implement for internal state machine actions
  // ----------------------------------------------------------------------

  //! Implementation for action setSequenceFilePath of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! sets the current sequence file path member var
  void Svc_FpySequencer_SequencerStateMachine_action_setSequenceFilePath(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
      const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
      ) override;

  //! Implementation for action setSequenceShouldBlock of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! sets the block state of the sequence to be run
  void Svc_FpySequencer_SequencerStateMachine_action_setSequenceShouldBlock(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
      const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
      ) override;

  //! Implementation for action resetSequenceFile of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! resets the sequence file member var
  void Svc_FpySequencer_SequencerStateMachine_action_resetSequenceFile(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action openSequenceFile of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! opens the sequence file based on the sequence file path member var
  void Svc_FpySequencer_SequencerStateMachine_action_openSequenceFile(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action closeSequenceFile of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! closes the open sequence file
  void Svc_FpySequencer_SequencerStateMachine_action_closeSequenceFile(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action readHeader of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! loads from disk and formats the header into memory representation
  void Svc_FpySequencer_SequencerStateMachine_action_readHeader(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action readBody of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! loads from disk and formats the body into memory representation
  void Svc_FpySequencer_SequencerStateMachine_action_readBody(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action readFooter of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! loads from disk and formats the footer into memory representation
  void Svc_FpySequencer_SequencerStateMachine_action_readFooter(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action reportError_stepStatement of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports an error that occurred in the stepStatement method
  void Svc_FpySequencer_SequencerStateMachine_action_reportError_stepStatement(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
      const Svc::FpySequencer_StepStatementFailure& value     //!< The value
      ) override;

  //! Implementation for action reportError_openSequenceFile of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports an error that occurred in the openSequenceFile method
  void
  Svc_FpySequencer_SequencerStateMachine_action_reportError_openSequenceFile(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
      const Svc::FpySequencer_OpenSequenceFileFailure& value  //!< The value
      ) override;

  //! Implementation for action reportError_readHeader of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports an error that occurred in the readHeader method
  void Svc_FpySequencer_SequencerStateMachine_action_reportError_readHeader(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
      const Svc::FpySequencer_ReadHeaderFailure& value        //!< The value
      ) override;

  //! Implementation for action reportError_readBody of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports an error that occurred in the readBody method
  void Svc_FpySequencer_SequencerStateMachine_action_reportError_readBody(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
      const Svc::FpySequencer_ReadBodyFailure& value          //!< The value
      ) override;

  //! Implementation for action reportError_readFooter of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports an error that occurred in the readFooter method
  void Svc_FpySequencer_SequencerStateMachine_action_reportError_readFooter(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
      const Svc::FpySequencer_ReadFooterFailure& value        //!< The value
      ) override;

  //! Implementation for action reportError_statementFinished of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports an error that occurred in the statementFinished method
  void
  Svc_FpySequencer_SequencerStateMachine_action_reportError_statementFinished(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal,   //!< The signal
      const Svc::FpySequencer_StatementFinishedFailure& value  //!< The value
      ) override;

  //! Implementation for action report_seqDone of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports that a sequence was completed
  void Svc_FpySequencer_SequencerStateMachine_action_report_seqDone(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action report_seqCancelled of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! reports that a sequence was cancelled
  void Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action stepStatement of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! iterates to the next statement and dispatches it
  void Svc_FpySequencer_SequencerStateMachine_action_stepStatement(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action cancelNextStepStatement of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! indicates to the component that the next call to stepStatement should
  //! cancel
  void Svc_FpySequencer_SequencerStateMachine_action_cancelNextStepStatement(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action setGoalState_RUNNING of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! sets the goal state to RUNNING
  void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action setGoalState_VALID of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! sets the goal state to VALID
  void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;

  //! Implementation for action warn_noSequenceToCancel of state machine
  //! Svc_FpySequencer_SequencerStateMachine
  //!
  //! warns that the user said to cancel a sequence but there is no sequence to
  //! cancel
  void Svc_FpySequencer_SequencerStateMachine_action_warn_noSequenceToCancel(
      SmId smId,  //!< The state machine id
      Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) override;
  PROTECTED :

      // ----------------------------------------------------------------------
      // Functions to implement for internal state machine guards
      // ----------------------------------------------------------------------

      //! Implementation for guard goalStateIs_RUNNING of state machine
      //! Svc_FpySequencer_SequencerStateMachine
      //!
      //! return true if the goal state is RUNNING
      bool
      Svc_FpySequencer_SequencerStateMachine_guard_goalStateIs_RUNNING(
          SmId smId,  //!< The state machine id
          Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
      ) const override;

  enum { CRC_INITIAL_VALUE = 0xFFFFFFFFU };

  // assigned by the user
  Fw::String m_sequenceFilePath;
  // created by opening the m_sequenceFilePath
  Os::File m_sequenceFileObj;
  // whether or not the sequence we're about to run should return immediately or
  // block on completion
  Fw::Wait m_sequenceShouldBlock;
  // live running computation of CRC (updated as we read)
  U32 m_computedCRC;

  // allocated at runtime
  Fw::ExternalSerializeBuffer m_sequenceBuffer;
  // id of allocator that gave us m_sequenceBuffer
  NATIVE_INT_TYPE m_allocatorId;

  void allocateBuffer(NATIVE_INT_TYPE identifier,
                      Fw::MemAllocator& allocator,
                      NATIVE_UINT_TYPE bytes);

  void deallocateBuffer(Fw::MemAllocator& allocator);

  void initializeComputedCRC();

  void updateComputedCRC(const BYTE* buffer,          //!< The buffer
                         NATIVE_UINT_TYPE bufferSize  //!< The buffer size
  );

  void finalizeComputedCRC();
};

}  // namespace Svc

#endif
