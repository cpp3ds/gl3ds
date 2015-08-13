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


/**
 * \file bufferobj.c
 * \brief Functions for the GL_ARB_vertex/pixel_buffer_object extensions.
 * \author Brian Paul, Ian Romanick
 */

#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>  /* for PRId64 macro */
#include "glheader.h"
//#include "enums.h"
#include "hash.h"
#include "imports.h"
//#include "image.h"
#include "bufferobj.h"
//#include "fbobject.h"
#include "mtypes.h"
//#include "texobj.h"
//#include "teximage.h"
#include "glformats.h"
#include "shared.h"
#include "context.h"

/* Debug flags */
/*#define VBO_DEBUG*/
/*#define BOUNDS_CHECK*/


/**
 * Used as a placeholder for buffer objects between glGenBuffers() and
 * glBindBuffer() so that glIsBuffer() can work correctly.
 */
static struct gl_buffer_object DummyBufferObject;


/**
 * Return pointer to address of a buffer object target.
 * \param ctx  the GL context
 * \param target  the buffer object target to be retrieved.
 * \return   pointer to pointer to the buffer object bound to \c target in the
 *           specified context or \c NULL if \c target is invalid.
 */
static inline struct gl_buffer_object **
get_buffer_target(struct gl_context *ctx, GLenum target)
{

   switch (target) {
   case GL_ARRAY_BUFFER:
      return &ctx->Array.ArrayBufferObj;
   case GL_ELEMENT_ARRAY_BUFFER:
      return &ctx->Array.VAO->IndexBufferObj;
//   case GL_PIXEL_PACK_BUFFER_EXT:
//      return &ctx->Pack.BufferObj;
//   case GL_PIXEL_UNPACK_BUFFER_EXT:
//      return &ctx->Unpack.BufferObj;
//   case GL_COPY_READ_BUFFER:
//      return &ctx->CopyReadBuffer;
//   case GL_COPY_WRITE_BUFFER:
//      return &ctx->CopyWriteBuffer;
//   case GL_DRAW_INDIRECT_BUFFER:
//      if (ctx->API == API_OPENGL_CORE &&
//          ctx->Extensions.ARB_draw_indirect) {
//         return &ctx->DrawIndirectBuffer;
//      }
//      break;
//   case GL_TRANSFORM_FEEDBACK_BUFFER:
//      if (ctx->Extensions.EXT_transform_feedback) {
//         return &ctx->TransformFeedback.CurrentBuffer;
//      }
//      break;
//   case GL_TEXTURE_BUFFER:
//      if (ctx->API == API_OPENGL_CORE &&
//          ctx->Extensions.ARB_texture_buffer_object) {
//         return &ctx->Texture.BufferObject;
//      }
//      break;
//   case GL_UNIFORM_BUFFER:
//      if (ctx->Extensions.ARB_uniform_buffer_object) {
//         return &ctx->UniformBuffer;
//      }
//      break;
//   case GL_ATOMIC_COUNTER_BUFFER:
//      if (ctx->Extensions.ARB_shader_atomic_counters) {
//         return &ctx->AtomicBuffer;
//      }
//      break;
//   case GL_EXTERNAL_VIRTUAL_MEMORY_BUFFER_AMD:
//      if (ctx->Extensions.AMD_pinned_memory) {
//         return &ctx->ExternalVirtualMemoryBuffer;
//      }
//      break;
   default:
      return NULL;
   }
}


/**
 * Get the buffer object bound to the specified target in a GL context.
 * \param ctx  the GL context
 * \param target  the buffer object target to be retrieved.
 * \param error  the GL error to record if target is illegal.
 * \return   pointer to the buffer object bound to \c target in the
 *           specified context or \c NULL if \c target is invalid.
 */
static inline struct gl_buffer_object *
get_buffer(struct gl_context *ctx, const char *func, GLenum target,
           GLenum error)
{
   struct gl_buffer_object **bufObj = get_buffer_target(ctx, target);

   if (!bufObj) {
      _mesa_error(ctx, GL_INVALID_ENUM, "%s(target)", func);
      return NULL;
   }

   if (!_mesa_is_bufferobj(*bufObj)) {
      _mesa_error(ctx, error, "%s(no buffer bound)", func);
      return NULL;
   }

   return *bufObj;
}


/**
 * Convert a GLbitfield describing the mapped buffer access flags
 * into one of GL_READ_WRITE, GL_READ_ONLY, or GL_WRITE_ONLY.
 */
static GLenum
simplified_access_mode(struct gl_context *ctx, GLbitfield access)
{
   const GLbitfield rwFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
   if ((access & rwFlags) == rwFlags)
      return GL_READ_WRITE;
   if ((access & GL_MAP_READ_BIT) == GL_MAP_READ_BIT)
      return GL_READ_ONLY;
   if ((access & GL_MAP_WRITE_BIT) == GL_MAP_WRITE_BIT)
      return GL_WRITE_ONLY;

   /* Otherwise, AccessFlags is zero (the default state).
    *
    * Table 2.6 on page 31 (page 44 of the PDF) of the OpenGL 1.5 spec says:
    *
    * Name           Type  Initial Value  Legal Values
    * ...            ...   ...            ...
    * BUFFER_ACCESS  enum  READ_WRITE     READ_ONLY, WRITE_ONLY
    *                                     READ_WRITE
    *
    * However, table 6.8 in the GL_OES_mapbuffer extension says:
    *
    * Get Value         Type Get Command          Value          Description
    * ---------         ---- -----------          -----          -----------
    * BUFFER_ACCESS_OES Z1   GetBufferParameteriv WRITE_ONLY_OES buffer map flag
    *
    * The difference is because GL_OES_mapbuffer only supports mapping buffers
    * write-only.
    */
   assert(access == 0);

//   return _mesa_is_gles(ctx) ? GL_WRITE_ONLY : GL_READ_WRITE;
	return GL_WRITE_ONLY;
}


/**
 * Test if the buffer is mapped, and if so, if the mapped range overlaps the
 * given range.
 * The regions do not overlap if and only if the end of the given
 * region is before the mapped region or the start of the given region
 * is after the mapped region.
 *
 * \param obj     Buffer object target on which to operate.
 * \param offset  Offset of the first byte of the subdata range.
 * \param size    Size, in bytes, of the subdata range.
 * \return   true if ranges overlap, false otherwise
 *
 */
static bool
bufferobj_range_mapped(const struct gl_buffer_object *obj,
                       GLintptr offset, GLsizeiptr size)
{
   if (_mesa_bufferobj_mapped(obj, MAP_USER)) {
      const GLintptr end = offset + size;
      const GLintptr mapEnd = obj->Mappings[MAP_USER].Offset +
                              obj->Mappings[MAP_USER].Length;

      if (!(end <= obj->Mappings[MAP_USER].Offset || offset >= mapEnd)) {
         return true;
      }
   }
   return false;
}


/**
 * Tests the subdata range parameters and sets the GL error code for
 * \c glBufferSubDataARB, \c glGetBufferSubDataARB and
 * \c glClearBufferSubData.
 *
 * \param ctx     GL context.
 * \param bufObj  The buffer object.
 * \param offset  Offset of the first byte of the subdata range.
 * \param size    Size, in bytes, of the subdata range.
 * \param mappedRange  If true, checks if an overlapping range is mapped.
 *                     If false, checks if buffer is mapped.
 * \param caller  Name of calling function for recording errors.
 * \return   false if error, true otherwise
 *
 * \sa glBufferSubDataARB, glGetBufferSubDataARB, glClearBufferSubData
 */
static bool
buffer_object_subdata_range_good(struct gl_context *ctx,
                                 struct gl_buffer_object *bufObj,
                                 GLintptr offset, GLsizeiptr size,
                                 bool mappedRange, const char *caller)
{
   if (size < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(size < 0)", caller);
      return false;
   }

   if (offset < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(offset < 0)", caller);
      return false;
   }

   if (offset + size > bufObj->Size) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(offset %lu + size %lu > buffer size %lu)", caller,
                  (unsigned long) offset,
                  (unsigned long) size,
                  (unsigned long) bufObj->Size);
      return false;
   }

//   if (bufObj->Mappings[MAP_USER].AccessFlags & GL_MAP_PERSISTENT_BIT)
//      return true;

   if (mappedRange) {
      if (bufferobj_range_mapped(bufObj, offset, size)) {
         _mesa_error(ctx, GL_INVALID_OPERATION,
                     "%s(range is mapped without persistent bit)",
                     caller);
         return false;
      }
   }
   else {
      if (_mesa_bufferobj_mapped(bufObj, MAP_USER)) {
         _mesa_error(ctx, GL_INVALID_OPERATION,
                     "%s(buffer is mapped without persistent bit)",
                     caller);
         return false;
      }
   }

   return true;
}


/**
 * Test the format and type parameters and set the GL error code for
 * \c glClearBufferData and \c glClearBufferSubData.
 *
 * \param ctx             GL context.
 * \param internalformat  Format to which the data is to be converted.
 * \param format          Format of the supplied data.
 * \param type            Type of the supplied data.
 * \param caller          Name of calling function for recording errors.
 * \return   If internalformat, format and type are legal the mesa_format
 *           corresponding to internalformat, otherwise MESA_FORMAT_NONE.
 *
 * \sa glClearBufferData and glClearBufferSubData
 */
