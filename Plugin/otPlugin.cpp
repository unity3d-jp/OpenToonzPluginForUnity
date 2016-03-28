#include "pch.h"
#include "OpenToonzPluginForUnity.h"
#include "fcFoundation.h"
#include "otPlugin.h"

otPlugin::otPlugin()
    : m_module()
    , m_probe()
    , m_param_info()
{
}
otPlugin::~otPlugin()
{
    if (m_module) {
        DLLUnload(m_module);
        m_module = nullptr;
    }
}

bool otPlugin::load(const char *path)
{
    m_module = DLLLoad(path);
    if (!m_module) { return false; }

    // todo: gather plugin info

    return true;
}

const char* otPlugin::getName() const   { return m_probe->name; }
const char* otPlugin::getVender() const { return m_probe->vendor; }
const char* otPlugin::getNote() const   { return m_probe->note; }
int         otPlugin::getVersion() const{ return m_probe->plugin_ver.major; }

int otPlugin::getNumParams() const
{
    if (!m_param_info) { return 0; }

    return m_param_info->num;
}
void otPlugin::getParamInfo(otParamInfo *dst)
{
    if (!m_param_info) { return; }

    int n = m_param_info->num;
    for (int i = 0; i < n; ++i) {
        // todo
    }
}

void otPlugin::applyEffect(otParamData *params, const void *src_pixels, void *dst_pixels)
{
    // todo
}
