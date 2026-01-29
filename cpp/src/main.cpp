#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include "Manager.h"
#include <chrono>
#include "Log.h"

int main(int argc, char** argv)
{
    // 打印当前工作路径
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    LOGI("Current working directory: %s", cwd);
    auto begin_time = std::chrono::steady_clock::now();
    if (argc != 2) {
        LOGE("usage: %s <file_path>", argv[0]);
        return 1;
    }
    std::string config_file = argv[1];
    Manager* manager = new Manager(config_file); // "../../res/config3.json"
    manager->startServer();
    delete manager;
    auto end_time = std::chrono::steady_clock::now();
    auto cost_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
    LOGI("program over! cost time %lld(ms)", (long long)cost_ms);
    return 0;
}