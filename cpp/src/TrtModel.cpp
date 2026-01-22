#include "TrtModel.h"
#include "Log.h"
#include <fstream>

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
    std::ifstream file(mModelFilePath, std::ios::binary);
    std::string engineStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    mRuntime = nvinfer1::createInferRuntime(mLogger);
    mEngine = mRuntime->deserializeCudaEngine(engineStr.data(), engineStr.size());
    mContext = mEngine->createExecutionContext();
    
    int inputIndex = mEngine->getBindingIndex("input");
    int outputIndex = mEngine->getBindingIndex("output");
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
