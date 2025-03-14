#include <cstring>  // for memcpy
#include "Svc/FpySequencer/FpySequencer.hpp"

namespace Svc {

//! Internal interface handler for directive_waitRel
void FpySequencer::directive_waitRel_internalInterfaceHandler(const Svc::FpySequencer_WaitRelDirective& directive) {
    Fw::Time wakeupTime = getTime();

    wakeupTime.add(directive.getduration().getSeconds(), directive.getduration().getUSeconds());
    this->sequencer_sendSignal_directiveResponse_beginSleep(wakeupTime);
}

//! Internal interface handler for directive_waitAbs
void FpySequencer::directive_waitAbs_internalInterfaceHandler(const Svc::FpySequencer_WaitAbsDirective& directive) {
    this->sequencer_sendSignal_directiveResponse_beginSleep(directive.getwakeupTime());
}

//! Internal interface handler for directive_setLocalVar
void FpySequencer::directive_setLocalVar_internalInterfaceHandler(
    const Svc::FpySequencer_SetLocalVarDirective& directive) {
    if (directive.getindex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    // coding error. should have checked this when we were deserializing the directive. prefer to crash
    // rather than just fail the sequence
    FW_ASSERT(directive.get_valueSize() <= Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE, directive.get_valueSize(),
              Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE);

    this->m_runtime.localVariables[directive.getindex()].valueSize = directive.get_valueSize();

    (void)std::memcpy(this->m_runtime.localVariables[directive.getindex()].value, directive.getvalue(),
                      directive.get_valueSize());

    this->sequencer_sendSignal_directiveResponse_success();
}

//! Internal interface handler for directive_goto
void FpySequencer::directive_goto_internalInterfaceHandler(const Svc::FpySequencer_GotoDirective& directive) {
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.getstatementIndex() > m_sequenceObj.getheader().getstatementCount()) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    m_runtime.nextStatementIndex = directive.getstatementIndex();
    this->sequencer_sendSignal_directiveResponse_success();
}

//! Internal interface handler for directive_if
void FpySequencer::directive_if_internalInterfaceHandler(const Svc::FpySequencer_IfDirective& directive) {
    if (directive.getconditionalLocalVarIndex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    if (directive.getfalseGotoStmtIndex() >= m_sequenceObj.getheader().getstatementCount()) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }

    U8* conditionalBuf = this->m_runtime.localVariables[directive.getconditionalLocalVarIndex()].value;
    FwSizeType conditionalBufSize = this->m_runtime.localVariables[directive.getconditionalLocalVarIndex()].valueSize;
    // create an esb so we don't modify the actual buf
    Fw::ExternalSerializeBuffer conditionalEsb(conditionalBuf, conditionalBufSize);
    Fw::SerializeStatus status = conditionalEsb.setBuffLen(conditionalBufSize);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);  // coding error if this fails
    bool conditional;
    status = conditionalEsb.deserialize(conditional);

    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        // failed to interpret this local variable as a boolean
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }

    if (conditional) {
        // proceed to next instruction
        this->sequencer_sendSignal_directiveResponse_success();
        return;
    }

    // conditional false case
    this->m_runtime.nextStatementIndex = directive.getfalseGotoStmtIndex();
    this->sequencer_sendSignal_directiveResponse_success();
}
//! Internal interface handler for directive_statementBufPop
void FpySequencer::directive_statementBufPop_internalInterfaceHandler(
    const Svc::FpySequencer_StatementBufPopDirective& directive) {
    // first make sure we don't have another statement already popped but not executed
    if (this->m_runtime.popStatementOnStack) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    // okay good to pop it off the stack!
    this->m_runtime.popStatementOnStack = true;
    this->m_runtime.statementOnStack.setopCode(directive.getopcode());
    this->m_runtime.statementOnStack.settype(directive.gettype());
    // dispatchStatement will handle actually doing this for us
    this->sequencer_sendSignal_directiveResponse_success();
}

//! Internal interface handler for directive_statementBufPush
void FpySequencer::directive_statementBufPush_internalInterfaceHandler(
    const Svc::FpySequencer_StatementBufPushDirective& directive) {
    // check valid local var idx
    if (directive.getlocalVarIdx() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    FwSizeType valueSize = this->m_runtime.localVariables[directive.getlocalVarIdx()].valueSize;
    const U8* valuePtr = this->m_runtime.localVariables[directive.getlocalVarIdx()].value;

    // append the local var to the stmt arg stack
    Fw::SerializeStatus status = this->m_runtime.statementOnStack.getargBuf().serialize(valuePtr, valueSize, true);

    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    this->sequencer_sendSignal_directiveResponse_success();
}
}  // namespace Svc