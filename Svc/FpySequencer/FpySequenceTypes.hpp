#ifndef __FPY_SEQUENCE_TYPES_HPP__
#define __FPY_SEQUENCE_TYPES_HPP__
#include <config/FpConfig.h>
#include <Fw/Cmd/CmdArgBuffer.hpp>

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

  enum {
    SERIALIZED_SIZE = sizeof(U8) * 5 + sizeof(U16)
  };
};

struct Statement {
  FwOpcodeType m_opcode;
  Fw::CmdArgBuffer m_args;
};

struct Footer {
  U16 m_crc;
};

struct Sequence {
  Header m_header;
  // an array of size m_argumentCount mapping argument position to local
  // variable count
  U8* m_argArray;
  Statement* m_statementArray;
  Footer m_footer;
};

}  // namespace Fpy

}  // namespace Svc

#endif