#ifndef OPENGL_H
#define OPENGL_H

#include "log.h"
#include "types.h"

#define GL_ARRAY_BUFFER					  0x8892
#define GL_ELEMENT_ARRAY_BUFFER 		  0x8893
#define GL_STATIC_DRAW 					  0x88E4
#define GL_FRAGMENT_SHADER 				  0x8B30
#define GL_VERTEX_SHADER 				  0x8B31
#define GL_COMPILE_STATUS 				  0x8B81
#define GL_LINK_STATUS 					  0x8B82
#define GL_DYNAMIC_DRAW 				  0x88E8
#define GL_TEXTURE0 					  0x84C0
#define GL_BGR 							  0x80E0
#define GL_BGRA                           0x80E1
#define GL_VERTEX_ARRAY_BINDING 		  0x85B5
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_RGB                            0x1907
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_NEAREST                        0x2600
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_2D                     0x0DE1
#define GL_R32UI                          0x8236
#define GL_R32F                           0x822E
#define GL_RG                             0x8227
#define GL_RG32F                          0x8230
#define GL_RGB32F                         0x8815
#define GL_RGBA32F                        0x8814
#define GL_RED_INTEGER                    0x8D94
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_RED                            0x1903
#define GL_LINEAR                         0x2601
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_RGBA                           0x1908
#define GL_FRONT_AND_BACK                 0x0408
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02
#define GL_READ_ONLY                      0x88B8
#define GL_BLEND                          0x0BE2
#define GL_FUNC_ADD                       0x8006
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
#define GL_ZERO                           0
#define GL_ONE                            1

#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007

#define GL_BYTE            				  0x1400
#define GL_UNSIGNED_BYTE   				  0x1401
#define GL_SHORT           				  0x1402
#define GL_UNSIGNED_SHORT  				  0x1403
#define GL_INT             				  0x1404
#define GL_UNSIGNED_INT    				  0x1405
#define GL_FLOAT                          0x1406
#define GL_FALSE                          0
#define GL_TRUE                           1

typedef size_t GLsizeiptr;
typedef char GLchar;
typedef u32 GLenum;
typedef float GLfloat;
typedef s32 GLint;
typedef s32 GLsizei;
typedef double GLdouble;
typedef u32 GLuint;
typedef u32 GLbitfield;

#ifndef WIN32
	typedef s8 GLboolean;
#endif

