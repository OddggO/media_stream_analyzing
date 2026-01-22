#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
#include "opencv2/opencv.hpp"
#include "TrtModel.h"
#include "Assignment.h"
#include <vector>
#include "cpp-httplib/httplib.h"
#include <fstream>
#include <thread>
#include <chrono>
#include "Log.h"
#define MAX_READ_FAIL_NUM 5
#define POST_FAILED_SLEEP_TIME 2

void analyze_stream(int thread_pos, const nlohmann::json& config)
{
    // 打开视频流
    std::string source = config.at("source").get<std::string>();
    LOGI("thread %d begin, source %s analyzed start", thread_pos, source.data());
    cv::VideoCapture cap(source);
    if (!cap.isOpened()) {
        LOGE("open source[%s] failed", source.data());
        throw std::invalid_argument("open source " + source + " failed");
    }

    // 构建模型
    std::string engine_path = config.at("engine_path").get<std::string>();
    int inImgWidth = config["inImgWidth"];; // TODO 模型输入是224，在多线程版本使用错误的32可以跑，但是在这里cudaMemcpy会段错误
    int inImgHeight = config["inImgHeight"];;
    std::vector<std::string> class_names = config["class_names"];
    std::vector<int> risk_cls_idx = config["risk_cls_idx"];
    int max_result_size = config["max_result_size"];
    std::string assignment_name = config["assignment_name"];
    std::string destination_url = config["destination_url"];
    TrtModel model = TrtModel("AlexNet", engine_path, inImgWidth, inImgHeight, 
        AssignmentAnalyze::imageClassification, class_names, risk_cls_idx);
    model.modelInit();

    std::string dest_host = "0.0.0.0";
    int dest_port = 12021;
    std::string dest_path = "test";
    cv::Mat frame;
    int failedCnt = 0;
    int cnt = 0;
    bool isStop = false;
    while (!isStop) {
        ++cnt;
        // LOGI("read %d frame", cnt);
        if (!cap.read(frame) || frame.empty()) {
            ++failedCnt;
            if (failedCnt >= MAX_READ_FAIL_NUM) {
                LOGI("read %s end/EOF", source.data());
                break;
            }
            continue;
        }
        // 推理
        nlohmann::json result;
        // LOGI("read frame, size = %d", frame.rows * frame.cols);
        model.inference(frame, result);
        result["assgnment_name"] = assignment_name;
        // result["source_type"] = mSourceType;
        result["source"] = source;
        result["destination_url"] = destination_url; 

        httplib::Client client(dest_host, dest_port);
        httplib::Result res = client.Post(dest_path.data(), result.dump(), "application/json");
        if (!res) {
            LOGI("请求失败, 无法连接到服务器. url: %s", destination_url.data());
            sleep(POST_FAILED_SLEEP_TIME);
        }

        if (res->status == 200) {
            // LOGI("发送成功，发送内容: %s", result.dump().data());
            nlohmann::json respJson = nlohmann::json::parse(res->body);
            // LOGI("响应内容: %s", respJson.dump());
            // LOGI("响应内容: %s", res->body.data());
        } else {
            LOGI("请求失败, 错误码: %d", res->status);
        }
    }
    LOGI("thread %d end, source %s analyzed over, analyzed %d frame", thread_pos, source.data(), cnt);
}


int main(int argc, char** argv)
{
    auto begin_time = std::chrono::steady_clock::now();
    if (argc != 2) {
        LOGE("usage: %s <file_path>", argv[0]);
        return 1;
    }
    std::string config_file = argv[1];
    std::ifstream f(config_file);
    if(!f.is_open()) {
        LOGE("open file %s failed!", config_file.data());
        return 1;
    }
    nlohmann::json configs = nlohmann::json::parse(f);
    f.close();
    if (!configs.is_array()) {
        LOGI("open config file %s must be array", config_file.data());
        return 1;
    }
    int cinfig_len = configs.size();
    std::vector<std::thread> threads;
    for (int i = 0; i < configs.size(); ++i) {
        const auto& config = configs[i];
        std::thread t(analyze_stream, i, config);
        threads.push_back(std::move(t));
    }

    for (auto& t: threads)
    {
        t.join();
    }
    auto end_time = std::chrono::steady_clock::now();
    auto cost_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
    LOGI("program over! cost time %lld(ms)", (long long)cost_ms);
    return 0;
}
