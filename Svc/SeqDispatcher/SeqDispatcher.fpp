module Svc {
    @ Dispatches command sequences to available command sequencers
    active component SeqDispatcher {

        enum CmdSequencerState {
            AVAILABLE = 0
            RUNNING_SEQUENCE_BLOCK = 1
            RUNNING_SEQUENCE_NO_BLOCK = 2
        }

        include "SeqDispatcherCommands.fppi"
        include "SeqDispatcherTelemetry.fppi"
        include "SeqDispatcherEvents.fppi"

        async input port seqRunIn: Svc.CmdSeqIn

        output port seqRunOut: [SeqDispatcherSequencerPorts] Svc.CmdSeqIn

        async input port seqDoneIn: [SeqDispatcherSequencerPorts] Fw.CmdResponse

        # should be called by cmdsequencer whenever it starts a sequence (whether
        # called by SeqDisp or manually by ground)
        async input port seqStartIn: [SeqDispatcherSequencerPorts] Svc.CmdSeqIn

        match seqRunOut with seqDoneIn

        match seqRunOut with seqStartIn

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