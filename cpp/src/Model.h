#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "nlohmann/json.hpp"
#include <vector>
#include "Log.h"

class Model
{
public:
    // using PostProcessCb = void (*)(
    //     nlohmann::json&,
    //     std::vector<std::string>,
    //     std::vector<int>
    // );
    typedef void (*PostProcessCb)(const std::vector<float>& output, nlohmann::json& result, std::vector<std::string> className, std::vector<int> riskClsIdx);
    Model(const std::string& modelName, const std::string& modelFilePath, int inImgWidth, int inImgHeight, 
        PostProcessCb cb, const std::vector<std::string>& classNames, const std::vector<int>& riskClsIdx);

    virtual ~Model();

    std::string modelName() const;
    std::string modelFilePath() const;

protected:
    virtual bool modelInit() = 0;

    cv::Mat preprocess(const cv::Mat& img)
    {
        cv::Mat resized;
        cv::resize(img, resized, cv::Size(mInImgWidth, mInImgHeight));

        cv::Mat float_img;
        resized.convertTo(float_img, CV_32FC3, 1.0 / 255.0);

        CV_Assert(float_img.channels() == 3);

        std::vector<cv::Mat> channels;
        cv::split(float_img, channels);
        CV_Assert(channels.size() == 3);

        int sizes[] = {1, 3, mInImgHeight, mInImgWidth};
        cv::Mat ret(4, sizes, CV_32F);
        CV_Assert(ret.isContinuous());

        const size_t plane_size = mInImgHeight * mInImgWidth * sizeof(float);

        for (int c = 0; c < 3; ++c) {
            CV_Assert(channels[c].isContinuous());
            std::memcpy(ret.ptr<float>(0, c),
                        channels[c].ptr<float>(),
                        plane_size);
        }

        return ret.clone(); // 确保连续
    }

    virtual void inference(const cv::Mat& img, nlohmann::json& result) = 0;

//     void setPostProcessCb(PostProcessCb cb)
//     {
//         mPostProcessCb = cb;
//     }
protected:
    std::string mModelName;
    std::string mModelFilePath;
    int mInImgWidth;
    int mInImgHeight;
    /*
    vscode提示,
    typedef void (*Model::PostProcessCb)(nlohmann::json_abi_v3_12_0::json &result, 
    std::vector<cv::String> className, 
    cv::dnn::dnn4_v20231225::MatShape riskClsIdx)
    */
    PostProcessCb mPostProcessCb;
    std::vector<std::string> mClassNames;
    std::vector<int> mRiskClsIdx;
};