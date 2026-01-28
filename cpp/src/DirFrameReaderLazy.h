#pragma once
#include "FrameReader.h"
#include <string>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif // _WIN32

class DirFrameReaderLazy:public FrameReader
{
public:
    DirFrameReaderLazy(const std::string& dirName);
    ~DirFrameReaderLazy();
    bool read(cv::Mat& frame) override;
private:
    bool isImageFile(const std::string& path) const;
private:
    std::string mDirName;
#ifdef _WIN32
#else
    DIR* mDirStream;
    struct dirent* mEntry;
#endif // _WIN32
};

