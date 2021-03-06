#include "opengl.h"
//#include "util.h"

#define MAX_SHADER_SOURCE_LENGTH 5000
#define MAX_GL_INFO_LOG_LENGTH 5000

void
CopyFramebufferToScreen(Framebuffer framebuffer)
{
    gl->BindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.id);
    gl->FramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, framebuffer.texture.id, 0);

    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    gl->BlitFramebuffer(0, 0, framebuffer.width, framebuffer.height,
                        0, 0, framebuffer.width, framebuffer.height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void
CopyTexture(Texture dst, Texture src)
{

}

Texture
CreateEmptyTexture(int width, int height)
{
    Texture texture = {};
    texture.width = width;
    texture.height = height;

    gl->GenTextures(1, &texture.id);
    gl->ActiveTexture(GL_TEXTURE0);
    gl->BindTexture(GL_TEXTURE_2D, texture.id);

    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                   width, height,
                   0, GL_RGBA, GL_FLOAT, nullptr);

    return texture;
}

void
FreeTexture(Texture *texture)
{
    gl->DeleteTextures(1, &texture->id);
    *texture = {};
}

Framebuffer
CreateFramebuffer(int width, int height)
{
    // game->prepass_framebuffer = {};
    Framebuffer framebuffer = {};
    framebuffer.width = width;
    framebuffer.height = height;

    gl->GenFramebuffers(1, &framebuffer.id);
    gl->BindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

    framebuffer.texture = CreateEmptyTexture(width, height);

    // Generate and attach depth RBO
    gl->GenRenderbuffers(1, &framebuffer.depth_stencil_rbo);
    gl->BindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth_stencil_rbo);
    gl->RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    gl->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depth_stencil_rbo);

    // Generate and attach stencil RBO
    // gl->GenRenderbuffers(1, &framebuffer.stencil_rbo);
    // gl->BindRenderbuffer(GL_RENDERBUFFER, framebuffer.stencil_rbo);
    // gl->RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT8 )

    GLenum color_attachments[] = {GL_COLOR_ATTACHMENT0};
    gl->DrawBuffers(sizeof(color_attachments)/sizeof(GLenum), color_attachments);

    if(c::verbose_success_logging)
    {
        GLenum fb_status = gl->CheckFramebufferStatus(GL_FRAMEBUFFER);
        Log("Framebuffer Status: %u", fb_status);
    }

    return framebuffer;
}

