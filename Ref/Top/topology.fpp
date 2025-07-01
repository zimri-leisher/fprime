module Ref {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

  enum Ports_RateGroups {
    rateGroup1
    rateGroup2
    rateGroup3
  }



  topology Ref {
    # ----------------------------------------------------------------------
    # Subtopology imports
    # ----------------------------------------------------------------------
    import CdhCore.Subtopology
    import ComCcsds.Subtopology
    import FileHandling.Subtopology
    import DataProducts.Subtopology
    
    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance SG1
    instance SG2
    instance SG3
    instance SG4
    instance SG5
    instance blockDrv
    instance posixTime
    instance adcs
    instance rateGroup1Comp
    instance rateGroup2Comp
    instance rateGroup3Comp
    instance rateGroupDriverComp
    instance recvBuffComp
    instance sendBuffComp
    instance typeDemo
    instance systemResources
    instance linuxTimer

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance CdhCore.cmdDisp

    event connections instance CdhCore.events

    telemetry connections instance CdhCore.tlmSend

    text event connections instance CdhCore.textLogger

    health connections instance CdhCore.$health

    param connections instance FileHandling.prmDb

    time connections instance posixTime

    # ----------------------------------------------------------------------
    # Telemetry packets
    # ----------------------------------------------------------------------

    include "RefPackets.fppi"

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {

      # Linux timer to drive cycle
      linuxTimer.CycleOut -> rateGroupDriverComp.CycleIn

      # Rate group 1
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1Comp.CycleIn
      rateGroup1Comp.RateGroupMemberOut[0] -> SG1.schedIn
      rateGroup1Comp.RateGroupMemberOut[1] -> SG2.schedIn
      rateGroup1Comp.RateGroupMemberOut[2] -> CdhCore.tlmSend.Run
      rateGroup1Comp.RateGroupMemberOut[3] -> FileHandling.fileDownlink.Run
      rateGroup1Comp.RateGroupMemberOut[4] -> systemResources.run
      rateGroup1Comp.RateGroupMemberOut[5] -> ComCcsds.comQueue.run

      # Rate group 2
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut[0] -> ComCcsds.cmdSeq0.checkTimers
      rateGroup2Comp.RateGroupMemberOut[4] -> ComCcsds.cmdSeq0.tlmWrite
      rateGroup2Comp.RateGroupMemberOut[5] -> ComCcsds.cmdSeq1.checkTimers
      rateGroup2Comp.RateGroupMemberOut[6] -> ComCcsds.cmdSeq1.tlmWrite
      rateGroup2Comp.RateGroupMemberOut[1] -> sendBuffComp.SchedIn
      rateGroup2Comp.RateGroupMemberOut[2] -> SG3.schedIn
      rateGroup2Comp.RateGroupMemberOut[3] -> SG4.schedIn

      # Rate group 3
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup3] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut[0] -> CdhCore.$health.Run
      rateGroup3Comp.RateGroupMemberOut[1] -> SG5.schedIn
      rateGroup3Comp.RateGroupMemberOut[2] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[3] -> ComCcsds.commsBufferManager.schedIn
      rateGroup3Comp.RateGroupMemberOut[4] -> DataProducts.dpBufferManager.schedIn
      rateGroup3Comp.RateGroupMemberOut[5] -> DataProducts.dpWriter.schedIn
      rateGroup3Comp.RateGroupMemberOut[6] -> DataProducts.dpMgr.schedIn
    }

    connections Ref {
      sendBuffComp.Data -> blockDrv.BufferIn
      blockDrv.BufferOut -> recvBuffComp.Data

      ### Moved this out of DataProducts Subtopology --> anything specific to deployment should live in Ref connections
      # Synchronous request. Will have both request kinds for demo purposes, not typical
      SG1.productGetOut -> DataProducts.dpMgr.productGetIn
      # Asynchronous request
      SG1.productRequestOut -> DataProducts.dpMgr.productRequestIn
      DataProducts.dpMgr.productResponseOut -> SG1.productRecvIn
      # Send filled DP
      SG1.productSendOut -> DataProducts.dpMgr.productSendIn

    }

    connections ComCcsds_CdhCore{
      # events and telemetry to comQueue
      CdhCore.events.PktSend        -> ComCcsds.comQueue.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.EVENTS]
      CdhCore.tlmSend.PktSend            -> ComCcsds.comQueue.comPacketQueueIn[ComCcsds.Ports_ComPacketQueue.TELEMETRY]

      # Router <-> CmdDispatcher
      ComCcsds.fprimeRouter.commandOut  -> CdhCore.cmdDisp.seqCmdBuff[0]
      CdhCore.cmdDisp.seqCmdStatus[0]     -> ComCcsds.fprimeRouter.cmdResponseIn
      ComCcsds.cmdSeq0.cmdOut -> CdhCore.cmdDisp.seqCmdBuff[1]
      ComCcsds.cmdSeq1.cmdOut -> CdhCore.cmdDisp.seqCmdBuff[2]
      CdhCore.cmdDisp.seqCmdStatus[1] -> ComCcsds.cmdSeq0.cmdResponseIn
      CdhCore.cmdDisp.seqCmdStatus[2] -> ComCcsds.cmdSeq1.cmdResponseIn

      ComCcsds.seqDisp.seqRunOut[0] -> ComCcsds.cmdSeq0.seqRunIn
      ComCcsds.seqDisp.seqRunOut[1] -> ComCcsds.cmdSeq1.seqRunIn

      ComCcsds.cmdSeq0.seqStartOut -> ComCcsds.seqDisp.seqStartIn[0]
      ComCcsds.cmdSeq0.seqDoneOut -> ComCcsds.seqDisp.seqDoneIn[0]
      ComCcsds.cmdSeq0.getTlmChan -> CdhCore.tlmSend.TlmGet

      ComCcsds.cmdSeq1.seqStartOut -> ComCcsds.seqDisp.seqStartIn[1]
      ComCcsds.cmdSeq1.seqDoneOut -> ComCcsds.seqDisp.seqDoneIn[1]
      ComCcsds.cmdSeq1.getTlmChan -> CdhCore.tlmSend.TlmGet
    }

    connections ComCcsds_FileHandling {
      # File Downlink <-> ComQueue
      FileHandling.fileDownlink.bufferSendOut -> ComCcsds.comQueue.bufferQueueIn[FileHandling.Ports_ComBufferQueue.FILE_DOWNLINK]
      ComCcsds.comQueue.bufferReturnOut[FileHandling.Ports_ComBufferQueue.FILE_DOWNLINK] -> FileHandling.fileDownlink.bufferReturn

      # Router <-> FileUplink
      ComCcsds.fprimeRouter.fileOut     -> FileHandling.fileUplink.bufferSendIn
      FileHandling.fileUplink.bufferSendOut -> ComCcsds.fprimeRouter.fileBufferReturnIn
    }

    connections FileHandling_DataProducts{
      # Data Products
      DataProducts.dpCat.fileOut             -> FileHandling.fileDownlink.SendFile
      FileHandling.fileDownlink.FileComplete -> DataProducts.dpCat.fileDone
    }

  }

}