#include "pch.h"
#include "Foundation.h"
using namespace utj;

#include "OpenToonzPlugin.h"
#include "otpModule.h"
#include "otpInstance.h"
#include "otpPort.h"
#include "otpParam.h"

otpParam::otpParam(otpInstance *parent, const toonz_param_desc_t& desc)
    : m_parent(parent)
{
    m_info.name = desc.key;
    m_info.note = desc.note;
    m_info.type = (otpParamType)desc.traits_tag;

    switch (m_info.type) {
    case otParamType_Double:
        m_traits.t_double.def = desc.traits.d.def;
        m_traits.t_double.min = desc.traits.d.min;
        m_traits.t_double.max = desc.traits.d.max;
        setValue(&desc.traits.d.def);
        break;
    case otParamType_Range:
        m_traits.t_range.def = (otpRangeParamValue&)desc.traits.rd.def;
        m_traits.t_range.minmax = (otpRangeParamValue&)desc.traits.rd.minmax;
        setValue(&desc.traits.rd.def);
        break;
    case otParamType_Color:
        setValue(&desc.traits.c.def);
        break;
    case otParamType_Point:
        m_traits.t_point.def = (otpPointParamValue&)desc.traits.p.def;
        m_traits.t_point.min = (otpPointParamValue&)desc.traits.p.min;
        m_traits.t_point.max = (otpPointParamValue&)desc.traits.p.max;
        setValue(&desc.traits.p.def);
        break;
    case otParamType_Enum:
        m_traits.t_enum.def = desc.traits.i.def;
        m_traits.t_enum.num = desc.traits.e.enums;
        m_traits.t_enum.names = desc.traits.e.array;
        setValue(&desc.traits.e.def);
        break;
    case otParamType_Int:
        m_traits.t_int.def = desc.traits.i.def;
        m_traits.t_int.min = desc.traits.i.min;
        m_traits.t_int.max = desc.traits.i.max;
        setValue(&desc.traits.i.def);
        break;
    case otParamType_Bool:
        m_traits.t_enum.def = desc.traits.i.def;
        setValue(&desc.traits.b.def);
        break;
    case otParamType_Spectrum:
        if (desc.traits.g.points > 0) {
            setValue(&desc.traits.g.array[0]);
        }
        break;
    case otParamType_String:
        setValue(desc.traits.s.def);
        break;
    case otParamType_ToneCurve:
        // no default value
        break;
    }
}

otpParam::~otpParam()
{
}

otpInstance* otpParam::getInstance() const { return m_parent; }
otpParamType otpParam::getType() const { return (otpParamType)m_info.type; }
const char* otpParam::getName() const { return m_info.name; }
const char* otpParam::getNote() const { return m_info.note; }

void otpParam::getTraits(void *dst) const
{
    if (!dst) { return; }

    switch (getType()) {
    case otParamType_Double:
        *(otpDoubleParamTraits*)dst = m_traits.t_double;
        break;
    case otParamType_Range:
        *(otpRangeParamTraits*)dst = m_traits.t_range;
        break;
    case otParamType_Color:
        *(otpColorParamTraits*)dst = m_traits.t_color;
        break;
    case otParamType_Point:
        *(otpPointParamTraits*)dst = m_traits.t_point;
        break;
    case otParamType_Enum:
        *(otpEnumParamTraits*)dst = m_traits.t_enum;
        break;
    case otParamType_Int:
        *(otpIntParamTraits*)dst = m_traits.t_int;
        break;
    case otParamType_Bool:
        *(otpBoolParamTraits*)dst = m_traits.t_bool;
        break;
    case otParamType_Spectrum:
        *(otpSpectrumParamTraits*)dst = m_traits.t_spectrum;
        break;
    case otParamType_String:
        *(otpStringParamTraits*)dst = m_traits.t_string;
        break;
    case otParamType_ToneCurve:
        *(otpToneCurveParamTraits*)dst = m_traits.t_tonecurve;
        break;
    }
}

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
    case otParamType_Double:
        *(otpDoubleParamValue*)dst = m_value.v_double;
        break;
    case otParamType_Range:
        *(otpRangeParamValue*)dst = m_value.v_range;
        break;
    case otParamType_Color:
        *(otpColorParamValue*)dst = m_value.v_color;
        break;
    case otParamType_Point:
        *(otpPointParamValue*)dst = m_value.v_point;
        break;
    case otParamType_Enum:
        *(otpEnumParamValue*)dst = m_value.v_enum;
        break;
    case otParamType_Int:
        *(otpIntParamValue*)dst = m_value.v_int;
        break;
    case otParamType_Bool:
        *(otpBoolParamValue*)dst = m_value.v_bool;
        break;
    case otParamType_Spectrum:
        *(otpSpectrumParamValue*)dst = m_value.v_spectrum;
        break;
    case otParamType_String:
        memcpy(dst, m_string.c_str(), m_string.size());
        break;
    case otParamType_ToneCurve:
        memcpy(dst, &m_tonecurve[0], sizeof(otpToneCurveParamValue)*m_tonecurve.size());
        break;
    }
}


