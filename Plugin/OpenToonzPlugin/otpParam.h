#ifndef otpParam_h
#define otpParam_h


class otpParam
{
public:
    otpParam(otpInstance *parent = nullptr);
    ~otpParam();

    otpInstance*        getInstance() const;

    otpParamType        getType() const;
    const char*         getName() const;
    const char*         getNote() const;

    int                 getLength() const;
    void                getValue(void *dst) const;
    void                setValue(const void *data, int len = 1); // len is relevant only if type is tonecurve

    otpParamInfo&       getRawInfo();

private:
    otpInstance *m_parent;
    otpParamInfo m_info;
    union
    {
        otpDoubleValue    double_v;
        otpIntValue       int_v;
        otpBoolValue      bool_v;
        otpEnumValue      enum_v;
        otpRangeValue     range_v;
        otpColorValue     color_v;
        otpPointValue     point_v;
        otpSpectrumValue  spectrum_v;
    } m_value;
    std::string m_string;
    std::vector<otpToneCurveValue> m_tonecurve;
};

#endif // otpParam_h