//static mesa_format
//validate_clear_buffer_format(struct gl_context *ctx,
//                             GLenum internalformat,
//                             GLenum format, GLenum type,
//                             const char *caller)
//{
//   mesa_format mesaFormat;
//   GLenum errorFormatType;
//
//   mesaFormat = _mesa_validate_texbuffer_format(ctx, internalformat);
//   if (mesaFormat == MESA_FORMAT_NONE) {
//      _mesa_error(ctx, GL_INVALID_ENUM,
//                  "%s(invalid internalformat)", caller);
//      return MESA_FORMAT_NONE;
//   }
//
//   /* NOTE: not mentioned in ARB_clear_buffer_object but according to
//    * EXT_texture_integer there is no conversion between integer and
//    * non-integer formats
//   */
//   if (_mesa_is_enum_format_signed_int(format) !=
//       _mesa_is_format_integer_color(mesaFormat)) {
//      _mesa_error(ctx, GL_INVALID_OPERATION,
//                  "%s(integer vs non-integer)", caller);
//      return MESA_FORMAT_NONE;
//   }
//
//   if (!_mesa_is_color_format(format)) {
//      _mesa_error(ctx, GL_INVALID_ENUM,
//                  "%s(format is not a color format)", caller);
//      return MESA_FORMAT_NONE;
//   }
//
//   errorFormatType = _mesa_error_check_format_and_type(ctx, format, type);
//   if (errorFormatType != GL_NO_ERROR) {
//      _mesa_error(ctx, GL_INVALID_ENUM,
//                  "%s(invalid format or type)", caller);
//      return MESA_FORMAT_NONE;
//   }
//
//   return mesaFormat;
//}


/**
 * Convert user-specified clear value to the specified internal format.
 *
 * \param ctx             GL context.
 * \param internalformat  Format to which the data is converted.
 * \param clearValue      Points to the converted clear value.
 * \param format          Format of the supplied data.
 * \param type            Type of the supplied data.
 * \param data            Data which is to be converted to internalformat.
 * \param caller          Name of calling function for recording errors.
 * \return   true if data could be converted, false otherwise.
 *
 * \sa glClearBufferData, glClearBufferSubData
 */
//static bool
//convert_clear_buffer_data(struct gl_context *ctx,
//                          mesa_format internalformat,
//                          GLubyte *clearValue, GLenum format, GLenum type,
//                          const GLvoid *data, const char *caller)
//{
//   GLenum internalformatBase = _mesa_get_format_base_format(internalformat);
//
//   if (_mesa_texstore(ctx, 1, internalformatBase, internalformat,
//                      0, &clearValue, 1, 1, 1,
//                      format, type, data, &ctx->Unpack)) {
//      return true;
//   }
//   else {
//      _mesa_error(ctx, GL_OUT_OF_MEMORY, "%s", caller);
//      return false;
//   }
//}


/**
 * Allocate and initialize a new buffer object.
 * 
 * Default callback for the \c dd_function_table::NewBufferObject() hook.
 */
static struct gl_buffer_object *
_mesa_new_buffer_object(struct gl_context *ctx, GLuint name)
{
   struct gl_buffer_object *obj;

   (void) ctx;

   obj = MALLOC_STRUCT(gl_buffer_object);
   _mesa_initialize_buffer_object(ctx, obj, name);
   return obj;
}


/**
 * Delete a buffer object.
 * 
 * Default callback for the \c dd_function_table::DeleteBuffer() hook.
 */
static void
_mesa_delete_buffer_object(struct gl_context *ctx,
                           struct gl_buffer_object *bufObj)
{
   (void) ctx;

   _mesa_align_free(bufObj->Data);

   /* assign strange values here to help w/ debugging */
   bufObj->RefCount = -1000;
   bufObj->Name = ~0;

//   mtx_destroy(&bufObj->Mutex);
   free(bufObj->Label);
   free(bufObj);
}



/**
 * Set ptr to bufObj w/ reference counting.
 * This is normally only called from the _mesa_reference_buffer_object() macro
 * when there's a real pointer change.
 */
void
_mesa_reference_buffer_object_(struct gl_context *ctx,
                               struct gl_buffer_object **ptr,
                               struct gl_buffer_object *bufObj)
{
   if (*ptr) {
      /* Unreference the old buffer */
      GLboolean deleteFlag = GL_FALSE;
      struct gl_buffer_object *oldObj = *ptr;

//      mtx_lock(&oldObj->Mutex);
      assert(oldObj->RefCount > 0);
      oldObj->RefCount--;
#if 0
      printf("BufferObj %p %d DECR to %d\n",
             (void *) oldObj, oldObj->Name, oldObj->RefCount);
#endif
      deleteFlag = (oldObj->RefCount == 0);
//      mtx_unlock(&oldObj->Mutex);

      if (deleteFlag) {

         /* some sanity checking: don't delete a buffer still in use */
#if 0
         /* unfortunately, these tests are invalid during context tear-down */
	 assert(ctx->Array.ArrayBufferObj != bufObj);
	 assert(ctx->Array.VAO->IndexBufferObj != bufObj);
	 assert(ctx->Array.VAO->Vertex.BufferObj != bufObj);
#endif

	 assert(ctx->Driver.DeleteBuffer);
         ctx->Driver.DeleteBuffer(ctx, oldObj);
      }

      *ptr = NULL;
   }
   assert(!*ptr);

   if (bufObj) {
      /* reference new buffer */
//      mtx_lock(&bufObj->Mutex);
      if (bufObj->RefCount == 0) {
         /* this buffer's being deleted (look just above) */
         /* Not sure this can every really happen.  Warn if it does. */
         _mesa_problem(NULL, "referencing deleted buffer object");
         *ptr = NULL;
      }
      else {
         bufObj->RefCount++;
#if 0
         printf("BufferObj %p %d INCR to %d\n",
                (void *) bufObj, bufObj->Name, bufObj->RefCount);
#endif
         *ptr = bufObj;
      }
//      mtx_unlock(&bufObj->Mutex);
   }
}


/**
 * Initialize a buffer object to default values.
 */
void
_mesa_initialize_buffer_object(struct gl_context *ctx,
                               struct gl_buffer_object *obj,
                               GLuint name)
{
   memset(obj, 0, sizeof(struct gl_buffer_object));
//   mtx_init(&obj->Mutex, mtx_plain);
   obj->RefCount = 1;
   obj->Name = name;
   obj->Usage = GL_STATIC_DRAW;
}



/**
 * Callback called from _mesa_HashWalk()
 */
static void
count_buffer_size(GLuint key, void *data, void *userData)
{
   const struct gl_buffer_object *bufObj =
      (const struct gl_buffer_object *) data;
   GLuint *total = (GLuint *) userData;

   *total = *total + bufObj->Size;
}


/**
 * Compute total size (in bytes) of all buffer objects for the given context.
 * For debugging purposes.
 */
GLuint
_mesa_total_buffer_object_memory(struct gl_context *ctx)
{
   GLuint total = 0;

   _mesa_HashWalk(ctx->Shared->BufferObjects, count_buffer_size, &total);

   return total;
}


/**
 * Allocate space for and store data in a buffer object.  Any data that was
 * previously stored in the buffer object is lost.  If \c data is \c NULL,
 * memory will be allocated, but no copy will occur.
 *
 * This is the default callback for \c dd_function_table::BufferData()
 * Note that all GL error checking will have been done already.
 *
 * \param ctx     GL context.
 * \param target  Buffer object target on which to operate.
 * \param size    Size, in bytes, of the new data store.
 * \param data    Pointer to the data to store in the buffer object.  This
 *                pointer may be \c NULL.
 * \param usage   Hints about how the data will be used.
 * \param bufObj  Object to be used.
 *
 * \return GL_TRUE for success, GL_FALSE for failure
 * \sa glBufferDataARB, dd_function_table::BufferData.
 */
static GLboolean
buffer_data_fallback(struct gl_context *ctx, GLenum target, GLsizeiptr size,
                     const GLvoid *data, GLenum usage, GLenum storageFlags,
                     struct gl_buffer_object *bufObj)
{
   void * new_data;

   (void) target;

   _mesa_align_free( bufObj->Data );

   new_data = _mesa_align_malloc( size, ctx->Const.MinMapBufferAlignment );
   if (new_data) {
      bufObj->Data = (GLubyte *) new_data;
      bufObj->Size = size;
      bufObj->Usage = usage;
      bufObj->StorageFlags = storageFlags;

      if (data) {
	 memcpy( bufObj->Data, data, size );
      }

      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
}


/**
 * Replace data in a subrange of buffer object.  If the data range
 * specified by \c size + \c offset extends beyond the end of the buffer or
 * if \c data is \c NULL, no copy is performed.
 *
 * This is the default callback for \c dd_function_table::BufferSubData()
 * Note that all GL error checking will have been done already.
 *
 * \param ctx     GL context.
 * \param offset  Offset of the first byte to be modified.
 * \param size    Size, in bytes, of the data range.
 * \param data    Pointer to the data to store in the buffer object.
 * \param bufObj  Object to be used.
 *
 * \sa glBufferSubDataARB, dd_function_table::BufferSubData.
 */
static void
buffer_sub_data_fallback(struct gl_context *ctx, GLintptr offset,
                         GLsizeiptr size, const GLvoid *data,
                         struct gl_buffer_object *bufObj)
{
   (void) ctx;

