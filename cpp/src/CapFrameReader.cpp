#include "CapFrameReader.h"

CapFrameReader::CapFrameReader(cv::VideoCapture& cap): mCap(cap){}

bool CapFrameReader::read(cv::Mat& frame)
{
    return mCap.read(frame) && !frame.empty();
}