typedef GLenum 		fnsig_glGetError(void);
typedef void 		fnsig_glBindBuffer(GLenum, GLuint);
typedef void 		fnsig_glGenBuffers(GLsizei, GLuint*);
typedef void 		fnsig_glDeleteBuffers(GLsizei, GLuint*);
typedef void 		fnsig_glBufferData(GLenum, GLsizeiptr, void*, GLenum);
typedef void 		fnsig_glNamedBufferData(GLuint, GLsizeiptr, void*, GLenum);
typedef GLuint 		fnsig_glCreateShader(GLenum);
typedef void 		fnsig_glShaderSource(GLuint, GLsizei, GLchar**, GLint*);
typedef void		fnsig_glCompileShader(GLuint);
typedef void		fnsig_glGetShaderInfoLog(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void 		fnsig_glGetShaderiv(GLuint, GLenum, GLint*);
typedef GLuint 		fnsig_glCreateProgram();
typedef void 		fnsig_glLinkProgram(GLuint);
typedef void 		fnsig_glGetProgramiv(GLuint, GLenum, GLint*);
typedef void 		fnsig_glGetProgramInfoLog(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void 		fnsig_glAttachShader(GLuint, GLuint);
typedef void 		fnsig_glUseProgram(GLuint);
typedef GLint 		fnsig_glGetAttribLocation(GLuint, GLchar*);
typedef void 		fnsig_glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, void*);
typedef void 		fnsig_glDrawArrays(GLenum, GLint, GLsizei);
typedef void 		fnsig_glEnableVertexAttribArray(GLuint);
typedef void 		fnsig_glDisableVertexAttribArray(GLuint);
typedef void 		fnsig_glEnableVertexArrayAttrib(GLuint, GLuint);
typedef void		fnsig_glBindFragDataLocation(GLuint, GLuint, char*);
typedef GLint 		fnsig_glGetUniformLocation(GLuint, GLchar*);
typedef void 		fnsig_glUniform1f(GLint, GLfloat);
typedef void		fnsig_glUniform2f(GLint, GLfloat, GLfloat);
typedef void		fnsig_glUniform3f(GLint, GLfloat, GLfloat, GLfloat);
typedef void		fnsig_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void		fnsig_glUniform3fv(GLint, GLsizei, GLfloat*);
typedef void		fnsig_glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
typedef void 		fnsig_glUniform1ui(GLint, GLuint);
typedef void 		fnsig_glGenTextures(GLsizei, GLuint*);
typedef void 		fnsig_glBindTexture(GLenum, GLuint);
typedef void 		fnsig_glTexParameteri(GLenum, GLenum, GLint);
typedef void 		fnsig_glTexImage2D(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
typedef void 		fnsig_glActiveTexture(GLenum);
typedef void 		fnsig_glUniform1i(GLint, GLint);
typedef GLboolean 	fnsig_glIsBuffer(GLuint);
typedef void 		fnsig_glGetIntegerv(GLenum, GLint*);
typedef void 		fnsig_glGenVertexArrays(GLsizei, GLuint*);
typedef void		fnsig_glBindVertexArray(GLuint);
typedef void 		fnsig_glDeleteVertexArrays(GLsizei, GLuint*);
typedef void		fnsig_glDeleteTextures(GLsizei n, const GLuint *textures);
typedef void 		fnsig_glClearColor(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void 		fnsig_glClear(GLbitfield);
typedef void		fnsig_glEnable(GLenum);
typedef void		fnsig_glDisable(GLenum);
typedef void 		fnsig_glDrawElements(GLenum, GLsizei, GLenum, void*);
typedef void 		fnsig_glGenFramebuffers(GLsizei n, GLuint *framebuffers);
typedef void 		fnsig_glBindFramebuffer(GLenum target, GLuint framebuffer);
typedef void 		fnsig_glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
typedef void 		fnsig_glBindRenderbuffer(GLenum target, GLuint renderbuffer);
typedef void 		fnsig_glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void 		fnsig_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef GLenum 		fnsig_glCheckFramebufferStatus(GLenum target);
typedef void		fnsig_glDeleteShader(GLuint shader);
typedef void 		fnsig_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void 		fnsig_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void 		fnsig_glDrawBuffers(GLsizei n, const GLenum *bufs);
typedef void		fnsig_glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void		fnsig_glGetTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void		fnsig_glFinish(void);
typedef void		fnsig_glFlush(void);
typedef void		fnsig_glPixelStorei(GLenum pname, GLint param);
typedef void		fnsig_glPolygonMode(GLenum face, GLenum mode);
typedef void 	   *fnsig_glMapNamedBuffer(GLuint buffer, GLenum access);
typedef GLboolean	fnsig_glUnmapNamedBuffer(GLuint buffer);
typedef void		fnsig_glBlendEquation (GLenum mode);
typedef void		fnsig_glBlendFunc (GLenum sfactor, GLenum dfactor);


// ProgramUniform...()
typedef void 		fnsig_glProgramUniform1i(GLuint program, GLint location, GLint v0);
typedef void 		fnsig_glProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void 		fnsig_glProgramUniform1f(GLuint program, GLint location, GLfloat v0);
typedef void 		fnsig_glProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void 		fnsig_glProgramUniform1d(GLuint program, GLint location, GLdouble v0);
typedef void 		fnsig_glProgramUniform1dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void 		fnsig_glProgramUniform1ui(GLuint program, GLint location, GLuint v0);
typedef void 		fnsig_glProgramUniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void 		fnsig_glProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1);
typedef void 		fnsig_glProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void 		fnsig_glProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1);
typedef void 		fnsig_glProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void 		fnsig_glProgramUniform2d(GLuint program, GLint location, GLdouble v0, GLdouble v1);
typedef void 		fnsig_glProgramUniform2dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void 		fnsig_glProgramUniform2ui(GLuint program, GLint location, GLuint v0, GLuint v1);
typedef void 		fnsig_glProgramUniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void 		fnsig_glProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
typedef void 		fnsig_glProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void 		fnsig_glProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void 		fnsig_glProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void 		fnsig_glProgramUniform3d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
typedef void 		fnsig_glProgramUniform3dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void 		fnsig_glProgramUniform3ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void 		fnsig_glProgramUniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void 		fnsig_glProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void 		fnsig_glProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void 		fnsig_glProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void 		fnsig_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void 		fnsig_glProgramUniform4d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
typedef void 		fnsig_glProgramUniform4dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void 		fnsig_glProgramUniform4ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void 		fnsig_glProgramUniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void 		fnsig_glProgramUniformMatrix2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix2x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix3x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix2x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix4x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix3x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix4x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void 		fnsig_glProgramUniformMatrix2x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix3x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix2x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix4x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix3x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void 		fnsig_glProgramUniformMatrix4x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);


