#ifndef Image_h
#define Image_h
#include "Buffer.h"
#include "PixelFormat.h"


template<class T, int L> union TPixel;

template<class T>
union TPixel<T, 1>
{
    typedef T value_type;
    static const int length = 1;

    T r;
    T v[1];
};

template<class T>
union TPixel<T, 2>
{
    typedef T value_type;
    static const int length = 2;

    T r, g;
    T v[2];
};

template<class T>
union TPixel<T, 3>
{
    typedef T value_type;
    static const int length = 3;

    T r, g, b;
    T v[3];
};

template<class T>
union TPixel<T, 4>
{
    typedef T value_type;
    static const int length = 4;

    T r, g, b, a;
    T v[4];
};

class half;
template<class T> struct TGetPixelFormatType;
template<> struct TGetPixelFormatType<half>    { static const fcPixelFormat value = fcPixelFormat_Type_f16; };
template<> struct TGetPixelFormatType<float>   { static const fcPixelFormat value = fcPixelFormat_Type_f32; };
template<> struct TGetPixelFormatType<uint8_t> { static const fcPixelFormat value = fcPixelFormat_Type_u8; };
template<> struct TGetPixelFormatType<int16_t> { static const fcPixelFormat value = fcPixelFormat_Type_i16; };
template<> struct TGetPixelFormatType<int32_t> { static const fcPixelFormat value = fcPixelFormat_Type_i32; };

template<class T> struct TGetPixelFormat
{
    static const fcPixelFormat value = fcPixelFormat(TGetPixelFormatType<typename T::value_type>::value | T::length);
};


template<class PixelT> class TImageBase;
template<class PixelT> class TImage;
template<class PixelT> class TImageIntrusive;

class ImageBase
{
public:
    ImageBase() : m_width(), m_height() {}
    virtual ~ImageBase() {}

    int         getWidth() const { return m_width; }
    int         getHeight() const { return m_height; }
    virtual int getPixelSize() const = 0;

protected:
    virtual void* getPixelsImpl() = 0;

    int m_width;
    int m_height;
};

template<class PixelT>
class TImageBase : public ImageBase
{
public:
    typedef PixelT pixel_t;
    typedef TImage<pixel_t> image_t;

    TImageBase() : m_width(), m_height() {}
    virtual ~TImageBase() {}

    pixel_t*        getPixels()                     { return (pixel_t*)getPixelsImpl(); }
    const pixel_t*  getPixels() const               { return const_cast<TImageBase>(this)->getPixels(); }
    int             getPixelSize() const override   { return sizeof(pixel_t); }

    pixel_t&        get(int x, int y)        { return getPixels()[m_width*y + x]; }
    const pixel_t&  get(int x, int y) const  { return getPixels()[m_width*y + x]; }

    image_t copyRect(int x, int y, int w, int h) const;
};

template<class PixelT>
class TImage : public TImageBase<PixelT>
{
public:
    TImage() {}
    TImage(int w, int h) { resize(w, h); }

    void resize(int w, int h)
    {
        m_width = w;
        m_height = h;
        m_data.resize(w * h);
    }

private:
    void* getPixelsImpl() override { return m_data; }

    TBuffer<pixel_t> m_data;
};

template<class PixelT>
class TImageIntrusive : public TImageBase<PixelT>
{
public:
    TImageIntrusive() : m_data() {}
    TImageIntrusive(pixel_t *data, int w, int h)
        : m_data(data)
    {
        m_width = w;
        m_height = h;
    }

protected:
    void* getPixelsImpl() override { return m_data; }

    pixel_t *m_data;
};

template<class PixelT>
inline TImage<PixelT> TImageBase<PixelT>::copyRect(int x, int y, int w, int h) const
{
    x = std::min<int>(m_width, x);
    y = std::min<int>(m_height, y);
    w = std::min<int>(m_width - x, w);
    h = std::min<int>(m_height - y, h);

    image_t ret;
    ret.resize(w, h);
    const pixel_t* src = getPixels();
    pixel_t* dst = ret.getPixels();
    for (int iy = 0; iy < h; ++iy) {
        memcpy(&dst[w * iy], &src[m_width * y + x], sizeof(pixel_t)*w);
    }
    return ret;
}



#endif // Image_h
