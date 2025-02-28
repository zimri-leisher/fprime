#include "Fw/Com/ComPacket.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

void FpySequencer::stepStatement() {
    // conops:
    // check should cancel
    // check no more statements
    // get next statement
    // dispatch that statement

    if (m_runtime.cancelNextStatement) {
        this->sequencer_sendSignal_result_stepStatement_cancelled();
        return;
    }

    if (m_runtime.statementIndex == m_sequenceObj.header.statementCount) {
        this->sequencer_sendSignal_result_stepStatement_noMoreStatements();
        return;
    }

    // check to make sure no array out of bounds
    FW_ASSERT(m_runtime.statementIndex < m_sequenceObj.header.statementCount);
    Fpy::Statement nextStatement = m_sequenceObj.statementArray[m_runtime.statementIndex];

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    bool isDirective = checkOpcodeIsDirective(nextStatement.opcode);
    bool result = false;

    if (isDirective) {
        result = dispatchDirective(nextStatement);
    } else {
        result = dispatchCommand(nextStatement);
    }

    m_runtime.statementIndex++;

    m_runtime.currentStatementOpcode = nextStatement.opcode;

    if (result) {
        if (isDirective) {
            this->sequencer_sendSignal_result_stepStatement_successDirective();
        } else {
            this->sequencer_sendSignal_result_stepStatement_successCmd();
        }
    } else {
        this->sequencer_sendSignal_result_stepStatement_failure();
    }
}

// dispatches a command out via port.
// return true if successfully dispatched.
bool FpySequencer::dispatchCommand(const Fpy::Statement& stmt) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat = cmdBuf.serialize(Fw::ComPacket::FW_PACKET_COMMAND);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                            sizeof(Fw::ComPacket::FW_PACKET_COMMAND), stat);
        return false;
    }
    stat = cmdBuf.serialize(stmt.opcode);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(), sizeof(stmt.opcode),
                                            stat);
        return false;
    }
    stat = cmdBuf.serialize(stmt.args.getBuffAddr(), stmt.args.getBuffLength(), true);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(), stmt.args.getBuffLength(),
                                            stat);
        return false;
    }
    this->cmdOut_out(0, cmdBuf, 0);
    return true;
}

void FpySequencer::handleCmdResult(FwOpcodeType opCode,             //!< Command Op Code
                                   U32 cmdSeq,                      //!< Command Sequence
                                   const Fw::CmdResponse& response  //!< The command response argument
) {
    if (opCode != this->m_runtime.currentStatementOpcode) {
        // just got an opcode back for a cmd that we didn't expect
        
    }
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