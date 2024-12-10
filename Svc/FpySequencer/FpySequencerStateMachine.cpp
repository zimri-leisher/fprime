#include <Svc/FpySequencer/FpySequencer.hpp>
// try to transition to OPENING
bool FpySequencer::stateTo_OPENING() {
  m_state = State::OPENING;

  transtionCallback_OPENING();
  return true;
}

// try to transition to READING_HEADER
bool FpySequencer::stateTo_READING_HEADER() {
  m_state = State::READING_HEADER;

  transtionCallback_READING_HEADER();
  return true;
}

// try to transition to READING_BODY
bool FpySequencer::stateTo_READING_BODY() {
  m_state = State::READING_BODY;

  transtionCallback_READING_BODY();
  return true;
}

// try to transition to READING_FOOTER
bool FpySequencer::stateTo_READING_FOOTER() {
  m_state = State::READING_FOOTER;

  transtionCallback_READING_FOOTER();
  return true;
}

// try to transition to RUNNING
bool FpySequencer::stateTo_RUNNING() {
  m_state = State::RUNNING;

  transtionCallback_RUNNING();
  return true;
}

// try to transition to INVALID
bool FpySequencer::stateTo_INVALID() {
  m_state = State::INVALID;

  transtionCallback_INVALID();
  return true;
}
