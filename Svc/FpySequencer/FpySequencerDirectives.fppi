struct WaitRelDirective {
    duration: Fw.TimeInterval
}

struct WaitAbsDirective {
    wakeupTime: Fw.Time
}

struct SetLocalVarDirective {
    @ the index of the local variable to set
    index: U8
    @ the value of the local variable
    value: Fw.StatementArgBuffer
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
    # if directive will not goto anywhere if the variable is true
}

internal port directive_waitRel(directive: WaitRelDirective) priority 6 assert

internal port directive_waitAbs(directive: WaitAbsDirective) priority 6 assert

internal port directive_setLocalVar(directive: SetLocalVarDirective) priority 6 assert

internal port directive_goto(directive: GotoDirective) priority 6 assert

internal port directive_if(directive: IfDirective) priority 6 assert