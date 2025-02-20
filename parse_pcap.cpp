#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#ifdef _WIN32
#include <windows.h>
// UTF-8转ANSI
static std::string UTF8TOANSIString(const std::string& utf8Str) {
	// 获取UTF-8字符串的长度
	int utf8Length = static_cast<int>(utf8Str.length());

	// 将UTF-8转换为宽字符（UTF-16）
	int wideLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Length, nullptr, 0);
	std::wstring wideStr(wideLength, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Length, &wideStr[0], wideLength);

	// 将宽字符（UTF-16）转换为ANSI
	int ansiLength = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideLength, nullptr, 0, nullptr, nullptr);
	std::string ansiStr(ansiLength, '\0');
	WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideLength, &ansiStr[0], ansiLength, nullptr, nullptr);
	return ansiStr;
}

#endif



// PCAP全局文件头
struct PcapHeader {
	uint32_t magic_number;      // 文件格式 (0xa1b2c3d4为大端，0xd4c3b2a1为小端)
	uint16_t version_major;     // pcap文件版本号
	uint16_t version_minor;     // 次版本号
	uint32_t thiszone;			// 时间偏移
	uint32_t sigfigs;			// 时间戳精度
	uint32_t snaplen;			// 捕获数据包长度
	uint32_t network;			// 链路层类型
};


// 每个数据报文头
struct PacketHeader {
	uint32_t ts_sec;		// 时间戳（秒）
	uint32_t ts_usec;		// 时间戳（微秒）
	uint32_t caplen;		// 实际捕获数据包长度
	uint32_t len;			// 数据包原始长度
};



struct Packet {
	int frame_number;		// 数据包编号
	std::string time;		// 数据包的时间戳
	uint32_t cap_len;		// 捕获数据包的长度
	std::string src_ip;		// 源IP地址
	size_t src_port;		// 源端口
	std::string dst_ip;		// 目的IP地址
	size_t dst_port;		// 目的端口
	std::string protocol;	// 协议
	std::string info;		// 数据包的概要信息
	uint32_t file_offset;	// 在PACP文件中的偏移量

};

