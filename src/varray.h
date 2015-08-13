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


#ifndef VARRAY_H
#define VARRAY_H


#include "glheader.h"
#include "bufferobj.h"

struct gl_client_array;
struct gl_context;

/**
 * Returns a pointer to the vertex attribute data in a client array,
 * or the offset into the vertex buffer for an array that resides in
 * a vertex buffer.
 */
static inline const GLubyte *
_mesa_vertex_attrib_address(const struct gl_vertex_attrib_array *array,
							const struct gl_vertex_buffer_binding *binding)
{
	if (_mesa_is_bufferobj(binding->BufferObj))
		return (const GLubyte *) (binding->Offset + array->RelativeOffset);
	else
		return array->Ptr;
}

/**
 * Sets the fields in a gl_client_array to values derived from a
 * gl_vertex_attrib_array and a gl_vertex_buffer_binding.
 */
static inline void
_mesa_update_client_array(struct gl_context *ctx,
						  struct gl_client_array *dst,
						  const struct gl_vertex_attrib_array *src,
						  const struct gl_vertex_buffer_binding *binding)
{
	dst->Size = src->Size;
	dst->Type = src->Type;
	dst->Format = src->Format;
	dst->Stride = src->Stride;
	dst->StrideB = binding->Stride;
	dst->Ptr = _mesa_vertex_attrib_address(src, binding);
	dst->Enabled = src->Enabled;
	dst->Normalized = src->Normalized;
	dst->Integer = src->Integer;
	dst->Doubles = src->Doubles;
	dst->InstanceDivisor = binding->InstanceDivisor;
	dst->_ElementSize = src->_ElementSize;
	_mesa_reference_buffer_object(ctx, &dst->BufferObj, binding->BufferObj);
}

static inline bool
_mesa_attr_zero_aliases_vertex(struct gl_context *ctx)
{
	return false;
}

extern unsigned
_mesa_primitive_restart_index(const struct gl_context *ctx, GLenum ib_type);


extern void
_mesa_copy_client_array(struct gl_context *ctx,
						struct gl_client_array *dst,
						struct gl_client_array *src);

extern void
_mesa_copy_vertex_attrib_array(struct gl_context *ctx,
							   struct gl_vertex_attrib_array *dst,
							   const struct gl_vertex_attrib_array *src);

extern void
_mesa_copy_vertex_buffer_binding(struct gl_context *ctx,
								 struct gl_vertex_buffer_binding *dst,
								 const struct gl_vertex_buffer_binding *src);

extern void
		_mesa_print_arrays(struct gl_context *ctx);

extern void
		_mesa_init_varray( struct gl_context * ctx );

extern void
		_mesa_free_varray_data(struct gl_context *ctx);


//void init_varray(struct gl_context *ctx);

#endif