   /* this should have been caught in _mesa_BufferSubData() */
   assert(size + offset <= bufObj->Size);

   if (bufObj->Data) {
      memcpy( (GLubyte *) bufObj->Data + offset, data, size );
   }
}


/**
 * Retrieve data from a subrange of buffer object.  If the data range
 * specified by \c size + \c offset extends beyond the end of the buffer or
 * if \c data is \c NULL, no copy is performed.
 *
 * This is the default callback for \c dd_function_table::GetBufferSubData()
 * Note that all GL error checking will have been done already.
 *
 * \param ctx     GL context.
 * \param target  Buffer object target on which to operate.
 * \param offset  Offset of the first byte to be fetched.
 * \param size    Size, in bytes, of the data range.
 * \param data    Destination for data
 * \param bufObj  Object to be used.
 *
 * \sa glBufferGetSubDataARB, dd_function_table::GetBufferSubData.
 */
static void
_mesa_buffer_get_subdata( struct gl_context *ctx, GLintptr offset,
			  GLsizeiptr size, GLvoid * data,
			  struct gl_buffer_object * bufObj )
{
   (void) ctx;

   if (bufObj->Data && ((GLsizeiptr) (size + offset) <= bufObj->Size)) {
      memcpy( data, (GLubyte *) bufObj->Data + offset, size );
   }
}


/**
 * Clear a subrange of the buffer object with copies of the supplied data.
 * If data is NULL the buffer is filled with zeros.
 *
 * This is the default callback for \c dd_function_table::ClearBufferSubData()
 * Note that all GL error checking will have been done already.
 *
 * \param ctx             GL context.
 * \param offset          Offset of the first byte to be cleared.
 * \param size            Size, in bytes, of the to be cleared range.
 * \param clearValue      Source of the data.
 * \param clearValueSize  Size, in bytes, of the supplied data.
 * \param bufObj          Object to be cleared.
 *
 * \sa glClearBufferSubData, glClearBufferData and
 * dd_function_table::ClearBufferSubData.
 */
void
_mesa_ClearBufferSubData_sw(struct gl_context *ctx,
                            GLintptr offset, GLsizeiptr size,
                            const GLvoid *clearValue,
                            GLsizeiptr clearValueSize,
                            struct gl_buffer_object *bufObj)
{
   GLsizeiptr i;
   GLubyte *dest;

   assert(ctx->Driver.MapBufferRange);
   dest = ctx->Driver.MapBufferRange(ctx, offset, size,
                                     GL_MAP_WRITE_BIT |
                                     GL_MAP_INVALIDATE_RANGE_BIT,
                                     bufObj, MAP_INTERNAL);

   if (!dest) {
      _mesa_error(ctx, GL_OUT_OF_MEMORY, "glClearBuffer[Sub]Data");
      return;
   }

   if (clearValue == NULL) {
      /* Clear with zeros, per the spec */
      memset(dest, 0, size);
      ctx->Driver.UnmapBuffer(ctx, bufObj, MAP_INTERNAL);
      return;
   }

   for (i = 0; i < size/clearValueSize; ++i) {
      memcpy(dest, clearValue, clearValueSize);
      dest += clearValueSize;
   }

   ctx->Driver.UnmapBuffer(ctx, bufObj, MAP_INTERNAL);
}


/**
 * Default fallback for \c dd_function_table::MapBufferRange().
 * Called via glMapBufferRange().
 */
static void *
map_buffer_range_fallback(struct gl_context *ctx, GLintptr offset,
                          GLsizeiptr length, GLbitfield access,
                          struct gl_buffer_object *bufObj,
                          gl_map_buffer_index index)
{
   (void) ctx;
   assert(!_mesa_bufferobj_mapped(bufObj, index));
   /* Just return a direct pointer to the data */
   bufObj->Mappings[index].Pointer = bufObj->Data + offset;
   bufObj->Mappings[index].Length = length;
   bufObj->Mappings[index].Offset = offset;
   bufObj->Mappings[index].AccessFlags = access;
   return bufObj->Mappings[index].Pointer;
}


/**
 * Default fallback for \c dd_function_table::FlushMappedBufferRange().
 * Called via glFlushMappedBufferRange().
 */
static void
flush_mapped_buffer_range_fallback(struct gl_context *ctx,
                                   GLintptr offset, GLsizeiptr length,
                                   struct gl_buffer_object *obj,
                                   gl_map_buffer_index index)
{
   (void) ctx;
   (void) offset;
   (void) length;
   (void) obj;
   /* no-op */
}


/**
 * Default callback for \c dd_function_table::UnmapBuffer().
 *
 * The input parameters will have been already tested for errors.
 *
 * \sa glUnmapBufferARB, dd_function_table::UnmapBuffer
 */
static GLboolean
unmap_buffer_fallback(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                      gl_map_buffer_index index)
{
   (void) ctx;
   /* XXX we might assert here that bufObj->Pointer is non-null */
   bufObj->Mappings[index].Pointer = NULL;
   bufObj->Mappings[index].Length = 0;
   bufObj->Mappings[index].Offset = 0;
   bufObj->Mappings[index].AccessFlags = 0x0;
   return GL_TRUE;
}


/**
 * Default fallback for \c dd_function_table::CopyBufferSubData().
 * Called via glCopyBufferSubData().
 */
static void
copy_buffer_sub_data_fallback(struct gl_context *ctx,
                              struct gl_buffer_object *src,
                              struct gl_buffer_object *dst,
                              GLintptr readOffset, GLintptr writeOffset,
                              GLsizeiptr size)
{
   GLubyte *srcPtr, *dstPtr;

   if (src == dst) {
      srcPtr = dstPtr = ctx->Driver.MapBufferRange(ctx, 0, src->Size,
						   GL_MAP_READ_BIT |
						   GL_MAP_WRITE_BIT, src,
                                                   MAP_INTERNAL);

      if (!srcPtr)
	 return;

      srcPtr += readOffset;
      dstPtr += writeOffset;
   } else {
      srcPtr = ctx->Driver.MapBufferRange(ctx, readOffset, size,
					  GL_MAP_READ_BIT, src,
                                          MAP_INTERNAL);
      dstPtr = ctx->Driver.MapBufferRange(ctx, writeOffset, size,
					  (GL_MAP_WRITE_BIT |
					   GL_MAP_INVALIDATE_RANGE_BIT), dst,
                                          MAP_INTERNAL);
   }

   /* Note: the src and dst regions will never overlap.  Trying to do so
    * would generate GL_INVALID_VALUE earlier.
    */
   if (srcPtr && dstPtr)
      memcpy(dstPtr, srcPtr, size);

   ctx->Driver.UnmapBuffer(ctx, src, MAP_INTERNAL);
   if (dst != src)
      ctx->Driver.UnmapBuffer(ctx, dst, MAP_INTERNAL);
}



/**
 * Initialize the state associated with buffer objects
 */
void
_mesa_init_buffer_objects( struct gl_context *ctx )
{
   GLuint i;

   memset(&DummyBufferObject, 0, sizeof(DummyBufferObject));
//   mtx_init(&DummyBufferObject.Mutex, mtx_plain);
   DummyBufferObject.RefCount = 1000*1000*1000; /* never delete */

   _mesa_reference_buffer_object(ctx, &ctx->Array.ArrayBufferObj,
                                 ctx->Shared->NullBufferObj);

//   _mesa_reference_buffer_object(ctx, &ctx->CopyReadBuffer,
//                                 ctx->Shared->NullBufferObj);
//   _mesa_reference_buffer_object(ctx, &ctx->CopyWriteBuffer,
//                                 ctx->Shared->NullBufferObj);
//
//   _mesa_reference_buffer_object(ctx, &ctx->UniformBuffer,
//				 ctx->Shared->NullBufferObj);
//
//   _mesa_reference_buffer_object(ctx, &ctx->AtomicBuffer,
//				 ctx->Shared->NullBufferObj);
//
//   _mesa_reference_buffer_object(ctx, &ctx->DrawIndirectBuffer,
//				 ctx->Shared->NullBufferObj);
//
//   for (i = 0; i < MAX_COMBINED_UNIFORM_BUFFERS; i++) {
//      _mesa_reference_buffer_object(ctx,
//				    &ctx->UniformBufferBindings[i].BufferObject,
//				    ctx->Shared->NullBufferObj);
//      ctx->UniformBufferBindings[i].Offset = -1;
//      ctx->UniformBufferBindings[i].Size = -1;
//   }
//
//   for (i = 0; i < MAX_COMBINED_ATOMIC_BUFFERS; i++) {
//      _mesa_reference_buffer_object(ctx,
//				    &ctx->AtomicBufferBindings[i].BufferObject,
//				    ctx->Shared->NullBufferObj);
//      ctx->AtomicBufferBindings[i].Offset = -1;
//      ctx->AtomicBufferBindings[i].Size = -1;
//   }
}


void
_mesa_free_buffer_objects( struct gl_context *ctx )
{
   GLuint i;

   _mesa_reference_buffer_object(ctx, &ctx->Array.ArrayBufferObj, NULL);

//   _mesa_reference_buffer_object(ctx, &ctx->CopyReadBuffer, NULL);
//   _mesa_reference_buffer_object(ctx, &ctx->CopyWriteBuffer, NULL);
//
//   _mesa_reference_buffer_object(ctx, &ctx->UniformBuffer, NULL);
//
//   _mesa_reference_buffer_object(ctx, &ctx->AtomicBuffer, NULL);
//
//   _mesa_reference_buffer_object(ctx, &ctx->DrawIndirectBuffer, NULL);
//
//   for (i = 0; i < MAX_COMBINED_UNIFORM_BUFFERS; i++) {
//      _mesa_reference_buffer_object(ctx,
//				    &ctx->UniformBufferBindings[i].BufferObject,
//				    NULL);
//   }
//
//   for (i = 0; i < MAX_COMBINED_ATOMIC_BUFFERS; i++) {
//      _mesa_reference_buffer_object(ctx,
//				    &ctx->AtomicBufferBindings[i].BufferObject,
//				    NULL);
//   }

}

bool
_mesa_handle_bind_buffer_gen(struct gl_context *ctx,
                             GLenum target,
                             GLuint buffer,
                             struct gl_buffer_object **buf_handle,
                             const char *caller)
{
   struct gl_buffer_object *buf = *buf_handle;

   if (!buf || buf == &DummyBufferObject) {
      /* If this is a new buffer object id, or one which was generated but
       * never used before, allocate a buffer object now.
       */
      assert(ctx->Driver.NewBufferObject);
      buf = ctx->Driver.NewBufferObject(ctx, buffer);
      if (!buf) {
	 _mesa_error(ctx, GL_OUT_OF_MEMORY, "%s", caller);
	 return false;
      }
      _mesa_HashInsert(ctx->Shared->BufferObjects, buffer, buf);
      *buf_handle = buf;
   }

   return true;
}

/**
 * Bind the specified target to buffer for the specified context.
 * Called by glBindBuffer() and other functions.
 */
static void
bind_buffer_object(struct gl_context *ctx, GLenum target, GLuint buffer)
{
   struct gl_buffer_object *oldBufObj;
   struct gl_buffer_object *newBufObj = NULL;
   struct gl_buffer_object **bindTarget = NULL;

   bindTarget = get_buffer_target(ctx, target);
   if (!bindTarget) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glBindBufferARB(target 0x%x)", target);
      return;
   }

