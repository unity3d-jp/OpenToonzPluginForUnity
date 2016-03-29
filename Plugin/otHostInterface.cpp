#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otPlugin.h"

#ifndef otMaster
    #define Trace(Fmt, ...) printf(__FUNCTION__ " " Fmt "\n", __VA_ARGS__)
#else
    #define Trace(Fmt, ...)
#endif

int param_get_type(toonz_param_handle_t handle, double frame, int *type, int *counts)
{
    Trace("handle: %p, frame: %.4lf", handle, frame);
    auto obj = (otParam*)handle;
    return TOONZ_OK;
}

int param_get_value(toonz_param_handle_t handle, double frame, int *pcounts, void *pvalue)
{
    Trace("handle: %p, frame: %.4lf", handle, frame);
    auto obj = (otParam*)handle;
    return TOONZ_OK;
}

int param_set_value(toonz_param_handle_t handle, double frame, int counts, const void *pvalue)
{
    Trace("handle: %p, frame: %.4lf", handle, frame);
    auto obj = (otParam*)handle;
    return TOONZ_OK;
}

int param_get_string_value(toonz_param_handle_t handle, int *wholesize, int rcvbufsize, char *pvalue)
{
    Trace("handle: %p", handle);
    auto obj = (otParam*)handle;
    return TOONZ_OK;
}

int param_get_spectrum_value(toonz_param_handle_t handle, double frame, double x, toonz_param_spectrum_t *pvalue)
{
    Trace("handle: %p, frame: %.4lf", handle, frame);
    auto obj = (otParam*)handle;
    return TOONZ_OK;
}

static toonz_param_interface_t g_toonz_param_interface = {
    { 1, 0 },
    param_get_type,
    param_get_value,
    param_set_value,
    param_get_string_value,
    param_get_spectrum_value,
};



