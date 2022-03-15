
#ifndef _EDGE_DETECTOR_HPP_
#define _EDGE_DETECTOR_HPP_

#include "ed_pixel.h"
#include "imgMgr.hpp"

namespace detector {

class EdgeDetector
{
    public:
        EdgeDetector(std::shared_ptr<ImgMgr> image);
        ~EdgeDetector();

        virtual void detect_edges(bool serial, int num_threads) = 0;
        void single_channel_to_grayscale(pixel_t *out_grayscale, pixel_channel_t *in_pixels, unsigned rows, unsigned cols);

    protected:
        std::shared_ptr<ImgMgr> m_image_mgr;
};

}
#endif // _EDGE_DETECTOR_HPP_
