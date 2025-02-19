// EasyTshark.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

struct Packet {
	int frame_number;		// 数据包编号
	std::string time;		// 数据包的时间戳
	std::string src_ip;		// 源IP地址
	size_t src_port;		// 源端口
	std::string dst_ip;		// 目的IP地址
	size_t dst_port;		// 目的端口
	std::string protocol;	// 协议
	std::string info;		// 数据包的概要信息

};

void ParseLine(std::string line, Packet& packet) {
	if (line.back() == '\n') {
		line.pop_back();
	}

	std::stringstream ss(line);
	std::string field;
	std::vector<std::string> fields;

	while (std::getline(ss, field, '\t')) {		// 字段用tab分隔
		fields.push_back(field);
	}

	// 如果是TCP协议，字段数量为10，因为最后一个提取的字段为空，同时填充了\t，提取不到。
	if (fields.size() >= 10) {
		packet.frame_number = std::stoi(fields[0]);
		packet.time = fields[1];
		packet.src_ip = fields[2];
		packet.dst_ip = fields[3];
		packet.protocol = fields[4];
		packet.info = fields[5];
		if (fields[6] != "" && std::stoi(fields[6]) == 6) {	// 标识协议类型 6:TCP
			packet.src_port = std::stoi(fields[7]);
			packet.dst_port = std::stoi(fields[9]);
		}
		if (fields[6] != "" && std::stoi(fields[6]) == 17) {	// 标识协议类型 17:UDP
			packet.src_port = std::stoi(fields[8]);
			packet.dst_port = std::stoi(fields[10]);
		}
	}
}

void PrintPacket(const Packet& packet) {
	// 构建JSON对象
	rapidjson::Document pkt_obj;
	rapidjson::Document::AllocatorType& allocator = pkt_obj.GetAllocator();

	// 设置JSON为Object对象类型
	pkt_obj.SetObject();

	// 添加JSON字段
	pkt_obj.AddMember("frame_number", packet.frame_number, allocator);
	pkt_obj.AddMember("timestamp", rapidjson::Value(packet.time.c_str(), allocator), allocator);
	pkt_obj.AddMember("src_ip", rapidjson::Value(packet.src_ip.c_str(), allocator), allocator);
	pkt_obj.AddMember("src_port", packet.src_port, allocator);
	pkt_obj.AddMember("dst_ip", rapidjson::Value(packet.dst_ip.c_str(), allocator), allocator);
	pkt_obj.AddMember("dst_port", packet.dst_port, allocator);
	pkt_obj.AddMember("protocol", rapidjson::Value(packet.protocol.c_str(), allocator), allocator);
	pkt_obj.AddMember("info", rapidjson::Value(packet.info.c_str(), allocator), allocator);


	// 序列化为JSON字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	pkt_obj.Accept(writer);

	// 打印JSON输出
	std::cout << buffer.GetString() << std::endl;
}


int main()
{
	// 设置控制台输出编码为 UTF-8
	SetConsoleOutputCP(CP_UTF8);

	std::string exe_path = R"(D:\setup\Wireshark\tshark)";
	std::string pcap_path = R"(D:\code\C++\EasyTshark\packets.pcap)";
	//std::string options = " -T fields -e frame.number -e frame.time -e ip.src -e ip.dst -e _ws.col.Protocol -e _ws.col.info";
	std::string options = " -T fields  -e frame.number -e frame.time -e ip.src  -e ip.dst  -e _ws.col.Protocol \
							-e _ws.col.info -e ip.proto -e tcp.srcport -e udp.srcport -e tcp.dstport  -e udp.dstport";
	std::string cmd = exe_path + " -r " + pcap_path + options;

	FILE* pipe = _popen(cmd.c_str(), "r");
	if (!pipe) {
		std::cout << "Failed to run tshark command";
		return 1;
	}

	std::vector<Packet> packets;
	char buffer[1024]{};
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		//std::cout << buffer;
		Packet packet;
		ParseLine(buffer, packet);
		packets.push_back(packet);
	}

	/*for (auto& p : packets) {
		printf("frame_number: %d time: %s src_ip: %s src_port: %d dst_ip: %s dst_port: %d protocol: %s info: %s\n",
			p.frame_number,
			p.time.c_str(),
			p.src_ip.c_str(),
			p.src_port,
			p.dst_ip.c_str(),
			p.dst_port,
			p.protocol.c_str(),
			p.info.c_str());
	}*/
	for (auto& p : packets) {
		PrintPacket(p);
	}

	_pclose(pipe);
	return 0;
}


