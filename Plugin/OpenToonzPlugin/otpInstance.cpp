#include "pch.h"
#include "Foundation.h"
using namespace utj;

#include "OpenToonzPlugin.h"
#include "otpModule.h"
#include "otpInstance.h"
#include "otpPort.h"
#include "otpParam.h"


otpPluginInfo otpProbeToInfo(toonz_plugin_probe_t *probe);

otpInstance::otpInstance(otpModule *module, toonz_plugin_probe_t *probe)
    : m_module(module)
    , m_probe(probe)
    , m_info(otpProbeToInfo(probe))
    , m_userdata()
    , m_frame()
    , m_dst_image()
    , m_canceled()
{
    if (m_probe->handler) {
        if (m_probe->handler->setup) {
            m_probe->handler->setup(this);
        }
        if (m_probe->handler->create) {
            m_probe->handler->create(this);
        }
    }
}

otpInstance::~otpInstance()
{
    if (m_probe->handler) {
        if (m_probe->handler->destroy) {
            m_probe->handler->destroy(this);
        }
    }
}

static void otpSetupParam(toonz_param_desc_t& desc, otpParam& dst)
{
    otpParamInfo& info = dst.getRawInfo();
    info.name = desc.key;
    info.note = desc.note;
    info.type = (otpParamType)desc.traits_tag;

    switch (info.type) {
    case otParamType_Double:    dst.setValue(&desc.traits.d.def); break;
    case otParamType_Range:     dst.setValue(&desc.traits.rd.def); break;
    case otParamType_Pixel:     dst.setValue(&desc.traits.c.def); break;
    case otParamType_Point:     dst.setValue(&desc.traits.p.def); break;
    case otParamType_Enum:      dst.setValue(&desc.traits.e.def); break;
    case otParamType_Int:       dst.setValue(&desc.traits.i.def); break;
    case otParamType_Bool:      dst.setValue(&desc.traits.b.def); break;
    case otParamType_Spectrum:
        if (desc.traits.g.points > 0) {
            dst.setValue(&desc.traits.g.array[0]);
        }
        break;
    case otParamType_String:    dst.setValue(desc.traits.s.def); break;
    case otParamType_ToneCurve: /* no default value*/ break;
    }
}

void otpInstance::setParamInfo(toonz_param_page_t *pages, int num_pages)
{
    m_params.clear();
    for (int pi = 0; pi < num_pages; ++pi) {
        toonz_param_page_t& page = pages[pi];
        for (int gi = 0; gi < page.num; ++gi) {
            toonz_param_group_t& group = page.array[gi];
            for (int i = 0; i < group.num; ++i) {
                toonz_param_desc_t& desc = group.array[i];

                auto *param = new otpParam(this);
                m_params.emplace_back(otpParamPtr(param));
                otpSetupParam(desc, *param);
            }
        }
    }
}

void otpInstance::addPort(const char *name)
{
    m_ports.emplace_back(otpPortPtr(new otpPort(this, name)));
}

const otpPluginInfo& otpInstance::getPluginInfo() const
{
    return m_info;
}


int otpInstance::getNumParams() const
{
    return (int)m_params.size();
}
otpParam* otpInstance::getParam(int i)
{
    return i < m_params.size() ? m_params[i].get() : nullptr;
}
otpParam* otpInstance::getParamByName(const char *name)
{
    for (auto& p : m_params) {
        if (strcmp(p->getName(), name) == 0) {
            return p.get();
        }
    }
    return nullptr;
}

int otpInstance::getNumPorts() const
{
    return (int)m_ports.size();
}
otpPort* otpInstance::getPort(int i)
{
    return i < m_ports.size() ? m_ports[i].get() : nullptr;
}
otpPort* otpInstance::getPortByName(const char *name)
{
    for (auto& p : m_ports) {
        if (strcmp(p->getName(), name) == 0) {
            return p.get();
        }
    }
    return nullptr;
}

void* otpInstance::getUserData() const { return m_userdata; }
void otpInstance::setUsertData(void *v) { m_userdata = v; }
ImageRGBAu8* otpInstance::getDstImage() { return m_dst_image.get(); }
double otpInstance::getFrame() const { return m_frame; }

void otpInstance::beginRender(int width, int height)
{
    if (!m_dst_image || (width != m_dst_image->getWidth() || height != m_dst_image->getHeight())) {
        m_dst_image.reset(new ImageRGBAu8(width, height));
    }

    m_rs = {
        { 1, 0 },
        this,
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
        1.0,
        1.0, 1.0,
        1.0,
        32,
        1024,
        100,
        0,
        0,
        0,
        0,
        0,
        { 0.0, 0.0, 0.0, 0.0 },
        &m_canceled
    };
    m_probe->handler->start_render(this);
}

otpImage* otpInstance::render(double frame)
{
    if (!m_dst_image) {
        utjDebugLog("dst image is null");
        return nullptr;
    }
    auto dst = m_dst_image.get();

    m_frame = frame;
    m_canceled = 0;

    m_probe->handler->do_get_bbox(this, &m_rs, frame, &m_rect);
    size_t mem = m_probe->handler->get_memory_requirement(this, &m_rs, frame, &m_rect);

    m_probe->handler->on_new_frame(this, &m_rs, frame);
    m_probe->handler->do_compute(this, &m_rs, frame, dst);
    m_probe->handler->on_end_frame(this, &m_rs, frame);

    return dst;
}

void otpInstance::endRender()
{
    m_probe->handler->end_render(this);
}


