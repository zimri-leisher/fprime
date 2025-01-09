state machine Controller {

    signal cmd_VALIDATE
    signal cmd_RUN
    signal cmd_CANCEL

    signal finish_VALIDATION

    action goToValidation


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