template<class Traits, class Value>
static inline void sanitize(const Traits& t, Value& v)
{
    typedef decltype(t.min) value_type;
    if (t.min != t.max) {
        v.value = std::max<value_type>(std::min<value_type>(v.value, t.max), t.min);
    }
}
template<>
static inline void sanitize(const otpBoolParamTraits& t, otpBoolParamValue& v)
{
    v.value = std::max<int>(std::min<int>(v.value, 1), 0);
}
template<>
static inline void sanitize(const otpEnumParamTraits& t, otpEnumParamValue& v)
{
    v.value = std::max<int>(std::min<int>(v.value, t.num), 0);
}
template<>
static inline void sanitize(const otpRangeParamTraits& t, otpRangeParamValue& v)
{
    if (t.minmax.a != t.minmax.b) {
        v.a = std::max<double>(std::min<double>(v.a, t.minmax.b), t.minmax.a);
        v.b = std::max<double>(std::min<double>(v.b, t.minmax.b), t.minmax.a);
    }
}
template<>
static inline void sanitize(const otpPointParamTraits& t, otpPointParamValue& v)
{
    if (t.min.x != t.max.x) {
        v.x = std::max<double>(std::min<double>(v.x, t.max.x), t.min.x);
        v.y = std::max<double>(std::min<double>(v.y, t.max.y), t.min.y);
    }
}

void otpParam::setValue(const void *src, int len)
{
    if (!src) { return; }

    switch (getType()) {
    case otParamType_Double:
        m_value.v_double = *(otpDoubleParamValue*)src;
        sanitize(m_traits.t_double, m_value.v_double);
        break;
    case otParamType_Range:
        m_value.v_range = *(otpRangeParamValue*)src;
        sanitize(m_traits.t_range, m_value.v_range);
        break;
    case otParamType_Color:
        m_value.v_color = *(otpColorParamValue*)src;
        break;
    case otParamType_Point:
        m_value.v_point = *(otpPointParamValue*)src;
        sanitize(m_traits.t_point, m_value.v_point);
        break;
    case otParamType_Enum:
        m_value.v_enum = *(otpEnumParamValue*)src;
        sanitize(m_traits.t_enum, m_value.v_enum);
        break;
    case otParamType_Int:
        m_value.v_int = *(otpIntParamValue*)src;
        sanitize(m_traits.t_int, m_value.v_int);
        break;
    case otParamType_Bool:
        m_value.v_bool = *(otpBoolParamValue*)src;
        sanitize(m_traits.t_bool, m_value.v_bool);
        break;
    case otParamType_Spectrum:
        m_value.v_spectrum = *(otpSpectrumParamValue*)src;
        break;
    case otParamType_String:
        m_string = (const char*)src;
        break;
    case otParamType_ToneCurve:
        m_tonecurve.assign((otpToneCurveParamValue*)src, (otpToneCurveParamValue*)src + len);
        break;
    }
}

otpParamInfo& otpParam::getRawInfo() { return m_info; }
