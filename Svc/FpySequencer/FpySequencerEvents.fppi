event InvalidSequencer(
    idx: U16
) \
    severity warning high \
    format "Invalid sequence index {}"

event FileReadError(
    filePath: string
    errorCode: I32
) \
    severity warning high \
    format "File read error encountered while reading {}: {}"

event EndOfFileError(
    filePath: string
) \
    severity warning high \
    format "End of file encountered unexpectedly while reading {}"

event DeserializeError(
    filePath: string
    errorCode: I32
    buffLeft: U32
    buffLength: U32
) \
    severity warning high \
    format "Deserialize error encountered while reading {}: {} ({} bytes left out of {})"

event ReadHeaderSuccess(
    filePath: string
) \
    severity diagnostic \
    format "Successfully read header of {}"