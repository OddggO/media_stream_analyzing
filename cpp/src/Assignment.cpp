#include "Assignment.h"

Model::PostProcessCb AssignmentAnalyze::getPostProcessFunc(AssgnmentType type)
{
    switch (type)
    {
    case IMAGE_CLASSIFICATION:
        return &imageClassification;
    case IMAGE_MULTI_CLASSIFICATION:
        return &imageMultiClassification;
    case IMAGE_DETECTION:
        return &imageDection;
    default:
        return nullptr;
    }
}

Model::PostProcessCb AssignmentAnalyze::getPostProcessFunc(std::string type)
{
    if (type.compare("IMAGE_CLASSIFICATION") == 0)
        return &imageClassification;
    else if (type.compare("IMAGE_MULTI_CLASSIFICATION") == 0)
        return &imageMultiClassification;
    else if (type.compare("IMAGE_DETECTION") == 0)
        return &imageDection_yolov5;
    else
        return nullptr;
}

void AssignmentAnalyze::imageClassification(const std::vector<float>& output, nlohmann::json& result, std::vector<std::string> className, std::vector<int> riskClsIdx)
{
    int predictClsIdx = std::max_element(output.begin(), output.end()) - output.begin();
    for (auto& clsIdx : riskClsIdx)
    {
        if (predictClsIdx == clsIdx) {
            result["predict_cls_idx"] = predictClsIdx;
            result["predict_cls_name"] = className[predictClsIdx];
        }
    }
}

void AssignmentAnalyze::imageMultiClassification(const std::vector<float>& output, nlohmann::json& result, std::vector<std::string> className, std::vector<int> riskClsIdx)
{
    // TODO
}

void AssignmentAnalyze::imageDection_yolov5(const std::vector<float>& output, nlohmann::json& result, std::vector<std::string> className, std::vector<int> riskClsIdx)
{
    // 1.完成目标检测的后处理过程


    // 2.根据检测结果，将风险类别写入result中
}
