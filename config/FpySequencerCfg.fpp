module Svc {
    module Fpy {
        @ The maximum number of arguments a sequence can have
        constant MAX_SEQUENCE_ARG_COUNT = 8
        @ The maximum number of statements a sequence can have
        constant MAX_SEQUENCE_STATEMENT_COUNT = 1024
        @ The maximum bytes in a statement arg buffer (should be no greater than the cmd arg buf max size)
        constant MAX_STATEMENT_ARG_BUF_SIZE = 500
        # -1 byte because a complete reference to a local var is a 1 byte id and the remainder is the value
        # it should be less than statement arg buf size, cuz otherwise a local variable value won't fit
        # in a statement arg buf
        @ the maximum size of a local variable
        constant MAX_LOCAL_VARIABLE_VALUE_SIZE = MAX_STATEMENT_ARG_BUF_SIZE - 1
        @ The maximum number of local variables a sequence can have
        constant MAX_SEQUENCE_LOCAL_VARIABLES = 16
    }
}