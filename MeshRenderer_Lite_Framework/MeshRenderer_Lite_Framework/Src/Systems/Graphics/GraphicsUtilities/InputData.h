#pragma once

#include <string>
#include <vector>

enum class DataFormat : char;

struct InputData
{
public:
	InputData(const std::string& semanticName, const DataFormat& format, const bool instanceData = false);

	std::string semanticName;
	DataFormat format;
	bool instanceData;
};

typedef std::vector<InputData> InputLayout;