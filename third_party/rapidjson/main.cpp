#include <iostream>

#include "parse_config_file.h"
#include "rapidjson/document.h"      // DOM API
#include "rapidjson/error/en.h"      // 解析错误码
#include "rapidjson/prettywriter.h"  // 美化输出
#include "rapidjson/stringbuffer.h"  // 输出缓冲
#include "rapidjson/writer.h"        // 生成JSON
using namespace rapidjson;

void test_rapidjson() {
    // 创建空的DOM文档
    Document document;

    // document.SetObject();  // JOSN值类型设置为对象
    // document.SetArray();   // JOSN值类型设置为数组

    // 键值对字符串
    const char* json = "{\"name\":\"zhazharu\",\"age\":30}";

    // 使用DOM解析JSON字符串
    // 相当于把json值挂载到DOM树上，由DOM来管理JSON
    document.Parse(json);

    // 检查解析错误
    if (document.HasParseError()) {
        std::cout << "解析错误： " << document.GetParseError() << document.GetErrorOffset() << std::endl;
    }

    // 从DOM生成JSON
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    // Accept 函数：用于将一个 JSON 文档（Document）序列化为一个 JSON 字符串
    document.Accept(writer);

    std::cout << "=================================" << std::endl;
#if 0
    // 使用Writer生成JSON
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    // 添加键值对
    writer.StartObject();
    writer.Key("name");
    writer.String("张三");
    writer.Key("age");
    writer.Int(30);
    writer.EndObject();
// #else
    // 使用PrettyWriter生成格式化JSON
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);

    writer.StartObject();
    writer.Key("name");
    writer.String("张三");
    writer.Key("age");
    writer.Int(30);
    writer.EndObject();

#endif
    const char* output = buffer.GetString();

    std::cout << output << std::endl;
}

// 访问数组元素
void test_array(const char* data) {
    Document document;
    const char* json =
        "{\"name\":\"张三\",\"array\":[11,22,33,44,55], \"active\":true, \"count\":1024, \"pi\": 3.1415}";
    document.Parse(json);
    if (document.HasParseError()) {
        std::cout << "解析错误： " << document.GetParseError() << document.GetErrorOffset() << std::endl;
    }
    // 使用[]获取值
    const Value& array = document["array"];
    for (SizeType i = 0; i < array.Size(); i++) {
        printf("array[%d] = %d\n", i, array[i].GetInt());
    }

    // 使用迭代器
    for (Value::ConstValueIterator iter = array.Begin(); iter != array.End(); ++iter) {
        printf("%d\n", iter->GetInt());
    }
}

// DOM操作
void test_dom() {
    Document document;
    const char* json =
        "{\"name\":\"张三\",\"age\": 100, \"array\":[11,22,33,44,55], \"active\":true, \"count\":1024, \"pi\": 3.1415}";
    document.Parse(json);
    if (document.HasParseError()) {
        ParseErrorCode code = document.GetParseError();
        size_t offset = document.GetErrorOffset();
        printf("解析错误: %s (%zu)\n", GetParseError_En(code), offset);
    }

    std::cout << "==============访问对象成员==================" << std::endl;
    // 1. 访问对象成员
    // 检查成员是否存在
    if (document.HasMember("name")) {
        // 使用[]访问成员
        const Value& name = document["name"];  // 张三
        std::cout << "name: " << name.GetString() << std::endl;

        int age = document["age"].GetInt();
        std::cout << "age: " << age << std::endl;
    }

    // 使用FindMember查找
    Value::ConstMemberIterator iter = document.FindMember("name");
    if (iter != document.MemberEnd()) {
        std::cout << "名字: " << iter->value.GetString() << std::endl;
    }
    std::cout << "==============访问数组元素==================" << std::endl;
    // 2. 访问数组元素
    test_array(json);

    std::cout << "==============类型检查与转换==================" << std::endl;
    // 3.1类型检查
    if (document["name"].IsString()) {
        const char* name = document["name"].GetString();
    }
    // 3.2 获取不同类型的值
    bool b = document["active"].GetBool();
    int i = document["count"].GetInt();
    double d = document["pi"].GetDouble();
    const char* s = document["name"].GetString();
    auto arr = document["array"].GetArray();
    std::cout << "b : " << b << std::endl;
    std::cout << "i : " << i << std::endl;
    std::cout << "d : " << d << std::endl;
    std::cout << "s : " << s << std::endl;
    std::cout << "arr : " << arr[0].GetInt() << std::endl;

    std::cout << "==============修改dom==================" << std::endl;

    // 4. 修改dom
    // 修改值
    document["age"].SetInt(99);
    // 添加成员
    // Value& new_object = document.GetObject();
    document.AddMember("gender", "male", document.GetAllocator());
    // 添加数组元素
    Value& array = document["array"].GetArray();
    array.PushBack(Value().SetInt(100), document.GetAllocator());

    std::cout << "age: " << document["age"].GetInt() << std::endl;
    std::cout << "gender: " << document["gender"].GetString() << std::endl;
    std::cout << "array : " << arr[5].GetInt() << std::endl;

    std::cout << "==============使用Value创建值==================" << std::endl;

    // 5. 创建值
    Value author;
    author.SetString("李四", document.GetAllocator());
    document.AddMember("author", author, document.GetAllocator());
    // 使用 Value 构造函数
    document.AddMember("love", Value("cpp", document.GetAllocator()), document.GetAllocator());

    std::cout << "author : " << document["author"].GetString() << std::endl;
    std::cout << "love : " << document["love"].GetString() << std::endl;

    std::cout << "==============深拷贝与移动语义==================" << std::endl;
    // 深拷贝
    // Value a(42);
    // Value b(a, document.GetAllocator());  // 深拷贝 a 到 b
    // 移动语义
    document.AddMember("key", Value(42), document.GetAllocator());             // 临时值直接移动
    document.AddMember("key", std::move(Value(42)), document.GetAllocator());  // 使用 std::move
}

int main() {
    // test_rapidjson();
    // test_dom();

    print_config(load_config("/home/ubuntu/github_code/test/test_rapidjson/config.json"));
    return 0;
}
