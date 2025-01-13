state machine Controller {

    # this state machine controls the "goal state" of the actual
    # state machine. 
    signal cmd_VALIDATE
    signal cmd_RUN
    signal cmd_CANCEL

    initial enter IDLE
    
    state IDLE {
        on cmd_VALIDATE enter VALIDATE
        on cmd_RUN enter RUN
    }

    state VALIDATE {
        on cmd_RUN enter RUN
        on cmd_CANCEL enter IDLE
    }

    state RUN {
        on cmd_CANCEL enter IDLE
    }
}