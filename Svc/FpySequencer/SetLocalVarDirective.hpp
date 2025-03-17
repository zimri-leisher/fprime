// ======================================================================
// \title  SetLocalVarDirective.hpp
// \author zimri.leisher
// \brief  hpp file for SetLocalVarDirective implementation class
// ======================================================================

#ifndef SVC_FPY_SET_LOCAL_VAR_DIRECTIVE_HPP
#define SVC_FPY_SET_LOCAL_VAR_DIRECTIVE_HPP

#include "Fw/Types/Serializable.hpp"

namespace Svc {
namespace Fpy {
class SetLocalVarDirective : public Fw::Serializable {
  public:
    SetLocalVarDirective();

    SetLocalVarDirective(const SetLocalVarDirective& src);

    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const override;  //!< serialize contents
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) override;      //!< deserialize to contents
#if FW_SERIALIZABLE_TO_STRING || FW_ENABLE_TEXT_LOGGING || BUILD_UT
    void toString(Fw::StringBase& text) const override;  //!< generate text from serializable
#endif
  PRIVATE:
    // the index of the local variable to set
    U8 m_index;

    FwSizeType m_valueSize;
    
    // the value to set the local variable to
    U8 m_value[Fpy::MAX_LOCAL_VARIABLE_VALUE_SIZE];
};
}  // namespace Fpy
}  // namespace Svc

#endif