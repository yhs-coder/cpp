#include <cstdio>
#include <iostream>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;

// 配置文件解析
struct AppConfig {
    std::string server_url;
    int port;
    bool debug_mode;
    std::string log_level;
};

static void print_config(AppConfig&& config) {
    std::cout << "server_url:" << config.server_url << std::endl;
    std::cout << "port:" << config.port << std::endl;
    std::cout << "debug_mode:" << config.debug_mode << std::endl;
    std::cout << "log_level:" << config.log_level << std::endl;
    std::cout << "配置文件解析完成" << std::endl;
}

static AppConfig load_config(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        throw std::runtime_error("无法打开配置文件");
    }

    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document document;
    document.ParseStream(is);
    fclose(fp);

    if (document.HasParseError()) {
        throw std::runtime_error("配置文件解析错误");
    }

    AppConfig config;

    if (document.HasMember("server_url") && document["server_url"].IsString()) {
        config.server_url = document["server_url"].GetString();
    } else {
        config.server_url = "http://localhost";  // 默认值
    }

    if (document.HasMember("port") && document["port"].IsInt()) {
        config.port = document["port"].GetInt();
    } else {
        config.port = 8080;  // 默认值
    }

    if (document.HasMember("debug_mode") && document["debug_mode"].IsBool()) {
        config.debug_mode = document["debug_mode"].GetBool();
    } else {
        config.debug_mode = false;  // 默认值
    }

    if (document.HasMember("log_level") && document["log_level"].IsString()) {
        config.log_level = document["log_level"].GetString();
    } else {
        config.log_level = "info";  // 默认值
    }

    return config;
}
