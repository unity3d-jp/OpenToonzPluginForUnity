#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otHostInterface.h"
#include "otPlugin.h"


othNode::othNode(otPlugin *plugin)
    : m_plugin(plugin)
{
}

void othNode::setParam(const char *name, otParamType type, const void *value)
{
}

othParam* othNode::getParam(const char *name)
{
    return nullptr;
}

void othNode::setParamInfo(toonz_param_page_t *pages, int num_pages)
{
    m_plugin->setParamInfo(pages, num_pages);
}