void
ApplyComputeShaderToTexture_VERYSLOW(GLuint shader, Texture *texture)
{
    Texture *src = texture;
    Texture *dst = &CreateEmptyTexture(src->width, src->height);
    Assert(src->width == dst->width);
    Assert(src->height = dst->height);

    gl->UseProgram(shader);
    gl->BindTextureUnit(0, src->id);
    gl->BindImageTexture(1, dst->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    gl->DispatchCompute(dst->width, dst->height, 1);

    // Swap [texture] and [temp_screen_texture] GL ids
    u32 temp_id = src->id;
    src->id = dst->id;
    dst->id = temp_id;

    FreeTexture(dst);
}

void
ApplyComputeShaderToScreenTexture(GLuint shader, Texture *texture)
{
    Texture *src = texture;
    Texture *dst = &game->temp_screen_texture;
    Assert(src->width == dst->width);
    Assert(src->height = dst->height);

    gl->UseProgram(shader);
    gl->BindTextureUnit(0, src->id);
    gl->BindImageTexture(1, dst->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    gl->DispatchCompute(dst->width, dst->height, 1);

    // Swap [texture] and [temp_screen_texture] GL ids
    u32 temp_id = src->id;
    src->id = dst->id;
    dst->id = temp_id;
}

void
GlDebugMessageCallback(GLenum source,
                       GLenum type,
                       GLuint id,
                       GLenum severity,
                       GLsizei length,
                       const GLchar* message,
                       const void* userParam)
{
    if(severity == GL_DEBUG_SEVERITY_HIGH_ARB or
       severity == GL_DEBUG_SEVERITY_HIGH)
    {
        Log("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message);
    }
}

GLuint
CompileShaderFromSourceFile(char *filename, GLenum type)
{
    u8 source_text[MAX_SHADER_SOURCE_LENGTH];
    bool load_file_result = platform->LoadFileIntoFixedBufferAndNullTerminate(filename, source_text, sizeof(source_text));
    if(load_file_result == false)
    {
        Log("Couldn't load shader source file (file didn't exist or source was larger than MAX_SHADER_SOURCE_LENGTH [%d]): %s", MAX_SHADER_SOURCE_LENGTH, filename);
        return 0;
    }

    GLuint shader = gl->CreateShader(type);
    GLchar *array_of_source_texts[1] = {(GLchar*)&source_text};
    gl->ShaderSource(shader, 1, array_of_source_texts, nullptr);
    gl->CompileShader(shader);

    GLint compile_status;
    gl->GetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if(compile_status == false)
    {
        char compile_info_log[MAX_GL_INFO_LOG_LENGTH];
        gl->GetShaderInfoLog(shader, sizeof(compile_info_log), nullptr, compile_info_log);
        Log("Failed to compile shader source file (%s):\n%s", filename, compile_info_log);
        gl->DeleteShader(shader);
        return 0;
    }
    else
    {
        //Log("Successfully compiled shader source file: %s", filename);
    }

    return shader;
}

GLuint
LinkShaders(GLuint vert_shader, GLuint frag_shader)
{
    GLuint program = gl->CreateProgram();
    if(vert_shader != 0) gl->AttachShader(program, vert_shader);
    if(frag_shader != 0) gl->AttachShader(program, frag_shader);
    gl->BindFragDataLocation(program, 0, "out_color");
    gl->LinkProgram(program);

    GLint link_status;
    gl->GetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(link_status == false)
    {
        char link_fail_info[MAX_GL_INFO_LOG_LENGTH];
        gl->GetProgramInfoLog(program, sizeof(link_fail_info), nullptr, link_fail_info);
        Log("Failed to link shader program:\n%s", link_fail_info);
        return 0;
    }

    //Log("Successfully linked shader program.");
    return program;
}

GLuint
LinkComputeShader(GLuint compute_shader)
{
    GLuint program = gl->CreateProgram();
    if(compute_shader != 0) gl->AttachShader(program, compute_shader);
    gl->LinkProgram(program);

    GLint link_status;
    gl->GetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(link_status == false)
    {
        char link_fail_info[MAX_GL_INFO_LOG_LENGTH];
        gl->GetProgramInfoLog(program, sizeof(link_fail_info), nullptr, link_fail_info);
        Log("Failed to link shader program:\n%s", link_fail_info);
        return 0;
    }

    return program;
}

GLuint
GenerateShaderProgramFromFiles(char *vs_filename, char *fs_filename)
{
    GLuint vs = 0;
    GLuint fs = 0;
    if(vs_filename) vs = CompileShaderFromSourceFile(vs_filename, GL_VERTEX_SHADER);
    if(fs_filename) fs = CompileShaderFromSourceFile(fs_filename, GL_FRAGMENT_SHADER);
    if(vs != 0 or fs != 0)
    {
        //Log("Successfully generated shader program.");
        return LinkShaders(vs, fs);
    }
    else
    {
        Log("Failed to generate shader program.");
        if(vs != 0) gl->DeleteShader(vs);
        if(fs != 0) gl->DeleteShader(fs);
        return 0;
    }
}

GLuint
GenerateComputeShaderFromFile(char *cs_filename)
{
    GLuint cs = CompileShaderFromSourceFile(cs_filename, GL_COMPUTE_SHADER);
    if(cs != 0)
    {
        //Log("Successfully generated shader program.");
        return LinkComputeShader(cs);
    }
    else
    {
        Log("Failed to generate compute shader program.");
        if(cs != 0) gl->DeleteShader(cs);
        return 0;
    }
}

void
LogGlError()
{
    GLenum error = gl->GetError();
    if(error == 0) return;
    Log("Current GL Error Code: %u", error);
}

void
LogGlVersion()
{
    const u8 *gl_version = gl->GetString(GL_VERSION);
    Log("OpenGL Version: \"%s\"", gl_version);
}