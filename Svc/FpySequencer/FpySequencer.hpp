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

//   //! sets the current sequence file path member
//   void action_setSequenceFilePath(Signal signal,               //!< The signal
//                                   const Fw::StringBase& value  //!< The value
//   );

//   //! resets the sequence file path member
//   void action_resetSequenceFilePath(Signal signal  //!< The signal
//   );

//   void action_resetSequenceFileObj(Signal signal  //!< The signal
//   );

//   void action_validateSequence(Signal signal  //!< The signal
//   );

//   void action_reportValidationError(
//       Signal signal,                                  //!< The signal
//       const Svc::FpySequencer_ValidationError& value  //!< The value
//   );

//   void action_stepStatement(Signal signal  //!< The signal
//   );

//   PROTECTED :

//       // ----------------------------------------------------------------------
//       // Guards
//       // ----------------------------------------------------------------------

//       //! check whether the sequence file exists
//       bool
//       guard_sequenceFileExists(Signal signal  //!< The signal
//       ) const;

//   //! check whether the sequence file is open
//   bool guard_sequenceFileOpen(Signal signal  //!< The signal
//   ) const;

//   //! check whether the header is valid
//   bool guard_headerValid(Signal signal  //!< The signal
//   ) const;

//   bool guard_bodyValid(Signal signal  //!< The signal
//   ) const;

//   bool guard_footerValid(Signal signal  //!< The signal
//   ) const;

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
