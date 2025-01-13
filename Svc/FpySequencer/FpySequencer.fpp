module Svc {
    @ Dispatches command sequences to available command sequencers
    active component FpySequencer {

        enum StatementFinishedFailure {
            UNKNOWN
        }

        enum OpenSequenceFileFailure {
            UNKNOWN
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