#define mOpenGLFunction(name) fnsig_gl##name *name

struct OpenGL {
	mOpenGLFunction(GetError);
	mOpenGLFunction(BindBuffer);
	mOpenGLFunction(DeleteBuffers);
	mOpenGLFunction(GenBuffers);
	mOpenGLFunction(BufferData);
	mOpenGLFunction(NamedBufferData);
	mOpenGLFunction(CreateShader);
	mOpenGLFunction(ShaderSource);
	mOpenGLFunction(CompileShader);
	mOpenGLFunction(GetShaderInfoLog);
	mOpenGLFunction(GetShaderiv);
	mOpenGLFunction(CreateProgram);
	mOpenGLFunction(LinkProgram);
	mOpenGLFunction(GetProgramiv);
	mOpenGLFunction(GetProgramInfoLog);
	mOpenGLFunction(AttachShader);
 	mOpenGLFunction(UseProgram);
 	mOpenGLFunction(GetAttribLocation);
 	mOpenGLFunction(VertexAttribPointer);
 	mOpenGLFunction(DrawArrays);
	mOpenGLFunction(EnableVertexAttribArray);
	mOpenGLFunction(DisableVertexAttribArray);
	mOpenGLFunction(EnableVertexArrayAttrib);
	mOpenGLFunction(BindFragDataLocation);
	mOpenGLFunction(GetUniformLocation);
	mOpenGLFunction(Uniform1f);
	mOpenGLFunction(Uniform2f);
	mOpenGLFunction(Uniform3f);
	mOpenGLFunction(Uniform4f);
	mOpenGLFunction(Uniform3fv);
	mOpenGLFunction(Uniform4fv);
	mOpenGLFunction(Uniform1ui);
 	mOpenGLFunction(GenTextures);
 	mOpenGLFunction(BindTexture);
 	mOpenGLFunction(TexParameteri);
 	mOpenGLFunction(TexImage2D);
 	mOpenGLFunction(ActiveTexture);
 	mOpenGLFunction(Uniform1i);
 	mOpenGLFunction(IsBuffer);
 	mOpenGLFunction(GetIntegerv);
 	mOpenGLFunction(GenVertexArrays);
 	mOpenGLFunction(BindVertexArray);
 	mOpenGLFunction(DeleteVertexArrays);
 	mOpenGLFunction(DeleteTextures);
	mOpenGLFunction(ClearColor);
	mOpenGLFunction(Clear);
	mOpenGLFunction(Enable);
	mOpenGLFunction(Disable);
	mOpenGLFunction(DrawElements);
	mOpenGLFunction(GenFramebuffers);
	mOpenGLFunction(BindFramebuffer);
	mOpenGLFunction(GenRenderbuffers);
	mOpenGLFunction(BindRenderbuffer);
	mOpenGLFunction(RenderbufferStorage);
	mOpenGLFunction(FramebufferRenderbuffer);
	mOpenGLFunction(CheckFramebufferStatus);
	mOpenGLFunction(DeleteShader);
	//mOpenGLFunction(BindFramebuffer);
	mOpenGLFunction(BlitFramebuffer);
	mOpenGLFunction(FramebufferTexture2D);
	mOpenGLFunction(DrawBuffers);
	mOpenGLFunction(GetTextureImage);
	mOpenGLFunction(GetTextureSubImage);
	mOpenGLFunction(Finish);
	mOpenGLFunction(Flush);
	mOpenGLFunction(PixelStorei);
	mOpenGLFunction(PolygonMode);
	mOpenGLFunction(MapNamedBuffer);
	mOpenGLFunction(UnmapNamedBuffer);

