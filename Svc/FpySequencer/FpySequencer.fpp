module Svc {
    @ Dispatches command sequences to available command sequencers
    active component FpySequencer {

        enum StatementFinishedFailure {
            UNKNOWN
        }

        struct OpenSequenceFileFailure {
            fileOpenErrorCode: U16
        }

        struct ReadHeaderFailure {
            fileReadErrorCode: U16
        }

        struct ReadBodyFailure {
            fileReadErrorCode: U16
        }

        struct ReadFooterFailure {
            fileReadErrorCode: U16
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