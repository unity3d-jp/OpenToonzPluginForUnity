#ifndef utj_Image_h
#define utj_Image_h
#include "Buffer.h"
#include "PixelFormat.h"

namespace utj {

template<class T, int L> union TPixel;

template<class T>
union TPixel<T, 1>
{
    typedef T element_type;
    static const int num_elements = 1;

    struct { T r; };
    T v[1];
};

template<class T>
union TPixel<T, 2>
{
    typedef T element_type;
    static const int num_elements = 2;

    struct { T r, g; };
    T v[2];
};

template<class T>
union TPixel<T, 3>
{
    typedef T element_type;
    static const int num_elements = 3;

    struct { T r, g, b; };
    T v[3];
};

template<class T>
union TPixel<T, 4>
{
    typedef T element_type;
    static const int num_elements = 4;

    struct { T r, g, b, a; };
    T v[4];
};

class half;
template<class T> struct TGetPixelFormatType;
template<> struct TGetPixelFormatType<half>    { static const PixelFormat value = PixelFormat_Type_f16; };
template<> struct TGetPixelFormatType<float>   { static const PixelFormat value = PixelFormat_Type_f32; };
template<> struct TGetPixelFormatType<uint8_t> { static const PixelFormat value = PixelFormat_Type_u8; };
template<> struct TGetPixelFormatType<int16_t> { static const PixelFormat value = PixelFormat_Type_i16; };
template<> struct TGetPixelFormatType<int32_t> { static const PixelFormat value = PixelFormat_Type_i32; };

template<class T> struct TGetPixelFormat
{
    static const PixelFormat value = PixelFormat(TGetPixelFormatType<typename T::element_type>::value | T::num_elements);
};


template<class PixelT> class TImageBase;
template<class PixelT> class TImage;
template<class PixelT> class TImageIntrusive;

class ImageBase
{
public:
    ImageBase() : m_width(), m_height() {}
    virtual ~ImageBase() {}

    virtual ImageBase*  create(int w, int h) const = 0;
    virtual ImageBase*  clone() const = 0;

    int                 getWidth() const { return m_width; }
    int                 getHeight() const { return m_height; }
    virtual int         getPixelSize() const = 0;
    virtual PixelFormat getPixelType() const = 0;

    virtual void*       getData() = 0;
    const void*         getData() const { return const_cast<ImageBase*>(this)->getData(); }
    void*               getData(int x, int y) { return (uint8_t*)getData() + (m_width*y + x) * getPixelSize(); }
    const void*         getData(int x, int y) const { return const_cast<ImageBase*>(this)->getData(x, y); }

protected:
    int m_width;
    int m_height;
};

template<class PixelT>
class TImageBase : public ImageBase
{
public:
    typedef PixelT pixel_t;
    typedef TImage<pixel_t> image_t;

    TImageBase() {}
    virtual ~TImageBase() {}

    ImageBase*      create(int w, int h) const override;
    ImageBase*      clone() const override;

    pixel_t*        getPixels()                     { return (pixel_t*)getData(); }
    const pixel_t*  getPixels() const               { return const_cast<TImageBase*>(this)->getPixels(); }
    int             getPixelSize() const override   { return sizeof(pixel_t); }
    PixelFormat     getPixelType() const override   { return TGetPixelFormat<pixel_t>::value; }

    pixel_t&        getPixel(int x, int y)          { return getPixels()[m_width*y + x]; }
    const pixel_t&  getPixel(int x, int y) const    { return getPixels()[m_width*y + x]; }

    image_t         copyRect(int x, int y, int w, int h) const;
};

template<class PixelT>
class TImage : public TImageBase<PixelT>
{
public:
    TImage() {}
    TImage(int w, int h, const pixel_t *data = nullptr)
    {
        resize(w, h);
        if (data) {
            memcpy(getData(), data, w*h*sizeof(pixel_t));
        }
    }

    void clear()
    {
        m_width = 0;
        m_height = 0;
        m_data.clear();
    }

    void resize(int w, int h)
    {
        m_width = w;
        m_height = h;
        m_data.resize(w * h);
    }

    void* getData() override { return m_data.empty() ? nullptr : &m_data[0]; }

    void move(TImage& other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_data.clear();
        m_data = std::move(other.m_data);
        other.clear();
    }

private:
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

    void* getData() override { return m_data; }

protected:
    pixel_t *m_data;
};

template<class PixelT>
inline ImageBase* TImageBase<PixelT>::create(int w, int h) const
{
    return new TImage<PixelT>(w, h);
}

template<class PixelT>
inline ImageBase* TImageBase<PixelT>::clone() const
{
    return new TImage<PixelT>(getWidth(), getHeight(), getPixels());
}

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
        memcpy(&dst[w * iy], &src[m_width * (y + iy) + x], sizeof(pixel_t)*w);
    }
    return ret;
}

typedef TPixel<uint8_t, 4>  RGBAu8;
typedef TPixel<float, 4>    RGBAf32;

typedef TImage<RGBAu8>      ImageRGBAu8;
typedef TImage<RGBAf32>     ImageRGBAf32;

typedef TImageIntrusive<RGBAu8>     ImageIntrusiveRGBAu8;
typedef TImageIntrusive<RGBAf32>    ImageIntrusiveRGBAf32;

} // namespace utj
#endif // utj_Image_h
