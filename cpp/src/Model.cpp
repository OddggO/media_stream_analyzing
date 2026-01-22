#include "Model.h"

Model::Model(const std::string& modelName, const std::string& modelFilePath, int inImgWidth, int inImgHeight, 
    PostProcessCb cb, const std::vector<std::string>& classNames, const std::vector<int>& riskClsIdx): 
        mModelName(modelName), mModelFilePath(modelFilePath), mInImgWidth(inImgWidth), mInImgHeight(inImgHeight), 
        mPostProcessCb(cb), mClassNames(classNames), mRiskClsIdx(riskClsIdx)
{
    LOGI("model constructed, model name = %s, model path = %s", mModelName.data(), mModelFilePath.data());
}

Model::~Model()
{
    LOGI("model destroy, model name = %s, model path = %s", mModelName.data(), mModelFilePath.data());
}

std::string Model::modelName() const
{
    return mModelName;
}

std::string Model::modelFilePath() const
{
    return mModelFilePath;
}

