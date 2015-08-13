/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2008  Brian Paul   All Rights Reserved.
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


#ifndef FBOBJECT_H
#define FBOBJECT_H

#include "compiler.h"
#include "glheader.h"
#include <stdbool.h>

struct gl_context;
struct gl_texture_object;


/**
 * Is the given FBO a user-created FBO?
 */
static inline GLboolean
_mesa_is_user_fbo(const struct gl_framebuffer *fb)
{
   return fb->Name != 0;
}


/**
 * Is the given FBO a window system FBO (like an X window)?
 */
static inline GLboolean
_mesa_is_winsys_fbo(const struct gl_framebuffer *fb)
{
   return fb->Name == 0;
}



extern void
_mesa_init_fbobjects(struct gl_context *ctx);

extern struct gl_framebuffer *
_mesa_get_incomplete_framebuffer(void);

extern struct gl_renderbuffer *
_mesa_lookup_renderbuffer(struct gl_context *ctx, GLuint id);

extern struct gl_renderbuffer *
_mesa_lookup_renderbuffer_err(struct gl_context *ctx, GLuint id,
                              const char *func);

extern struct gl_framebuffer *
_mesa_lookup_framebuffer(struct gl_context *ctx, GLuint id);

extern struct gl_framebuffer *
_mesa_lookup_framebuffer_err(struct gl_context *ctx, GLuint id,
                             const char *func);


void
_mesa_update_texture_renderbuffer(struct gl_context *ctx,
                                  struct gl_framebuffer *fb,
                                  struct gl_renderbuffer_attachment *att);

extern void
_mesa_FramebufferRenderbuffer_sw(struct gl_context *ctx,
                                 struct gl_framebuffer *fb,
                                 GLenum attachment,
                                 struct gl_renderbuffer *rb);

extern void
_mesa_framebuffer_renderbuffer(struct gl_context *ctx,
                               struct gl_framebuffer *fb,
                               GLenum attachment,
                               struct gl_renderbuffer *rb,
                               const char *func);

extern void
_mesa_validate_framebuffer(struct gl_context *ctx, struct gl_framebuffer *fb);

extern GLboolean
_mesa_has_depthstencil_combined(const struct gl_framebuffer *fb);

extern void
_mesa_test_framebuffer_completeness(struct gl_context *ctx,
                                    struct gl_framebuffer *fb);

extern GLboolean
_mesa_is_legal_color_format(const struct gl_context *ctx, GLenum baseFormat);

extern GLenum
_mesa_base_fbo_format(struct gl_context *ctx, GLenum internalFormat);

extern bool
_mesa_detach_renderbuffer(struct gl_context *ctx,
                          struct gl_framebuffer *fb,
                          const void *att);

extern void
_mesa_framebuffer_texture(struct gl_context *ctx, struct gl_framebuffer *fb,
                          GLenum attachment,
                          struct gl_texture_object *texObj, GLenum textarget,
                          GLint level, GLuint layer, GLboolean layered,
                          const char *caller);

extern GLenum
_mesa_check_framebuffer_status(struct gl_context *ctx,
                               struct gl_framebuffer *fb);

extern void
_mesa_get_framebuffer_attachment_parameter(struct gl_context *ctx,
                                           struct gl_framebuffer *buffer,
                                           GLenum attachment, GLenum pname,
                                           GLint *params, const char *caller);


#endif /* FBOBJECT_H */
