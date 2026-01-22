#pragma once
#include "Model.h"
#include "MediaWorker.h"
#include "NvInfer.h"
#include "cuda_runtime_api.h"
#include <vector>

class TrtModel: public Model
{
private:
    class Logger: public nvinfer1::ILogger {
    public:
        static Logger& instance() {
            static Logger logger;
            return logger;
        }
    private:
        Logger() = default;
        void log(Severity severity, const char* msg) noexcept override{
            if (severity <= Severity::kINFO) {
                std::cout << msg << std::endl;
            }
        }
    };
public:
    TrtModel(const std::string& modelName, const std::string& modelFilePath, int inImgWidth, int inImgHeight, 
        PostProcessCb cb, const std::vector<std::string>& classNames, const std::vector<int>& riskClsIdx);
    ~TrtModel();
    bool modelInit() override;
    void inference(const cv::Mat& img, nlohmann::json& result) override;
private:

    size_t getLenByDim(const nvinfer1::Dims& dims);
    static void* safeCudaMalloc(size_t size);
private:
    Logger& mLogger;
    nvinfer1::IRuntime* mRuntime;
    nvinfer1::ICudaEngine* mEngine;
    nvinfer1::IExecutionContext* mContext;
    size_t mInputLen;
    size_t mOutputLen; 
    const size_t mElementSize = sizeof(float);
    size_t mInputSize; 
    size_t mOutputSize; 
    void* mDeviceInput; // cuda上的输入
    void* mDeviceOutput; // cuda上的输出
    void* mBindings[2]; 
    std::vector<float> mHostOutput;
};