bool ParseLine(std::string line, Packet& packet) {
	line = UTF8TOANSIString(line);
	
	if (line.back() == '\n') {
		line.pop_back();
	}

	std::stringstream ss(line);
	std::string field;
	std::vector<std::string> fields;
	//while (std::getline(ss, field, '\t')) {  // 假设字段用 tab 分隔
	//	fields.push_back(field);
	//}
	size_t start = 0, end;
	while ((end = line.find('\t', start)) != std::string::npos) {
		fields.push_back(line.substr(start, end - start));
		start = end + 1;
	}
	fields.push_back(line.substr(start));	// 添加最后一个子串

	// 字段顺序： -e frame.number -e frame.time -e frame.cap_len -e ip.src  -e ipv6.src -e ip.dst -e ipv6.dst \
				-e tcp.srcport -e udp.srcport -e tcp.dstport  -e udp.dstport -e _ws.col.Protocol -e _ws.col.info
	// 0: frame.number
	// 1: frame.time
	// 2: frame.cap_len
	// 3: ip.src
	// 4: ipv6.src
	// 5: ip.dst
	// 6: ipv6.dst
	// 7: tcp.srcport
	// 8: udp.srcport
	// 9: tcp.dstport
	// 10: udp.dstport
	// 11: _ws.col.Protocol
	// 12: _ws.col.Info

	if (fields.size() >= 13) {
		packet.frame_number = std::stoi(fields[0]);
		packet.time = fields[1];
		packet.cap_len = std::stoi(fields[2]);
		packet.src_ip = fields[3].empty() ? fields[4] : fields[3];
		packet.dst_ip = fields[5].empty() ? fields[6] : fields[5];
		if (!fields[7].empty() || !fields[8].empty()) {
			packet.src_port = std::stoi(fields[7].empty() ? fields[8] : fields[7]);
		}
		if (!fields[9].empty() || !fields[10].empty()) {
			packet.dst_port = std::stoi(fields[9].empty() ? fields[10] : fields[9]);
		}
		packet.protocol = fields[11];
		packet.info = fields[12];
		return true;
	}
	else {
		printf("ParseLine error!\n");
		return false;
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
	pkt_obj.AddMember("file_offset", packet.file_offset, allocator);
	pkt_obj.AddMember("cap_len", packet.cap_len, allocator);


	// 序列化为JSON字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	pkt_obj.Accept(writer);

	// 打印JSON输出
	std::cout << buffer.GetString() << std::endl;
}


void ParsePacpFile() {
	std::string packet_file = R"(D:\code\C++\EasyTshark\packets.pcap)";
	std::ifstream file(packet_file.c_str(), std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file." << std::endl;
		return ;
	}

	// 读取pcap文件头
	PcapHeader pcap_header;
	file.read(reinterpret_cast<char*>(&pcap_header), sizeof(PcapHeader));

	// 循环读取每一个数据报文
	while (file) {
		// 先读取这一个报文的头
		PacketHeader packet_header;
		file.read(reinterpret_cast<char*>(&packet_header), sizeof(PacketHeader));
		if (!file) break;
		// 等价于
		// if (file.fail() || file.bad()) break;

		// 读取这一个报文的内容
		std::vector<unsigned char> data(packet_header.caplen);
		file.read(reinterpret_cast<char*>(data.data()),packet_header.caplen);

		printf("数据包[时间: %d  长度: %d]: ", packet_header.ts_sec, packet_header.caplen);
		for (auto byte : data) {
			printf("%02X ", byte);
		}
		std::cout << "\n";
	}
	file.close();

}

bool ReadPacketHex(const std::string& file_path, uint32_t offset, uint32_t length, std::vector<unsigned char>& buffer) {
	std::ifstream file(file_path, std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file." << std::endl;
		return false;
	}

	// 移动到报文数据部分位置
	file.seekg(offset, std::fstream::beg);
	// 确保以下进行读取时缓冲区大小足够
	buffer.resize(length);
	file.read(reinterpret_cast<char*>(buffer.data()), length);

	// 将标记移动到文件开始
	//file.seekg(0, std::fstream::beg);

	file.close();
	return true;
}

int main()
{
	// 设置控制台输出编码为 UTF-8
	//SetConsoleOutputCP(CP_UTF8);
	//std::cout << "你好，数据包[时间: %d  长度: %d]" << std::endl;
	//ParsePacpFile();
#if 1
	std::string exe_path = R"(D:\setup\Wireshark\tshark)";
	std::string packet_file = R"(D:\code\C++\EasyTshark\packets.pcap)";
	//std::string options = " -T fields -e frame.number -e frame.time -e ip.src -e ip.dst -e _ws.col.Protocol -e _ws.col.info";
	std::string options = " -T fields  -e frame.number -e frame.time -e frame.cap_len -e ip.src  -e ipv6.src -e ip.dst -e ipv6.dst \
							-e tcp.srcport -e udp.srcport -e tcp.dstport  -e udp.dstport -e _ws.col.Protocol -e _ws.col.info";
	std::string cmd = exe_path + " -r " + packet_file + options;

	FILE* pipe = _popen(cmd.c_str(), "r");
	if (!pipe) {
		std::cout << "Failed to run tshark command";
		return 1;
	}

	std::vector<Packet> packets;
	char buffer[4096]{};

	// 获取pcap全局文件头的偏移量
	uint32_t file_offset = sizeof(PcapHeader);
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		//std::cout << buffer;
		Packet packet;
		if (ParseLine(buffer, packet)) {
			// 处理当前报文数据在pcap文件中的偏移量: 偏移游标 + 当前数据包头 ,并记录在packet对象中
			packet.file_offset = file_offset + sizeof(PacketHeader);

			// 更新偏移游标, 指向下一个数据包
			file_offset = file_offset + sizeof(PacketHeader) + packet.cap_len;

			packets.push_back(packet);
		}
		else {
			// 断言失败，程序就会崩溃,方便失败排查问题
			assert(false); 
		}
		
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
		
		// 读取报文的原始十六进制数据
		std::vector<unsigned char> data;
		ReadPacketHex(packet_file, p.file_offset, p.cap_len, data);

		// 打印读取到的数据
		printf("Packet Hex: ");
		for (auto byte : data) {
			printf("%02X ", byte);
		}
		printf("\n\n");

	}

	_pclose(pipe);
#endif
	return 0;
}

