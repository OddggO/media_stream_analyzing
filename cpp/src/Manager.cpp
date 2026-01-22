#include "Manager.h"
#include "Log.h"
#include <fstream>
#include <string>

Manager::Manager(const std::string& configFile): mConfigFile(configFile)
{
    LOGI("Manager construct, configFile: %s", mConfigFile.data());
}

Manager::~Manager()
{
    LOGI("Manager destroy, configFile: %s", mConfigFile.data());
}

bool Manager::startServer()
{
    LOGI("begin manager server");
    std::ifstream f(mConfigFile);
    if (!f.is_open()) {
        LOGI("open config file %s failed", mConfigFile.data());
        return false;
    }
    nlohmann::json configs = nlohmann::json::parse(f);
    if (!configs.is_array()) {
        LOGI("open config file %s must be array", mConfigFile.data());
        return false;
    }
    LOGI("read configs: \n%s", configs.dump().data());

    std::vector<std::shared_ptr<TrtModelWorker>> trtModelWorkers;
    for (int i = 0; i < configs.size(); ++i) {
        const auto& config = configs[i];

        std::string assignment_name = config.at("assignment_name").get<std::string>();
        std::string source_type      = config.at("source_type").get<std::string>();
        std::string source           = config.at("source").get<std::string>();
        int max_frame_size           = config.at("max_frame_size").get<int>();
        std::string assignment_type  = config.at("assignment_type").get<std::string>();
        std::string model_name  = config.at("model_name").get<std::string>();
        std::string engine_path      = config.at("engine_path").get<std::string>();
        int inImgWidth = config.at("inImgWidth").get<int>();
        int inImgHeight = config.at("inImgHeight").get<int>();
        int max_result_size          = config.at("max_result_size").get<int>();
        std::string destination_url  = config.at("destination_url").get<std::string>();
        const auto& class_names_json = config.at("class_names");
        std::vector<std::string> class_names = class_names_json.get<std::vector<std::string>>();
        const auto& risk_idx_json = config.at("risk_cls_idx");
        std::vector<int> risk_cls_idx = risk_idx_json.get<std::vector<int>>();

        // if (assignment_name.compare(""))
        StreamAnalyzer streamAnalyzer;
        streamAnalyzer.mMediaWorker = std::make_shared<MediaWorker>("media_worker_" + std::to_string(i), assignment_name, 
            source_type, source, destination_url, max_frame_size);
        streamAnalyzer.mTrtModelWorker = std::make_shared<TrtModelWorker>("trt_model_worker_" + std::to_string(i), model_name, 
            engine_path, inImgWidth, inImgHeight, AssignmentAnalyze::getPostProcessFunc(assignment_type), class_names, risk_cls_idx, streamAnalyzer.mMediaWorker, max_result_size);
        streamAnalyzer.mMediaWorker->start();
        streamAnalyzer.mTrtModelWorker->start();
        trtModelWorkers.push_back(streamAnalyzer.mTrtModelWorker);
        mStreamAnalyzers.push_back(streamAnalyzer);
    }
    mMessageWorker = std::make_shared<MessageWorker>("message_worker", "0.0.0.0", 12020, 
        2, trtModelWorkers);
    mMessageWorker->start();

    for (int i = 0; i < mStreamAnalyzers.size(); ++i)
    {
        mStreamAnalyzers[i].mMediaWorker->join();
        mStreamAnalyzers[i].mTrtModelWorker->stop();
    }
    mMessageWorker->stop();
    // mMessageWorker->join();
    return true;
}
