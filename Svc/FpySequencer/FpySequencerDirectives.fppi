struct WaitRelDirective {
    duration: Fw.TimeInterval
}

struct WaitAbsDirective {
    wakeupTime: Fw.Time
}

# this directive should not be directly serialize/deserialized
struct SetLocalVarDirective {
    @ the index of the local variable to set
    index: U8
    
    @ the value of the local variable
    value: [Fpy.MAX_LOCAL_VARIABLE_VALUE_SIZE] U8

    @ the length of the value byte array. 
    # note, the underscore signifies this field is implied and 
    # is not actually present in the binary form of the directive
    _valueSize: FwSizeType
}

struct GotoDirective {
    @ the statement index to execute next
    statementIndex: U32
}

struct IfDirective {
    @ the local variable to interpret as a boolean
    conditionalLocalVarIndex: U8
    @ the statement index to go to if the local variable is false
    falseGotoStmtIndex: U32
    # this directive will not goto anywhere if the variable is true
}

@ pushes a value from a local variable onto a buf containing
@ opcode/arguments for a statement we're about to send
struct StatementBufPushDirective {
    @ the local variable containing the value to push to the stmt buf
    localVarIdx: U8
}

@ pops everything off the statement buf we've been building and dispatches
@ it on the next step, directly after this directive. uses the given stmt
@ type and opcode
struct StatementBufPopDirective {
    $type: Fpy.StatementType
    $opcode: FwOpcodeType
}

struct GetTlmDirective {
    chanId: FwChanIdType
    # underscore signifies field is implied, and not actually present in this form
    # in the seq binary
    _destVarIdx: U8
    _getTime: bool
}

struct GetPrmValDirective {
    prmId: FwPrmIdType
    localVarIdx: U8
}

enum NumericType {
    U_64 = 0
}

struct EqualityOpDirective {
    lhsIdx: U8
    rhsIdx: U8
    resultIdx: U8
    _lhsType: NumericType
    _rhsType: NumericType
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_setLocalVar(directive: SetLocalVarDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert

internal port directive_statementBufPush(directive: StatementBufPushDirective) priority 6 assert

internal port directive_statementBufPop(directive: StatementBufPopDirective) priority 6 assert

internal port directive_getTlm(directive: GetTlmDirective) priority 6 assert

internal port directive_getPrmVal(directive: GetPrmValDirective) priority 6 assert

internal port directive_equalityOp(directive: EqualityOpDirective) priority 6 assert