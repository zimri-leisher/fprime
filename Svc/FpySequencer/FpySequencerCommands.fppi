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

@ Runs a previously validated sequence
async command RUN_VALIDATED(
                            $block: BlockState @< Return command status when complete or not
                           ) \
    opcode 2

@ Cancels a running or validated sequence
async command CANCEL() \
    opcode 3