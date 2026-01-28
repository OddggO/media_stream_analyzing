#pragma once
#include "FrameReader.h"

class CapFrameReader: public FrameReader
{
public:
    CapFrameReader(cv::VideoCapture& cap);
    bool read(cv::Mat& frame) override;
private:
    cv::VideoCapture& mCap;
};
