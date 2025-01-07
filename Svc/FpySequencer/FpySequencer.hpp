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
      //! Dispatches a sequence to the first available sequencer
      void
      RUN_cmdHandler(
          FwOpcodeType opCode,               //!< The opcode
          U32 cmdSeq,                        //!< The command sequence number
          const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
          Fw::Wait block  //!< Return command status when complete or not
      );

  //! Handler for command OPEN
  void OPEN_cmdHandler(
      FwOpcodeType opCode,              //!< The opcode
      U32 cmdSeq,                       //!< The command sequence number
      const Fw::CmdStringArg& fileName  //!< The name of the sequence file
  );

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
  void Svc_FpySequencer_StateMachine_action_setSequenceFilePath(
      SmId smId,                                     //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal,  //!< The signal
      const Fw::StringBase& value                    //!< The value
  );

  //! Implementation for action resetSequenceFilePath of state machine
  //! Svc_FpySequencer_StateMachine
  //!
  //! resets the sequence file path member
  void Svc_FpySequencer_StateMachine_action_resetSequenceFilePath(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  );

  //! Implementation for action openSequenceFile of state machine
  //! Svc_FpySequencer_StateMachine
  //!
  //! opens the file descriptor
  void Svc_FpySequencer_StateMachine_action_openSequenceFile(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  );

  //! Implementation for action readHeader of state machine
  //! Svc_FpySequencer_StateMachine
  void Svc_FpySequencer_StateMachine_action_readHeader(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  );

  //! Implementation for action readBody of state machine
  //! Svc_FpySequencer_StateMachine
  void Svc_FpySequencer_StateMachine_action_readBody(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  );

  //! Implementation for action readFooter of state machine
  //! Svc_FpySequencer_StateMachine
  void Svc_FpySequencer_StateMachine_action_readFooter(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  );

  PROTECTED :

      // ----------------------------------------------------------------------
      // Functions to implement for internal state machine guards
      // ----------------------------------------------------------------------

      //! Implementation for guard sequenceFileExists of state machine
      //! Svc_FpySequencer_StateMachine
      //!
      //! check whether the sequence file exists
      bool
      Svc_FpySequencer_StateMachine_guard_sequenceFileExists(
          SmId smId,  //!< The state machine id
          Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
      ) const;

  //! Implementation for guard sequenceFileOpen of state machine
  //! Svc_FpySequencer_StateMachine
  //!
  //! check whether the sequence file is open
  bool Svc_FpySequencer_StateMachine_guard_sequenceFileOpen(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  ) const;

  //! Implementation for guard headerValid of state machine
  //! Svc_FpySequencer_StateMachine
  //!
  //! check whether the header is valid
  bool Svc_FpySequencer_StateMachine_guard_headerValid(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  ) const;

  //! Implementation for guard bodyValid of state machine
  //! Svc_FpySequencer_StateMachine
  bool Svc_FpySequencer_StateMachine_guard_bodyValid(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  ) const;

  //! Implementation for guard footerValid of state machine
  //! Svc_FpySequencer_StateMachine
  bool Svc_FpySequencer_StateMachine_guard_footerValid(
      SmId smId,                                    //!< The state machine id
      Svc_FpySequencer_StateMachine::Signal signal  //!< The signal
  ) const;

  enum { CRC_INITIAL_VALUE = 0xFFFFFFFFU };

  // assigned by the user
  Fw::String m_sequenceFilePath;
  // created by opening the m_sequenceFilePath
  Os::File m_sequenceFileObj;
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
