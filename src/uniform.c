#include "glheader.h"
#include "mtypes.h"
#include "errors.h"
#include "context.h"

static void set_uniform(GLint location, GLsizei count, const GLfloat* value, bool need_swap)
{
	GET_CURRENT_CONTEXT(ctx);
	if (ctx->Shared->Shader->Program)
	{
		int i;
		struct gl_shader_uniform* uniform;
		for (i = 0; i < ctx->Shared->Shader->UniformCount; i++) {
			if (ctx->Shared->Shader->UniformVals[i].location == location) {
				break;
			}
		}
		uniform = &ctx->Shared->Shader->UniformVals[i];
		if (i == ctx->Shared->Shader->UniformCount) {
			uniform->count = (u8) count;
			uniform->location = (u32) location;
			uniform->value = malloc(sizeof(float) * count * 4);
			if (!uniform->value)
				return;
			ctx->Shared->Shader->UniformCount++;
		}

		if (need_swap) {
			for (i = 0; i < count*4; i+=4) {
				uniform->value[i]   = value[i+3];
				uniform->value[i+1] = value[i+2];
				uniform->value[i+2] = value[i+1];
				uniform->value[i+3] = value[i];
			}
		} else {
			memcpy(uniform->value, value, sizeof(float) * count * 4);
		}

		uniform->changed = true;
	}
}

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

//	printf("\nv:%f,%f,%f,%f\nv:%f,%f,%f,%f\nv:%f,%f,%f,%f\nv:%f,%f,%f,%f\n",
//		   value[0],value[1],value[2],value[3],
//		   value[4],value[5],value[6],value[7],
//		   value[8],value[9],value[10],value[11],
//		   value[12],value[13],value[14],value[15]);
	set_uniform(location, count * 4, value, true);
}

GLint glGetUniformLocation(GLuint program, const GLchar* name)
{
	shaderProgram_s* prog = (shaderProgram_s*) program;
	// TODO: need to check geometry shader too?
	return (GLint) shaderInstanceGetUniformLocation(prog->vertexShader, name);
}

/* UNIFORM FUNCTIONS */
void glUniform1f(GLint location, GLfloat v0){
	float params[4] = {0, 0, 0, v0};
	set_uniform(location, 1, params, false);
}

void glUniform2f(GLint location, GLfloat v0, GLfloat v1){
	float params[4] = {0, 0, v1, v0};
	set_uniform(location, 1, params, false);
}

void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2){
	float params[4] = {0, v2, v1, v0};
	set_uniform(location, 1, params, false);
}

void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){
	float params[4] = {v3, v2, v1, v0};
	set_uniform(location, 1, params, false);
}

void glUniform1i(GLint location, GLint v0){}
void glUniform2i(GLint location, GLint v0, GLint v1){}
void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2){}
void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3){}

void glUniform4iv(GLint location, GLsizei count, const GLint *value)
{
	GPUCMD_AddSingleParam(0x000F02C0, 0x80000000 | location);
	GPUCMD_Add(0x000F02C1, (u32*) value, count * 4);
}


void glUniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
	// TODO: update shader state
	GPUCMD_AddSingleParam(0x000F02C0, 0x80000000 | location);
	GPUCMD_Add(0x000F02C1, (u32*) value, count * 4);
}
