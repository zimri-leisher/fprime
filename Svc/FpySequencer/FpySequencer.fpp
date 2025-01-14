module Svc {
    @ Dispatches command sequences to available command sequencers
    active component FpySequencer {

        enum StatementFinishedFailure {
            UNKNOWN
        }

        enum OpenSequenceFileFailure {
            UNKNOWN
            DOESNT_EXIST @<  File doesn't exist (for read)
            NO_SPACE @<  No space left
            NO_PERMISSION @<  No permission to read/write file
            BAD_SIZE @<  Invalid size parameter
            NOT_OPENED @<  file hasn't been opened yet
            FILE_EXISTS @< file already exist (for CREATE with O_EXCL enabled)
            NOT_SUPPORTED @< Kernel or file system does not support operation
            INVALID_MODE @< Mode for file access is invalid for current operation
            INVALID_ARGUMENT @< Invalid argument passed in
            OTHER_ERROR @<  A catch-all for other errors. Have to look in implementation-specific code
            MAX_STATUS @< Maximum value of status
        }

        enum ReadHeaderFailure {
            UNKNOWN
        }

        enum ReadBodyFailure {
            UNKNOWN
        }

        enum ReadFooterFailure {
            UNKNOWN
            CRC
        }

        enum StepStatementFailure {
            UNKNOWN
            INVALID_CMD
        }

        enum StatementType {
            COMMAND
            DIRECTIVE
        }

        struct StatementResponse {
            $opcode: FwOpcodeType
            $type: StatementType
            response: Fw.CmdResponse
        }

        struct SequenceExecutionArgs {
            filePath: string
            $block: Fw.Wait
        }
     
        include "FpySequencerCommands.fppi"
        include "FpySequencerTelemetry.fppi"
        include "FpySequencerEvents.fppi"
        include "FpySequencerStateMachine.fppi"

        state machine instance sequencer: SequencerStateMachine

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

    }
}