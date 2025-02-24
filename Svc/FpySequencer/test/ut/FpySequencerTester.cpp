// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  cpp file for FpySequencer test harness implementation class
// ======================================================================

#include "FpySequencerTester.hpp"

namespace Svc{

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FpySequencerTester ::FpySequencerTester()
    : FpySequencerGTestBase("FpySequencerTester", FpySequencerTester::MAX_HISTORY_SIZE),
      component("FpySequencer") {
  this->connectPorts();
  this->initComponents();
}

FpySequencerTester ::~FpySequencerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------


}  // end namespace components
