#pragma once
#include "WorkerThread.h"
#include <vector>
#include <string>
#include "SafeQueue.h"
#include "nlohmann/json.hpp"
#include "MediaWorker.h"
#include "TrtModel.h"
#include <memory>
#define DEFAULT_MAX_RESULT_SIZE 100

class TrtModelWorker: public WorkerThread
{
public:
    TrtModelWorker(const std::string& threadName, const std::string& modelName, const std::string& modelFilePath, 
        int inImgWidth, int inImgHeight, TrtModel::PostProcessCb cb, const std::vector<std::string>& classNames, 
        const std::vector<int>& riskClsIdx, std::shared_ptr<MediaWorker> mediaWorker, int maxResultSize = DEFAULT_MAX_RESULT_SIZE);
    ~TrtModelWorker();
        
    void push(const nlohmann::json& result);
    nlohmann::json pop();
    std::shared_ptr<MediaWorker> mediaWorker() const;
private:
    void run() override;
private:
    TrtModel* mTrtModel;
    // MediaWorker* mMediaWorker;
    std::shared_ptr<MediaWorker> mMediaWorker;
    SafeQueue<nlohmann::json> mQueue;
};
