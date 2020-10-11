#include "opengl.h"
//#include "util.h"

#define MAX_SHADER_SOURCE_LENGTH 5000
#define MAX_GL_INFO_LOG_LENGTH 5000

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
	gl->AttachShader(program, vert_shader);
	gl->AttachShader(program, frag_shader);
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
GenerateShaderProgramFromFiles(char *vs_filename, char *fs_filename)
{
	GLuint vs = CompileShaderFromSourceFile(vs_filename, GL_VERTEX_SHADER);
	GLuint fs = CompileShaderFromSourceFile(fs_filename, GL_FRAGMENT_SHADER);
	if(vs != 0 and fs != 0)
	{
		//Log("Successfully generated shader program.");
		return LinkShaders(vs, fs);
	}
	else
	{
		Log("Failed to generate shader program.");
		if(vs == 0) gl->DeleteShader(vs);
		if(fs == 0) gl->DeleteShader(fs);
		return 0;
	}
}

void
LogGlError()
{
	GLenum error = gl->GetError();
	Log("Current GL Error Code: %u", error);
}