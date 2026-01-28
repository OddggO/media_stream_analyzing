#pragma once
#include <opencv2/opencv.hpp>

/**
 * 
 */
class FrameReader
{
public:
    virtual ~FrameReader() = default;
    virtual bool read(cv::Mat& frame) = 0;
};
