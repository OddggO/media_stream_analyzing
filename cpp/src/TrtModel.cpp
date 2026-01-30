#include "TrtModel.h"
#include "Log.h"
#include <fstream>
#include <opencv2/core/utils/logger.hpp>

TrtModel::TrtModel(const std::string& modelName, const std::string& modelFilePath, 
    int inImgWidth, int inImgHeight, PostProcessCb cb, 
    const std::vector<std::string>& classNames, const std::vector<int>& riskClsIdx): 
    Model(modelName, modelFilePath, inImgWidth, inImgHeight, cb, classNames, riskClsIdx), mLogger(Logger::instance())
{
    LOGI("TrtModel constructed");
}

TrtModel::~TrtModel()
{
    LOGI("TrtModel destroy");
}

bool TrtModel::modelInit()
{
    LOGI("TrtModel modelInit, modelName=%s, modelFilePath=%s", 
        mModelName.data(), mModelFilePath.data());
    std::ifstream file(mModelFilePath, std::ios::binary);
    std::string engineStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    mRuntime = nvinfer1::createInferRuntime(mLogger);
    mEngine = mRuntime->deserializeCudaEngine(engineStr.data(), engineStr.size());
    mContext = mEngine->createExecutionContext();

    int nBindings = mEngine->getNbBindings();
    if (nBindings != 2) {
        LOGE("Only support model with 1 input and 1 output");
        return false;
    }

    // for (int i = 0; i < nBindings; ++i) {
    //     LOGI("Binding %d: name=%s, isInput=%s", i, mEngine->getBindingName(i), mEngine->bindingIsInput(i) ? "true" : "false");
    // }
    std::string inputName = mEngine->getBindingName(0);
    std::string outputName = mEngine->getBindingName(1);
    LOGI("Input name: %s, Output name: %s", inputName.data(), outputName.data());

    int inputIndex = mEngine->getBindingIndex(inputName.data());
    int outputIndex = mEngine->getBindingIndex(outputName.data());
    nvinfer1::Dims inputDims = mEngine->getBindingDimensions(inputIndex);
    nvinfer1::Dims outputDims = mEngine->getBindingDimensions(outputIndex);

    mInputLen = getLenByDim(inputDims);
    mOutputLen = getLenByDim(outputDims);
    mInputSize = mInputLen * mElementSize;
    mOutputSize = mOutputLen * mElementSize;

    mDeviceInput = safeCudaMalloc(mInputSize);
    mDeviceOutput = safeCudaMalloc(mOutputSize);

    mBindings[0] = mDeviceInput;
    mBindings[1] = mDeviceOutput;

    mHostOutput.reserve(mOutputLen);
    LOGI("Model initialized successfully, inputLen=%zu, outputLen=%zu", mInputLen, mOutputLen);

    {
        // LOGI("\n%s", cv::getBuildInformation().data());
        // 测试能不能跑通
        // 读取test.jpg
        LOGI("realpath = %s", realpath("test.jpg", nullptr));
        cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);
        cv::Mat testImg = cv::imread("test.jpg", cv::IMREAD_COLOR);
        if (testImg.empty()) {
            LOGI("read test.jpg failed, skip test inference");
        } else {
            nlohmann::json result;
            inference(testImg, result);
            LOGI("test inference result: %s", result.dump().data());
        }
    }
    return true;
}

size_t TrtModel::getLenByDim(const nvinfer1::Dims& dims)
{
    size_t ret = 1;
    for (int i = 0; i < dims.nbDims; ++i) {
        ret *= dims.d[i];
    }
    return ret;
}

void* TrtModel::safeCudaMalloc(size_t size)
{
    void* deviceMem;
    cudaMalloc(&deviceMem, size);
    return deviceMem;
}

void TrtModel::inference(const cv::Mat& img, nlohmann::json& result)
{
    // 前处理
    cv::Mat pImg = preprocess(img);

    // copy host to gpu
    cudaMemcpy(mDeviceInput, pImg.ptr<float>(), mInputSize, cudaMemcpyHostToDevice);

    // inference
    mContext->executeV2(mBindings);

    // copy gpu to host
    cudaMemcpy(mHostOutput.data(), mDeviceOutput, mOutputSize, cudaMemcpyDeviceToHost);

    // 后处理
    mPostProcessCb(mHostOutput, result, mClassNames, mRiskClsIdx);
    // mPostProcessCb(result, mClassNames, mR);
}
