#ifndef __FPY_SEQUENCE_TYPES_HPP__
#define __FPY_SEQUENCE_TYPES_HPP__
#include <config/FpConfig.h>

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
  // an array of size m_argumentCount mapping argument position to local
  // variable count
  U8* m_argArray;

  U16 m_statementCount;
};

struct Statement {
  U16 m_opcode;

  // the number of bytes in m_argBuf
  U16 m_argBufLen;
  // an array of serialized command arguments
  U8* m_argBuf;
};

struct Footer {
  U16 m_crc;
};

struct Sequence {
  Header m_header;
  Statement* m_statementArray;
  Footer m_footer;
};

}  // namespace Fpy

}  // namespace Svc

#endif