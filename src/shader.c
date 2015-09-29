#include "glheader.h"
#include "context.h"


GLuint glCreateShader(GLenum shaderType)
{
	return (GLuint) malloc(sizeof(shaderInstance_s));
}

void glAttachShader(GLuint program, GLuint shader)
{
	// Not implemented
}


GLuint glCreateProgram(void)
{
	// TODO: store in shared context hash, return hash value
	shaderProgram_s* prog = (shaderProgram_s*) malloc(sizeof(shaderProgram_s));
	shaderProgramInit(prog);
	return (GLuint) prog;
}


void glDeleteProgram(GLuint program)
{
	shaderProgram_s* prog = (shaderProgram_s*) program;

	if (prog == NULL)
		return;

	GET_CURRENT_CONTEXT(ctx);
	if (ctx->Shared->Shader->Program == prog)
		ctx->Shared->Shader->Program = NULL;

	free(prog);
}

void glUseProgram(GLuint program) {
	// TODO: unset program when 0?
	if (program == 0)
		return;

	GET_CURRENT_CONTEXT(ctx);

	shaderProgram_s* prog = (shaderProgram_s*) program;

	if (ctx->Shared->Shader->Program == prog)
		return;

	ctx->Shared->Shader->Program = prog;

	FLUSH_VERTICES(ctx, _NEW_PROGRAM);
}


void glProgramBinary(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
{
	GET_CURRENT_CONTEXT(ctx);

	// set GL_PROGRAM_BINARY_FORMATS enum
	shaderProgram_s* prog = (shaderProgram_s*) program;
	DVLB_s* dvlb;

	if (binaryFormat & GL_VERTEX_SHADER_BINARY)
	{
		dvlb = DVLB_ParseFile((u32*)binary, length);
		shaderProgramSetVsh(prog, &dvlb->DVLE[0]);
	}
	else if (binaryFormat & GL_GEOMETRY_SHADER_BINARY)
	{
		dvlb = DVLB_ParseFile((u32*)binary, length);
		shaderProgramSetGsh(prog, &dvlb->DVLE[0], 1); // TODO: figure out geometry stride
	}
}

void _mesa_init_program(struct gl_context *ctx)
{
	ctx->Shared->Shader = CALLOC_STRUCT(gl_shader_program);
}

void _mesa_free_program_data(struct gl_context *ctx)
{
	if (ctx->Shared->Shader->Program)
		shaderProgramFree(ctx->Shared->Shader->Program);

	int i;
	for (i = 0; i < ctx->Shared->Shader->UniformCount; i++) {
		free(ctx->Shared->Shader->UniformVals[i].value);
	}

	free(ctx->Shared->Shader);
}

static void upload_program(struct gl_context *ctx)
{

}

#define GET_VSH_UNIFORM(name) (GLint) shaderInstanceGetUniformLocation(ctx->Shared->Shader->Program->vertexShader, name)

void _gl3ds_update_program(struct gl_context *ctx)
{
	if (ctx->Shared->Shader->Program)
	{
		shaderProgramUse(ctx->Shared->Shader->Program);

		// TODO: something better than forcing usage of these uniforms?
		ctx->Shared->Shader->ProjectionUniform = GET_VSH_UNIFORM("projection");
		ctx->Shared->Shader->ModelviewUniform = GET_VSH_UNIFORM("modelview");
		ctx->Shared->Shader->TextureUniform = GET_VSH_UNIFORM("texture");

		int i;
		for (i = 0; i < ctx->Shared->Shader->UniformCount; i++) {
			struct gl_shader_uniform* uniform = &ctx->Shared->Shader->UniformVals[i];
			if (uniform->changed) {
				GPU_SetFloatUniform(GPU_VERTEX_SHADER, uniform->location, (u32*) uniform->value, uniform->count);
				uniform->changed = false;
			}
		}
		if (!ctx->Shared->Shader->Uploaded) {
			ctx->Shared->Shader->Uploaded = GL_TRUE;
		}
	}
}
