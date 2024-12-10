// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer component implementation class
// ======================================================================

#ifndef FpySequencer_HPP
#define FpySequencer_HPP

#include "FppConstantsAc.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Fw/Types/WaitEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/FpySequencer/FpySequencerComponentAc.hpp"
#include "Svc/FpySequencer/FpySequencer_CmdSequencerStateEnumAc.hpp"

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

  // ----------------------------------------------------------------------
  // state machine
  // ----------------------------------------------------------------------
  enum class State {
    // sequence is available to run a sequence
    AVAILABLE,

    // sequencer is opening the file
    OPENING,

    // sequencer is reading the file header
    READING_HEADER,

    // sequencer is reading the file body
    READING_BODY,

    // sequencer is reading the file footer
    READING_FOOTER,

    // sequencer is ready to run the file
    READY_TO_RUN,

    // sequencer is in the process of dispatching a statement from the file
    RUNNING,

    // sequencer has dispatched a statement and is awaiting its completion
    AWAITING_STATEMENT_RESPONSE,

    // sequencer was unable to open the file
    UNABLE_TO_OPEN,

    // the sequence file is invalid
    INVALID,
  } m_state;

  // try to transition to OPENING
  bool stateTo_OPENING();
  // try to transition to READING_HEADER
  bool stateTo_READING_HEADER();
  // try to transition to READING_BODY
  bool stateTo_READING_BODY();
  // try to transition to READING_FOOTER
  bool stateTo_READING_FOOTER();
  // try to transition to RUNNING
  bool stateTo_RUNNING();
  // try to transition to INVALID
  bool stateTo_INVALID();

  // called when transitioned to OPENING. return the new state
  State transtionCallback_OPENING();
  // called when transitioned to READING_HEADER. return the new state
  State transtionCallback_READING_HEADER();
  // called when transitioned to READING_BODY. return the new state
  State transitionCallback_READING_BODY();
  // called when transitioned to READING_FOOTER. return the new state
  State transitionCallback_READING_FOOTER();
  // called when transitioned to RUNNING. return the new state
  State transtionCallback_RUNNING();
  // called when transitioned to INVALID. return the new state
  State transtionCallback_INVALID();

  // ----------------------------------------------------------------------
  // end state machine
  // ----------------------------------------------------------------------

  enum {
    CRC_INITIAL_VALUE = 0xFFFFFFFFU
  };

  // assigned by the user
  const char* m_sequenceFilePath;
  // created by opening the m_sequenceFilePath
  Os::File m_sequenceFileObj;
  // live running computation of CRC (updated as we read)
  U32 m_computedCRC;

  // allocated at runtime
  Fw::ExternalSerializeBuffer m_sequenceBuffer;
  // id of allocator that gave us m_sequenceBuffer
  NATIVE_INT_TYPE m_allocatorId;

  void initializeComputedCRC();

  void updateComputedCRC(const BYTE* buffer,          //!< The buffer
                         NATIVE_UINT_TYPE bufferSize  //!< The buffer size
  );

  void finalizeComputedCRC();

};

}  // namespace Svc

#endif
