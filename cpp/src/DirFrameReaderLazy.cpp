#include "DirFrameReaderLazy.h"
#include "Log.h"

DirFrameReaderLazy::DirFrameReaderLazy(const std::string& dirName): mDirName(dirName)
{
    if (mDirName.back() != '/') {
        mDirName += '/';
    }
    mDirStream = opendir(mDirName.data());
    if (!mDirStream) {
        LOGE("open director %s failed!", mDirName.data());
        return;
    }
}

DirFrameReaderLazy::~DirFrameReaderLazy()
{
#ifndef _WIN32
    if (mDirStream)
        closedir(mDirStream);
#endif // _WIN32
}

bool DirFrameReaderLazy::read(cv::Mat& frame)
{
#ifdef _WIN32
    // TODO
    return false;
#else
    if (!mDirStream) // 如果构造函数处opendir失败了, 为了避免在不知情的情况下调用read, 需要先判断一下
        return false;
    while ((mEntry = readdir(mDirStream)) == nullptr) 
    {
        if (strcmp(".", mEntry->d_name) == 0 || 
            strcmp("..", mEntry->d_name) == 0)
            continue;

        std::string fullPath = mDirName + mEntry->d_name;
        struct stat st;
        if (stat(fullPath.data(), &st) != 0) 
            continue;
        if (!S_ISREG(st.st_mode))
            continue;
        if (!isImageFile(fullPath))
            continue;

        frame = cv::imread(fullPath);
        if (!frame.empty())
            return true;
    }

    return false; // EOF
#endif // _WIN32
}

bool DirFrameReaderLazy::isImageFile(const std::string& path) const
{
    auto pos = path.find_last_of('.');
    if (pos == std::string::npos) 
        return false;
    std::string ext = path.substr(pos);
    if (strcasecmp(ext.data(), ".jpg") == 0)
        return true;
    if (strcasecmp(ext.data(), ".jpeg") == 0)
        return true;
    if (strcasecmp(ext.data(), ".png") == 0)
        return true;
    return false;
}