	// ProgramUniform...()
	mOpenGLFunction(ProgramUniform1i);
	mOpenGLFunction(ProgramUniform1iv);
	mOpenGLFunction(ProgramUniform1f);
	mOpenGLFunction(ProgramUniform1fv);
	mOpenGLFunction(ProgramUniform1d);
	mOpenGLFunction(ProgramUniform1dv);
	mOpenGLFunction(ProgramUniform1ui);
	mOpenGLFunction(ProgramUniform1uiv);
	mOpenGLFunction(ProgramUniform2i);
	mOpenGLFunction(ProgramUniform2iv);
	mOpenGLFunction(ProgramUniform2f);
	mOpenGLFunction(ProgramUniform2fv);
	mOpenGLFunction(ProgramUniform2d);
	mOpenGLFunction(ProgramUniform2dv);
	mOpenGLFunction(ProgramUniform2ui);
	mOpenGLFunction(ProgramUniform2uiv);
	mOpenGLFunction(ProgramUniform3i);
	mOpenGLFunction(ProgramUniform3iv);
	mOpenGLFunction(ProgramUniform3f);
	mOpenGLFunction(ProgramUniform3fv);
	mOpenGLFunction(ProgramUniform3d);
	mOpenGLFunction(ProgramUniform3dv);
	mOpenGLFunction(ProgramUniform3ui);
	mOpenGLFunction(ProgramUniform3uiv);
	mOpenGLFunction(ProgramUniform4i);
	mOpenGLFunction(ProgramUniform4iv);
	mOpenGLFunction(ProgramUniform4f);
	mOpenGLFunction(ProgramUniform4fv);
	mOpenGLFunction(ProgramUniform4d);
	mOpenGLFunction(ProgramUniform4dv);
	mOpenGLFunction(ProgramUniform4ui);
	mOpenGLFunction(ProgramUniform4uiv);
	mOpenGLFunction(ProgramUniformMatrix2fv);
	mOpenGLFunction(ProgramUniformMatrix3fv);
	mOpenGLFunction(ProgramUniformMatrix4fv);
	mOpenGLFunction(ProgramUniformMatrix2dv);
	mOpenGLFunction(ProgramUniformMatrix3dv);
	mOpenGLFunction(ProgramUniformMatrix4dv);
	mOpenGLFunction(ProgramUniformMatrix2x3fv);
	mOpenGLFunction(ProgramUniformMatrix3x2fv);
	mOpenGLFunction(ProgramUniformMatrix2x4fv);
	mOpenGLFunction(ProgramUniformMatrix4x2fv);
	mOpenGLFunction(ProgramUniformMatrix3x4fv);
	mOpenGLFunction(ProgramUniformMatrix4x3fv);
	mOpenGLFunction(ProgramUniformMatrix2x3dv);
	mOpenGLFunction(ProgramUniformMatrix3x2dv);
	mOpenGLFunction(ProgramUniformMatrix2x4dv);
	mOpenGLFunction(ProgramUniformMatrix4x2dv);
	mOpenGLFunction(ProgramUniformMatrix3x4dv);
	mOpenGLFunction(ProgramUniformMatrix4x3dv);

	mOpenGLFunction(BlendEquation);
	mOpenGLFunction(BlendFunc);
};

bool
CheckOpenGLBinds(const OpenGL *opengl)
{
	u8 *start = (u8*)opengl;
	u8 *end = start+sizeof(OpenGL);
	u8 *p = start;
	int fnptr_size = sizeof(void(*)(void));
	while(p < end) {
		if(*(void(**)(void))p == nullptr) {
			Log("An OpenGL bind failed.");
			return false;
		}
		p += fnptr_size;
	}

	return true;
}

#endif