   /* Get pointer to old buffer object (to be unbound) */
   oldBufObj = *bindTarget;
   if (oldBufObj && oldBufObj->Name == buffer && !oldBufObj->DeletePending)
      return;   /* rebinding the same buffer object- no change */

   /*
    * Get pointer to new buffer object (newBufObj)
    */
   if (buffer == 0) {
      /* The spec says there's not a buffer object named 0, but we use
       * one internally because it simplifies things.
       */
      newBufObj = ctx->Shared->NullBufferObj;
   }
   else {
      /* non-default buffer object */
      newBufObj = _mesa_lookup_bufferobj(ctx, buffer);
      if (!_mesa_handle_bind_buffer_gen(ctx, target, buffer,
                                        &newBufObj, "glBindBuffer"))
         return;
   }
   
   /* bind new buffer */
   _mesa_reference_buffer_object(ctx, bindTarget, newBufObj);
}


/**
 * Update the default buffer objects in the given context to reference those
 * specified in the shared state and release those referencing the old 
 * shared state.
 */
void
_mesa_update_default_objects_buffer_objects(struct gl_context *ctx)
{
   /* Bind the NullBufferObj to remove references to those
    * in the shared context hash table.
    */
   bind_buffer_object( ctx, GL_ARRAY_BUFFER, 0);
   bind_buffer_object( ctx, GL_ELEMENT_ARRAY_BUFFER, 0);
//   bind_buffer_object( ctx, GL_PIXEL_PACK_BUFFER, 0);
//   bind_buffer_object( ctx, GL_PIXEL_UNPACK_BUFFER, 0);
}



/**
 * Return the gl_buffer_object for the given ID.
 * Always return NULL for ID 0.
 */
struct gl_buffer_object *
_mesa_lookup_bufferobj(struct gl_context *ctx, GLuint buffer)
{
   if (buffer == 0)
      return NULL;
   else
      return (struct gl_buffer_object *)
         _mesa_HashLookup(ctx->Shared->BufferObjects, buffer);
}


struct gl_buffer_object *
_mesa_lookup_bufferobj_locked(struct gl_context *ctx, GLuint buffer)
{
   return (struct gl_buffer_object *)
      _mesa_HashLookupLocked(ctx->Shared->BufferObjects, buffer);
}

/**
 * A convenience function for direct state access functions that throws
 * GL_INVALID_OPERATION if buffer is not the name of an existing
 * buffer object.
 */
struct gl_buffer_object *
_mesa_lookup_bufferobj_err(struct gl_context *ctx, GLuint buffer,
                           const char *caller)
{
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj(ctx, buffer);
   if (!bufObj || bufObj == &DummyBufferObject) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(non-existent buffer object %u)", caller, buffer);
      return NULL;
   }

   return bufObj;
}


void
_mesa_begin_bufferobj_lookups(struct gl_context *ctx)
{
   _mesa_HashLockMutex(ctx->Shared->BufferObjects);
}


void
_mesa_end_bufferobj_lookups(struct gl_context *ctx)
{
   _mesa_HashUnlockMutex(ctx->Shared->BufferObjects);
}


/**
 * Look up a buffer object for a multi-bind function.
 *
 * Unlike _mesa_lookup_bufferobj(), this function also takes care
 * of generating an error if the buffer ID is not zero or the name
 * of an existing buffer object.
 *
 * If the buffer ID refers to an existing buffer object, a pointer
 * to the buffer object is returned.  If the ID is zero, a pointer
 * to the shared NullBufferObj is returned.  If the ID is not zero
 * and does not refer to a valid buffer object, this function
 * returns NULL.
 *
 * This function assumes that the caller has already locked the
 * hash table mutex by calling _mesa_begin_bufferobj_lookups().
 */
struct gl_buffer_object *
_mesa_multi_bind_lookup_bufferobj(struct gl_context *ctx,
                                  const GLuint *buffers,
                                  GLuint index, const char *caller)
{
   struct gl_buffer_object *bufObj;

   if (buffers[index] != 0) {
      bufObj = _mesa_lookup_bufferobj_locked(ctx, buffers[index]);

      /* The multi-bind functions don't create the buffer objects
         when they don't exist. */
      if (bufObj == &DummyBufferObject)
         bufObj = NULL;
   } else
      bufObj = ctx->Shared->NullBufferObj;

   if (!bufObj) {
      /* The ARB_multi_bind spec says:
       *
       *    "An INVALID_OPERATION error is generated if any value
       *     in <buffers> is not zero or the name of an existing
       *     buffer object (per binding)."
       */
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(buffers[%u]=%u is not zero or the name "
                  "of an existing buffer object)",
                  caller, index, buffers[index]);
   }

   return bufObj;
}


/**
 * If *ptr points to obj, set ptr = the Null/default buffer object.
 * This is a helper for buffer object deletion.
 * The GL spec says that deleting a buffer object causes it to get
 * unbound from all arrays in the current context.
 */
static void
unbind(struct gl_context *ctx,
       struct gl_buffer_object **ptr,
       struct gl_buffer_object *obj)
{
   if (*ptr == obj) {
      _mesa_reference_buffer_object(ctx, ptr, ctx->Shared->NullBufferObj);
   }
}


/**
 * Plug default/fallback buffer object functions into the device
 * driver hooks.
 */
void
_mesa_init_buffer_object_functions(struct dd_function_table *driver)
{
   /* GL_ARB_vertex/pixel_buffer_object */
   driver->NewBufferObject = _mesa_new_buffer_object;
   driver->DeleteBuffer = _mesa_delete_buffer_object;
   driver->BufferData = buffer_data_fallback;
   driver->BufferSubData = buffer_sub_data_fallback;
   driver->GetBufferSubData = _mesa_buffer_get_subdata;
   driver->UnmapBuffer = unmap_buffer_fallback;

   /* GL_ARB_clear_buffer_object */
   driver->ClearBufferSubData = _mesa_ClearBufferSubData_sw;

   /* GL_ARB_map_buffer_range */
   driver->MapBufferRange = map_buffer_range_fallback;
   driver->FlushMappedBufferRange = flush_mapped_buffer_range_fallback;

   /* GL_ARB_copy_buffer */
   driver->CopyBufferSubData = copy_buffer_sub_data_fallback;
}


void
_mesa_buffer_unmap_all_mappings(struct gl_context *ctx,
                                struct gl_buffer_object *bufObj)
{
   int i;

