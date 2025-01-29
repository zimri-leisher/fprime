state machine SequencerStateMachine {
    ####################
    # guards           #
    ####################
    @ return true if the goal state is RUNNING
    # see explanation in FpySequencer::m_goalState
    guard goalStateIs_RUNNING
    @ return true if the sequence should block
    guard sequenceShouldBlock

    ####################
    # signals          #
    ####################
    @ called on VALIDATE cmd with the path of the sequence file to validate
    signal cmd_VALIDATE: SequenceExecutionArgs
    @ called on RUN cmd with the path of the sequence file to run
    signal cmd_RUN: SequenceExecutionArgs
    @ called on RUN_VALIDATED cmd
    signal cmd_RUN_VALIDATED: SequenceExecutionArgs
    @ called on CANCEL cmd
    signal cmd_CANCEL

    @ called in stepStatement method when a command was successfully sent to the cmd disp
    signal stepStatement_successCmd
    @ called in stepStatement method when a directive was sent to this sequencer
    signal stepStatement_successDirective
    @ called in stepStatement method when a statement was unable to be sent out
    signal stepStatement_failure
    @ called in stepStatement method when there were no more statements in the sequence
    signal stepStatement_noMoreStatements
    @ called in stepStatement method when the sequence execution was cancelled
    signal stepStatement_cancelled

    @ called in statementFinished method when a statement was successfully executed
    signal statementFinished_success
    @ called in statementFinished method when a statement was unsuccessfully executed
    signal statementFinished_failure
    @ called in openSequenceFile method when the sequence file was successfully opened
    signal openSequenceFile_success
    @ called in openSequenceFile method when the sequence file was unsuccessfully opened
    signal openSequenceFile_failure
    @ called in readHeader method when the header was successfully read
    signal readHeader_success
    @ called in readHeader method when the header was unsuccessfully read
    signal readHeader_failure
    @ called in readBody method when the body was successfully read
    signal readBody_success
    @ called in readBody method when the body was unsuccessfully read
    signal readBody_failure
    @ called in readFooter method when the footer was successfully read
    signal readFooter_success
    @ called in readFooter method when the footer was unsuccessfully read
    signal readFooter_failure

    @ always called in cmdResponse_OK
    signal cmdResponse_OK_done
    @ always called in cmdResponse_EXECUTION_ERROR
    signal cmdResponse_EXECUTION_ERROR_done

    ####################
    # actions          #
    ####################
    @ sets the current sequence file path member var
    action setSequenceFilePath: SequenceExecutionArgs
    @ sets the block state of the sequence to be run
    action setSequenceShouldBlock: SequenceExecutionArgs
    @ resets the sequence execution arguments, memory object, CRC, saved opcode/cmdseq
    action resetSequence
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

    @ reports that a sequence was completed
    action report_seqDone
    @ reports that a sequence was cancelled
    action report_seqCancelled

    @ warns that the user cmd was invalid
    action warn_invalidCmd
    @ warns that the sequence failed validation
    action warn_invalidSequence

    @ iterates to the next statement and dispatches it
    action stepStatement
    
    @ indicates to the component that the next call to stepStatement should cancel
    action cancelNextStepStatement

    @ sets the goal state to RUNNING
    # see explanation in FpySequencer::m_goalState
    action setGoalState_RUNNING
    @ sets the goal state to VALID
    # see explanation in FpySequencer::m_goalState
    action setGoalState_VALID
    @ sets the goal state to IDLE
    # see explanation in FpySequencer::m_goalState
    action setGoalState_IDLE

    @ responds to the calling command with OK
    action cmdResponse_OK
    @ responds to the calling command with EXECUTION_ERROR
    action cmdResponse_EXECUTION_ERROR

    ####################
    # topology         #
    ####################
    initial enter IDLE

    @ sequencer is ready to load, validate and run a sequence
    state IDLE {
        # start with an unset sequence file, and unset goal state
        entry do { closeSequenceFile, resetSequenceExecutionArgs, setGoalState_IDLE }
        # -->
        # wait for a cmd

        # **valid cmds**
        # validate does not take as input a block state, only a path
        on cmd_VALIDATE do { setGoalState_VALID, setSequenceFilePath } enter VALIDATION
        # run takes both path and block state
        on cmd_RUN do { setGoalState_RUNNING, setSequenceFilePath, setSequenceShouldBlock } enter VALIDATION

        # **invalid cmds**
        @ not running a seq
        on cmd_CANCEL do { warn_invalidCmd }
        @ haven't validated yet
        on cmd_RUN_VALIDATED do { warn_invalidCmd }
    }

    @ generally, the process of validating the sequence, as well as waiting to run once
    @ it has finished validating
    state VALIDATION {

        initial enter VALIDATING

        ############
        # commands #
        ############
        # **valid cmds**
        @ cancelled the sequence while it was validating
        on cmd_CANCEL enter CANCELLED

        # **invalid cmds**
        @ already validating
        on cmd_VALIDATE do { warn_invalidCmd }
        @ cannot run while validating
        on cmd_RUN do { warn_invalidCmd }
        # cmd_RUN_VALIDATED is handled by the substates

        @ sequencer is loading and validating the sequence
        state VALIDATING {

            initial enter OPEN_SEQUENCE_FILE

            ############
            # commands #
            ############
            @ cannot run validated, not finished validating
            on cmd_RUN_VALIDATED do { warn_invalidCmd }

            @ sequencer is opening the sequence file
            state OPEN_SEQUENCE_FILE {
                entry do { openSequenceFile }
                # -->
                on openSequenceFile_success enter READ_HEADER
                on openSequenceFile_failure enter INVALID
            }

            @ sequencer is reading the header
            state READ_HEADER {
                entry do { readHeader }
                # -->
                on readHeader_success enter READ_BODY
                on readHeader_failure enter INVALID
            }

            @ sequencer is reading the body
            state READ_BODY {
                entry do { readBody }
                # -->
                on readBody_success enter READ_FOOTER
                on readBody_failure enter INVALID
            }

            @ sequencer is reading the footer
            state READ_FOOTER {
                entry do { readFooter }
                # -->
                on readFooter_success enter VALID
                on readFooter_failure enter INVALID
            }

            # cmd_RUN_VALIDATED is handled by substates
        }


        state VALID {
            # close the sequence file now that it's in memory
            entry do { closeSequenceFile }

            ############
            # commands #
            ############
            @ the sequence has already been decided on, so only set the sequenceShouldBlock var
            on cmd_RUN_VALIDATED do { setSequenceShouldBlock } enter RUNNING

            initial enter WAIT_FOR_CMD_OR_RUN
            @ decide whether we should stop after validating, or proceed to running
            choice WAIT_FOR_CMD_OR_RUN {
                # if we already received the RUN cmd, move on to RUNNING. otherwise wait
                if goalStateIs_RUNNING enter RUNNING else enter AWAITING_RUN_VALIDATED_CMD
            }

            @ sequencer has validated the sequence and is waiting for a command to run it
            state AWAITING_RUN_VALIDATED_CMD {
                # we explicitly ran the VALIDATE cmd, so we can respond now
                entry do { cmdResponse_OK }
                # wait here until we get the RUN_VALIDATED cmd

                # **valid cmds**
                
            }
        }

        state INVALID {
            entry do { warn_invalidSequence, cmdResponse_EXECUTION_ERROR }
            
            on cmdResponse_EXECUTION_ERROR_done enter IDLE
        }
    }

    @ sequencer is executing all statements in the sequence
    state RUNNING {
        initial enter STEP_STATEMENT

        @ sequencer is stepping into a single statement and sending it out to the dispatcher
        state STEP_STATEMENT {
            entry do { stepStatement }
            # -->
            on stepStatement_cancelled enter CANCELLED
            on stepStatement_noMoreStatements enter SUCCEEDED
            on stepStatement_failure enter FAILED
            on stepStatement_successCmd enter AWAIT_CMD
            on stepStatement_successDirective enter AWAIT_DIRECTIVE
        }
        
        @ sequencer is awaiting the execution result of a directive
        state AWAIT_DIRECTIVE {
            on statementFinished_success enter STEP_STATEMENT
            on statementFinished_failure enter FAILED
        }

        @ sequencer is awaiting the execution result of a cmd
        state AWAIT_CMD {
            on statementFinished_success enter STEP_STATEMENT
            on statementFinished_failure enter FAILED
        }

        # don't immediately enter cancellation. the only valid cancellation
        # point during the run loop is just before we're about to send out
        # a statement
        on cmd_CANCEL do { cancelNextStepStatement }

    }

    state CANCELLED {
        # raise an execution error if we're blocking
        entry do { cmdResponse_EXECUTION_ERROR }

        # always succeeds
        on cmdResponse_EXECUTION_ERROR_done enter IDLE
    }

    state SUCCEEDED {
        # raise OK if we're blocking
        entry do { report_seqSucceeded, cmdResponse_OK }

        # always succeeds
        on cmdResponse_OK_done enter IDLE
    }

    state FAILED {
        # raise an execution error if we're blocking
        entry do { report_seqFailed, cmdResponse_EXECUTION_ERROR }

        # always succeeds
        on cmdResponse_EXECUTION_ERROR_done enter IDLE
    }

}
