#ifndef __FPY_SEQUENCE_TYPES_HPP__
#define __FPY_SEQUENCE_TYPES_HPP__
#include <config/FpConfig.h>
#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <config/FppConstantsAc.hpp>

namespace Svc {

namespace Fpy {

struct Header {
    // the major version of the FSW
    U8 m_majorVersion;
    // the minor version of the FSW
    U8 m_minorVersion;
    // the patch version of the FSW
    U8 m_patchVersion;
    // the schema version of this file
    U8 m_schemaVersion;

    // the number of input arguments to this sequence
    // these will become locals in the sequence
    U8 m_argumentCount;

    // the number of statements in the sequence
    U16 m_statementCount;

    static constexpr FwSizeType SERIALIZED_SIZE = sizeof(m_majorVersion) + sizeof(m_minorVersion) +
                                                  sizeof(m_patchVersion) + sizeof(m_schemaVersion) +
                                                  sizeof(m_argumentCount) + sizeof(m_statementCount);
};

struct Statement {
    FwOpcodeType m_opcode;
    Fw::CmdArgBuffer m_args;

    static constexpr FwSizeType MAX_SERIALIZED_SIZE = sizeof(m_opcode) + Fw::CmdArgBuffer::SERIALIZED_SIZE;
};

struct Footer {
    U16 m_crc;

    static constexpr FwSizeType SERIALIZED_SIZE = sizeof(m_crc);
};

struct Sequence {
    Header m_header;
    // an array of size m_argumentCount mapping argument position to local
    // variable count
    U8 m_argArray[MAX_SEQUENCE_ARG_COUNT];
    Statement m_statementArray[MAX_SEQUENCE_STATEMENT_COUNT];
    Footer m_footer;

    static constexpr FwSizeType MAX_SERIALIZED_SIZE = Header::SERIALIZED_SIZE + MAX_SEQUENCE_ARG_COUNT +
                                                      Statement::MAX_SERIALIZED_SIZE * MAX_SEQUENCE_STATEMENT_COUNT +
                                                      Footer::SERIALIZED_SIZE;
};

}  // namespace Fpy

}  // namespace Svc

#endif