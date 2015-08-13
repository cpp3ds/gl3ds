#ifndef GL3DS_SHADER
#define GL3DS_SHADER

struct gl_context;

void _gl3ds_update_program(struct gl_context *ctx);
void _mesa_init_program(struct gl_context *ctx);
void _mesa_free_program_data(struct gl_context *ctx);

#endif
