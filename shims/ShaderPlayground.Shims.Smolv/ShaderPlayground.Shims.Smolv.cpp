#include <iostream>
#include <fstream>
#include <string>

#include "Source/source/smolv.h"

static std::vector<char> ReadBinaryFile(const char* fileName)
{
	std::ifstream ifs(fileName, std::ios::binary | std::ios::ate);
	std::ifstream::pos_type pos = ifs.tellg();

	std::vector<char> result(pos);

	ifs.seekg(0, std::ios::beg);
	ifs.read(&result[0], pos);

	return result;
}

enum InputLanguage
{
	Spirv,
	Smolv
};

int main(int argc, const char* argv[])
{
	auto shaderBytes = ReadBinaryFile(argv[1]);

	auto inputLanguage = (InputLanguage)std::stoi(argv[2]);
	auto encodeFlags = (smolv::EncodeFlags)std::stoi(argv[3]);

	bool result;
	smolv::ByteArray outputBytes;

	auto stats = smolv::StatsCreate();

	switch (inputLanguage)
	{
	case Spirv:
		result = smolv::Encode(shaderBytes.data(), shaderBytes.size(), outputBytes, encodeFlags);
		if (result)
		{
			smolv::StatsCalculate(stats, shaderBytes.data(), shaderBytes.size());
		}
		break;

	case Smolv:
		outputBytes.resize(smolv::GetDecodedBufferSize(shaderBytes.data(), shaderBytes.size()));
		result = smolv::Decode(shaderBytes.data(), shaderBytes.size(), outputBytes.data(), outputBytes.size());
		if (result)
		{
			smolv::StatsCalculateSmol(stats, shaderBytes.data(), shaderBytes.size());
		}
		break;

	default:
		return 1;
	}

	smolv::StatsPrint(stats);

	smolv::StatsDelete(stats);

	std::ofstream out(argv[4], std::ios::binary | std::ios::out);
	out.write((char*) outputBytes.data(), outputBytes.size());
	out.close();

	return 0;
}

