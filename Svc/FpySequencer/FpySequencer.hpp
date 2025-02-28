// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer component implementation class
// ======================================================================

#ifndef FpySequencer_HPP
#define FpySequencer_HPP

#include "FppConstantsAc.hpp"
#include "FpySequencerTypes.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Fw/Types/WaitEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/FpySequencer/FpySequencerComponentAc.hpp"
#include "Svc/FpySequencer/FpySequencer_GoalStateEnumAc.hpp"

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
        RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                       U32 cmdSeq,                        //!< The command sequence number
                       const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                       FpySequencer_BlockState block      //!< Return command status when complete or not
                       ) override;

    //! Handler for command VALIDATE
    //!
    //! Loads and validates a sequence
    void VALIDATE_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                             U32 cmdSeq,                       //!< The command sequence number
                             const Fw::CmdStringArg& fileName  //!< The name of the sequence file
                             ) override;

    //! Handler for command RUN_VALIDATED
    //!
    //! Runs a previously validated sequence
    void RUN_VALIDATED_cmdHandler(FwOpcodeType opCode,           //!< The opcode
                                  U32 cmdSeq,                    //!< The command sequence number
                                  FpySequencer_BlockState block  //!< Return command status when complete or not
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
        SmId smId,                                              //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
        ) override;

    //! Implementation for action setSequenceBlockState of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the block state of the sequence to be run
    void Svc_FpySequencer_SequencerStateMachine_action_setSequenceBlockState(
        SmId smId,                                              //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
        ) override;

    //! Implementation for action resetRuntime of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! resets the sequence runtime
    void Svc_FpySequencer_SequencerStateMachine_action_resetRuntime(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action validate of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! performs all steps necessary for sequence validation, and raises a signal
    //! result_success or result_failure
    void Svc_FpySequencer_SequencerStateMachine_action_validate(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action closeSequenceFile of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! closes the open sequence file
    void Svc_FpySequencer_SequencerStateMachine_action_closeSequenceFile(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqSucceeded of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a sequence succeeded
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqSucceeded(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqCancelled of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a sequence was cancelled
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqFailed of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a sequence failed
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqFailed(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_invalidCmd of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! warns that the user cmd was invalid
    void Svc_FpySequencer_SequencerStateMachine_action_report_invalidCmd(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_invalidSeq of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! warns that the sequence failed validation
    void Svc_FpySequencer_SequencerStateMachine_action_report_invalidSeq(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action stepStatement of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! iterates to the next statement and dispatches it
    void Svc_FpySequencer_SequencerStateMachine_action_stepStatement(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action cancelNextStepStatement of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! indicates to the component that the next call to stepStatement should
    //! cancel
    void Svc_FpySequencer_SequencerStateMachine_action_cancelNextStepStatement(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setGoalState_RUNNING of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the goal state to RUNNING
    void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setGoalState_VALID of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the goal state to VALID
    void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setGoalState_IDLE of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the goal state to IDLE
    void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_IDLE(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action cmdResponseOut_OK of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! responds to the calling command with OK
    void Svc_FpySequencer_SequencerStateMachine_action_cmdResponseOut_OK(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action cmdResponseOut_EXECUTION_ERROR of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! responds to the calling command with EXECUTION_ERROR
    void Svc_FpySequencer_SequencerStateMachine_action_cmdResponseOut_EXECUTION_ERROR(
        SmId smId,                                             //!< The state machine id
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
            SmId smId,                                             //!< The state machine id
            Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) const override;

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port cmdResponseIn
    void cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                               FwOpcodeType opCode,             //!< Command Op Code
                               U32 cmdSeq,                      //!< Command Sequence
                               const Fw::CmdResponse& response  //!< The command response argument
                               ) override;

    //! Handler for input port pingIn
    void pingIn_handler(FwIndexType portNum,  //!< The port number
                        U32 key               //!< Value to return to pinger
                        ) override;

    enum { CRC_INITIAL_VALUE = 0xFFFFFFFFU };

    // allocated at runtime
    Fw::ExternalSerializeBuffer m_sequenceBuffer;
    // id of allocator that gave us m_sequenceBuffer
    NATIVE_INT_TYPE m_allocatorId;

    // assigned by the user
    Fw::String m_sequenceFilePath;
    // used to open and read the m_sequenceFilePath
    Os::File m_sequenceFileObj;
    // the sequence, loaded in memory
    Fpy::Sequence m_sequenceObj;
    // live running computation of CRC (updated as we read)
    U32 m_computedCRC;

    // whether or not the sequence we're about to run should return immediately or
    // block on completion
    FpySequencer_BlockState m_sequenceBlockState;
    // if we are to block on completion, save the opCode and cmdSeq we should
    // return
    FwOpcodeType m_savedOpCode;
    U32 m_savedCmdSeq;

    // the goal state is the state that we're trying to reach in the sequencer
    // if it's RUNNING, then we should promptly go to RUNNING once we validate the
    // sequence. if it's VALID, we should wait after VALIDATING
    FpySequencer_GoalState m_goalState;

    // the runtime state of the sequence. encapsulates all state
    // needed to run the sequence.
    // this is distinct from the state of the sequencer. the
    // sequencer and all its state is really just a shell to load
    // and execute this runtime.
    Fpy::Runtime m_runtime;

    void allocateBuffer(NATIVE_INT_TYPE identifier, Fw::MemAllocator& allocator, NATIVE_UINT_TYPE bytes);

    void deallocateBuffer(Fw::MemAllocator& allocator);

    void initializeComputedCRC();

    void updateComputedCRC(const BYTE* buffer,          //!< The buffer
                           NATIVE_UINT_TYPE bufferSize  //!< The buffer size
    );

    void finalizeComputedCRC();

    // opens the sequence file. return true if success
    bool openSequenceFile();
    // reads the header of the sequence file. return true if success
    bool readHeader();
    // reads the body of the sequence file. return true if success
    bool readBody();
    // reads the footer of the sequence file. return true if success
    bool readFooter();

    void stepStatement();

    bool checkOpcodeIsDirective(FwOpcodeType opcode);

    // dispatches a command out via port.
    // return true if successfully dispatched.
    bool dispatchCommand(const Fpy::Statement& stmt);

    // dispatches a sequencer directive to the right handler.
    // return true if successfully handled.
    bool dispatchDirective(const Fpy::Statement& stmt);

    // ----------------------------------------------------------------------
    // Sequence directive handlers
    // return true if no error was encountered
    // ----------------------------------------------------------------------
    bool handleDirective_WAIT_REL(const Fpy::Statement& stmt);

    bool handleDirective_WAIT_ABS(const Fpy::Statement& stmt);

    void handleCmdResult(FwOpcodeType opCode,             //!< Command Op Code
                         U32 cmdSeq,                      //!< Command Sequence
                         const Fw::CmdResponse& response  //!< The command response argument
    );
};

}  // namespace Svc

#endif
