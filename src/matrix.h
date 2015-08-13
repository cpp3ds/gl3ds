#ifndef GL3DS_MATRICES_H
#define GL3DS_MATRICES_H

#include "glheader.h"
#include "math/m_matrix.h"

struct gl_context;

void _mesa_init_matrix( struct gl_context * ctx );
void _mesa_init_transform( struct gl_context *ctx );
void _mesa_free_matrix_data( struct gl_context *ctx );
void _mesa_update_modelview_project( struct gl_context *ctx, GLuint newstate );
void _gl3ds_upload_matrix(GLmatrix *mat, GLint uniform, bool rotate);

#endif
