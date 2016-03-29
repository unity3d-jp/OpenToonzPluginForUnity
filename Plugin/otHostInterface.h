#ifndef otHostInterface_h
#define otHostInterface_h

class othPort;
class othNode;
class othFXNode;
class othParam;
class othTile;
class othSetup;




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
    othNode(otPlugin *plugin);
    void        setParam(const char *name, otParamType type, const void *value);
    othParam*   getParam(const char *name);
    void setParamInfo(toonz_param_page_t *pages, int num_pages);

private:
    otPlugin *m_plugin;
    std::map<std::string, othParam> m_params;
    void *m_userdata;
};


class othFXNode
{
public:
    othFXNode();

private:
};

#endif // otHostInterface_h
