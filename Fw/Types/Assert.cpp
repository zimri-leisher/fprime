#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/format.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <cassert>
#include <cstdio>

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define fileIdFs "Assert: 0x%08" PRIx32 ":%" PRI_PlatformUIntType
#else
#define fileIdFs "Assert: \"%s:%" PRI_PlatformUIntType "\""
#endif

namespace Fw {

void defaultPrintAssert(const CHAR* msg) {
    // Write to stderr w/o formatting
    (void) fputs(msg, stderr);
    (void) fputs("\n", stderr);
}

void defaultReportAssert(FILE_NAME_ARG file,
                         NATIVE_UINT_TYPE lineNo,
                         NATIVE_UINT_TYPE numArgs,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwAssertArgType arg5,
                         FwAssertArgType arg6,
                         CHAR* destBuffer,
                         NATIVE_INT_TYPE buffSize) {
    static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<NATIVE_INT_TYPE>::max(),
                  "NATIVE_INT_TYPE cannot fit into FwSizeType");
    switch (numArgs) {
        case 0:
            (void)stringFormat(destBuffer,  static_cast<FwSizeType>(buffSize), fileIdFs, file, lineNo);
            break;
        case 1:
            (void)stringFormat(destBuffer,  static_cast<FwSizeType>(buffSize),
                               fileIdFs " %" PRI_FwAssertArgType, file, lineNo, arg1);
            break;
        case 2:
            (void)stringFormat(destBuffer,  static_cast<FwSizeType>(buffSize),
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType, file, lineNo, arg1, arg2);
            break;
        case 3:
            (void)stringFormat(destBuffer,  static_cast<FwSizeType>(buffSize),
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType, file,
                               lineNo, arg1, arg2, arg3);
            break;
        case 4:
            (void)stringFormat(destBuffer,  static_cast<FwSizeType>(buffSize),
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                                        " %" PRI_FwAssertArgType,
                           file, lineNo, arg1, arg2, arg3, arg4);
            break;
        case 5:
            (void)stringFormat(destBuffer,  static_cast<FwSizeType>(buffSize),
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                                        " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                               file, lineNo, arg1, arg2, arg3, arg4, arg5);
            break;
        case 6:
            (void)stringFormat(destBuffer,  static_cast<FwSizeType>(buffSize),
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                                        " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                               file, lineNo, arg1, arg2, arg3, arg4, arg5, arg6);
            break;
        default:  // in an assert already, what can we do?
            break;
    }
}

void AssertHook::printAssert(const CHAR* msg) {
    defaultPrintAssert(msg);
}

void AssertHook::reportAssert(FILE_NAME_ARG file,
                              NATIVE_UINT_TYPE lineNo,
                              NATIVE_UINT_TYPE numArgs,
                              FwAssertArgType arg1,
                              FwAssertArgType arg2,
                              FwAssertArgType arg3,
                              FwAssertArgType arg4,
                              FwAssertArgType arg5,
                              FwAssertArgType arg6) {
    CHAR destBuffer[FW_ASSERT_TEXT_SIZE];
    defaultReportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6, destBuffer, sizeof(destBuffer));
    // print message
    this->printAssert(destBuffer);
}

void AssertHook::doAssert() {
    assert(0);
}

STATIC AssertHook* s_assertHook = nullptr;

void AssertHook::registerHook() {
    this->previousHook = s_assertHook;
    s_assertHook = this;
}

void AssertHook::deregisterHook() {
    s_assertHook = this->previousHook;
}

// Default handler of SwAssert functions
NATIVE_INT_TYPE defaultSwAssert(FILE_NAME_ARG file,
                                NATIVE_UINT_TYPE lineNo,
                                NATIVE_UINT_TYPE numArgs,
                                FwAssertArgType arg1,
                                FwAssertArgType arg2,
                                FwAssertArgType arg3,
                                FwAssertArgType arg4,
                                FwAssertArgType arg5,
                                FwAssertArgType arg6) {
    if (nullptr == s_assertHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        defaultReportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6, assertMsg, sizeof(assertMsg));
        defaultPrintAssert(assertMsg);
        assert(0);
    } else {
        s_assertHook->reportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);
        s_assertHook->doAssert();
    }
    return 0;
}

NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo) {
    return defaultSwAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0);
}

NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, NATIVE_UINT_TYPE lineNo) {
    return defaultSwAssert(file, lineNo, 1, arg1, 0, 0, 0, 0, 0);
}

NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, NATIVE_UINT_TYPE lineNo) {
    return defaultSwAssert(file, lineNo, 2, arg1, arg2, 0, 0, 0, 0);
}

NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         NATIVE_UINT_TYPE lineNo) {
    return defaultSwAssert(file, lineNo, 3, arg1, arg2, arg3, 0, 0, 0);
}

NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         NATIVE_UINT_TYPE lineNo) {
    return defaultSwAssert(file, lineNo, 4, arg1, arg2, arg3, arg4, 0, 0);
}

NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwAssertArgType arg5,
                         NATIVE_UINT_TYPE lineNo) {
    return defaultSwAssert(file, lineNo, 5, arg1, arg2, arg3, arg4, arg5, 0);
}

NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwAssertArgType arg5,
                         FwAssertArgType arg6,
                         NATIVE_UINT_TYPE lineNo) {
    return defaultSwAssert(file, lineNo, 6, arg1, arg2, arg3, arg4, arg5, arg6);
}
}  // namespace Fw

// define C asserts.
extern "C" {
NATIVE_INT_TYPE CAssert0(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo);
}

NATIVE_INT_TYPE CAssert0(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo) {
    if (nullptr == Fw::s_assertHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::defaultReportAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0, assertMsg, sizeof(assertMsg));
    } else {
        Fw::s_assertHook->reportAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0);
        Fw::s_assertHook->doAssert();
    }
    return 0;
}
