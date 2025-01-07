state machine StateMachine {
    ####################
    # guards           #
    ####################
    @ check whether the sequence file exists
    guard sequenceFileExists
    @ check whether the sequence file is open
    guard sequenceFileOpen

    @ check whether the header is valid
    guard headerValid
    guard bodyValid
    guard footerValid

    ####################
    # signals          #
    ####################
    @ called on cmd LOAD received
    signal cmd_LOAD: string
    signal cmd_RUN

    @ called when a sequence file is done opening, with the status of that operation
    signal sequenceFileOpenStatus: I16

    @ called once per rate group cycle
    signal cycle

    signal invalidCrc
    signal failureToOpen
    signal finishValidation

    signal noMoreStatements

    ####################
    # actions          #
    ####################
    @ sets the current sequence file path member
    action setSequenceFilePath: string
    @ resets the sequence file path member
    action resetSequenceFilePath
    @ opens the file descriptor
    action openSequenceFile

    action readHeader
    action readBody
    action readFooter

    action incrementFailureCount

    action stepStatement

    ####################
    # topology         #
    ####################
    initial enter AVAILABLE

    @ sequencer is ready to load and run a sequence
    state AVAILABLE {
        @ start with an unset sequence file path
        entry do { resetSequenceFilePath }

        @ if user tried to LOAD a sequence, set the sequence file path and start validating the sequence
        on cmd_LOAD do { setSequenceFilePath } enter VALIDATION
    }

    state VALIDATION {

        initial enter OPEN_FILE

        choice OPEN_FILE {
            entry do openSequenceFile
            if sequenceFileOpen
        }

        entry do { validateSequence }

        on invalidCrc do { incrementFailureCount, warnInvalidCrc } enter AVAILABLE
        on failureToOpen enter INVALID
        on finishValidation enter RUNNING
    }

    state INVALID {
        entry do { incrementFailureCount }

        on cycle enter AVAILABLE
    }

    state RUNNING {

        entry do { sendNextStatement }
        on cycle do { stepStatement }

        on noMoreStatements enter AVAILABLE
    }
}
