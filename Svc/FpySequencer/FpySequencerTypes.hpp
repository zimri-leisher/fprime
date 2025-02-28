#ifndef __FPY_SEQUENCE_TYPES_HPP__
#define __FPY_SEQUENCE_TYPES_HPP__
#include <config/FpConfig.h>
#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <config/FppConstantsAc.hpp>

namespace Svc {

namespace Fpy {

struct Header {
    // the major version of the FSW
    U8 majorVersion;
    // the minor version of the FSW
    U8 minorVersion;
    // the patch version of the FSW
    U8 patchVersion;
    // the schema version of this file
    U8 schemaVersion;

    // the number of input arguments to this sequence
    // these will become locals in the sequence
    U8 argumentCount;

    // the number of statements in the sequence
    U16 statementCount;

    // the size of the body in bytes
    U32 bodySize;

    static constexpr FwSizeType SERIALIZED_SIZE =
        sizeof(majorVersion) + sizeof(minorVersion) + sizeof(patchVersion) + sizeof(schemaVersion) +
        sizeof(argumentCount) + sizeof(statementCount) + sizeof(bodySize);
};

struct Statement {
    FwOpcodeType opcode;
    Fw::CmdArgBuffer args;

    static constexpr FwSizeType MAX_SERIALIZED_SIZE = sizeof(opcode) + Fw::CmdArgBuffer::SERIALIZED_SIZE;
};

struct Footer {
    U16 crc;

    static constexpr FwSizeType SERIALIZED_SIZE = sizeof(crc);
};

struct Sequence {
    Header header;
    // an array of size m_header.argumentCount mapping argument position to local
    // variable count
    U8 argArray[MAX_SEQUENCE_ARG_COUNT];
    Statement statementArray[MAX_SEQUENCE_STATEMENT_COUNT];
    Footer footer;

    static constexpr FwSizeType MAX_SERIALIZED_SIZE = Header::SERIALIZED_SIZE + MAX_SEQUENCE_ARG_COUNT +
                                                      Statement::MAX_SERIALIZED_SIZE * MAX_SEQUENCE_STATEMENT_COUNT +
                                                      Footer::SERIALIZED_SIZE;
};

struct Runtime {
    // the index of the next statement to be executed
    U32 statementIndex = 0;

    // whether we should cancel the sequence execution before trying to start
    // the next statement
    bool cancelNextStatement = false;
};

enum DirectiveId : FwOpcodeType {
    WAIT_REL = 0x00000000,
    WAIT_ABS = 0x00000001,
    MAX_DIRECTIVE_ID = 0x00000010
};

}  // namespace Fpy

}  // namespace Svc

#endif