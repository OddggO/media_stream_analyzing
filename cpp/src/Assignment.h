#pragma once
#include <vector>
#include "nlohmann/json.hpp"
#include <string>
#include "Model.h"

namespace AssignmentAnalyze
{
    enum AssgnmentType
    {
        IMAGE_CLASSIFICATION = 0,
        IMAGE_MULTI_CLASSIFICATION = 1,
        IMAGE_DETECTION = 2
    };

    Model::PostProcessCb getPostProcessFunc(AssgnmentType type);
    Model::PostProcessCb getPostProcessFunc(std::string type);

    void imageClassification(const std::vector<float>& output, nlohmann::json& result, std::vector<std::string> className, std::vector<int> riskClsIdx);
    void imageMultiClassification(const std::vector<float>& output, nlohmann::json& result, std::vector<std::string> className, std::vector<int> riskClsIdx);
    void imageDection(const std::vector<float>& output, nlohmann::json& result, std::vector<std::string> className, std::vector<int> riskClsIdx);
};
