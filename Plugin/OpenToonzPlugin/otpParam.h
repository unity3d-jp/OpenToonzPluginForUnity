#ifndef otpParam_h
#define otpParam_h


class otpParam
{
public:
    otpParam(otpInstance *parent, const toonz_param_desc_t& desc);
    ~otpParam();

    otpInstance*        getInstance() const;

    otpParamType        getType() const;
    const char*         getName() const;
    const char*         getNote() const;
    void                getTraits(void *dst) const;

    int                 getLength() const;
    void                getValue(void *dst) const;
    void                setValue(const void *data, int len = 1); // len is relevant only if type is tonecurve

    otpParamInfo&       getRawInfo();

private:
    otpInstance *m_parent;
    otpParamInfo m_info;
    union {
        otpIntParamTraits       t_int;
        otpBoolParamTraits      t_bool;
        otpEnumParamTraits      t_enum;
        otpDoubleParamTraits    t_double;
        otpRangeParamTraits     t_range;
        otpPointParamTraits     t_point;
        otpColorParamTraits     t_color;
        otpStringParamTraits    t_string;
        otpSpectrumParamTraits  t_spectrum;
        otpToneCurveParamTraits t_tonecurve;
    } m_traits;
    union
    {
        otpIntParamValue       v_int;
        otpBoolParamValue      v_bool;
        otpEnumParamValue      v_enum;
        otpDoubleParamValue    v_double;
        otpRangeParamValue     v_range;
        otpPointParamValue     v_point;
        otpColorParamValue     v_color;
        otpSpectrumParamValue  v_spectrum;
    } m_value;
    std::string m_string;
    std::vector<otpToneCurveParamValue> m_tonecurve;
};

#endif // otpParam_h
