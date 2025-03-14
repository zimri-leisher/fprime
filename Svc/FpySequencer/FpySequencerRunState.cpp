#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Svc/FpySequencer/StatementTypeEnumAc.hpp"
namespace Svc {
const Fpy::Statement* FpySequencer::getNextStatement() {
    if (m_runtime.popStatementOnStack) {
        // we have a dynamically built statement to pop off the stack
        m_runtime.popStatementOnStack = false;
        return &m_runtime.statementOnStack;
    }

    if (m_runtime.nextStatementIndex == m_sequenceObj.getheader().getstatementCount()) {
        return nullptr;
    }

    // no dynamically built statement, just go to the next one in the file
    // check to make sure no array out of bounds (coding error if so)
    FW_ASSERT(m_runtime.nextStatementIndex < m_sequenceObj.getheader().getstatementCount());

    const Fpy::Statement& nextStatement = m_sequenceObj.getstatements()[m_runtime.nextStatementIndex];
    m_runtime.nextStatementIndex++;

    return &nextStatement;
}

void FpySequencer::dispatchStatement() {
    const Fpy::Statement* nextStatementPtr = this->getNextStatement();

    if (nextStatementPtr == nullptr) {
        this->sequencer_sendSignal_result_dispatchStatement_noMoreStatements();
        return;
    }

    const Fpy::Statement& nextStatement = *nextStatementPtr;

    m_runtime.currentStatementOpcode = nextStatement.getopCode();

    bool result;

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    if (nextStatement.gettype() == Fpy::StatementType::DIRECTIVE) {
        // the problem is that this is both parsing and completely executing
        // the directive. i think we need to split it up into parsing and
        // executing. by the time this line executes, the directive could be completely done
        result = dispatchDirective(nextStatement);
    } else {
        // whereas this one just sends the cmd out. there's no chance we see the signal
        // come in until this func finishes, cuz it has to go on the queue
        result = dispatchCommand(nextStatement);
    }

    if (result) {
        m_tlm.statementsDispatched++;
        this->sequencer_sendSignal_result_dispatchStatement_success();
    } else {
        this->sequencer_sendSignal_result_dispatchStatement_failure();
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
    stat = cmdBuf.serialize(stmt.getopCode());
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(), sizeof(stmt.getopCode()),
                                            stat);
        return false;
    }
    stat = cmdBuf.serialize(stmt.getargBuf().getBuffAddr(), stmt.getargBuf().getBuffLength(), true);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerializeError(cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                            stmt.getargBuf().getBuffLength(), stat);
        return false;
    }

    // little note--theoretically this could produce a cmdResponse before we send the
    // dispatchSuccess signal. however b/c of priorities the dispatchSuccess signal will
    // always get processed first, leaving us in the right state for the cmdresponse
    this->cmdOut_out(0, cmdBuf, 0);

    return true;
}

bool FpySequencer::dispatchDirective(const Fpy::Statement& stmt) {
    // make our own esb so we can deser from stmt without breaking its constness
    Fw::ExternalSerializeBuffer argBuf(const_cast<U8*>(stmt.getargBuf().getBuffAddr()),
                                       stmt.getargBuf().getBuffLength());
    Fw::SerializeStatus status = argBuf.setBuffLen(stmt.getargBuf().getBuffLength());
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);

    switch (stmt.getopCode()) {
        case Fpy::DirectiveId::WAIT_REL: {
            FpySequencer_WaitRelDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }
            directive_waitRel_internalInterfaceInvoke(directive);
            break;
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            FpySequencer_WaitAbsDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }
            directive_waitAbs_internalInterfaceInvoke(directive);
            break;
        }
        case Fpy::DirectiveId::SET_LOCAL_VAR: {
            // set local var has some custom deserialization behavior
            // we don't write a custom class for it though because that deserialization behavior only
            // applies for the initial time we deserialize it out of the statement

            // the behavior in question is that it will grab the entire remaining part of the statement
            // arg buf. that is, it uses the remaining length of the statement arg buf to determine the length
            // of its value buf. this way we get to save on serializing the value length

            // TODO do some trades on the best way to do this. not confident on this one
            FpySequencer_SetLocalVarDirective directive;

            // first deserialize the index
            U8 index;
            status = argBuf.deserialize(index);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }
            directive.setindex(index);

            // okay, now deserialize the remaining bytes in the stmt arg buf into the value buf

            //  how many bytes are left?
            FwSizeType valueSize = argBuf.getBuffLeft();

            // check to make sure the value will fit
            if (valueSize > Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(),
                                                               Fw::SerializeStatus::FW_SERIALIZE_FORMAT_ERROR,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return false;
            }

            // okay, it will fit. put it in
            status = argBuf.deserialize(directive.getvalue(), valueSize, true);

            // now we can fail if there is any buf left
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }

            // and set the buf size now that we know it
            directive.set_valueSize(valueSize);

            directive_setLocalVar_internalInterfaceInvoke(directive);
            break;
        }
        case Fpy::DirectiveId::GOTO: {
            FpySequencer_GotoDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }
            directive_goto_internalInterfaceInvoke(directive);
            break;
        }
        case Fpy::DirectiveId::IF: {
            FpySequencer_IfDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }
            directive_if_internalInterfaceInvoke(directive);
            break;
        }
        case Fpy::DirectiveId::STATEMENT_BUF_PUSH: {
            FpySequencer_StatementBufPushDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }
            directive_statementBufPush_internalInterfaceInvoke(directive);
            break;
        }
        case Fpy::DirectiveId::STATEMENT_BUF_POP: {
            FpySequencer_StatementBufPopDirective directive;
            status = argBuf.deserialize(directive);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), status, argBuf.getBuffLeft(),
                                                               argBuf.getBuffLength());
                return false;
            }
            directive_statementBufPop_internalInterfaceInvoke(directive);
            break;
        }
        default: {
            // unsure what this opcode is. check compiler version matches sequencer
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.getopCode());
            return false;
        }
    }
    return true;
}

}  // namespace Svc