#include <cstdlib>

struct GraphicsInfo
{
    size_t MAX_TEXTURE_UNITS = 16; // Guaranteed to be at least 16 by OpenGL
};

extern GraphicsInfo ginfo;

int GraphicsInfoLoad();