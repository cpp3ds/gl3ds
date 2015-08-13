#include "glheader.h"
#include "mtypes.h"
#include "errors.h"
#include "context.h"

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	GET_CURRENT_CONTEXT(ctx);

	// TODO: make use of transpose according to OGL standards
	if (location < 0) {
		_mesa_error(ctx, GL_INVALID_VALUE,
					"glUniformMatrix4fv(location=%d)",
					location);
		return;
	}

	if (!transpose) {
		GLfloat copy[16];
		_math_transposef(copy, value);
		memcpy(value, copy, sizeof(GLfloat)*16);
	}

	float param[16];
	// TODO: Flip xyzw positions? Why not in shader?
	param[0x0] = value[3];
	param[0x1] = value[2];
	param[0x2] = value[1];
	param[0x3] = value[0];

	param[0x4] = value[7];
	param[0x5] = value[6];
	param[0x6] = value[5];
	param[0x7] = value[4];

	param[0x8] = value[11];
	param[0x9] = value[10];
	param[0xa] = value[9];
	param[0xb] = value[8];

	param[0xc] = value[15];
	param[0xd] = value[14];
	param[0xe] = value[13];
	param[0xf] = value[12];

//	printf("\nv:%f,%f,%f,%f\nv:%f,%f,%f,%f\nv:%f,%f,%f,%f\nv:%f,%f,%f,%f\n",
//		   value[0],value[1],value[2],value[3],
//		   value[4],value[5],value[6],value[7],
//		   value[8],value[9],value[10],value[11],
//		   value[12],value[13],value[14],value[15]);
	GPU_SetFloatUniform(GPU_VERTEX_SHADER, location, (u32*) param, 4);
}

GLint glGetUniformLocation(GLuint program, const GLchar* name)
{
	struct gl_shader_program* prog = (struct gl_shader_program*) program;
	// TODO: need to check geometry shader too?
	return (GLint) shaderInstanceGetUniformLocation(prog->Program->vertexShader, name);
}

/* UNIFORM FUNCTIONS */
void glUniform1f(GLint location, GLfloat v0){}
void glUniform2f(GLint location, GLfloat v0, GLfloat v1){}
void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2){}
void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){}
void glUniform1i(GLint location, GLint v0){}

void glUniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
	// TODO: update shader state
	GPUCMD_AddSingleParam(0x000F02C0, 0x80000000 | location);
	GPUCMD_Add(0x000F02C1, (u32*) value, count * 4);
}
