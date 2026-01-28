#include "DirFrameReader.h"
#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif // _WIN32
#include <algorithm>
#include "Log.h"

DirFrameReader::DirFrameReader(const std::string& dirName): mDirName(dirName)
{
    if (mDirName.back() != '/') {
        mDirName += '/';
    }
    scanDir();
}

bool DirFrameReader::read(cv::Mat& frame)
{
    if (mIdx >= mFiles.size()) 
        return false;
    frame = cv::imread(mFiles[mIdx++]);
    return !frame.empty();
}

void DirFrameReader::scanDir()
{
    mFiles.clear();
#ifndef _WIN32
    scanDirPosix();
#else
    scanDirWindows();
#endif // _WIN32
}

void DirFrameReader::scanDirPosix()
{
#ifndef _WIN32
    DIR* dp = opendir(mDirName.data());
    if (!dp) {
        LOGE("open director %s failed!", mDirName.data());
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dp)) != nullptr) 
    {
        if (strcmp(".", entry->d_name) == 0 || 
            strcmp("..", entry->d_name) == 0)
        {
            continue;
        }

        std::string fullPath = mDirName;
        // if (fullPath.back() != '/') { // 改为确保mDirName是'/'结尾
        //     fullPath += '/';
        // }
        fullPath += entry->d_name;

        struct stat st;
        if (stat(fullPath.data(), &st) != 0) 
            continue;
        if (!S_ISREG(st.st_mode))
            continue;
        if (isImageFile(fullPath)) {
            mFiles.emplace_back(fullPath);
        }
    }
    closedir(dp);

    std::sort(mFiles.begin(), mFiles.end());
#endif // _WIN32
}

void DirFrameReader::scanDirWindows()
{
#ifdef _WIN32
#endif // _WIN32
}

bool DirFrameReader::isImageFile(const std::string& path) const
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
