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
void FpySequencer::directive_setLocalVar_internalInterfaceHandler(const Svc::FpySequencer_SetLocalVarDirective& directive) {
    if (directive.getindex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    this->m_runtime.localVariables[directive.getindex()] = directive.getvalue();
    this->sequencer_sendSignal_directiveResponse_success();
}

//! Internal interface handler for directive_goto
void FpySequencer::directive_goto_internalInterfaceHandler(const Svc::FpySequencer_GotoDirective& directive) {
    // check within sequence bounds
    if (directive.getstatementIndex() >= m_sequenceObj.getheader().getstatementCount()) {
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

    Fw::StatementArgBuffer& conditionalBuf = m_runtime.localVariables[directive.getconditionalLocalVarIndex()];
    // create an esb so we don't modify the actual buf
    Fw::ExternalSerializeBuffer conditionalEsb(conditionalBuf.getBuffAddr(), conditionalBuf.getBuffLength());
    conditionalEsb.setBuffLen(conditionalBuf.getBuffLength());
    bool conditional;
    conditionalEsb.deserialize(conditional);
    
    if (conditional) {
        this->sequencer_sendSignal_directiveResponse_success();
        return;
    }

    // conditional false case
    this->m_runtime.nextStatementIndex = directive.getfalseGotoStmtIndex();
    this->sequencer_sendSignal_directiveResponse_success();
}
}  // namespace Svc