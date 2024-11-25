// ======================================================================
// \title  FpySequencer.cpp
// \author zimri.leisher
// \brief  cpp file for FpySequencer component implementation class
// ======================================================================

#include <Svc/FpySequencer/FpySequencer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

FpySequencer ::FpySequencer(const char* const compName)
    : FpySequencerComponentBase(compName) {}

FpySequencer ::~FpySequencer() {}

// try to transition to OPENING
bool FpySequencer::stateTo_OPENING() {
  m_state = State::OPENING;

  transtionCallback_OPENING();
  return true;
}
// try to transition to VALIDATING
bool FpySequencer::stateTo_VALIDATING() {
  m_state = State::VALIDATING;

  transtionCallback_VALIDATING();
  return true;
}
// try to transition to RUNNING
bool FpySequencer::stateTo_RUNNING() {
  m_state = State::RUNNING;

  transtionCallback_RUNNING();
  return true;
}

void FpySequencer::transtionCallback_OPENING() {
  Os::File::Status status = this->m_sequenceFileObj.open(
      this->m_sequenceFilePath.toChar(), Os::File::OPEN_READ);

  if (status == Os::File::OP_OK) {
    stateTo_VALIDATING();
  } else if (status == Os::File::DOESNT_EXIST) {
    this->m_events.fileNotFound();
    // stateTo_INVALID();
  } else {
    this->m_events.fileReadError();
    // stateTo_INVALID();
  }

  this->m_sequenceFile.close();
}

// called when transitioned to VALIDATING
void FpySequencer::transtionCallback_VALIDATING() {
  
}
// called when transitioned to RUNNING
void FpySequencer::transtionCallback_RUNNING() {

}
}  // namespace Svc
