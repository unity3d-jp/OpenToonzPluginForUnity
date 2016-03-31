#ifndef otpPort_h
#define otpPort_h


class otpPort
{
public:
    otpPort(otpInstance *parent, const char *name);

    otpInstance*    getInstance() const;
    const char*     getName() const;
    void            setImage(otpImage *img);
    ImageRGBAu8*    getImage() const;

private:
    otpInstance *m_parent;
    std::string m_name;
    ImageRGBAu8 *m_image;
};

#endif // otpPort_h
