#include <iostream>
#include <map>
#include <vector>

#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// json序列化示例1 - 基本使用
std::string func1() {
    json js;
    js["msg_type"] = 1;
    js["from"] = "tom";
    js["to"] = "jerry";
    js["msg"] = "good morning, hava a good day!";
    cout << js << endl;

    // dump 方法用于将 JSON 对象序列化为字符串形式
    std::string send_buf = js.dump();
    cout << js << endl;
    return send_buf;
}

// json序列化示例2 - 普通数据序列化
std::string func2() {
    json js;
    // 添加数组
    js["id"] = {1, 2, 3, 4, 5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象,嵌套键值对
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    // cout << js << endl;
    return js.dump();
}

// json序列化示例2 - 容器序列化
std::string func3() {
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;
    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;

    std::string send_buf = js.dump();
    cout << js << endl;
    return send_buf;
}

int main() {
    // std::string recv_buf = func1();
    // // 数据反序列化， 将json格式的字符串 解析为 json数据对象
    // json jsbuf = json::parse(recv_buf);
    // cout << jsbuf["msg_type"] << endl;
    // cout << jsbuf["from"] << endl;
    // cout << jsbuf["to"] << endl;
    // cout << jsbuf["msg"] << endl;

    // std::string recv_buf = func2();
    // json jsbuf = json::parse(recv_buf);
    // cout << jsbuf["id"] << endl;
    // auto arr = jsbuf["id"];
    // cout << arr[2] << endl;
    // auto msg = jsbuf["msg"];
    // cout << msg["zhang san"] << endl;

    std::string recv_buf = func3();
    json jsbuf = json::parse(recv_buf);

    // json对象里面的数组类型，直接放入vector容器中
    vector<int> vec = jsbuf["list"];
    for (auto& v : vec) {
        cout << v << " ";
    }
    cout << endl;

    map<int, string> mymap = jsbuf["path"];
    for (auto& p : mymap) {
        cout << p.first << " " << p.second << endl;
    }
    cout << endl;
    return 0;
}