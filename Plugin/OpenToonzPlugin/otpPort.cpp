#include "pch.h"
#include "Foundation.h"
using namespace utj;

#include "OpenToonzPlugin.h"
#include "otpModule.h"
#include "otpInstance.h"
#include "otpPort.h"
#include "otpParam.h"

otpPort::otpPort(otpInstance *parent, const char *name)
    : m_parent(parent)
    , m_name(name)
    , m_image()
{
}

otpInstance* otpPort::getInstance() const { return m_parent; }
const char* otpPort::getName() const { return m_name.c_str(); }
void otpPort::setImage(otpImage *img) { m_image = (ImageRGBAu8*)img; }
ImageRGBAu8* otpPort::getImage() const { return m_image; }
