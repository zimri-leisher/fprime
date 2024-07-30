// ======================================================================
// \title  SeqDispatcher/test/ut/Tester.hpp
// \author zimri.leisher
// \brief  hpp file for SeqDispatcher test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "SeqDispatcherGTestBase.hpp"
#include "Svc/SeqDispatcher/SeqDispatcher.hpp"

namespace Svc{

class Tester : public SeqDispatcherGTestBase {
  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

 public:
  // Maximum size of histories storing events, telemetry, and port outputs
  static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;
  // Instance ID supplied to the component instance under test
  static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;
  // Queue depth supplied to component instance under test
  static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;

  //! Construct object Tester
  //!
  Tester();

  //! Destroy object Tester
  //!
  ~Tester();

 public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void testDispatch();
  void testLogStatus();

 private:
  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void seqRunOut_handler(
        FwIndexType portNum, //!< The port number
        const Svc::CmdSeqInPortStrings::StringSize240& filename //!< The sequence file
  );

  void textLogIn(const FwEventIdType id,         /*!< The event ID*/
                 Fw::Time& timeTag,              /*!< The time*/
                 const Fw::LogSeverity severity, /*!< The severity*/
                 const Fw::TextLogString& text   /*!< The event string*/
  );

 private:
  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  //! Connect ports
  //!
  void connectPorts();

  //! Initialize components
  //!
  void initComponents();

 private:
  // ----------------------------------------------------------------------
  // Variables
  // ----------------------------------------------------------------------

  //! The component under test
  //!
  SeqDispatcher component;
};

}  // end namespace components

#endif
