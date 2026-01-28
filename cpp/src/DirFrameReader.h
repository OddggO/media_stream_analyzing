#pragma once
#include "FrameReader.h"
#include <string>

class DirFrameReader: FrameReader
{
public:
    DirFrameReader(std::string dirName);
    bool read(cv::Mat& frame) override;
private:
    std::string mDirName;
};

