// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer component implementation class
// ======================================================================

#ifndef FpySequencer_HPP
#define FpySequencer_HPP

#include "Svc/FpySequencer/FpySequencerComponentAc.hpp"
#include "Svc/FpySequencer/FpySequencer_CmdSequencerStateEnumAc.hpp"
#include "FppConstantsAc.hpp"
#include "Fw/Types/WaitEnumAc.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Os/File.hpp"

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

  PRIVATE:

    enum class State {
      // sequence is available to run a sequence
      AVAILABLE,

      // sequencer is opening the file
      OPENING,

      // sequencer is validating the file
      VALIDATING,

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

    // assigned by the user
    const char* m_sequenceFilePath;

    Os::File m_sequenceFileObj;

    // try to transition to OPENING
    bool stateTo_OPENING();
    // try to transition to VALIDATING
    bool stateTo_VALIDATING();
    // try to transition to RUNNING
    bool stateTo_RUNNING();

    // called when transitioned to OPENING
    void transtionCallback_OPENING();
    // called when transitioned to VALIDATING
    void transtionCallback_VALIDATING();
    // called when transitioned to RUNNING
    void transtionCallback_RUNNING();


};

}  // end namespace components

#endif
