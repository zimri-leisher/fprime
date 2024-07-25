// ======================================================================
// \title  SeqDispatcher.hpp
// \author zimri.leisher
// \brief  hpp file for SeqDispatcher component implementation class
// ======================================================================

#ifndef SeqDispatcher_HPP
#define SeqDispatcher_HPP

#include "Svc/SeqDispatcher/SeqDispatcherComponentAc.hpp"
#include "Svc/SeqDispatcher/SeqDispatcher_CmdSequencerStateEnumAc.hpp"
#include "FppConstantsAc.hpp"
#include "Fw/Types/WaitEnumAc.hpp"

namespace Svc {

class SeqDispatcher : public SeqDispatcherComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object SeqDispatcher
    //!
    SeqDispatcher(const char* const compName /*!< The component name*/
    );

    //! Destroy object SeqDispatcher
    //!
    ~SeqDispatcher();

    //!  \brief Component initialization routine
    //!
    //!  The initialization function calls the initialization
    //!  routine for the base class.
    //!
    //!  \param queueDepth the depth of the message queue for the component
    void init(NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
              NATIVE_INT_TYPE instance    /*!< The instance number*/
    );                                    //!< initialization function

  PROTECTED:

    //! Handler for input port seqDoneIn
    void
    seqDoneIn_handler(NATIVE_INT_TYPE portNum,         //!< The port number
                      FwOpcodeType opCode,             //!< Command Op Code
                      U32 cmdSeq,                      //!< Command Sequence
                      const Fw::CmdResponse& response  //!< The command response argument
    );

    //! Handler for input port seqStartIn
    void seqStartIn_handler(NATIVE_INT_TYPE portNum,  //!< The port number
                            Fw::String& fileName      //!< The sequence file name
    );

    //! Handler for input port seqRunIn
    void seqRunIn_handler(NATIVE_INT_TYPE portNum,  //!< The port number
                          Fw::String& fileName);

  PRIVATE:

    // number of sequences dispatched (successful or otherwise)
    U32 m_dispatchedCount = 0;
    // number of errors from dispatched sequences (CmdResponse::EXECUTION_ERROR)
    U32 m_errorCount = 0;
    // number of sequencers in state AVAILABLE
    U32 m_sequencersAvailable = SeqDispatcherSequencerPorts;

    struct DispatchEntry {
        FwOpcodeType opCode;  //!< opcode of entry
        U32 cmdSeq;
        // store the state of each sequencer
        SeqDispatcher_CmdSequencerState state;
        // store the sequence currently running for each sequencer
        Fw::String sequenceRunning = "<no seq>";
    } m_entryTable[SeqDispatcherSequencerPorts];  //!< table of dispatch
                                                  //!< entries

    NATIVE_INT_TYPE getNextAvailableSequencerIdx();

    bool runSequence(NATIVE_INT_TYPE sequencerIdx, const Fw::String& fileName, Fw::Wait block);

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    //! Implementation for RUN command handler
    //!
    void RUN_cmdHandler(const FwOpcodeType opCode,        /*!< The opcode*/
                        const U32 cmdSeq,                 /*!< The command sequence number*/
                        const Fw::CmdStringArg& fileName, /*!< The name of the sequence file*/
                        Fw::Wait block);

    void LOG_STATUS_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                               const U32 cmdSeq);         /*!< The command sequence number*/
};

}  // end namespace components

#endif
