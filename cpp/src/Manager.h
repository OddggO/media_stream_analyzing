#pragma once
#include "MediaWorker.h"
#include "TrtModelWorker.h"
#include "MessageWorker.h"
#include "Assignment.h"
#include <vector>
#include <string>
#include <memory>

class Manager
{
    struct StreamAnalyzer 
    {
        std::shared_ptr<MediaWorker> mMediaWorker;
        std::shared_ptr<TrtModelWorker> mTrtModelWorker;
    };
public:
    Manager(const std::string& configFile);
    ~Manager();

    bool startServer();
private:
    std::string mConfigFile;
    std::vector<StreamAnalyzer> mStreamAnalyzers;
    std::shared_ptr<MessageWorker> mMessageWorker;
};
