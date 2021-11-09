#include <Core/Assets.h>

#include <spdlog/spdlog.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>

std::string s_ProjectDir = "../Game/";

std::string ReadFile(const std::string &p)
{
	std::string path = s_ProjectDir + p;

	std::ifstream is;
	is.open(path);

	if (!is.is_open())
	{
		spdlog::error("Failed to open file: {}", path);
	}

	std::stringstream ss;
	ss << is.rdbuf();

	std::string str = ss.str();

	is.close();

	return str;
}

std::string ReadAsset(const std::string &p)
{
	return ReadFile("assets/" + p);
}

Image::Image(const std::string &path, bool flipVertically)
{
	Path = s_ProjectDir + "assets/" + path;
	Type = ImageType::STB_IMAGE;
	stbi_set_flip_vertically_on_load(flipVertically);
	Data = stbi_load(Path.c_str(), &Width, &Height, &Channels, 0);
	if (!Data)
	{
		spdlog::error("Couldn't load image: {}", Path);
	}
}

Image::Image(unsigned char *data, int width, int height, int channels, bool owns_data)
{
	Width = width;
	Height = height;
	Channels = channels;

	// Copy data if not owned
	if (owns_data)
	{
		Data = data;
	}
	else
	{
		Data = new unsigned char[width * height * channels];
		memcpy(Data, data, width * height * channels);
	}
}

Image::~Image()
{
	if (Data)
	{
		switch (Type)
		{
		case ImageType::STB_IMAGE:
			stbi_image_free(Data);
			break;
		case ImageType::RAW_IMAGE:
			delete[] Data;
		}
	}
}