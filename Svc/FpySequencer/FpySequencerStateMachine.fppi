state machine SequencerStateMachine {
    ####################
    # guards           #
    ####################
    @ return true if the goal state is RUNNING
    guard goalStateIs_RUNNING

    ####################
    # signals          #
    ####################
    @ called on VALIDATE cmd with the path of the sequence file to validate
    signal cmd_VALIDATE: SequenceExecutionArgs
    @ called on RUN cmd with the path of the sequence file to run
    signal cmd_RUN: SequenceExecutionArgs
    @ called on RUN_VALIDATED cmd
    signal cmd_RUN_VALIDATED
    @ called on CANCEL cmd
    signal cmd_CANCEL

    @ called in stepStatement method when a command was successfully sent to the cmd disp
    signal stepStatement_successCmd
    @ called in stepStatement method when a directive was sent to this sequencer
    signal stepStatement_successDirective
    @ called in stepStatement method when a statement was unable to be sent out
    signal stepStatement_failure: StepStatementFailure
    @ called in stepStatement method when there were no more statements in the sequence
    signal stepStatement_noMoreStatements
    @ called in stepStatement method when the sequence execution was cancelled
    signal stepStatement_cancelled

    @ called in statementFinished method when a statement was successfully executed
    signal statementFinished_success
    @ called in statementFinished method when a statement was unsuccessfully executed
    signal statementFinished_failure: StatementFinishedFailure
    @ called in openSequenceFile method when the sequence file was successfully opened
    signal openSequenceFile_success
    @ called in openSequenceFile method when the sequence file was unsuccessfully opened
    signal openSequenceFile_failure: OpenSequenceFileFailure
    @ called in readHeader method when the header was successfully read
    signal readHeader_success
    @ called in readHeader method when the header was unsuccessfully read
    signal readHeader_failure: ReadHeaderFailure
    @ called in readBody method when the body was successfully read
    signal readBody_success
    @ called in readBody method when the body was unsuccessfully read
    signal readBody_failure: ReadBodyFailure
    @ called in readFooter method when the footer was successfully read
    signal readFooter_success
    @ called in readFooter method when the footer was unsuccessfully read
    signal readFooter_failure: ReadFooterFailure

    ####################
    # actions          #
    ####################
    @ sets the current sequence file path member var
    action setSequenceFilePath: SequenceExecutionArgs
    @ sets the block state of the sequence to be run
    action setSequenceShouldBlock: SequenceExecutionArgs
    @ resets the sequence file member var
    action resetSequenceFile
    @ opens the sequence file based on the sequence file path member var
    action openSequenceFile
    @ loads from disk and formats the header into memory representation
    action readHeader
    @ loads from disk and formats the body into memory representation
    action readBody
    @ loads from disk and formats the footer into memory representation
    action readFooter
    @ closes the open sequence file
    action closeSequenceFile

    @ reports an error that occurred in the stepStatement method
    action reportError_stepStatement: StepStatementFailure
    @ reports an error that occurred in the openSequenceFile method
    action reportError_openSequenceFile: OpenSequenceFileFailure
    @ reports an error that occurred in the readHeader method
    action reportError_readHeader: ReadHeaderFailure
    @ reports an error that occurred in the readBody method
    action reportError_readBody: ReadBodyFailure
    @ reports an error that occurred in the readFooter method
    action reportError_readFooter: ReadFooterFailure
    @ reports an error that occurred in the statementFinished method
    action reportError_statementFinished: StatementFinishedFailure

    @ reports that a sequence was completed
    action report_seqDone
    @ reports that a sequence was cancelled
    action report_seqCancelled

    @ warns that the user said to cancel a sequence but there is no sequence to cancel
    action warn_noSequenceToCancel

    @ iterates to the next statement and dispatches it
    action stepStatement
    
    @ indicates to the component that the next call to stepStatement should cancel
    action cancelNextStepStatement

    # the goal state is the state that we're trying to reach in the sequencer
    # we will iterate through states until we get there or die trying
    @ sets the goal state to RUNNING
    action setGoalState_RUNNING
    @ sets the goal state to VALID
    action setGoalState_VALID

    ####################
    # topology         #
    ####################
    initial enter IDLE

    @ sequencer is ready to load, validate and run a sequence
    state IDLE {
        # start with an unset sequence file
        entry do { closeSequenceFile, resetSequenceFile }
        # -->
        # WAIT

        on cmd_VALIDATE do { setGoalState_VALID, setSequenceFilePath, setSequenceShouldBlock } enter VALIDATION
        on cmd_RUN do { setGoalState_RUNNING, setSequenceFilePath, setSequenceShouldBlock } enter VALIDATION
        on cmd_CANCEL do { warn_noSequenceToCancel }
    }

    @ sequencer is loading and validating the sequence
    state VALIDATION {

        initial enter OPEN_SEQUENCE_FILE

        @ sequencer is opening the sequence file
        state OPEN_SEQUENCE_FILE {
            entry do { openSequenceFile }
            # -->
            on openSequenceFile_success enter READ_HEADER
            on openSequenceFile_failure do { reportError_openSequenceFile } enter IDLE
        }

        @ sequencer is reading the header
        state READ_HEADER {
            entry do { readHeader }
            # -->
            on readHeader_success enter READ_BODY
            on readHeader_failure do { reportError_readHeader } enter IDLE
        }

        @ sequencer is reading the body
        state READ_BODY {
            entry do { readBody }
            # -->
            on readBody_success enter READ_FOOTER
            on readBody_failure do { reportError_readBody } enter IDLE
        }

        @ sequencer is reading the footer
        state READ_FOOTER {
            entry do { readFooter }
            # -->
            # close the sequence file now that it's in memory
            on readFooter_success do { closeSequenceFile } enter WAIT_FOR_CMD_OR_RUN
            on readFooter_failure do { reportError_readFooter } enter IDLE
        }

        on cmd_CANCEL do { report_seqCancelled } enter IDLE
    }

    @ decide whether we should stop after validating, or proceed to running
    choice WAIT_FOR_CMD_OR_RUN {
        # if we already received the RUN cmd, move on to RUNNING. otherwise wait
        if goalStateIs_RUNNING enter RUNNING else enter AWAITING_RUN_VALIDATED_CMD
    }

    @ sequencer has validated the sequence and is waiting for a command to run it
    state AWAITING_RUN_VALIDATED_CMD {
        # wait here until we get the RUN_VALIDATED cmd
        on cmd_RUN_VALIDATED enter RUNNING
        on cmd_CANCEL do { report_seqCancelled } enter IDLE
    }

    @ sequencer is executing all statements in the sequence
    state RUNNING {
        initial enter STEP_STATEMENT

        @ sequencer is stepping into a single statement and sending it out to the dispatcher
        state STEP_STATEMENT {
            entry do { stepStatement }
            # -->
            on stepStatement_cancelled do { report_seqCancelled } enter IDLE
            on stepStatement_noMoreStatements do { report_seqDone } enter IDLE
            on stepStatement_failure do { reportError_stepStatement } enter IDLE
            on stepStatement_successCmd enter AWAIT_CMD
            on stepStatement_successDirective enter AWAIT_DIRECTIVE
        }
        
        @ sequencer is awaiting the execution result of a directive
        state AWAIT_DIRECTIVE {
            on statementFinished_success enter STEP_STATEMENT
            on statementFinished_failure do { reportError_statementFinished } enter IDLE
        }

        @ sequencer is awaiting the execution result of a cmd
        state AWAIT_CMD {
            on statementFinished_success enter STEP_STATEMENT
            on statementFinished_failure do { reportError_statementFinished } enter IDLE
        }

        on cmd_CANCEL do { cancelNextStepStatement }
    }
}
