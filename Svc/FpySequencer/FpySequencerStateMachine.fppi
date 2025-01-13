state machine SequencerStateMachine {
    ####################
    # guards           #
    ####################
    guard goalStateIs_RUNNING
    guard statementWasDirective: StatementType
    guard shouldCancel

    ####################
    # signals          #
    ####################
    signal cmd_VALIDATE: string
    signal cmd_RUN: string
    signal cmd_RUN_VALIDATED
    signal cmd_CANCEL

    signal stepStatement_successCmd
    signal stepStatement_successDirective
    signal stepStatement_failure: StepStatementFailure
    signal stepStatement_noMoreStatements
    signal stepStatement_cancelled

    signal sequenceValid
    signal sequenceInvalid
    signal statementFinished_success
    signal statementFinished_failure: StatementFinishedFailure
    signal openSequenceFile_success
    signal openSequenceFile_failure: OpenSequenceFileFailure
    signal readHeader_success
    signal readHeader_failure: ReadHeaderFailure
    signal readBody_success
    signal readBody_failure: ReadBodyFailure
    signal readFooter_success
    signal readFooter_failure: ReadFooterFailure

    ####################
    # actions          #
    ####################
    @ sets the current sequence file path member
    action setSequenceFilePath: string
    @ resets the sequence file path member
    action resetSequenceFile
    action validateSequence
    action openSequenceFile
    action readHeader
    action readBody
    action readFooter

    action reportError_stepStatement: StepStatementFailure
    action reportError_statementResponse: StatementResponse
    action reportError_openSequenceFile: OpenSequenceFileFailure
    action reportError_readHeader: ReadHeaderFailure
    action reportError_readBody: ReadBodyFailure
    action reportError_readFooter: ReadFooterFailure
    action reportError_statementFinished: StatementFinishedFailure

    action report_seqDone

    action stepStatement
    
    action cancelNextStepStatement

    action setGoalState_RUNNING
    action setGoalState_VALID

    ####################
    # topology         #
    ####################
    initial enter IDLE

    @ sequencer is ready to load and run a sequence
    state IDLE {
        @ start with an unset sequence file
        entry do { resetSequenceFile }
        # -->
        # WAIT

        on cmd_VALIDATE do { setGoalState_VALID, setSequenceFilePath } enter VALIDATION
        on cmd_RUN do { setGoalState_RUNNING, setSequenceFilePath } enter VALIDATION
    }

    state VALIDATION {

        initial enter OPEN_SEQUENCE_FILE

        state OPEN_SEQUENCE_FILE {
            entry do { openSequenceFile }
            # -->
            on openSequenceFile_success enter READ_HEADER
            on openSequenceFile_failure do { reportError_openSequenceFile } enter IDLE
        }

        state READ_HEADER {
            entry do { readHeader }
            # -->
            on readHeader_success enter READ_BODY
            on readHeader_failure do { reportError_readHeader } enter IDLE
        }

        state READ_BODY {
            entry do { readBody }
            # -->
            on readBody_success enter READ_FOOTER
            on readBody_failure do { reportError_readBody } enter IDLE
        }

        state READ_FOOTER {
            entry do { readFooter }
            # -->
            on readFooter_success enter WAIT_FOR_CMD_OR_RUN
            on readFooter_failure do { reportError_readFooter } enter IDLE
        }
    }

    choice WAIT_FOR_CMD_OR_RUN {
        # if we already received the RUN cmd, move on to RUNNING. otherwise wait
        if goalStateIs_RUNNING enter RUNNING else enter AWAITING_CONTINUE_CMD
    }

    state AWAITING_CONTINUE_CMD {
        # wait here until we get the RUN_VALIDATED cmd
        on cmd_RUN_VALIDATED enter RUNNING
        on cmd_CANCEL enter IDLE
    }

    state RUNNING {
        initial enter STEP_STATEMENT

        state STEP_STATEMENT {
            entry do { stepStatement }
            # -->
            on stepStatement_cancelled enter IDLE
            on stepStatement_noMoreStatements do { report_seqDone } enter IDLE
            on stepStatement_failure do { reportError_stepStatement } enter IDLE
            on stepStatement_successCmd enter AWAIT_CMD
            on stepStatement_successDirective enter AWAIT_DIRECTIVE
        }
        
        state AWAIT_DIRECTIVE {
            on statementFinished_success enter STEP_STATEMENT
            on statementFinished_failure do { reportError_statementFinished } enter IDLE
        }

        state AWAIT_CMD {
            on statementFinished_success enter STEP_STATEMENT
            on statementFinished_failure do { reportError_statementFinished } enter IDLE
        }

        on cmd_CANCEL do { cancelNextStepStatement }
    }
}
