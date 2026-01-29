#include "MediaWorker.h"
#include "Log.h"
#include "CapFrameReader.h"
#include "DirFrameReaderLazy.h"
#define MAX_READ_FAIL_NUM 5

MediaWorker::MediaWorker(const std::string& threadName, const std::string& assignmentName, 
    const std::string& sourceType, const std::string& source, const std::string& destinationUrl, 
    int maxFrameSize): WorkerThread(threadName), mAssignmentName(assignmentName), mSource(source), 
                       mDestinationUrl(destinationUrl), mMaxFrameSize(maxFrameSize), mQueue(maxFrameSize), 
                       mReadFrameCnt(0)
{
    LOGI("MediaWorker construct, threadName=%s, assignmentName=%s, "
        "source=%s, destinationUrl=%s", mThreadName.data(), mAssignmentName.data(), 
        mSource.data(), mDestinationUrl.data());
    if (sourceType.compare("IMAGEDIR") == 0) {
        mSourceType = IMAGEDIR;
        mFrameReader = std::make_shared<DirFrameReaderLazy>(source);
    }
    else if (sourceType.compare("MP4") == 0) {
        mSourceType = MP4;
        mCap = cv::VideoCapture(source);
        if (!mCap.isOpened()) {
            LOGE("open source[%s] failed", source.data());
            throw std::invalid_argument("open source " + source + " failed");
        }
        mFrameReader = std::make_shared<CapFrameReader>(mCap);
    }
    else if (sourceType.compare("RTSP") == 0) {
        mSourceType = RTSP;
        mCap = cv::VideoCapture(source);
        if (!mCap.isOpened()) {
            LOGE("open source[%s] failed", source.data());
            throw std::invalid_argument("open source " + source + " failed");
        }
        mFrameReader = std::make_shared<CapFrameReader>(mCap);
    }
    else if (sourceType.compare("RTMP") == 0) {
        mSourceType = RTMP;
        mCap = cv::VideoCapture(source);
        if (!mCap.isOpened()) {
            LOGE("open source[%s] failed", source.data());
            throw std::invalid_argument("open source " + source + " failed");
        }
        mFrameReader = std::make_shared<CapFrameReader>(mCap);
    } else {
        LOGI("invalid sourceType: %s", sourceType.data());
        throw std::invalid_argument("invalid sourceType, must in " + getSourceTypeDescription());
    }
    if (!parseUrl(destinationUrl)) {
        throw std::invalid_argument("parse url failed. url=" + destinationUrl);
    }

}

MediaWorker::~MediaWorker()
{
    LOGI("MediaWorker construct, threadName=%s, assignmentName=%s, "
        "source=%s, destinationUrl=%s", mThreadName.data(), mAssignmentName.data(), 
        mSource.data(), mDestinationUrl.data());    
}

std::string MediaWorker::getSourceTypeDescription()
{
    return "(FILE, RTSP, RTMP)";
}

void MediaWorker::addStreamInfo(nlohmann::json& result)
{
    result["assgnment_name"] = mAssignmentName;
    // result["source_type"] = mSourceType;
    result["source"] = mSource;
    result["destination_url"] = mDestinationUrl; 
    // result["dest_scheme"] = mDestScheme;
    // result["dest_path"] = mDestPath;
    // result["dest_host"] = mDestHost;
    // result["dest_port"] = mDestPort;
}

cv::Mat MediaWorker::pop()
{
    return mQueue.pop();
}

std::string MediaWorker::destUrl() const
{
    return mDestinationUrl;
}

std::string MediaWorker::destScheme() const
{
    return mDestScheme;
}

std::string MediaWorker::destHost() const
{
    return mDestHost;
}

int MediaWorker::destPort() const
{
    return mDestPort;
}

std::string MediaWorker::destPath() const
{
    return mDestPath;
}


void MediaWorker::run() 
{
    LOGI("thread %s start, source %s analyzed begin", mThreadName.data(), mSource.data());
    cv::Mat frame;
    int failedCnt = 0;
    int cnt = 0;
    while (!mIsStop) {
        ++cnt;
        // LOGI("read %d frame", +cnt);
        // if (!mCap.read(frame) || frame.empty()) {
        if (!mFrameReader->read(frame)) {
            ++failedCnt;
            if (failedCnt >= MAX_READ_FAIL_NUM) {
                LOGI("read %s end/EOF", mSource.data());
                break;
            }
            continue;
        }
        mQueue.push(frame);
    }
    LOGI("thread %s end, source %s read over, read %d frame", mThreadName.data(), mSource.data(), cnt);
}

bool MediaWorker::parseUrl(const std::string& url)
{
    size_t schemePos = url.find("://");
    mDestScheme = url.substr(0, schemePos);
    std::string rest = url.substr(schemePos + 3);
    size_t pathPos = rest.find("/");
    std::string hostPort = rest.substr(0, pathPos);
    mDestPath = (pathPos == std::string::npos) ? "/" : rest.substr(pathPos);

    size_t portPos = hostPort.find(":");
    if (portPos != std::string::npos) {
        mDestHost = hostPort.substr(0, portPos);
        mDestPort = std::stoi(hostPort.substr(portPos + 1));
    } else {
        mDestHost = hostPort;
        mDestPort = (mDestScheme == "https") ? 443 : 80;
    }

    LOGI("parse url: %s, mDestScheme: %s, mDestPath: %s, mDestHost: %s, mDestPort: %d", 
        url.data(), mDestScheme.data(), mDestPath.data(), mDestHost.data(), mDestPort);
    
    return true;
}
