/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2008  Brian Paul   All Rights Reserved.
 * Copyright (C) 2009  VMware, Inc.  All Rights Reserved.
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



#ifndef BUFFEROBJ_H
#define BUFFEROBJ_H

#include <stdbool.h>
#include "mtypes.h"


/*
 * Internal functions
 */


/** Is the given buffer object currently mapped by the GL user? */
static inline GLboolean
_mesa_bufferobj_mapped(const struct gl_buffer_object *obj, gl_map_buffer_index index)
{
   return obj->Mappings[index].Pointer != NULL;
}

/** Can we not use this buffer while mapped? */
static inline GLboolean
_mesa_check_disallowed_mapping(const struct gl_buffer_object *obj)
{
//   return _mesa_bufferobj_mapped(obj, MAP_USER) &&
//          !(obj->Mappings[MAP_USER].AccessFlags &
//            GL_MAP_PERSISTENT_BIT);
	return GL_FALSE;
}

/**
 * Is the given buffer object a user-created buffer object?
 * Mesa uses default buffer objects in several places.  Default buffers
 * always have Name==0.  User created buffers have Name!=0.
 */
static inline GLboolean
_mesa_is_bufferobj(const struct gl_buffer_object *obj)
{
   return obj != NULL && obj->Name != 0;
}


extern void
_mesa_init_buffer_objects(struct gl_context *ctx);

extern void
_mesa_free_buffer_objects(struct gl_context *ctx);

extern bool
_mesa_handle_bind_buffer_gen(struct gl_context *ctx,
                             GLenum target,
                             GLuint buffer,
                             struct gl_buffer_object **buf_handle,
                             const char *caller);

extern void
_mesa_update_default_objects_buffer_objects(struct gl_context *ctx);


extern struct gl_buffer_object *
_mesa_lookup_bufferobj(struct gl_context *ctx, GLuint buffer);

extern struct gl_buffer_object *
_mesa_lookup_bufferobj_locked(struct gl_context *ctx, GLuint buffer);

extern struct gl_buffer_object *
_mesa_lookup_bufferobj_err(struct gl_context *ctx, GLuint buffer,
                           const char *caller);

extern void
_mesa_begin_bufferobj_lookups(struct gl_context *ctx);

extern void
_mesa_end_bufferobj_lookups(struct gl_context *ctx);

extern struct gl_buffer_object *
_mesa_multi_bind_lookup_bufferobj(struct gl_context *ctx,
                                  const GLuint *buffers,
                                  GLuint index, const char *caller);

extern void
_mesa_initialize_buffer_object(struct gl_context *ctx,
                               struct gl_buffer_object *obj,
                               GLuint name);

extern void
_mesa_reference_buffer_object_(struct gl_context *ctx,
                               struct gl_buffer_object **ptr,
                               struct gl_buffer_object *bufObj);

static inline void
_mesa_reference_buffer_object(struct gl_context *ctx,
                              struct gl_buffer_object **ptr,
                              struct gl_buffer_object *bufObj)
{
   if (*ptr != bufObj)
      _mesa_reference_buffer_object_(ctx, ptr, bufObj);
}

extern GLuint
_mesa_total_buffer_object_memory(struct gl_context *ctx);

extern void
_mesa_init_buffer_object_functions(struct dd_function_table *driver);

extern void
_mesa_buffer_storage(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                     GLenum target, GLsizeiptr size, const GLvoid *data,
                     GLbitfield flags, const char *func);

extern void
_mesa_buffer_data(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                  GLenum target, GLsizeiptr size, const GLvoid *data,
                  GLenum usage, const char *func);

extern void
_mesa_buffer_sub_data(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                      GLintptr offset, GLsizeiptr size, const GLvoid *data,
                      const char *func);

extern void
_mesa_buffer_unmap_all_mappings(struct gl_context *ctx,
                                struct gl_buffer_object *bufObj);

extern void
_mesa_copy_buffer_sub_data(struct gl_context *ctx,
                           struct gl_buffer_object *src,
                           struct gl_buffer_object *dst,
                           GLintptr readOffset, GLintptr writeOffset,
                           GLsizeiptr size, const char *func);

extern void *
_mesa_map_buffer_range(struct gl_context *ctx,
                       struct gl_buffer_object *bufObj,
                       GLintptr offset, GLsizeiptr length,
                       GLbitfield access, const char *func);

extern void
_mesa_flush_mapped_buffer_range(struct gl_context *ctx,
                                struct gl_buffer_object *bufObj,
                                GLintptr offset, GLsizeiptr length,
                                const char *func);

extern void
_mesa_ClearBufferSubData_sw(struct gl_context *ctx,
                            GLintptr offset, GLsizeiptr size,
                            const GLvoid *clearValue,
                            GLsizeiptr clearValueSize,
                            struct gl_buffer_object *bufObj);

extern void
_mesa_clear_buffer_sub_data(struct gl_context *ctx,
                            struct gl_buffer_object *bufObj,
                            GLenum internalformat,
                            GLintptr offset, GLsizeiptr size,
                            GLenum format, GLenum type,
                            const GLvoid *data,
                            const char *func, bool subdata);

extern GLboolean
_mesa_unmap_buffer(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                   const char *func);

/*
 * API functions
 */



#endif
