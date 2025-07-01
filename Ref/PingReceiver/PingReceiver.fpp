module Ref {

  @ A rate group active component with input and output ping ports
  active component PingReceiver {

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ The ping input port
    async input port PingIn: Svc.Ping

    @ The ping input port
    output port PingOut: Svc.Ping


    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port CmdDisp

    @ Command registration port
    command reg port CmdReg

    @ Command response port
    command resp port CmdStatus

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    @ Telemetry port
    telemetry port Tlm
    param get port prmget
    param set port prmset

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Command to disable ping response
    async command PR_StopPings

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Disabled ping responses
    event PR_PingsDisabled \
      severity activity high \
      id 0 \
      format "PingReceiver ping responses disabled"

    @ Got ping
    event PR_PingReceived(
                           code: U32 @< Ping code
                         ) \
      severity diagnostic \
      id 1 \
      format "PingReceiver pinged with code {}"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Number of pings received
    telemetry PR_NumPings: U32

    telemetry bodyRatesNorm: F32
    
    telemetry bodyRates: Vector3
    telemetry attitude: Quat

    param bodyRatesNorm_fake: F32 default 2.0
    param bodyRates_fake: Vector3 default [0.0, 0.0, 0.0]
    param attitude_fake: Quat default {scalar = 0.0, vec = [0.0, 0.0, 0.0]}

    array Vector3 = [3] F32
    struct Quat {
      scalar: F32
      vec: Vector3
    }
  }

}
