#include "Svc/FpySequencer/SetLocalVarDirective.hpp"
#include <cstring>
#include "Assert.hpp"
namespace Svc {
namespace Fpy {

SetLocalVarDirective::SetLocalVarDirective() : m_index(0), m_valueSize(0), m_value({}) {}

SetLocalVarDirective::SetLocalVarDirective(const SetLocalVarDirective& src) {
    this->m_index = src.m_index;
    this->m_valueSize = src.m_valueSize;
    FW_ASSERT(&src.m_value);
    (void)std::memcpy(this->m_value, src.m_value, src.m_valueSize);
}

Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) {
    
}
Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) = 0;      //!< deserialize to contents
#if FW_SERIALIZABLE_TO_STRING || FW_ENABLE_TEXT_LOGGING || BUILD_UT
void toString(Fw::StringBase& text) const;  //!< generate text from serializable
#endif
}  // namespace Fpy
}  // namespace Svc