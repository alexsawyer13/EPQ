#pragma once

#include <string>

std::string ReadFile(const std::string &path);
std::string ReadAsset(const std::string &path);

struct Stb_Image
{
	unsigned char *Data;
	int Width, Height, Channels;
	std::string Path;

	Stb_Image(const std::string &path, bool flipVertically);
	~Stb_Image();
};