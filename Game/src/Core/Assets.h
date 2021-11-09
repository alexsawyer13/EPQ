#pragma once

#include <string>

std::string ReadFile(const std::string &path);
std::string ReadAsset(const std::string &path);

enum class ImageType
{
	STB_IMAGE,
	RAW_IMAGE
};

struct Image
{
	unsigned char *Data;
	int Width, Height, Channels;
	std::string Path;
	ImageType Type;

	Image(const std::string &path, bool flipVertically = true);
	Image(unsigned char *data, int width, int height, int channels, bool owns_data = true);
	~Image();
};

extern std::string s_ProjectDir;