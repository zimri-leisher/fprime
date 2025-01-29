@ this state machine controls the "goal state" of the actual
@ state machine. 
state machine Controller {

    # user commands
    signal cmd_VALIDATE
    signal cmd_RUN_VALIDATED
    signal cmd_RUN
    signal cmd_CANCEL
    # sequencer state machine updates
    signal finished_VALIDATING
    signal finished_RUNNING


    # these actions send signals to the sequencer state machine
    action control_IDLE_TO_VALIDATING
    action control_VALIDATING_TO_RUNNING
    action control_VALIDATING_TO_IDLE
    action control_RUNNING_TO_IDLE

    initial enter IDLE
    
    state IDLE {
        on cmd_VALIDATE do { control_IDLE_TO_VALIDATING } enter VALIDATE
        on cmd_RUN do { control_IDLE_TO_VALIDATING } enter RUN
    }

    state VALIDATE {
        on cmd_RUN_VALIDATED do { control_VALIDATING_TO_RUNNING } enter RUN
        on cmd_CANCEL do { control_VALIDATING_TO_IDLE } enter IDLE
    }

    state RUN {
        on finished_VALIDATING do { control_VALIDATING_TO_RUNNING }
        on finished_RUNNING enter IDLE
        on cmd_CANCEL enter IDLE
    }
}