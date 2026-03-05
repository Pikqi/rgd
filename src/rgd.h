#include <glad/glad.h>
#include "logger.h"
#define ASSERT(x)                                                              \
    if (!(x))                                                                  \
        asm("int3");

#define GLCALL(x)                                                              \
    GLClearError();                                                            \
    x;                                                                         \
    ASSERT(!GLLogError(#x, __FILE__, __LINE__))

static inline void GLClearError()
{
    while (!glGetError)
        ;
}
static inline bool GLLogError(const char* line, const char* filename,
                              int line_num)
{
    bool has_error = false;
    while (GLenum error = glGetError())
    {
        LOG_FATAL("OPENGL ERRRO: {}", error);
        LOG_FATAL("{}", line);
        LOG_FATAL("{}:{}", filename, line_num);
        has_error = true;
    }
    Logger::flush();
    return has_error;
}
