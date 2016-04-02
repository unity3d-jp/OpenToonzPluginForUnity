#include "pch.h"
#include "Foundation.h"
using namespace utj;

#include "OpenToonzPlugin.h"
#include "otpModule.h"
#include "otpInstance.h"
#include "otpPort.h"
#include "otpParam.h"

otpParam::otpParam(otpInstance *parent)
    : m_parent(parent)
{
}

otpParam::~otpParam()
{
}

otpInstance* otpParam::getInstance() const { return m_parent; }
otpParamType otpParam::getType() const { return (otpParamType)m_info.type; }
const char* otpParam::getName() const { return m_info.name; }
const char* otpParam::getNote() const { return m_info.note; }

int otpParam::getLength() const
{
    switch (getType()) {
    case otParamType_String: return m_string.empty() ? 0 : (int)m_string.size() + 1;
    case otParamType_ToneCurve: return (int)m_tonecurve.size();
    }
    return 1;
}

void otpParam::getValue(void *dst) const
{
    if (!dst) { return; }

    switch (getType()) {
    case otParamType_Double:    *(otpDoubleValue*)dst = m_value.double_v; break;
    case otParamType_Range:     *(otpRangeValue*)dst = m_value.range_v; break;
    case otParamType_Color:     *(otpColorValue*)dst = m_value.color_v; break;
    case otParamType_Point:     *(otpPointValue*)dst = m_value.point_v; break;
    case otParamType_Enum:      *(otpEnumValue*)dst = m_value.enum_v; break;
    case otParamType_Int:       *(otpIntValue*)dst = m_value.int_v; break;
    case otParamType_Bool:      *(otpBoolValue*)dst = m_value.bool_v; break;
    case otParamType_Spectrum:  *(otpSpectrumValue*)dst = m_value.spectrum_v; break;
    case otParamType_String:    memcpy(dst, m_string.c_str(), m_string.size()); break;
    case otParamType_ToneCurve: memcpy(dst, &m_tonecurve[0], sizeof(otpToneCurveValue)*m_tonecurve.size()); break;
    }
}

void otpParam::setValue(const void *src, int len)
{
    if (!src) { return; }

    switch (getType()) {
    case otParamType_Double:    m_value.double_v = *(otpDoubleValue*)src; break;
    case otParamType_Range:     m_value.range_v = *(otpRangeValue*)src; break;
    case otParamType_Color:     m_value.color_v = *(otpColorValue*)src; break;
    case otParamType_Point:     m_value.point_v = *(otpPointValue*)src; break;
    case otParamType_Enum:      m_value.enum_v = *(otpEnumValue*)src; break;
    case otParamType_Int:       m_value.int_v = *(otpIntValue*)src; break;
    case otParamType_Bool:      m_value.bool_v = *(otpBoolValue*)src; break;
    case otParamType_Spectrum:  m_value.spectrum_v = *(otpSpectrumValue*)src; break;
    case otParamType_String:
        m_string = (const char*)src;
        break;
    case otParamType_ToneCurve:
        m_tonecurve.assign((otpToneCurveValue*)src, (otpToneCurveValue*)src + len);
        break;
    }
}

otpParamInfo& otpParam::getRawInfo() { return m_info; }