   for (i = 0; i < MAP_COUNT; i++) {
      if (_mesa_bufferobj_mapped(bufObj, i)) {
         ctx->Driver.UnmapBuffer(ctx, bufObj, i);
         assert(bufObj->Mappings[i].Pointer == NULL);
         bufObj->Mappings[i].AccessFlags = 0;
      }
   }
}


/**********************************************************************/
/* API Functions                                                      */
/**********************************************************************/

void glBindBuffer(GLenum target, GLuint buffer)
{
   GET_CURRENT_CONTEXT(ctx);

//   if (MESA_VERBOSE & VERBOSE_API)
//      _mesa_debug(ctx, "glBindBuffer(%s, %u)\n",
//                  _mesa_lookup_enum_by_nr(target), buffer);

   bind_buffer_object(ctx, target, buffer);
}


/**
 * Delete a set of buffer objects.
 * 
 * \param n      Number of buffer objects to delete.
 * \param ids    Array of \c n buffer object IDs.
 */
void glDeleteBuffers(GLsizei n, const GLuint *ids)
{
   GET_CURRENT_CONTEXT(ctx);
   GLsizei i;
   FLUSH_VERTICES(ctx, 0);

   if (n < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glDeleteBuffersARB(n)");
      return;
   }

//   mtx_lock(&ctx->Shared->Mutex);

   for (i = 0; i < n; i++) {
      struct gl_buffer_object *bufObj = _mesa_lookup_bufferobj(ctx, ids[i]);
      if (bufObj) {
         struct gl_vertex_array_object *vao = ctx->Array.VAO;
         GLuint j;

         assert(bufObj->Name == ids[i] || bufObj == &DummyBufferObject);

         _mesa_buffer_unmap_all_mappings(ctx, bufObj);

         /* unbind any vertex pointers bound to this buffer */
         for (j = 0; j < ARRAY_SIZE(vao->VertexBinding); j++) {
            unbind(ctx, &vao->VertexBinding[j].BufferObj, bufObj);
         }

         if (ctx->Array.ArrayBufferObj == bufObj) {
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
         }
         if (vao->IndexBufferObj == bufObj) {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
         }

         /* The ID is immediately freed for re-use */
         _mesa_HashRemove(ctx->Shared->BufferObjects, ids[i]);
         /* Make sure we do not run into the classic ABA problem on bind.
          * We don't want to allow re-binding a buffer object that's been
          * "deleted" by glDeleteBuffers().
          *
          * The explicit rebinding to the default object in the current context
          * prevents the above in the current context, but another context
          * sharing the same objects might suffer from this problem.
          * The alternative would be to do the hash lookup in any case on bind
          * which would introduce more runtime overhead than this.
          */
         bufObj->DeletePending = GL_TRUE;
         _mesa_reference_buffer_object(ctx, &bufObj, NULL);
      }
   }

//   mtx_unlock(&ctx->Shared->Mutex);
}


/**
 * This is the implementation for glGenBuffers and glCreateBuffers. It is not
 * exposed to the rest of Mesa to encourage the use of nameless buffers in
 * driver internals.
 */
static void
create_buffers(GLsizei n, GLuint *buffers, bool dsa)
{
   GET_CURRENT_CONTEXT(ctx);
   GLuint first;
   GLint i;
   struct gl_buffer_object *buf;

   const char *func = dsa ? "glCreateBuffers" : "glGenBuffers";

//   if (MESA_VERBOSE & VERBOSE_API)
//      _mesa_debug(ctx, "%s(%d)\n", func, n);

   if (n < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(n %d < 0)", func, n);
      return;
   }

   if (!buffers) {
      return;
   }

   /*
    * This must be atomic (generation and allocation of buffer object IDs)
    */
//   mtx_lock(&ctx->Shared->Mutex);

   first = _mesa_HashFindFreeKeyBlock(ctx->Shared->BufferObjects, n);

   /* Insert the ID and pointer into the hash table. If non-DSA, insert a
    * DummyBufferObject.  Otherwise, create a new buffer object and insert
    * it.
    */
   for (i = 0; i < n; i++) {
      buffers[i] = first + i;
      if (dsa) {
         assert(ctx->Driver.NewBufferObject);
         buf = ctx->Driver.NewBufferObject(ctx, buffers[i]);
         if (!buf) {
            _mesa_error(ctx, GL_OUT_OF_MEMORY, "%s", func);
//            mtx_unlock(&ctx->Shared->Mutex);
            return;
         }
      }
      else
         buf = &DummyBufferObject;

      _mesa_HashInsert(ctx->Shared->BufferObjects, buffers[i], buf);
   }

//   mtx_unlock(&ctx->Shared->Mutex);
}

/**
 * Generate a set of unique buffer object IDs and store them in \c buffers.
 *
 * \param n        Number of IDs to generate.
 * \param buffers  Array of \c n locations to store the IDs.
 */
void glGenBuffers(GLsizei n, GLuint *buffers)
{
   create_buffers(n, buffers, false);
}

/**
 * Create a set of buffer objects and store their unique IDs in \c buffers.
 *
 * \param n        Number of IDs to generate.
 * \param buffers  Array of \c n locations to store the IDs.
 */
void glCreateBuffers(GLsizei n, GLuint *buffers)
{
   create_buffers(n, buffers, true);
}


/**
 * Determine if ID is the name of a buffer object.
 * 
 * \param id  ID of the potential buffer object.
 * \return  \c GL_TRUE if \c id is the name of a buffer object, 
 *          \c GL_FALSE otherwise.
 */
GLboolean glIsBuffer(GLuint id)
{
   struct gl_buffer_object *bufObj;
   GET_CURRENT_CONTEXT(ctx);
//   ASSERT_OUTSIDE_BEGIN_END_WITH_RETVAL(ctx, GL_FALSE);

//   mtx_lock(&ctx->Shared->Mutex);
   bufObj = _mesa_lookup_bufferobj(ctx, id);
//   mtx_unlock(&ctx->Shared->Mutex);

   return bufObj && bufObj != &DummyBufferObject;
}


void
_mesa_buffer_storage(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                     GLenum target, GLsizeiptr size, const GLvoid *data,
                     GLbitfield flags, const char *func)
{
   if (size <= 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(size <= 0)", func);
      return;
   }

   if (flags & ~(GL_MAP_READ_BIT |
                 GL_MAP_WRITE_BIT |
//                 GL_MAP_PERSISTENT_BIT |
//                 GL_MAP_COHERENT_BIT |
                 GL_DYNAMIC_STORAGE_BIT |
                 GL_CLIENT_STORAGE_BIT)) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(invalid flag bits set)", func);
      return;
   }

//   if (flags & GL_MAP_PERSISTENT_BIT &&
//       !(flags & (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT))) {
//      _mesa_error(ctx, GL_INVALID_VALUE,
//                  "%s(PERSISTENT and flags!=READ/WRITE)", func);
//      return;
//   }
//
//   if (flags & GL_MAP_COHERENT_BIT && !(flags & GL_MAP_PERSISTENT_BIT)) {
//      _mesa_error(ctx, GL_INVALID_VALUE,
//                  "%s(COHERENT and flags!=PERSISTENT)", func);
//      return;
//   }

   if (bufObj->Immutable) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "%s(immutable)", func);
      return;
   }

   /* Unmap the existing buffer.  We'll replace it now.  Not an error. */
   _mesa_buffer_unmap_all_mappings(ctx, bufObj);

   FLUSH_VERTICES(ctx, _NEW_BUFFER_OBJECT);

   bufObj->Written = GL_TRUE;
   bufObj->Immutable = GL_TRUE;

   assert(ctx->Driver.BufferData);
   if (!ctx->Driver.BufferData(ctx, target, size, data, GL_DYNAMIC_DRAW,
                               flags, bufObj)) {
	 _mesa_error(ctx, GL_OUT_OF_MEMORY, "%s", func);
   }
}

void glBufferStorage(GLenum target, GLsizeiptr size, const GLvoid *data,
                    GLbitfield flags)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glBufferStorage", target, GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   _mesa_buffer_storage(ctx, bufObj, target, size, data, flags,
                        "glBufferStorage");
}

void glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const GLvoid *data,
                         GLbitfield flags)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer, "glNamedBufferStorage");
   if (!bufObj)
      return;

   /*
    * In direct state access, buffer objects have an unspecified target since
    * they are not required to be bound.
    */
   _mesa_buffer_storage(ctx, bufObj, GL_NONE, size, data, flags,
                        "glNamedBufferStorage");
}


