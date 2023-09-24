#include <Graphics/Info.h>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

GraphicsInfo ginfo;

int GraphicsInfoLoad()
{
    GLint texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);

    if (texture_units < 16)
    {
        spdlog::critical("OpenGL implementation supports {} texture units. This is less than the 16 expected.", texture_units);
        return false;
    }

    ginfo.MAX_TEXTURE_UNITS = (GLint)texture_units;

    return true;
}