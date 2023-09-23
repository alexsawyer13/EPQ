#include <Core/Assets.h>

#include <spdlog/spdlog.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>

std::string s_ProjectDir = "../../";

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
	Type = ImageType::STB_IMAGE;
	Ownership = ImageOwnership::KEEP;
	
	Path = s_ProjectDir + "assets/" + path;
	stbi_set_flip_vertically_on_load(flipVertically);
	Data = stbi_load(Path.c_str(), &Width, &Height, &Channels, 0);
	if (!Data)
	{
		spdlog::error("Couldn't load image: {}", Path);
	}
}

Image::Image(unsigned char *data, int width, int height, int channels, ImageOwnership ownership)
{
	Width = width;
	Height = height;
	Channels = channels;
	Ownership = ownership;
	Type = ImageType::RAW_IMAGE;

	switch (Ownership)
	{
	case ImageOwnership::BORROW:
		Data = data;
		break;
	case ImageOwnership::COPY:
		Data = new unsigned char[width * height * channels];
		memcpy(Data, data, width * height * channels);
		break;
	case ImageOwnership::KEEP:
		Data = data;
		break;
	default:
		Data = nullptr;
		spdlog::error("Unkown image ownership in Image constructor, defaulting to nullptr");
		break;
	}
}

Image::~Image()
{
	if (Data)
	{
		switch (Ownership)
		{
		case ImageOwnership::COPY:
		case ImageOwnership::KEEP:
			switch (Type)
			{
			case ImageType::STB_IMAGE:
				stbi_image_free(Data);
				break;
			case ImageType::RAW_IMAGE:
				delete[] Data;
				break;
			default:
				spdlog::critical("Image is marked to be deleted, but ImageType {} doesn't have delete operation", Type);
				break;
			}
			break;
		case ImageOwnership::BORROW:
			break;
		default:
			spdlog::error("Image has unknown ownership {}", Ownership);
			break;
		}
	}
}