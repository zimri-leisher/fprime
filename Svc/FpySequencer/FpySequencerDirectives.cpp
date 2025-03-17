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
    FW_ASSERT(directive.get_valueSize() <= Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE,
              static_cast<FwAssertArgType>(directive.get_valueSize()),
              static_cast<FwAssertArgType>(Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE));

    this->m_runtime.localVariables[directive.getindex()].valueSize = directive.get_valueSize();

    (void)memcpy(this->m_runtime.localVariables[directive.getindex()].value, directive.getvalue(),
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

//! Internal interface handler for directive_getTlm
void FpySequencer::directive_getTlm_internalInterfaceHandler(const Svc::FpySequencer_GetTlmDirective& directive) {
    if (directive.get_destVarIdx() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }

    Fw::Time tlmTime;
    Fw::TlmBuffer tlmValue;
    // start off empty
    tlmValue.setBuffLen(0);

    FW_ASSERT(this->isConnected_tlmGet_OutputPort(0));
    this->tlmGet_out(0, directive.getchanId(), tlmTime, tlmValue);

    if (tlmValue.getBuffLength() == 0) {
        // the assumption that this is a failure should remain true
        // as long as no zero-size types are implemented in fprime

        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }

    if (!directive.get_getTime()) {
        // check telemetry value small enough to fit in a local variable
        FW_ASSERT(tlmValue.getBuffLength() <= Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE,
                  static_cast<FwAssertArgType>(tlmValue.getBuffLength()),
                  static_cast<FwAssertArgType>(Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE));

        this->m_runtime.localVariables[directive.get_destVarIdx()].valueSize = tlmValue.getBuffLength();

        (void)memcpy(this->m_runtime.localVariables[directive.get_destVarIdx()].value, tlmValue.getBuffAddr(),
                     tlmValue.getBuffLength());
    } else {
        // store time
        Fw::ExternalSerializeBuffer varBuf(this->m_runtime.localVariables[directive.get_destVarIdx()].value,
                                           Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE);
        // start anew
        varBuf.setBuffLen(0);

        Fw::SerializeStatus status = varBuf.serialize(tlmTime);

        if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
            this->sequencer_sendSignal_directiveResponse_failure();
            return;
        }

        this->m_runtime.localVariables[directive.get_destVarIdx()].valueSize = varBuf.getBuffLength();
    }

    this->sequencer_sendSignal_directiveResponse_success();
}

//! Internal interface handler for directive_getPrmVal
void FpySequencer::directive_getPrmVal_internalInterfaceHandler(const Svc::FpySequencer_GetPrmValDirective& directive) {
    if (directive.getlocalVarIdx() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }

    Fw::ParamBuffer prmValue;
    // start off empty
    prmValue.setBuffLen(0);

    FW_ASSERT(this->isConnected_prmGet_OutputPort(0));

    Fw::ParamValid valid = this->prmGet_out(0, directive.getprmId(), prmValue);

    if (valid != Fw::ParamValid::VALID) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }

    // telemetry value is too big to fit in a local variable
    FW_ASSERT(prmValue.getBuffLength() <= Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE,
              static_cast<FwAssertArgType>(prmValue.getBuffLength()),
              static_cast<FwAssertArgType>(Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE));

    this->m_runtime.localVariables[directive.getlocalVarIdx()].valueSize = prmValue.getBuffLength();

    (void)memcpy(this->m_runtime.localVariables[directive.getlocalVarIdx()].value, prmValue.getBuffAddr(),
                 prmValue.getBuffLength());

    this->sequencer_sendSignal_directiveResponse_success();
}

//! Internal interface handler for directive_equalityOp
void FpySequencer::directive_equalityOp_internalInterfaceHandler(
    const Svc::FpySequencer_EqualityOpDirective& directive) {
    if (directive.getlhsIdx() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    if (directive.getrhsIdx() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    // only these two supported rn
    FW_ASSERT(directive.get_lhsType() == FpySequencer_NumericType::U_64 &&
              directive.get_rhsType() == FpySequencer_NumericType::U_64);

    U64 lhs;
    Fw::ExternalSerializeBuffer lhsBuf(this->m_runtime.localVariables[directive.getlhsIdx()].value,
                                       this->m_runtime.localVariables[directive.getlhsIdx()].valueSize);
    lhsBuf.setBuffLen(lhsBuf.getBuffCapacity());
    Fw::SerializeStatus status = lhsBuf.deserialize(lhs);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    U64 rhs;
    Fw::ExternalSerializeBuffer rhsBuf(this->m_runtime.localVariables[directive.getrhsIdx()].value,
                                       this->m_runtime.localVariables[directive.getrhsIdx()].valueSize);
    rhsBuf.setBuffLen(rhsBuf.getBuffCapacity());
    status = rhsBuf.deserialize(rhs);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }

    bool result = rhs == lhs;
    Fw::ExternalSerializeBuffer resultBuf(this->m_runtime.localVariables[directive.getresultIdx()].value,
                                          Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE);

    resultBuf.setBuffLen(0);
    status = resultBuf.serialize(result);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->sequencer_sendSignal_directiveResponse_failure();
        return;
    }
    this->sequencer_sendSignal_directiveResponse_success();
}

}  // namespace Svc