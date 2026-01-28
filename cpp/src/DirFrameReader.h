#pragma once
#include "FrameReader.h"
#include <string>
#include <vector>

class DirFrameReader: FrameReader
{
public:
    DirFrameReader(const std::string& dirName);
    bool read(cv::Mat& frame) override;
private:
    void scanDir();
    void scanDirPosix();
    void scanDirWindows();
    bool isImageFile(const std::string& path) const;
private:
    std::string mDirName;
    std::vector<std::string> mFiles;
    size_t mIdx = 0;
};

