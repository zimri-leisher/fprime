#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

void FpySequencer::stepStatement() {
    // conops:
    // check should cancel
    // get next statement
    // dispatch that statement

    if (m_runtime.cancelNextStatement) {
        this->sequencer_sendSignal_result_stepStatement_cancelled();
        return;
    }

    FW_ASSERT(m_runtime.statementIndex < m_sequenceObj.header.statementCount);
    Fpy::Statement nextStatement = m_sequenceObj.statementArray[m_runtime.statementIndex];

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    if (checkOpcodeIsDirective(nextStatement.opcode)) {
        bool result = dispatchDirective(nextStatement);
        if (result) {
            this->sequencer_sendSignal_result_stepStatement_successDirective();
        } else {
            this->sequencer_sendSignal_result_stepStatement_failure();
        }
    } else {
        bool result = dispatchCommand(nextStatement);
        if (result) {
            this->sequencer_sendSignal_result_stepStatement_successDirective();
        } else {
            this->sequencer_sendSignal_result_stepStatement_failure();
        }
    }
}

// dispatches a command out via port.
// return true if successfully dispatched.
bool FpySequencer::dispatchCommand(const Fpy::Statement& stmt) {
    
}

bool FpySequencer::checkOpcodeIsDirective(FwOpcodeType opcode) {
    return opcode < Fpy::DirectiveId::MAX_DIRECTIVE_ID;
}

bool FpySequencer::dispatchDirective(const Fpy::Statement& stmt) {
    switch (stmt.opcode) {
        case Fpy::DirectiveId::WAIT_REL: {
            return handleDirective_WAIT_REL(stmt);
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            return handleDirective_WAIT_ABS(stmt);
        }
        default: {
            // unsure what this opcode is. check compiler version?
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.opcode);
            return false;
        }
    }
}

bool FpySequencer::handleDirective_WAIT_REL(const Fpy::Statement& stmt) {}

bool FpySequencer::handleDirective_WAIT_ABS(const Fpy::Statement& stmt) {}
}  // namespace Svc