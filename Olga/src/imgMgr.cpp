
#include <iostream>
#include <assert.h>
#include "imgMgr.hpp"

namespace detector {

ImgMgr::ImgMgr(char *argv)
: m_img_width(0), m_img_height(0), m_channel_depth(0), m_pixels(nullptr)
{
    /* Initialize our image library */
    Magick::InitializeMagick(argv);
}

ImgMgr::~ImgMgr(void)
{
    if (m_pixels != nullptr) {
        delete [] m_pixels;
    }
}

unsigned ImgMgr::getImgWidth(void)
{
    return m_img_width;
}

unsigned ImgMgr::getImgHeight(void)
{
    return m_img_height;
}

unsigned ImgMgr::getChannelDepth(void)
{
    return m_channel_depth;
}

unsigned ImgMgr::getPixelCount(void)
{
    return m_img_width * m_img_height;
}

pixel_t* ImgMgr::getPixelHandle(void)
{
    return m_pixels;
}

void ImgMgr::read_image(const std::string &in_filename)
{
    try {
        Magick::Image img;
        img.read(in_filename);

        /* populate internal data structures */
        m_img_width = img.columns();
        m_img_height = img.rows();
        m_channel_depth = img.depth();
        
        if (nullptr == m_pixels) {
            m_pixels = new pixel_t[getPixelCount()]; // free'd in destructor
        } else {
            delete [] m_pixels;
            m_pixels = new pixel_t[getPixelCount()]; // free'd in destructor
        }
        assert(nullptr != m_pixels);

        #ifdef DEBUG
        std::cout << "Image: " << in_filename << std::endl;
        std::cout << "Resolution: " << m_img_width << "x" << m_img_height << std::endl;
        std::cout << "Pixel channels: " << img.channels() << std::endl;
        std::cout << "Channel depth: " << m_channel_depth << std::endl;
        #endif

        /* extract the pixels from the image, put them in a format we can export portably */
        const Magick::Quantum *pixels = img.getConstPixels(0, 0, img.columns(), img.rows());
        for (unsigned i = 0; i < img.rows(); i++) {
            for (unsigned j = 0; j < img.columns(); j++) {
                /* remap to a flat buffer of pixel structs */
                unsigned idx = img.columns() * i + j; 
                m_pixels[idx].red   = *pixels++;
                m_pixels[idx].green = *pixels++;
                m_pixels[idx].blue  = *pixels++;
            }
        }
    }

    catch (const Magick::Exception &e) {
        std::cerr << "Error reading image: " << e.what() << std::endl;
    }
}

void ImgMgr::write_image(const std::string &out_filename)
{
    try {
        /* create a new white image of the same size as our input image */
        Magick::Image img(Magick::Geometry(m_img_width, m_img_height), Magick::Color("white"));
        /* get image lock */
        img.modifyImage();
        /* overwrite new image's pixels with our own */
        Magick::Quantum *pixels = img.getPixels(0, 0, img.columns(), img.rows());
        for (unsigned i = 0; i < img.rows(); i++) {
            for (unsigned j = 0; j < img.columns(); j++) {
                /* extract channels from our array of pixel structs */
                unsigned idx = img.columns() * i + j; 
                *pixels++ = m_pixels[idx].red;
                *pixels++ = m_pixels[idx].green;
                *pixels++ = m_pixels[idx].blue;
            }
        }
        /* save image */
        img.syncPixels();
        /* write to output file */
        img.write(out_filename);
    }

    catch (const Magick::Exception &e) {
        std::cerr << "Error writing image: " << e.what() << std::endl;
    }
}

void ImgMgr::test_red_dot(const std::string &out_filename)
{
    /* create a white image with a red dot in the center */
    Magick::Image image("1000x1000", "white");
    image.pixelColor(499,499,"red");
    image.write(out_filename);
}
}