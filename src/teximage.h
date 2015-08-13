/**
 * \file teximage.h
 * Texture images manipulation functions.
 */

/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2005  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef TEXIMAGE_H
#define TEXIMAGE_H


#include "mtypes.h"
#include "formats.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Is the given value one of the 6 cube faces? */
static inline GLboolean
_mesa_is_cube_face(GLenum target)
{
//   return (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB &&
//           target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB);
	return false;
}

/** Are any of the dimensions of given texture equal to zero? */
static inline GLboolean
_mesa_is_zero_size_texture(const struct gl_texture_image *texImage)
{
   return (texImage->Width == 0 ||
           texImage->Height == 0 ||
           texImage->Depth == 0);
}

/** \name Internal functions */
/*@{*/

extern GLint
_mesa_base_tex_format( struct gl_context *ctx, GLint internalFormat );


extern GLboolean
_mesa_is_proxy_texture(GLenum target);

extern bool
_mesa_is_array_texture(GLenum target);

extern struct gl_texture_image *
_mesa_new_texture_image( struct gl_context *ctx );


extern void
_mesa_delete_texture_image( struct gl_context *ctx,
                            struct gl_texture_image *teximage );


extern void
_mesa_init_teximage_fields(struct gl_context *ctx,
                           struct gl_texture_image *img,
                           GLsizei width, GLsizei height, GLsizei depth,
                           GLint border, GLenum internalFormat,
                           mesa_format format);


extern mesa_format
_mesa_choose_texture_format(struct gl_context *ctx,
                            struct gl_texture_object *texObj,
                            GLenum target, GLint level,
                            GLenum internalFormat, GLenum format, GLenum type);

extern void
_mesa_update_fbo_texture(struct gl_context *ctx,
                         struct gl_texture_object *texObj,
                         GLuint face, GLuint level);

extern void
_mesa_clear_texture_image(struct gl_context *ctx,
                          struct gl_texture_image *texImage);


extern struct gl_texture_image *
_mesa_select_tex_image(const struct gl_texture_object *texObj,
                       GLenum target, GLint level);


extern struct gl_texture_image *
_mesa_get_tex_image(struct gl_context *ctx, struct gl_texture_object *texObj,
                    GLenum target, GLint level);


/**
 * Return the base-level texture image for the given texture object.
 */
static inline const struct gl_texture_image *
_mesa_base_tex_image(const struct gl_texture_object *texObj)
{
   return texObj->Image[0][texObj->BaseLevel];
}


extern GLint
_mesa_max_texture_levels(struct gl_context *ctx, GLenum target);


extern GLboolean
_mesa_test_proxy_teximage(struct gl_context *ctx, GLenum target, GLint level,
                          mesa_format format,
                          GLint width, GLint height, GLint depth, GLint border);

extern GLboolean
_mesa_target_can_be_compressed(const struct gl_context *ctx, GLenum target,
                               GLenum intFormat);

extern GLuint
_mesa_tex_target_to_face(GLenum target);

extern GLint
_mesa_get_texture_dimensions(GLenum target);

extern GLboolean
_mesa_tex_target_is_layered(GLenum target);

extern GLuint
_mesa_get_texture_layers(const struct gl_texture_object *texObj, GLint level);

extern GLsizei
_mesa_get_tex_max_num_levels(GLenum target, GLsizei width, GLsizei height,
                             GLsizei depth);

extern GLboolean
_mesa_legal_texture_dimensions(struct gl_context *ctx, GLenum target,
                               GLint level, GLint width, GLint height,
                               GLint depth, GLint border);

extern mesa_format
_mesa_validate_texbuffer_format(const struct gl_context *ctx,
                                GLenum internalFormat);


bool
_mesa_legal_texture_base_format_for_target(struct gl_context *ctx,
                                           GLenum target,
                                           GLenum internalFormat,
                                           unsigned dimensions,
                                           const char *caller);

extern void
_mesa_texture_sub_image(struct gl_context *ctx, GLuint dims,
                        struct gl_texture_object *texObj,
                        struct gl_texture_image *texImage,
                        GLenum target, GLint level,
                        GLint xoffset, GLint yoffset, GLint zoffset,
                        GLsizei width, GLsizei height, GLsizei depth,
                        GLenum format, GLenum type, const GLvoid *pixels,
                        bool dsa);

extern void
_mesa_compressed_texture_sub_image(struct gl_context *ctx, GLuint dims, 
                                   struct gl_texture_object *texObj, 
                                   struct gl_texture_image *texImage,
                                   GLenum target, GLint level,
                                   GLint xoffset, GLint yoffset,
                                   GLint zoffset,
                                   GLsizei width, GLsizei height,
                                   GLsizei depth,
                                   GLenum format, GLsizei imageSize,
                                   const GLvoid *data);

extern void
_mesa_copy_texture_sub_image(struct gl_context *ctx, GLuint dims,
                             struct gl_texture_object *texObj,
                             GLenum target, GLint level,
                             GLint xoffset, GLint yoffset, GLint zoffset,
                             GLint x, GLint y,
                             GLsizei width, GLsizei height,
                             const char *caller);

extern void
_mesa_texture_image_multisample(struct gl_context *ctx, GLuint dims,
                                struct gl_texture_object *texObj,
                                GLenum target, GLsizei samples,
                                GLint internalformat, GLsizei width,
                                GLsizei height, GLsizei depth,
                                GLboolean fixedsamplelocations,
                                GLboolean immutable, const char *func);

extern void
_mesa_texture_buffer_range(struct gl_context *ctx,
                           struct gl_texture_object *texObj,
                           GLenum internalFormat,
                           struct gl_buffer_object *bufObj,
                           GLintptr offset, GLsizeiptr size,
                           const char *caller);
/*@}*/

#ifdef __cplusplus
}
#endif

#endif
