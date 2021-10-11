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

Stb_Image::Stb_Image(const std::string &path, bool flipVertically)
	: Path(s_ProjectDir + "assets/" + path)
{
	stbi_set_flip_vertically_on_load(flipVertically);
	Data = stbi_load(Path.c_str(), &Width, &Height, &Channels, 0);
	if (!Data)
	{
		spdlog::error("Couldn't load image: {}", Path);
	}
}

Stb_Image::~Stb_Image()
{
	if (Data)
		stbi_image_free(Data);
}