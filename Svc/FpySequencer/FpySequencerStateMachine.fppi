state machine StateMachine {
    ####################
    # guards           #
    ####################
    guard goalStateIs_RUN
    guard statementWasDirective: StatementType
    guard successfulResponse: StatementResponse

    ####################
    # signals          #
    ####################
    signal cmd_VALIDATE: string
    signal cmd_RUN
    signal cmd_CANCEL

    signal sequenceValid
    signal sequenceInvalid: ValidationError

    signal step_failure: StatementStepError
    signal step_success: StatementType
    signal step_noMoreStatements

    signal statementFinished: StatementResponse

    ####################
    # actions          #
    ####################
    @ sets the current sequence file path member
    action setSequenceFilePath: string
    @ resets the sequence file path member
    action resetSequenceFile
    action validateSequence

    action reportError_validation: ValidationError
    action reportError_statementStep: StatementStepError
    action reportError_statementResponse: StatementResponse

    action stepStatement

    ####################
    # topology         #
    ####################
    initial enter AVAILABLE

    @ sequencer is ready to load and run a sequence
    state AVAILABLE {
        @ start with an unset sequence file
        entry do { resetSequenceFile }

        on cmd_VALIDATE do { setSequenceFilePath } enter VALIDATION
    }

    state VALIDATION {
        entry do { validateSequence }
        # -->
        on sequenceValid enter VALID
        on sequenceInvalid do { reportError_validation } enter AVAILABLE
    }

    choice WAIT_FOR_CMD_OR_RUN {
        # if we already received the RUN cmd, move on to RUN. otherwise wait
        if goalStateIs_RUNNING enter RUNNING else enter VALID
    }

    state VALID {
        # wait here until we get the RUN cmd
        on cmd_RUN enter RUNNING
    }

    state RUNNING {
        initial enter STEP_STATEMENT

        state STEP_STATEMENT {
            entry do { stepStatement }
            # -->
            on step_noMoreStatements enter AVAILABLE
            on step_failure do { reportError_statementStep } enter AVAILABLE
            on step_success enter DIRECTIVE_OR_CMD
        }

        choice DIRECTIVE_OR_CMD {
            if statementWasDirective enter AWAIT_DIRECTIVE else enter AWAIT_CMD
        }

        # i have these separated at the request of Canham
        state AWAIT_DIRECTIVE {
            on statementFinished enter STATEMENT_FINISHED
        }

        state AWAIT_CMD {
            on statementFinished enter STATEMENT_FINISHED
        }

        choice STATEMENT_FINISHED {
            if successfulResponse enter STEP_STATEMENT else do { reportError_statementResponse } enter AVAILABLE
        }

        on cmd_CANCEL enter AVAILABLE
    }
}