void
_mesa_buffer_data(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                  GLenum target, GLsizeiptr size, const GLvoid *data,
                  GLenum usage, const char *func)
{
   bool valid_usage;

//   if (MESA_VERBOSE & VERBOSE_API)
//      _mesa_debug(ctx, "%s(%s, %ld, %p, %s)\n",
//                  func,
//                  _mesa_lookup_enum_by_nr(target),
//                  (long int) size, data,
//                  _mesa_lookup_enum_by_nr(usage));

   if (size < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(size < 0)", func);
      return;
   }

   switch (usage) {
   case GL_STREAM_DRAW:
      valid_usage = false; //(ctx->API != API_OPENGLES);
      break;

   case GL_STATIC_DRAW:
   case GL_DYNAMIC_DRAW:
      valid_usage = true;
      break;

   case GL_STREAM_READ:
   case GL_STREAM_COPY:
   case GL_STATIC_READ:
   case GL_STATIC_COPY:
   case GL_DYNAMIC_READ:
   case GL_DYNAMIC_COPY:
      valid_usage = false; //_mesa_is_desktop_gl(ctx) || _mesa_is_gles3(ctx);
      break;

   default:
      valid_usage = false;
      break;
   }

   if (!valid_usage) {
      _mesa_error(ctx, GL_INVALID_ENUM, "%s(invalid usage: %s)", func,
                  ""); //_mesa_lookup_enum_by_nr(usage));
      return;
   }

   if (bufObj->Immutable) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "%s(immutable)", func);
      return;
   }

   /* Unmap the existing buffer.  We'll replace it now.  Not an error. */
   _mesa_buffer_unmap_all_mappings(ctx, bufObj);

   FLUSH_VERTICES(ctx, _NEW_BUFFER_OBJECT);

   bufObj->Written = GL_TRUE;

#ifdef VBO_DEBUG
   printf("glBufferDataARB(%u, sz %ld, from %p, usage 0x%x)\n",
                bufObj->Name, size, data, usage);
#endif

#ifdef BOUNDS_CHECK
   size += 100;
#endif

   assert(ctx->Driver.BufferData);
   if (!ctx->Driver.BufferData(ctx, target, size, data, usage,
                               GL_MAP_READ_BIT |
                               GL_MAP_WRITE_BIT |
                               GL_DYNAMIC_STORAGE_BIT,
                               bufObj)) {
         _mesa_error(ctx, GL_OUT_OF_MEMORY, "%s", func);
   }
}

void glBufferData(GLenum target, GLsizeiptr size,
                 const GLvoid *data, GLenum usage)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glBufferData", target, GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   _mesa_buffer_data(ctx, bufObj, target, size, data, usage,
                     "glBufferData");
}

void glNamedBufferData(GLuint buffer, GLsizeiptr size, const GLvoid *data,
                      GLenum usage)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer, "glNamedBufferData");
   if (!bufObj)
      return;

   /* In direct state access, buffer objects have an unspecified target since
    * they are not required to be bound.
    */
   _mesa_buffer_data(ctx, bufObj, GL_NONE, size, data, usage,
                     "glNamedBufferData");
}


/**
 * Implementation for glBufferSubData and glNamedBufferSubData.
 *
 * \param ctx     GL context.
 * \param bufObj  The buffer object.
 * \param offset  Offset of the first byte of the subdata range.
 * \param size    Size, in bytes, of the subdata range.
 * \param data    The data store.
 * \param func  Name of calling function for recording errors.
 *
 */
void
_mesa_buffer_sub_data(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                      GLintptr offset, GLsizeiptr size, const GLvoid *data,
                      const char *func)
{
   if (!buffer_object_subdata_range_good(ctx, bufObj, offset, size,
                                         false, func)) {
      /* error already recorded */
      return;
   }

   if (bufObj->Immutable &&
       !(bufObj->StorageFlags & GL_DYNAMIC_STORAGE_BIT)) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "%s", func);
      return;
   }

   if (size == 0)
      return;

   bufObj->Written = GL_TRUE;

   assert(ctx->Driver.BufferSubData);
   ctx->Driver.BufferSubData(ctx, offset, size, data, bufObj);
}

void glBufferSubData(GLenum target, GLintptr offset,
                    GLsizeiptr size, const GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glBufferSubData", target, GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   _mesa_buffer_sub_data(ctx, bufObj, offset, size, data, "glBufferSubData");
}

void glNamedBufferSubData(GLuint buffer, GLintptr offset,
                         GLsizeiptr size, const GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer, "glNamedBufferSubData");
   if (!bufObj)
      return;

   _mesa_buffer_sub_data(ctx, bufObj, offset, size, data,
                         "glNamedBufferSubData");
}


void glGetBufferSubData(GLenum target, GLintptr offset,
                       GLsizeiptr size, GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glGetBufferSubData", target,
                       GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   if (!buffer_object_subdata_range_good(ctx, bufObj, offset, size, false,
                                         "glGetBufferSubData")) {
      return;
   }

   assert(ctx->Driver.GetBufferSubData);
   ctx->Driver.GetBufferSubData(ctx, offset, size, data, bufObj);
}

void glGetNamedBufferSubData(GLuint buffer, GLintptr offset,
                            GLsizeiptr size, GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer,
                                       "glGetNamedBufferSubData");
   if (!bufObj)
      return;

   if (!buffer_object_subdata_range_good(ctx, bufObj, offset, size, false,
                                         "glGetNamedBufferSubData")) {
      return;
   }

   assert(ctx->Driver.GetBufferSubData);
   ctx->Driver.GetBufferSubData(ctx, offset, size, data, bufObj);
}


/**
 * \param subdata   true if caller is *SubData, false if *Data
 */
void
_mesa_clear_buffer_sub_data(struct gl_context *ctx,
                            struct gl_buffer_object *bufObj,
                            GLenum internalformat,
                            GLintptr offset, GLsizeiptr size,
                            GLenum format, GLenum type,
                            const GLvoid *data,
                            const char *func, bool subdata)
{
//   mesa_format mesaFormat;
//   GLubyte clearValue[MAX_PIXEL_BYTES];
   GLsizeiptr clearValueSize;

   /* This checks for disallowed mappings. */
   if (!buffer_object_subdata_range_good(ctx, bufObj, offset, size,
                                         subdata, func)) {
      return;
   }

//   mesaFormat = validate_clear_buffer_format(ctx, internalformat,
//                                             format, type, func);

//   if (mesaFormat == MESA_FORMAT_NONE) {
//      return;
//   }

//   clearValueSize = _mesa_get_format_bytes(mesaFormat);
   if (offset % clearValueSize != 0 || size % clearValueSize != 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(offset or size is not a multiple of "
                  "internalformat size)", func);
      return;
   }

   if (data == NULL) {
      /* clear to zeros, per the spec */
      if (size > 0) {
         ctx->Driver.ClearBufferSubData(ctx, offset, size,
                                        NULL, clearValueSize, bufObj);
      }
      return;
   }

//   if (!convert_clear_buffer_data(ctx, mesaFormat, clearValue,
//                                  format, type, data, func)) {
//      return;
//   }

//   if (size > 0) {
//      ctx->Driver.ClearBufferSubData(ctx, offset, size,
//                                     clearValue, clearValueSize, bufObj);
//   }
}

void glClearBufferData(GLenum target, GLenum internalformat, GLenum format,
                      GLenum type, const GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glClearBufferData", target, GL_INVALID_VALUE);
   if (!bufObj)
      return;

   _mesa_clear_buffer_sub_data(ctx, bufObj, internalformat, 0, bufObj->Size,
                               format, type, data,
                               "glClearBufferData", false);
}

void glClearNamedBufferData(GLuint buffer, GLenum internalformat,
                           GLenum format, GLenum type, const GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer, "glClearNamedBufferData");
   if (!bufObj)
      return;

   _mesa_clear_buffer_sub_data(ctx, bufObj, internalformat, 0, bufObj->Size,
                               format, type, data,
                               "glClearNamedBufferData", false);
}


void glClearBufferSubData(GLenum target, GLenum internalformat,
                         GLintptr offset, GLsizeiptr size,
                         GLenum format, GLenum type,
                         const GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glClearBufferSubData", target, GL_INVALID_VALUE);
   if (!bufObj)
      return;

   _mesa_clear_buffer_sub_data(ctx, bufObj, internalformat, offset, size,
                               format, type, data,
                               "glClearBufferSubData", true);
}

void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat,
                              GLintptr offset, GLsizeiptr size,
                              GLenum format, GLenum type,
                              const GLvoid *data)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer,
                                       "glClearNamedBufferSubData");
   if (!bufObj)
      return;

   _mesa_clear_buffer_sub_data(ctx, bufObj, internalformat, offset, size,
                               format, type, data,
                               "glClearNamedBufferSubData", true);
}


GLboolean
_mesa_unmap_buffer(struct gl_context *ctx, struct gl_buffer_object *bufObj,
                   const char *func)
{
   GLboolean status = GL_TRUE;
//   ASSERT_OUTSIDE_BEGIN_END_WITH_RETVAL(ctx, GL_FALSE);

   if (!_mesa_bufferobj_mapped(bufObj, MAP_USER)) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(buffer is not mapped)", func);
      return GL_FALSE;
   }

#ifdef BOUNDS_CHECK
   if (bufObj->Access != GL_READ_ONLY) {
      GLubyte *buf = (GLubyte *) bufObj->Pointer;
      GLuint i;
      /* check that last 100 bytes are still = magic value */
      for (i = 0; i < 100; i++) {
         GLuint pos = bufObj->Size - i - 1;
         if (buf[pos] != 123) {
            _mesa_warning(ctx, "Out of bounds buffer object write detected"
                          " at position %d (value = %u)\n",
                          pos, buf[pos]);
         }
      }
   }
