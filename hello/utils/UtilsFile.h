#pragma once

#include "UtilsStream.h"
#include "UtilsCommon.h"

inline I8Stream::Ptr ReadBinFile(const std::string& name)
{
	FILE* fp = fopen(name.c_str(), "rb");
	if (fp == nullptr) {
		return nullptr;
	}

	// 移动到文件末尾以获取文件大小  
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp); // 重置文件读取位置到开头  

	I8Stream::Ptr stream = std::make_shared<I8Stream>(size);

	size_t n = fread(stream->Data(), 1, size, fp);
	if (n != size) {
		return nullptr;
	}

	stream->SetSize(n);

	fclose(fp);

	return stream;
}

/**
 * @brief 追加写文件
 * @param name 
 * @param stream 
 * @return >0 写入文件的大小 <0 错误
 */
inline size_t WriteBinFile(const std::string& name, I8Stream::Ptr stream)
{
	if (!stream) {
		return -1;
	}

	FILE* fp = fopen(name.c_str(), "ab+");
	if (fp == nullptr) {
		return -1;
	}

	One one(nullptr, [fp]() { fclose(fp); });

	size_t n = fwrite(stream->Data(), 1, stream->Size(), fp);
	if (n != stream->Size()) {
		return -1;
	}

	return n;
}

inline size_t WriteBinFile(const std::string& name, const void* data, size_t size)
{
	FILE* fp = fopen(name.c_str(), "ab+");
	if (fp == nullptr) {
		return -1;
	}

	One one(nullptr, [fp]() { fclose(fp); });

	size_t n = fwrite(data, 1, size, fp);
	if (n != size) {
		return -1;
	}

	return n;
}

