@ Loads, validates and runs a sequence
async command RUN(
                      fileName: string size 240 @< The name of the sequence file
                      $block: BlockState @< Return command status when complete or not
                    ) \
    opcode 0

@ Loads and validates a sequence
async command VALIDATE(
                      fileName: string size 240 @< The name of the sequence file
                  ) \
    opcode 1

@ Must be called after VALIDATE. Runs the sequence that was validated.
async command RUN_VALIDATED(
                            $block: BlockState @< Return command status when complete or not
                           ) \
    opcode 2

@ Cancels a running or validated sequence. After running CANCEL, the sequencer
@ should return to IDLE
async command CANCEL() \
    opcode 3