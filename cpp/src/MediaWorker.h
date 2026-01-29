#pragma once
#include "WorkerThread.h"
#include <string>
#include "SafeQueue.h"
#include <opencv2/opencv.hpp>
#include "nlohmann/json.hpp"
#include "FrameReader.h"
#include <memory>
#define DEFATULT_MAX_FRAME_SIZE 50

class MediaWorker: public WorkerThread
{
public:
    enum SourceType {
        MP4 = 0, 
        RTSP = 1,
        RTMP = 2,
        IMAGEDIR = 3
    };
public:
    MediaWorker(const std::string& threadName, const std::string& assignmentName, const std::string& sourceType, const std::string& source, 
                const std::string& destinationUrl, int maxFrameSize = DEFATULT_MAX_FRAME_SIZE);
    virtual ~MediaWorker();

    static std::string getSourceTypeDescription();
    void addStreamInfo(nlohmann::json& result);
    cv::Mat pop();

    std::string destUrl() const;
    std::string destScheme() const;
    std::string destHost() const;
    int destPort() const;
    std::string destPath() const;
private:
    void run();
    bool parseUrl(const std::string& url);
private:
    std::string mAssignmentName;
    SourceType mSourceType;
    std::string mSource;
    std::string mDestinationUrl;
    std::string mDestScheme;
    std::string mDestHost;
    int mDestPort;
    std::string mDestPath;
    int mMaxFrameSize;
    SafeQueue<cv::Mat> mQueue;
    cv::VideoCapture mCap;
    int mReadFrameCnt;
    // typedef void (*read_one_frame)(cv::Mat& frame);
    std::shared_ptr<FrameReader> mFrameReader;
};