#endif

#ifdef VBO_DEBUG
   if (bufObj->AccessFlags & GL_MAP_WRITE_BIT) {
      GLuint i, unchanged = 0;
      GLubyte *b = (GLubyte *) bufObj->Pointer;
      GLint pos = -1;
      /* check which bytes changed */
      for (i = 0; i < bufObj->Size - 1; i++) {
         if (b[i] == (i & 0xff) && b[i+1] == ((i+1) & 0xff)) {
            unchanged++;
            if (pos == -1)
               pos = i;
         }
      }
      if (unchanged) {
         printf("glUnmapBufferARB(%u): %u of %ld unchanged, starting at %d\n",
                      bufObj->Name, unchanged, bufObj->Size, pos);
      }
   }
#endif

   status = ctx->Driver.UnmapBuffer(ctx, bufObj, MAP_USER);
   bufObj->Mappings[MAP_USER].AccessFlags = 0;
   assert(bufObj->Mappings[MAP_USER].Pointer == NULL);
   assert(bufObj->Mappings[MAP_USER].Offset == 0);
   assert(bufObj->Mappings[MAP_USER].Length == 0);

   return status;
}

GLboolean glUnmapBuffer(GLenum target)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glUnmapBuffer", target, GL_INVALID_OPERATION);
   if (!bufObj)
      return GL_FALSE;

   return _mesa_unmap_buffer(ctx, bufObj, "glUnmapBuffer");
}

GLboolean glUnmapNamedBuffer(GLuint buffer)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer, "glUnmapNamedBuffer");
   if (!bufObj)
      return GL_FALSE;

   return _mesa_unmap_buffer(ctx, bufObj, "glUnmapNamedBuffer");
}


static bool
get_buffer_parameter(struct gl_context *ctx,
                     struct gl_buffer_object *bufObj, GLenum pname,
                     GLint64 *params, const char *func)
{
   switch (pname) {
   case GL_BUFFER_SIZE:
      *params = bufObj->Size;
      break;
   case GL_BUFFER_USAGE:
      *params = bufObj->Usage;
      break;
   case GL_BUFFER_ACCESS:
      *params = simplified_access_mode(ctx,
                            bufObj->Mappings[MAP_USER].AccessFlags);
      break;
   case GL_BUFFER_MAPPED:
      *params = _mesa_bufferobj_mapped(bufObj, MAP_USER);
      break;
   case GL_BUFFER_ACCESS_FLAGS:
      *params = bufObj->Mappings[MAP_USER].AccessFlags;
      break;
   case GL_BUFFER_MAP_OFFSET:
      *params = bufObj->Mappings[MAP_USER].Offset;
      break;
   case GL_BUFFER_MAP_LENGTH:
      *params = bufObj->Mappings[MAP_USER].Length;
      break;
   case GL_BUFFER_IMMUTABLE_STORAGE:
      *params = bufObj->Immutable;
      break;
   case GL_BUFFER_STORAGE_FLAGS:
      *params = bufObj->StorageFlags;
      break;
   default:
      goto invalid_pname;
   }

   return true;

invalid_pname:
   _mesa_error(ctx, GL_INVALID_ENUM, "%s(invalid pname: %s)", func, "");
//               _mesa_lookup_enum_by_nr(pname));
   return false;
}

void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;
   GLint64 parameter;

   bufObj = get_buffer(ctx, "glGetBufferParameteriv", target,
                       GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   if (!get_buffer_parameter(ctx, bufObj, pname, &parameter,
                             "glGetBufferParameteriv"))
      return; /* Error already recorded. */

   *params = (GLint) parameter;
}

void glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;
   GLint64 parameter;

   bufObj = get_buffer(ctx, "glGetBufferParameteri64v", target,
                       GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   if (!get_buffer_parameter(ctx, bufObj, pname, &parameter,
                             "glGetBufferParameteri64v"))
      return; /* Error already recorded. */

   *params = parameter;
}

void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint *params)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;
   GLint64 parameter;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer,
                                       "glGetNamedBufferParameteriv");
   if (!bufObj)
      return;

   if (!get_buffer_parameter(ctx, bufObj, pname, &parameter,
                             "glGetNamedBufferParameteriv"))
      return; /* Error already recorded. */

   *params = (GLint) parameter;
}

void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname,
                                  GLint64 *params)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;
   GLint64 parameter;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer,
                                       "glGetNamedBufferParameteri64v");
   if (!bufObj)
      return;

   if (!get_buffer_parameter(ctx, bufObj, pname, &parameter,
                             "glGetNamedBufferParameteri64v"))
      return; /* Error already recorded. */

   *params = parameter;
}


void glGetBufferPointerv(GLenum target, GLenum pname, GLvoid **params)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   if (pname != GL_BUFFER_MAP_POINTER) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetBufferPointerv(pname != "
                  "GL_BUFFER_MAP_POINTER)");
      return;
   }

   bufObj = get_buffer(ctx, "glGetBufferPointerv", target,
                       GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   *params = bufObj->Mappings[MAP_USER].Pointer;
}

void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, GLvoid **params)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   if (pname != GL_BUFFER_MAP_POINTER) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetNamedBufferPointerv(pname != "
                  "GL_BUFFER_MAP_POINTER)");
      return;
   }

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer,
                                       "glGetNamedBufferPointerv");
   if (!bufObj)
      return;

   *params = bufObj->Mappings[MAP_USER].Pointer;
}


void
_mesa_copy_buffer_sub_data(struct gl_context *ctx,
                           struct gl_buffer_object *src,
                           struct gl_buffer_object *dst,
                           GLintptr readOffset, GLintptr writeOffset,
                           GLsizeiptr size, const char *func)
{
   if (_mesa_check_disallowed_mapping(src)) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(readBuffer is mapped)", func);
      return;
   }

   if (_mesa_check_disallowed_mapping(dst)) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(writeBuffer is mapped)", func);
      return;
   }

   if (readOffset < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(readOffset %d < 0)", func, (int) readOffset);
      return;
   }

   if (writeOffset < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(writeOffset %d < 0)", func, (int) writeOffset);
      return;
   }

   if (size < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(size %d < 0)", func, (int) size);
      return;
   }

   if (readOffset + size > src->Size) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(readOffset %d + size %d > src_buffer_size %d)", func,
                  (int) readOffset, (int) size, (int) src->Size);
      return;
   }

   if (writeOffset + size > dst->Size) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(writeOffset %d + size %d > dst_buffer_size %d)", func,
                  (int) writeOffset, (int) size, (int) dst->Size);
      return;
   }

   if (src == dst) {
      if (readOffset + size <= writeOffset) {
         /* OK */
      }
      else if (writeOffset + size <= readOffset) {
         /* OK */
      }
      else {
         /* overlapping src/dst is illegal */
         _mesa_error(ctx, GL_INVALID_VALUE,
                     "%s(overlapping src/dst)", func);
         return;
      }
   }

   ctx->Driver.CopyBufferSubData(ctx, src, dst, readOffset, writeOffset, size);
}

void glCopyBufferSubData(GLenum readTarget, GLenum writeTarget,
                        GLintptr readOffset, GLintptr writeOffset,
                        GLsizeiptr size)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *src, *dst;

   src = get_buffer(ctx, "glCopyBufferSubData", readTarget,
                    GL_INVALID_OPERATION);
   if (!src)
      return;

   dst = get_buffer(ctx, "glCopyBufferSubData", writeTarget,
                    GL_INVALID_OPERATION);
   if (!dst)
      return;

   _mesa_copy_buffer_sub_data(ctx, src, dst, readOffset, writeOffset, size,
                              "glCopyBufferSubData");
}

void glCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer,
                             GLintptr readOffset, GLintptr writeOffset,
                             GLsizeiptr size)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *src, *dst;

   src = _mesa_lookup_bufferobj_err(ctx, readBuffer,
                                    "glCopyNamedBufferSubData");
   if (!src)
      return;

   dst = _mesa_lookup_bufferobj_err(ctx, writeBuffer,
                                    "glCopyNamedBufferSubData");
   if (!dst)
      return;

   _mesa_copy_buffer_sub_data(ctx, src, dst, readOffset, writeOffset, size,
                              "glCopyNamedBufferSubData");
}


void *
_mesa_map_buffer_range(struct gl_context *ctx,
                       struct gl_buffer_object *bufObj,
                       GLintptr offset, GLsizeiptr length,
                       GLbitfield access, const char *func)
{
   void *map;
   GLbitfield allowed_access;

//   ASSERT_OUTSIDE_BEGIN_END_WITH_RETVAL(ctx, NULL);

   if (offset < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(offset %ld < 0)", func, (long) offset);
      return NULL;
   }

   if (length < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(length %ld < 0)", func, (long) length);
      return NULL;
   }

   /* Page 38 of the PDF of the OpenGL ES 3.0 spec says:
    *
    *     "An INVALID_OPERATION error is generated for any of the following
    *     conditions:
    *
    *     * <length> is zero."
    *
    * Additionally, page 94 of the PDF of the OpenGL 4.5 core spec
    * (30.10.2014) also says this, so it's no longer allowed for desktop GL,
    * either.
    */
   if (length == 0) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "%s(length = 0)", func);
      return NULL;
   }

   allowed_access = GL_MAP_READ_BIT |
                    GL_MAP_WRITE_BIT |
                    GL_MAP_INVALIDATE_RANGE_BIT |
                    GL_MAP_INVALIDATE_BUFFER_BIT |
                    GL_MAP_FLUSH_EXPLICIT_BIT |
                    GL_MAP_UNSYNCHRONIZED_BIT;