int node_get_input_port(toonz_node_handle_t handle, const char *name, toonz_port_handle_t *port)
{
    Trace("handle: %p, name: %s", handle, name);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

int node_get_rect(toonz_rect_t *rect, double *x0, double *y0, double *x1, double *y1)
{
    Trace("");
    return TOONZ_OK;
}

int node_set_rect(toonz_rect_t *rect, double x0, double y0, double x1, double y1)
{
    Trace("");
    return TOONZ_OK;
}

int node_get_param(toonz_node_handle_t handle, const char *name, toonz_param_handle_t *param)
{
    Trace("handle: %p, name: %s", handle, name);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

int node_set_user_data(toonz_node_handle_t handle, void *user_data)
{
    Trace("handle: %p, user_data: %p", handle, user_data);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

int node_get_user_data(toonz_node_handle_t handle, void **user_data)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

static toonz_node_interface_t g_toonz_node_interface = {
    { 1, 0 },
    node_get_input_port,
    node_get_rect,
    node_set_rect,
    node_get_param,
    node_set_user_data,
    node_get_user_data,
};



int fxnode_get_bbox(toonz_fxnode_handle_t handle, const toonz_rendering_setting_t *, double frame, toonz_rect_t *rect, int *get_bbox)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

int fxnode_can_handle(toonz_fxnode_handle_t handle, const toonz_rendering_setting_t *, double frame, int *can_handle)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

int fxnode_get_input_port_count(toonz_fxnode_handle_t handle, int *count)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

int fxnode_get_input_port(toonz_fxnode_handle_t handle, int index, toonz_port_handle_t *port)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

int fxnode_compute_to_tile(toonz_fxnode_handle_t handle, const toonz_rendering_setting_t *, double frame, const toonz_rect_t *rect, toonz_tile_handle_t intile, toonz_tile_handle_t outtile)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

static toonz_fxnode_interface_t g_toonz_fxnode_interface = {
    { 1, 0 },
    fxnode_get_bbox,
    fxnode_can_handle,
    fxnode_get_input_port_count,
    fxnode_get_input_port,
    fxnode_compute_to_tile,
};



int tile_get_raw_address_unsafe(toonz_tile_handle_t handle, void **dst)
{
    Trace("handle: %p", handle);
    auto obj = (ImageBase*)handle;
    *dst = obj->getData();
    return TOONZ_OK;
}

int tile_get_raw_stride(toonz_tile_handle_t handle, int *stride)
{
    Trace("handle: %p", handle);
    auto obj = (ImageBase*)handle;
    *stride = obj->getPixelSize() * obj->getWidth();
    return TOONZ_OK;
}

int tile_get_element_type(toonz_tile_handle_t handle, int *element)
{
    Trace("handle: %p", handle);
    auto obj = (ImageBase*)handle;
    switch (obj->getPixelType()) {
    case fcPixelFormat_RGBAu8:  *element = TOONZ_TILE_TYPE_32P; break;
    case fcPixelFormat_RGBAi16: *element = TOONZ_TILE_TYPE_64P; break;
    case fcPixelFormat_RGu8:    *element = TOONZ_TILE_TYPE_GR8P; break;
    case fcPixelFormat_RGi16:   *element = TOONZ_TILE_TYPE_GR16P; break;
    default: printf(__FUNCTION__ ": !should not be here!\n"); break;
    }
    return TOONZ_OK;
}

int tile_copy_rect(toonz_tile_handle_t handle, int left, int top, int width, int height, void *dst, int dststride)
{
    Trace("handle: %p", handle);
    auto obj = (ImageBase*)handle;

    left = std::min<int>(obj->getWidth(), left);
    top = std::min<int>(obj->getHeight(), top);
    width = std::min<int>(obj->getWidth() - left, width);
    height = std::min<int>(obj->getHeight() - top, height);
    int pitch = std::min<int>(width * obj->getPixelSize(), dststride);

    for (int yi = 0; yi < height; ++yi) {
        void *d = (char*)dst + dststride * yi;
        void *s = obj->getData(left, top + yi);
        memcpy(d, s, pitch);
    }
    return TOONZ_OK;
}

int tile_create_from(toonz_tile_handle_t handle, toonz_tile_handle_t *newhandle)
{
    Trace("handle: %p", handle);
    auto obj = (ImageBase*)handle;
    *newhandle = obj->clone();
    return TOONZ_OK;
}

int tile_create(toonz_tile_handle_t *newhandle)
{
    Trace("");
    *newhandle = new TImage<TPixel<uint8_t, 4>>();
    return TOONZ_OK;
}

int tile_destroy(toonz_tile_handle_t handle)
{
    Trace("handle: %p", handle);
    auto obj = (ImageBase*)handle;
    delete obj;
    return TOONZ_OK;
}

int tile_get_rectangle(toonz_tile_handle_t handle, toonz_rect_t *rect)
{
    Trace("handle: %p", handle);
    auto obj = (ImageBase*)handle;
    rect->x0 = 0.0f;
    rect->x1 = (double)obj->getWidth();
    rect->y0 = 0.0f;
    rect->y1 = (double)obj->getHeight();
    return TOONZ_OK;
}

int tile_safen(toonz_tile_handle_t handle)
{
    Trace("handle: %p", handle);
    // nothing to do
    return TOONZ_OK;
}

static toonz_tile_interface_t g_toonz_tile_interface = {
    { 1, 0 },
    tile_get_raw_address_unsafe,
    tile_get_raw_stride,
    tile_get_element_type,
    tile_copy_rect,
    tile_create_from,
    tile_create,
    tile_destroy,
    tile_get_rectangle,
    tile_safen,
};



int setup_set_parameter_pages(toonz_node_handle_t handle, int num, toonz_param_page_t *pages)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    obj->setParamInfo(pages, num);
    return TOONZ_OK;
}

int setup_set_parameter_pages_with_error(toonz_node_handle_t handle, int num, toonz_param_page_t *pages, int *reason, void **position)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    obj->setParamInfo(pages, num);
    return TOONZ_OK;
}

int setup_add_input_port(toonz_node_handle_t handle, const char *name, int type)
{
    Trace("handle: %p, name: %s, type: %d", handle, name, type);
    auto obj = (otPlugin*)handle;
    return TOONZ_OK;
}

static toonz_setup_interface_t g_toonz_setup_interface = {
    { 1, 0 },
    setup_set_parameter_pages,
    setup_set_parameter_pages_with_error,
    setup_add_input_port,
};



int port_is_connected(toonz_port_handle_t handle, int *is_connected)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    *is_connected = 1;
    return TOONZ_OK;
}

int port_get_fx(toonz_port_handle_t handle, toonz_fxnode_handle_t *fxnode)
{
    Trace("handle: %p", handle);
    auto obj = (otPlugin*)handle;
    *fxnode = obj;
    return TOONZ_OK;
}

static toonz_port_interface_t g_toonz_port_interface = {
    { 1, 0 },
    port_is_connected,
    port_get_fx,
};



inline bool operator==(const toonz_UUID& a, const toonz_UUID& b)
{
    return memcmp(&a, &b, sizeof(a)) == 0;
}

int host_query_interface(const toonz_UUID *id, void **dst)
{
    if      (*id == toonz_uuid_node__)   { *dst = &g_toonz_node_interface; }
    else if (*id == toonz_uuid_port__)   { *dst = &g_toonz_port_interface; }
    else if (*id == toonz_uuid_tile__)   { *dst = &g_toonz_tile_interface; }
    else if (*id == toonz_uuid_fx_node__){ *dst = &g_toonz_fxnode_interface; }
    else if (*id == toonz_uuid_param__)  { *dst = &g_toonz_param_interface; }
    else if (*id == toonz_uuid_setup__)  { *dst = &g_toonz_setup_interface; }
    else { return TOONZ_ERROR_UNKNOWN; }

    return TOONZ_OK;
}

void host_release_interface(void *)
{
    // do nothing
}

toonz_host_interface_t g_toonz_host_interface = {
    { 1, 0 },
    host_query_interface,
    host_release_interface,
};
