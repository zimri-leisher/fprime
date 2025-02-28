module Svc {
    @ Dispatches command sequences to available command sequencers
    active component FpySequencer {
        
        enum StatementType {
            COMMAND
            DIRECTIVE
        }

        enum BlockState {
            BLOCK
            NO_BLOCK
            UNSPECIFIED
        }

        enum GoalState {
            RUNNING
            VALID
            IDLE
        }

        struct StatementResponse {
            $opcode: FwOpcodeType
            $type: StatementType
            response: Fw.CmdResponse
        }

        struct SequenceExecutionArgs {
            filePath: string
            $block: BlockState
        }

        include "FpySequencerCommands.fppi"
        include "FpySequencerTelemetry.fppi"
        include "FpySequencerEvents.fppi"
        include "FpySequencerStateMachine.fppi"

        state machine instance sequencer: SequencerStateMachine

        @ output port for commands from the seq
        output port cmdOut: Fw.Com

        @ responses back from commands from the seq
        async input port cmdResponseIn: Fw.CmdResponse

        @ Ping in port
        async input port pingIn: Svc.Ping

        @ Ping out port
        output port pingOut: Svc.Ping

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