//   if (ctx->Extensions.ARB_buffer_storage) {
//         allowed_access |= GL_MAP_PERSISTENT_BIT |
//                           GL_MAP_COHERENT_BIT;
//   }

   if (access & ~allowed_access) {
      /* generate an error if any bits other than those allowed are set */
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(access has undefined bits set)", func);
      return NULL;
   }

   if ((access & (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)) == 0) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(access indicates neither read or write)", func);
      return NULL;
   }

   if ((access & GL_MAP_READ_BIT) &&
       (access & (GL_MAP_INVALIDATE_RANGE_BIT |
                  GL_MAP_INVALIDATE_BUFFER_BIT |
                  GL_MAP_UNSYNCHRONIZED_BIT))) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(read access with disallowed bits)", func);
      return NULL;
   }

   if ((access & GL_MAP_FLUSH_EXPLICIT_BIT) &&
       ((access & GL_MAP_WRITE_BIT) == 0)) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(access has flush explicit without write)", func);
      return NULL;
   }

   if (access & GL_MAP_READ_BIT &&
       !(bufObj->StorageFlags & GL_MAP_READ_BIT)) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(buffer does not allow read access)", func);
      return NULL;
   }

   if (access & GL_MAP_WRITE_BIT &&
       !(bufObj->StorageFlags & GL_MAP_WRITE_BIT)) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(buffer does not allow write access)", func);
      return NULL;
   }

//   if (access & GL_MAP_COHERENT_BIT &&
//       !(bufObj->StorageFlags & GL_MAP_COHERENT_BIT)) {
//      _mesa_error(ctx, GL_INVALID_OPERATION,
//                  "%s(buffer does not allow coherent access)", func);
//      return NULL;
//   }
//
//   if (access & GL_MAP_PERSISTENT_BIT &&
//       !(bufObj->StorageFlags & GL_MAP_PERSISTENT_BIT)) {
//      _mesa_error(ctx, GL_INVALID_OPERATION,
//                  "%s(buffer does not allow persistent access)", func);
//      return NULL;
//   }

   if (offset + length > bufObj->Size) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(offset %ld + length %ld > buffer_size %ld)", func,
                  offset, length, bufObj->Size);
      return NULL;
   }

   if (_mesa_bufferobj_mapped(bufObj, MAP_USER)) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(buffer already mapped)", func);
      return NULL;
   }

   if (!bufObj->Size) {
      _mesa_error(ctx, GL_OUT_OF_MEMORY, "%s(buffer size = 0)", func);
      return NULL;
   }


   assert(ctx->Driver.MapBufferRange);
   map = ctx->Driver.MapBufferRange(ctx, offset, length, access, bufObj,
                                    MAP_USER);
   if (!map) {
      _mesa_error(ctx, GL_OUT_OF_MEMORY, "%s(map failed)", func);
   }
   else {
      /* The driver callback should have set all these fields.
       * This is important because other modules (like VBO) might call
       * the driver function directly.
       */
      assert(bufObj->Mappings[MAP_USER].Pointer == map);
      assert(bufObj->Mappings[MAP_USER].Length == length);
      assert(bufObj->Mappings[MAP_USER].Offset == offset);
      assert(bufObj->Mappings[MAP_USER].AccessFlags == access);
   }

   if (access & GL_MAP_WRITE_BIT)
      bufObj->Written = GL_TRUE;

#ifdef VBO_DEBUG
   if (strstr(func, "Range") == NULL) { /* If not MapRange */
      printf("glMapBuffer(%u, sz %ld, access 0x%x)\n",
            bufObj->Name, bufObj->Size, access);
      /* Access must be write only */
      if ((access & GL_MAP_WRITE_BIT) && (!(access & ~GL_MAP_WRITE_BIT))) {
         GLuint i;
         GLubyte *b = (GLubyte *) bufObj->Pointer;
         for (i = 0; i < bufObj->Size; i++)
            b[i] = i & 0xff;
      }
   }
#endif

#ifdef BOUNDS_CHECK
   if (strstr(func, "Range") == NULL) { /* If not MapRange */
      GLubyte *buf = (GLubyte *) bufObj->Pointer;
      GLuint i;
      /* buffer is 100 bytes larger than requested, fill with magic value */
      for (i = 0; i < 100; i++) {
         buf[bufObj->Size - i - 1] = 123;
      }
   }
#endif

   return map;
}

void * glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length,
                     GLbitfield access)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glMapBufferRange", target, GL_INVALID_OPERATION);
   if (!bufObj)
      return NULL;

   return _mesa_map_buffer_range(ctx, bufObj, offset, length, access,
                                 "glMapBufferRange");
}

void * glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length,
                          GLbitfield access)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer, "glMapNamedBufferRange");
   if (!bufObj)
      return NULL;

   return _mesa_map_buffer_range(ctx, bufObj, offset, length, access,
                                 "glMapNamedBufferRange");
}

/**
 * Converts GLenum access from MapBuffer and MapNamedBuffer into
 * flags for input to _mesa_map_buffer_range.
 *
 * \return true if the type of requested access is permissible.
 */
static bool
get_map_buffer_access_flags(struct gl_context *ctx, GLenum access,
                            GLbitfield *flags)
{
   switch (access) {
   case GL_READ_ONLY:
      *flags = GL_MAP_READ_BIT;
      return false;
   case GL_WRITE_ONLY:
      *flags = GL_MAP_WRITE_BIT;
      return true;
   case GL_READ_WRITE:
      *flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
      return false;
   default:
      return false;
   }
}

void * glMapBuffer(GLenum target, GLenum access)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;
   GLbitfield accessFlags;

   if (!get_map_buffer_access_flags(ctx, access, &accessFlags)) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glMapBuffer(invalid access)");
      return NULL;
   }

   bufObj = get_buffer(ctx, "glMapBuffer", target, GL_INVALID_OPERATION);
   if (!bufObj)
      return NULL;

   return _mesa_map_buffer_range(ctx, bufObj, 0, bufObj->Size, accessFlags,
                                 "glMapBuffer");
}

void * glMapNamedBuffer(GLuint buffer, GLenum access)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;
   GLbitfield accessFlags;

   if (!get_map_buffer_access_flags(ctx, access, &accessFlags)) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glMapNamedBuffer(invalid access)");
      return NULL;
   }

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer, "glMapNamedBuffer");
   if (!bufObj)
      return NULL;

   return _mesa_map_buffer_range(ctx, bufObj, 0, bufObj->Size, accessFlags,
                                 "glMapNamedBuffer");
}


void
_mesa_flush_mapped_buffer_range(struct gl_context *ctx,
                                struct gl_buffer_object *bufObj,
                                GLintptr offset, GLsizeiptr length,
                                const char *func)
{
   if (offset < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(offset %ld < 0)", func, (long) offset);
      return;
   }

   if (length < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(length %ld < 0)", func, (long) length);
      return;
   }

   if (!_mesa_bufferobj_mapped(bufObj, MAP_USER)) {
      /* buffer is not mapped */
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(buffer is not mapped)", func);
      return;
   }

   if ((bufObj->Mappings[MAP_USER].AccessFlags &
        GL_MAP_FLUSH_EXPLICIT_BIT) == 0) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "%s(GL_MAP_FLUSH_EXPLICIT_BIT not set)", func);
      return;
   }

   if (offset + length > bufObj->Mappings[MAP_USER].Length) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "%s(offset %ld + length %ld > mapped length %ld)", func,
                  (long) offset, (long) length,
                  (long) bufObj->Mappings[MAP_USER].Length);
      return;
   }

   assert(bufObj->Mappings[MAP_USER].AccessFlags & GL_MAP_WRITE_BIT);

   if (ctx->Driver.FlushMappedBufferRange)
      ctx->Driver.FlushMappedBufferRange(ctx, offset, length, bufObj,
                                         MAP_USER);
}

void glFlushMappedBufferRange(GLenum target, GLintptr offset,
                             GLsizeiptr length)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = get_buffer(ctx, "glFlushMappedBufferRange", target,
                       GL_INVALID_OPERATION);
   if (!bufObj)
      return;

   _mesa_flush_mapped_buffer_range(ctx, bufObj, offset, length,
                                   "glFlushMappedBufferRange");
}

void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset,
                                  GLsizeiptr length)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_buffer_object *bufObj;

   bufObj = _mesa_lookup_bufferobj_err(ctx, buffer,
                                       "glFlushMappedNamedBufferRange");
   if (!bufObj)
      return;

   _mesa_flush_mapped_buffer_range(ctx, bufObj, offset, length,
                                   "glFlushMappedNamedBufferRange");
}

