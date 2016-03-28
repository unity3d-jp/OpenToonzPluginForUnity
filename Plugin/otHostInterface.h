#ifndef otHostInterface_h
#define otHostInterface_h

class othPort;
class othNode;
class othFXNode;
class othParam;
class othTile;
class othSetup;


class othPort
{
public:
    othPort();

private:
};


class othParam
{
public:
    othParam();
    othParam(const char *name, otParamType type);
    otParamType getType() const;
    const void* getValue() const;
    void        setValue(const void *value);

private:
    std::string name;
    otParamType type;
    otParamData value;
};


class othNode
{
public:
    othNode();
    void        setParam(const char *name, otParamType type, const void *value);
    othParam*   getParam(const char *name);

private:
    std::map<std::string, othParam> m_params;
    void *m_userdata;
};


class othFXNode
{
public:
    othFXNode();

private:
};


class othTile
{
public:
    othTile();
    othTile(void *pixels, int width, int height);

    int     getWidth() const;
    int     getHeight() const;
    void*   getRawData();
    int     getRawStride() const;

private:
    bool    m_needs_delete;
    void    *m_pixels;
    int     m_width;
    int     m_height;
};


class othSetup
{
public:
    othSetup();

private:
};

#endif // otHostInterface_h
