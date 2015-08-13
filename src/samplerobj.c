/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 2011  VMware, Inc.  All Rights Reserved.
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
 * \file samplerobj.c
 * \brief Functions for the GL_ARB_sampler_objects extension.
 * \author Brian Paul
 */


#include <assert.h>
#include "glheader.h"
#include "context.h"
//#include "dispatch.h"
#include "enums.h"
#include "hash.h"
#include "macros.h"
#include "mtypes.h"
#include "samplerobj.h"


struct gl_sampler_object *
_mesa_lookup_samplerobj(struct gl_context *ctx, GLuint name)
{
   if (name == 0)
      return NULL;
   else
      return (struct gl_sampler_object *)
         _mesa_HashLookup(ctx->Shared->SamplerObjects, name);
}


static inline void
begin_samplerobj_lookups(struct gl_context *ctx)
{
   _mesa_HashLockMutex(ctx->Shared->SamplerObjects);
}


static inline void
end_samplerobj_lookups(struct gl_context *ctx)
{
   _mesa_HashUnlockMutex(ctx->Shared->SamplerObjects);
}


static inline struct gl_sampler_object *
lookup_samplerobj_locked(struct gl_context *ctx, GLuint name)
{
   return (struct gl_sampler_object *)
         _mesa_HashLookupLocked(ctx->Shared->SamplerObjects, name);
}


/**
 * Handle reference counting.
 */
void
_mesa_reference_sampler_object_(struct gl_context *ctx,
                                struct gl_sampler_object **ptr,
                                struct gl_sampler_object *samp)
{
   assert(*ptr != samp); /* The inline wrapper should prevent no-op calls */

   if (*ptr) {
      /* Unreference the old sampler */
      GLboolean deleteFlag = GL_FALSE;
      struct gl_sampler_object *oldSamp = *ptr;

      /*mtx_lock(&oldSamp->Mutex);*/
      assert(oldSamp->RefCount > 0);
      oldSamp->RefCount--;
#if 0
      printf("SamplerObj %p %d DECR to %d\n",
             (void *) oldSamp, oldSamp->Name, oldSamp->RefCount);
#endif
      deleteFlag = (oldSamp->RefCount == 0);
      /*mtx_unlock(&oldSamp->Mutex);*/

      if (deleteFlag) {
	 assert(ctx->Driver.DeleteSamplerObject);
         ctx->Driver.DeleteSamplerObject(ctx, oldSamp);
      }

      *ptr = NULL;
   }
   assert(!*ptr);

   if (samp) {
      /* reference new sampler */
      /*mtx_lock(&samp->Mutex);*/
      if (samp->RefCount == 0) {
         /* this sampler's being deleted (look just above) */
         /* Not sure this can every really happen.  Warn if it does. */
         _mesa_problem(NULL, "referencing deleted sampler object");
         *ptr = NULL;
      }
      else {
         samp->RefCount++;
#if 0
         printf("SamplerObj %p %d INCR to %d\n",
                (void *) samp, samp->Name, samp->RefCount);
#endif
         *ptr = samp;
      }
      /*mtx_unlock(&samp->Mutex);*/
   }
}


/**
 * Initialize the fields of the given sampler object.
 */
static void
_mesa_init_sampler_object(struct gl_sampler_object *sampObj, GLuint name)
{
   sampObj->Name = name;
   sampObj->RefCount = 1;
   sampObj->WrapS = GL_REPEAT;
   sampObj->WrapT = GL_REPEAT;
   sampObj->WrapR = GL_REPEAT;
   sampObj->MinFilter = GL_NEAREST_MIPMAP_LINEAR;
   sampObj->MagFilter = GL_LINEAR;
   sampObj->BorderColor.f[0] = 0.0;
   sampObj->BorderColor.f[1] = 0.0;
   sampObj->BorderColor.f[2] = 0.0;
   sampObj->BorderColor.f[3] = 0.0;
   sampObj->MinLod = -1000.0F;
   sampObj->MaxLod = 1000.0F;
   sampObj->LodBias = 0.0F;
   sampObj->MaxAnisotropy = 1.0F;
   sampObj->CompareMode = GL_NONE;
   sampObj->CompareFunc = GL_LEQUAL;
//   sampObj->sRGBDecode = GL_DECODE_EXT;
   sampObj->CubeMapSeamless = GL_FALSE;
}

/**
 * Fallback for ctx->Driver.NewSamplerObject();
 */
struct gl_sampler_object *
_mesa_new_sampler_object(struct gl_context *ctx, GLuint name)
{
   struct gl_sampler_object *sampObj = CALLOC_STRUCT(gl_sampler_object);
   if (sampObj) {
      _mesa_init_sampler_object(sampObj, name);
   }
   return sampObj;
}


/**
 * Fallback for ctx->Driver.DeleteSamplerObject();
 */
static void
_mesa_delete_sampler_object(struct gl_context *ctx,
                            struct gl_sampler_object *sampObj)
{
   free(sampObj->Label);
   free(sampObj);
}

static void
create_samplers(struct gl_context *ctx, GLsizei count, GLuint *samplers,
                const char *caller)
{
   GLuint first;
   GLint i;

//   if (MESA_VERBOSE & VERBOSE_API)
//      _mesa_debug(ctx, "%s(%d)\n", caller, count);

   if (count < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(n<0)", caller);
      return;
   }

   if (!samplers)
      return;

   first = _mesa_HashFindFreeKeyBlock(ctx->Shared->SamplerObjects, count);

   /* Insert the ID and pointer to new sampler object into hash table */
   for (i = 0; i < count; i++) {
      struct gl_sampler_object *sampObj =
         ctx->Driver.NewSamplerObject(ctx, first + i);
      _mesa_HashInsert(ctx->Shared->SamplerObjects, first + i, sampObj);
      samplers[i] = first + i;
   }
}

void glGenSamplers(GLsizei count, GLuint *samplers)
{
   GET_CURRENT_CONTEXT(ctx);
   create_samplers(ctx, count, samplers, "glGenSamplers");
}

void glCreateSamplers(GLsizei count, GLuint *samplers)
{
   GET_CURRENT_CONTEXT(ctx);
   create_samplers(ctx, count, samplers, "glCreateSamplers");
}


void glDeleteSamplers(GLsizei count, const GLuint *samplers)
{
   GET_CURRENT_CONTEXT(ctx);
   GLsizei i;

   FLUSH_VERTICES(ctx, 0);

   if (count < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glDeleteSamplers(count)");
      return;
   }

//   mtx_lock(&ctx->Shared->Mutex);

   for (i = 0; i < count; i++) {
      if (samplers[i]) {
         GLuint j;
         struct gl_sampler_object *sampObj =
            _mesa_lookup_samplerobj(ctx, samplers[i]);
   
         if (sampObj) {
            /* If the sampler is currently bound, unbind it. */
            for (j = 0; j < ctx->Const.MaxCombinedTextureImageUnits; j++) {
               if (ctx->Texture.Unit[j].Sampler == sampObj) {
                  FLUSH_VERTICES(ctx, _NEW_TEXTURE);
                  _mesa_reference_sampler_object(ctx, &ctx->Texture.Unit[j].Sampler, NULL);
               }
            }

            /* The ID is immediately freed for re-use */
            _mesa_HashRemove(ctx->Shared->SamplerObjects, samplers[i]);
            /* But the object exists until its reference count goes to zero */
            _mesa_reference_sampler_object(ctx, &sampObj, NULL);
         }
      }
   }

//   mtx_unlock(&ctx->Shared->Mutex);
}


GLboolean glIsSampler(GLuint sampler)
{
   struct gl_sampler_object *sampObj;
   GET_CURRENT_CONTEXT(ctx);

//   ASSERT_OUTSIDE_BEGIN_END_WITH_RETVAL(ctx, GL_FALSE);

   if (sampler == 0)
      return GL_FALSE;

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);

   return sampObj != NULL;
}


void glBindSampler(GLuint unit, GLuint sampler)
{
   struct gl_sampler_object *sampObj;
   GET_CURRENT_CONTEXT(ctx);

   if (unit >= ctx->Const.MaxCombinedTextureImageUnits) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glBindSampler(unit %u)", unit);
      return;
   }

   if (sampler == 0) {
      /* Use the default sampler object, the one contained in the texture
       * object.
       */
      sampObj = NULL;
   }
   else {
      /* user-defined sampler object */
      sampObj = _mesa_lookup_samplerobj(ctx, sampler);
      if (!sampObj) {
         _mesa_error(ctx, GL_INVALID_OPERATION, "glBindSampler(sampler)");
         return;
      }
   }
   
   if (ctx->Texture.Unit[unit].Sampler != sampObj) {
      FLUSH_VERTICES(ctx, _NEW_TEXTURE);
   }

   /* bind new sampler */
   _mesa_reference_sampler_object(ctx, &ctx->Texture.Unit[unit].Sampler,
                                  sampObj);
}


void glBindSamplers(GLuint first, GLsizei count, const GLuint *samplers)
{
   GET_CURRENT_CONTEXT(ctx);
   GLint i;

   /* The ARB_multi_bind spec says:
    *
    *   "An INVALID_OPERATION error is generated if <first> + <count> is
    *    greater than the number of texture image units supported by
    *    the implementation."
    */
   if (first + count > ctx->Const.MaxCombinedTextureImageUnits) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glBindSamplers(first=%u + count=%d > the value of "
                  "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS=%u)",
                  first, count, ctx->Const.MaxCombinedTextureImageUnits);
      return;
   }

   FLUSH_VERTICES(ctx, 0);

   if (samplers) {
      /* Note that the error semantics for multi-bind commands differ from
       * those of other GL commands.
       *
       * The Issues section in the ARB_multi_bind spec says:
       *
       *    "(11) Typically, OpenGL specifies that if an error is generated by
       *          a command, that command has no effect.  This is somewhat
       *          unfortunate for multi-bind commands, because it would require
       *          a first pass to scan the entire list of bound objects for
       *          errors and then a second pass to actually perform the
       *          bindings.  Should we have different error semantics?
       *
       *       RESOLVED:  Yes.  In this specification, when the parameters for
       *       one of the <count> binding points are invalid, that binding
       *       point is not updated and an error will be generated.  However,
       *       other binding points in the same command will be updated if
       *       their parameters are valid and no other error occurs."
       */

      begin_samplerobj_lookups(ctx);

      for (i = 0; i < count; i++) {
         const GLuint unit = first + i;
         struct gl_sampler_object * const currentSampler =
             ctx->Texture.Unit[unit].Sampler;
         struct gl_sampler_object *sampObj;

         if (samplers[i] != 0) {
            if (currentSampler && currentSampler->Name == samplers[i])
               sampObj = currentSampler;
            else
               sampObj = lookup_samplerobj_locked(ctx, samplers[i]);

            /* The ARB_multi_bind spec says:
             *
             *    "An INVALID_OPERATION error is generated if any value
             *     in <samplers> is not zero or the name of an existing
             *     sampler object (per binding)."
             */
            if (!sampObj) {
               _mesa_error(ctx, GL_INVALID_OPERATION,
                           "glBindSamplers(samplers[%d]=%u is not zero or "
                           "the name of an existing sampler object)",
                           i, samplers[i]);
               continue;
            }
         } else {
            sampObj = NULL;
         }

         /* Bind the new sampler */
         if (sampObj != currentSampler) {
            _mesa_reference_sampler_object(ctx,
                                           &ctx->Texture.Unit[unit].Sampler,
                                           sampObj);
            ctx->NewState |= _NEW_TEXTURE;
         }
      }

      end_samplerobj_lookups(ctx);
   } else {
      /* Unbind all samplers in the range <first> through <first>+<count>-1 */
      for (i = 0; i < count; i++) {
         const GLuint unit = first + i;

         if (ctx->Texture.Unit[unit].Sampler) {
            _mesa_reference_sampler_object(ctx,
                                           &ctx->Texture.Unit[unit].Sampler,
                                           NULL);
            ctx->NewState |= _NEW_TEXTURE;
         }
      }
   }
}


/**
 * Check if a coordinate wrap mode is legal.
 * \return GL_TRUE if legal, GL_FALSE otherwise
 */
static GLboolean 
validate_texture_wrap_mode(struct gl_context *ctx, GLenum wrap)
{
//   const struct gl_extensions * const e = &ctx->Extensions;

   switch (wrap) {
   case GL_CLAMP:
   case GL_CLAMP_TO_EDGE:
   case GL_REPEAT:
//   case GL_MIRRORED_REPEAT:
      return GL_TRUE;
//   case GL_CLAMP_TO_BORDER:
//      return e->ARB_texture_border_clamp;
//   case GL_MIRROR_CLAMP_EXT:
//      return e->ATI_texture_mirror_once || e->EXT_texture_mirror_clamp;
//   case GL_MIRROR_CLAMP_TO_EDGE_EXT:
//      return e->ATI_texture_mirror_once || e->EXT_texture_mirror_clamp || e->ARB_texture_mirror_clamp_to_edge;
//   case GL_MIRROR_CLAMP_TO_BORDER_EXT:
//      return e->EXT_texture_mirror_clamp;
   default:
      return GL_FALSE;
   }
}


/**
 * This is called just prior to changing any sampler object state.
 */
static inline void
flush(struct gl_context *ctx)
{
   FLUSH_VERTICES(ctx, _NEW_TEXTURE);
}


#define INVALID_PARAM 0x100
#define INVALID_PNAME 0x101
#define INVALID_VALUE 0x102

static GLuint
set_sampler_wrap_s(struct gl_context *ctx, struct gl_sampler_object *samp,
                   GLint param)
{
   if (samp->WrapS == param)
      return GL_FALSE;
   if (validate_texture_wrap_mode(ctx, param)) {
      flush(ctx);
      samp->WrapS = param;
      return GL_TRUE;
   }
   return INVALID_PARAM;
}


static GLuint
set_sampler_wrap_t(struct gl_context *ctx, struct gl_sampler_object *samp,
                   GLint param)
{
   if (samp->WrapT == param)
      return GL_FALSE;
   if (validate_texture_wrap_mode(ctx, param)) {
      flush(ctx);
      samp->WrapT = param;
      return GL_TRUE;
   }
   return INVALID_PARAM;
}


static GLuint
set_sampler_wrap_r(struct gl_context *ctx, struct gl_sampler_object *samp,
                   GLint param)
{
   if (samp->WrapR == param)
      return GL_FALSE;
   if (validate_texture_wrap_mode(ctx, param)) {
      flush(ctx);
      samp->WrapR = param;
      return GL_TRUE;
   }
   return INVALID_PARAM;
}


static GLuint
set_sampler_min_filter(struct gl_context *ctx, struct gl_sampler_object *samp,
                       GLint param)
{
   if (samp->MinFilter == param)
      return GL_FALSE;

   switch (param) {
   case GL_NEAREST:
   case GL_LINEAR:
   case GL_NEAREST_MIPMAP_NEAREST:
   case GL_LINEAR_MIPMAP_NEAREST:
   case GL_NEAREST_MIPMAP_LINEAR:
   case GL_LINEAR_MIPMAP_LINEAR:
      flush(ctx);
      samp->MinFilter = param;
      return GL_TRUE;
   default:
      return INVALID_PARAM;
   }
}


static GLuint
set_sampler_mag_filter(struct gl_context *ctx, struct gl_sampler_object *samp,
                       GLint param)
{
   if (samp->MagFilter == param)
      return GL_FALSE;

   switch (param) {
   case GL_NEAREST:
   case GL_LINEAR:
      flush(ctx);
      samp->MagFilter = param;
      return GL_TRUE;
   default:
      return INVALID_PARAM;
   }
}


static GLuint
set_sampler_lod_bias(struct gl_context *ctx, struct gl_sampler_object *samp,
                     GLfloat param)
{
   if (samp->LodBias == param)
      return GL_FALSE;

   flush(ctx);
   samp->LodBias = param;
   return GL_TRUE;
}


static GLuint
set_sampler_border_colorf(struct gl_context *ctx,
                          struct gl_sampler_object *samp,
                          const GLfloat params[4])
{
   flush(ctx);
   samp->BorderColor.f[RCOMP] = params[0];
   samp->BorderColor.f[GCOMP] = params[1];
   samp->BorderColor.f[BCOMP] = params[2];
   samp->BorderColor.f[ACOMP] = params[3];
   return GL_TRUE;
}


static GLuint
set_sampler_border_colori(struct gl_context *ctx,
                          struct gl_sampler_object *samp,
                          const GLint params[4])
{
   flush(ctx);
   samp->BorderColor.i[RCOMP] = params[0];
   samp->BorderColor.i[GCOMP] = params[1];
   samp->BorderColor.i[BCOMP] = params[2];
   samp->BorderColor.i[ACOMP] = params[3];
   return GL_TRUE;
}


static GLuint
set_sampler_border_colorui(struct gl_context *ctx,
                           struct gl_sampler_object *samp,
                           const GLuint params[4])
{
   flush(ctx);
   samp->BorderColor.ui[RCOMP] = params[0];
   samp->BorderColor.ui[GCOMP] = params[1];
   samp->BorderColor.ui[BCOMP] = params[2];
   samp->BorderColor.ui[ACOMP] = params[3];
   return GL_TRUE;
}


static GLuint
set_sampler_min_lod(struct gl_context *ctx, struct gl_sampler_object *samp,
                    GLfloat param)
{
   if (samp->MinLod == param)
      return GL_FALSE;

   flush(ctx);
   samp->MinLod = param;
   return GL_TRUE;
}


static GLuint
set_sampler_max_lod(struct gl_context *ctx, struct gl_sampler_object *samp,
                    GLfloat param)
{
   if (samp->MaxLod == param)
      return GL_FALSE;

   flush(ctx);
   samp->MaxLod = param;
   return GL_TRUE;
}


static GLuint
set_sampler_compare_mode(struct gl_context *ctx,
                         struct gl_sampler_object *samp, GLint param)
{
//   if (!ctx->Extensions.ARB_shadow)
//      return INVALID_PNAME;

   if (samp->CompareMode == param)
      return GL_FALSE;

	if (param == GL_NONE) {
//   if (param == GL_NONE ||
//       param == GL_COMPARE_R_TO_TEXTURE_ARB) {
      flush(ctx);
      samp->CompareMode = param;
      return GL_TRUE;
   }

   return INVALID_PARAM;
}


static GLuint
set_sampler_compare_func(struct gl_context *ctx,
                         struct gl_sampler_object *samp, GLint param)
{
//   if (!ctx->Extensions.ARB_shadow)
//      return INVALID_PNAME;

   if (samp->CompareFunc == param)
      return GL_FALSE;

   switch (param) {
   case GL_LEQUAL:
   case GL_GEQUAL:
   case GL_EQUAL:
   case GL_NOTEQUAL:
   case GL_LESS:
   case GL_GREATER:
   case GL_ALWAYS:
   case GL_NEVER:
      flush(ctx);
      samp->CompareFunc = param;
      return GL_TRUE;
   default:
      return INVALID_PARAM;
   }
}


static GLuint
set_sampler_max_anisotropy(struct gl_context *ctx,
                           struct gl_sampler_object *samp, GLfloat param)
{
//   if (!ctx->Extensions.EXT_texture_filter_anisotropic)
//      return INVALID_PNAME;

   if (samp->MaxAnisotropy == param)
      return GL_FALSE;

   if (param < 1.0)
      return INVALID_VALUE;

   flush(ctx);
   /* clamp to max, that's what NVIDIA does */
   samp->MaxAnisotropy = MIN2(param, ctx->Const.MaxTextureMaxAnisotropy);
   return GL_TRUE;
}


static GLuint
set_sampler_cube_map_seamless(struct gl_context *ctx,
                              struct gl_sampler_object *samp, GLboolean param)
{
//   if (!_mesa_is_desktop_gl(ctx)
//       || !ctx->Extensions.AMD_seamless_cubemap_per_texture)
      return INVALID_PNAME;

   if (samp->CubeMapSeamless == param)
      return GL_FALSE;

   if (param != GL_TRUE && param != GL_FALSE)
      return INVALID_VALUE;

   flush(ctx);
   samp->CubeMapSeamless = param;
   return GL_TRUE;
}

static GLuint
set_sampler_srgb_decode(struct gl_context *ctx,
                              struct gl_sampler_object *samp, GLenum param)
{
//   if (!ctx->Extensions.EXT_texture_sRGB_decode)
//      return INVALID_PNAME;

   if (samp->sRGBDecode == param)
      return GL_FALSE;

//   if (param != GL_DECODE_EXT && param != GL_SKIP_DECODE_EXT)
//      return INVALID_VALUE;

   flush(ctx);
   samp->sRGBDecode = param;
   return GL_TRUE;
}

void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
   struct gl_sampler_object *sampObj;
   GLuint res;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      /* '3.8.2 Sampler Objects' section of the GL-ES 3.0 specification states:
       *
       *     "An INVALID_OPERATION error is generated if sampler is not the name
       *     of a sampler object previously returned from a call to GenSamplers."
       *
       * In desktop GL, an GL_INVALID_VALUE is returned instead.
       */
      _mesa_error(ctx, (GL_INVALID_OPERATION),
                  "glSamplerParameteri(sampler %u)", sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      res = set_sampler_wrap_s(ctx, sampObj, param);
      break;
   case GL_TEXTURE_WRAP_T:
      res = set_sampler_wrap_t(ctx, sampObj, param);
      break;
//   case GL_TEXTURE_WRAP_R:
//      res = set_sampler_wrap_r(ctx, sampObj, param);
//      break;
   case GL_TEXTURE_MIN_FILTER:
      res = set_sampler_min_filter(ctx, sampObj, param);
      break;
   case GL_TEXTURE_MAG_FILTER:
      res = set_sampler_mag_filter(ctx, sampObj, param);
      break;
//   case GL_TEXTURE_MIN_LOD:
//      res = set_sampler_min_lod(ctx, sampObj, (GLfloat) param);
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      res = set_sampler_max_lod(ctx, sampObj, (GLfloat) param);
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      res = set_sampler_lod_bias(ctx, sampObj, (GLfloat) param);
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      res = set_sampler_compare_mode(ctx, sampObj, param);
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      res = set_sampler_compare_func(ctx, sampObj, param);
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      res = set_sampler_max_anisotropy(ctx, sampObj, (GLfloat) param);
//      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      res = set_sampler_cube_map_seamless(ctx, sampObj, param);
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      res = set_sampler_srgb_decode(ctx, sampObj, param);
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      /* fall-through */
   default:
      res = INVALID_PNAME;
   }

   switch (res) {
   case GL_FALSE:
      /* no change */
      break;
   case GL_TRUE:
      /* state change - we do nothing special at this time */
      break;
   case INVALID_PNAME:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameteri(pname=%s)\n",
                  _mesa_lookup_enum_by_nr(pname));
      break;
   case INVALID_PARAM:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameteri(param=%d)\n",
                  param);
      break;
   case INVALID_VALUE:
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameteri(param=%d)\n",
                  param);
      break;
   default:
      ;
   }
}


void glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
{
   struct gl_sampler_object *sampObj;
   GLuint res;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      /* '3.8.2 Sampler Objects' section of the GL-ES 3.0 specification states:
       *
       *     "An INVALID_OPERATION error is generated if sampler is not the name
       *     of a sampler object previously returned from a call to GenSamplers."
       *
       * In desktop GL, an GL_INVALID_VALUE is returned instead.
       */
      _mesa_error(ctx, (GL_INVALID_OPERATION),
                  "glSamplerParameterf(sampler %u)", sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      res = set_sampler_wrap_s(ctx, sampObj, (GLint) param);
      break;
   case GL_TEXTURE_WRAP_T:
      res = set_sampler_wrap_t(ctx, sampObj, (GLint) param);
      break;
//   case GL_TEXTURE_WRAP_R:
//      res = set_sampler_wrap_r(ctx, sampObj, (GLint) param);
//      break;
   case GL_TEXTURE_MIN_FILTER:
      res = set_sampler_min_filter(ctx, sampObj, (GLint) param);
      break;
   case GL_TEXTURE_MAG_FILTER:
      res = set_sampler_mag_filter(ctx, sampObj, (GLint) param);
      break;
//   case GL_TEXTURE_MIN_LOD:
//      res = set_sampler_min_lod(ctx, sampObj, param);
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      res = set_sampler_max_lod(ctx, sampObj, param);
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      res = set_sampler_lod_bias(ctx, sampObj, param);
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      res = set_sampler_compare_mode(ctx, sampObj, (GLint) param);
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      res = set_sampler_compare_func(ctx, sampObj, (GLint) param);
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      res = set_sampler_max_anisotropy(ctx, sampObj, param);
//      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      res = set_sampler_cube_map_seamless(ctx, sampObj, (GLboolean) param);
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      res = set_sampler_srgb_decode(ctx, sampObj, (GLenum) param);
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      /* fall-through */
   default:
      res = INVALID_PNAME;
   }

   switch (res) {
   case GL_FALSE:
      /* no change */
      break;
   case GL_TRUE:
      /* state change - we do nothing special at this time */
      break;
   case INVALID_PNAME:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterf(pname=%s)\n",
                  _mesa_lookup_enum_by_nr(pname));
      break;
   case INVALID_PARAM:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterf(param=%f)\n",
                  param);
      break;
   case INVALID_VALUE:
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameterf(param=%f)\n",
                  param);
      break;
   default:
      ;
   }
}

void glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint *params)
{
   struct gl_sampler_object *sampObj;
   GLuint res;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      /* '3.8.2 Sampler Objects' section of the GL-ES 3.0 specification states:
       *
       *     "An INVALID_OPERATION error is generated if sampler is not the name
       *     of a sampler object previously returned from a call to GenSamplers."
       *
       * In desktop GL, an GL_INVALID_VALUE is returned instead.
       */
      _mesa_error(ctx, (GL_INVALID_OPERATION),
                  "glSamplerParameteriv(sampler %u)", sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      res = set_sampler_wrap_s(ctx, sampObj, params[0]);
      break;
   case GL_TEXTURE_WRAP_T:
      res = set_sampler_wrap_t(ctx, sampObj, params[0]);
      break;
//   case GL_TEXTURE_WRAP_R:
//      res = set_sampler_wrap_r(ctx, sampObj, params[0]);
//      break;
   case GL_TEXTURE_MIN_FILTER:
      res = set_sampler_min_filter(ctx, sampObj, params[0]);
      break;
   case GL_TEXTURE_MAG_FILTER:
      res = set_sampler_mag_filter(ctx, sampObj, params[0]);
      break;
//   case GL_TEXTURE_MIN_LOD:
//      res = set_sampler_min_lod(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      res = set_sampler_max_lod(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      res = set_sampler_lod_bias(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      res = set_sampler_compare_mode(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      res = set_sampler_compare_func(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      res = set_sampler_max_anisotropy(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      res = set_sampler_cube_map_seamless(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      res = set_sampler_srgb_decode(ctx, sampObj, params[0]);
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      {
         GLfloat c[4];
         c[0] = INT_TO_FLOAT(params[0]);
         c[1] = INT_TO_FLOAT(params[1]);
         c[2] = INT_TO_FLOAT(params[2]);
         c[3] = INT_TO_FLOAT(params[3]);
         res = set_sampler_border_colorf(ctx, sampObj, c);
      }
      break;
   default:
      res = INVALID_PNAME;
   }

   switch (res) {
   case GL_FALSE:
      /* no change */
      break;
   case GL_TRUE:
      /* state change - we do nothing special at this time */
      break;
   case INVALID_PNAME:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameteriv(pname=%s)\n",
                  _mesa_lookup_enum_by_nr(pname));
      break;
   case INVALID_PARAM:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameteriv(param=%d)\n",
                  params[0]);
      break;
   case INVALID_VALUE:
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameteriv(param=%d)\n",
                  params[0]);
      break;
   default:
      ;
   }
}

void glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *params)
{
   struct gl_sampler_object *sampObj;
   GLuint res;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      /* '3.8.2 Sampler Objects' section of the GL-ES 3.0 specification states:
       *
       *     "An INVALID_OPERATION error is generated if sampler is not the name
       *     of a sampler object previously returned from a call to GenSamplers."
       *
       * In desktop GL, an GL_INVALID_VALUE is returned instead.
       */
      _mesa_error(ctx, (GL_INVALID_OPERATION),
                  "glSamplerParameterfv(sampler %u)", sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      res = set_sampler_wrap_s(ctx, sampObj, (GLint) params[0]);
      break;
   case GL_TEXTURE_WRAP_T:
      res = set_sampler_wrap_t(ctx, sampObj, (GLint) params[0]);
      break;
//   case GL_TEXTURE_WRAP_R:
//      res = set_sampler_wrap_r(ctx, sampObj, (GLint) params[0]);
//      break;
   case GL_TEXTURE_MIN_FILTER:
      res = set_sampler_min_filter(ctx, sampObj, (GLint) params[0]);
      break;
   case GL_TEXTURE_MAG_FILTER:
      res = set_sampler_mag_filter(ctx, sampObj, (GLint) params[0]);
      break;
//   case GL_TEXTURE_MIN_LOD:
//      res = set_sampler_min_lod(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      res = set_sampler_max_lod(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      res = set_sampler_lod_bias(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      res = set_sampler_compare_mode(ctx, sampObj, (GLint) params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      res = set_sampler_compare_func(ctx, sampObj, (GLint) params[0]);
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      res = set_sampler_max_anisotropy(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      res = set_sampler_cube_map_seamless(ctx, sampObj, (GLboolean) params[0]);
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      res = set_sampler_srgb_decode(ctx, sampObj, (GLenum) params[0]);
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      res = set_sampler_border_colorf(ctx, sampObj, params);
      break;
   default:
      res = INVALID_PNAME;
   }

   switch (res) {
   case GL_FALSE:
      /* no change */
      break;
   case GL_TRUE:
      /* state change - we do nothing special at this time */
      break;
   case INVALID_PNAME:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterfv(pname=%s)\n",
                  _mesa_lookup_enum_by_nr(pname));
      break;
   case INVALID_PARAM:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterfv(param=%f)\n",
                  params[0]);
      break;
   case INVALID_VALUE:
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameterfv(param=%f)\n",
                  params[0]);
      break;
   default:
      ;
   }
}

void glSamplerParameterIiv(GLuint sampler, GLenum pname, const GLint *params)
{
   struct gl_sampler_object *sampObj;
   GLuint res;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameterIiv(sampler %u)",
                  sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      res = set_sampler_wrap_s(ctx, sampObj, params[0]);
      break;
   case GL_TEXTURE_WRAP_T:
      res = set_sampler_wrap_t(ctx, sampObj, params[0]);
      break;
//   case GL_TEXTURE_WRAP_R:
//      res = set_sampler_wrap_r(ctx, sampObj, params[0]);
//      break;
   case GL_TEXTURE_MIN_FILTER:
      res = set_sampler_min_filter(ctx, sampObj, params[0]);
      break;
   case GL_TEXTURE_MAG_FILTER:
      res = set_sampler_mag_filter(ctx, sampObj, params[0]);
      break;
//   case GL_TEXTURE_MIN_LOD:
//      res = set_sampler_min_lod(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      res = set_sampler_max_lod(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      res = set_sampler_lod_bias(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      res = set_sampler_compare_mode(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      res = set_sampler_compare_func(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      res = set_sampler_max_anisotropy(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      res = set_sampler_cube_map_seamless(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      res = set_sampler_srgb_decode(ctx, sampObj, (GLenum) params[0]);
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      res = set_sampler_border_colori(ctx, sampObj, params);
      break;
   default:
      res = INVALID_PNAME;
   }

   switch (res) {
   case GL_FALSE:
      /* no change */
      break;
   case GL_TRUE:
      /* state change - we do nothing special at this time */
      break;
   case INVALID_PNAME:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterIiv(pname=%s)\n",
                  _mesa_lookup_enum_by_nr(pname));
      break;
   case INVALID_PARAM:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterIiv(param=%d)\n",
                  params[0]);
      break;
   case INVALID_VALUE:
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameterIiv(param=%d)\n",
                  params[0]);
      break;
   default:
      ;
   }
}


void glSamplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint *params)
{
   struct gl_sampler_object *sampObj;
   GLuint res;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameterIuiv(sampler %u)",
                  sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      res = set_sampler_wrap_s(ctx, sampObj, params[0]);
      break;
   case GL_TEXTURE_WRAP_T:
      res = set_sampler_wrap_t(ctx, sampObj, params[0]);
      break;
//   case GL_TEXTURE_WRAP_R:
//      res = set_sampler_wrap_r(ctx, sampObj, params[0]);
//      break;
   case GL_TEXTURE_MIN_FILTER:
      res = set_sampler_min_filter(ctx, sampObj, params[0]);
      break;
   case GL_TEXTURE_MAG_FILTER:
      res = set_sampler_mag_filter(ctx, sampObj, params[0]);
      break;
//   case GL_TEXTURE_MIN_LOD:
//      res = set_sampler_min_lod(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      res = set_sampler_max_lod(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      res = set_sampler_lod_bias(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      res = set_sampler_compare_mode(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      res = set_sampler_compare_func(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      res = set_sampler_max_anisotropy(ctx, sampObj, (GLfloat) params[0]);
//      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      res = set_sampler_cube_map_seamless(ctx, sampObj, params[0]);
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      res = set_sampler_srgb_decode(ctx, sampObj, (GLenum) params[0]);
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      res = set_sampler_border_colorui(ctx, sampObj, params);
      break;
   default:
      res = INVALID_PNAME;
   }

   switch (res) {
   case GL_FALSE:
      /* no change */
      break;
   case GL_TRUE:
      /* state change - we do nothing special at this time */
      break;
   case INVALID_PNAME:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterIuiv(pname=%s)\n",
                  _mesa_lookup_enum_by_nr(pname));
      break;
   case INVALID_PARAM:
      _mesa_error(ctx, GL_INVALID_ENUM, "glSamplerParameterIuiv(param=%u)\n",
                  params[0]);
      break;
   case INVALID_VALUE:
      _mesa_error(ctx, GL_INVALID_VALUE, "glSamplerParameterIuiv(param=%u)\n",
                  params[0]);
      break;
   default:
      ;
   }
}


void glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params)
{
   struct gl_sampler_object *sampObj;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      /* '3.8.2 Sampler Objects' section of the GL-ES 3.0 specification states:
       *
       *     "An INVALID_OPERATION error is generated if sampler is not the name
       *     of a sampler object previously returned from a call to GenSamplers."
       *
       * In desktop GL, an GL_INVALID_VALUE is returned instead.
       */
      _mesa_error(ctx, (GL_INVALID_OPERATION),
                  "glGetSamplerParameteriv(sampler %u)", sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      *params = sampObj->WrapS;
      break;
   case GL_TEXTURE_WRAP_T:
      *params = sampObj->WrapT;
      break;
//   case GL_TEXTURE_WRAP_R:
//      *params = sampObj->WrapR;
//      break;
   case GL_TEXTURE_MIN_FILTER:
      *params = sampObj->MinFilter;
      break;
   case GL_TEXTURE_MAG_FILTER:
      *params = sampObj->MagFilter;
      break;
//   case GL_TEXTURE_MIN_LOD:
//      /* GL spec 'Data Conversions' section specifies that floating-point
//       * value in integer Get function is rounded to nearest integer
//       */
//      *params = IROUND(sampObj->MinLod);
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      /* GL spec 'Data Conversions' section specifies that floating-point
//       * value in integer Get function is rounded to nearest integer
//       */
//      *params = IROUND(sampObj->MaxLod);
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      /* GL spec 'Data Conversions' section specifies that floating-point
//       * value in integer Get function is rounded to nearest integer
//       */
//      *params = IROUND(sampObj->LodBias);
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = sampObj->CompareMode;
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = sampObj->CompareFunc;
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      /* GL spec 'Data Conversions' section specifies that floating-point
//       * value in integer Get function is rounded to nearest integer
//       */
//      *params = IROUND(sampObj->MaxAnisotropy);
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      params[0] = FLOAT_TO_INT(sampObj->BorderColor.f[0]);
      params[1] = FLOAT_TO_INT(sampObj->BorderColor.f[1]);
      params[2] = FLOAT_TO_INT(sampObj->BorderColor.f[2]);
      params[3] = FLOAT_TO_INT(sampObj->BorderColor.f[3]);
      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      if (!ctx->Extensions.AMD_seamless_cubemap_per_texture)
//         goto invalid_pname;
//      *params = sampObj->CubeMapSeamless;
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      if (!ctx->Extensions.EXT_texture_sRGB_decode)
//         goto invalid_pname;
//      *params = (GLenum) sampObj->sRGBDecode;
//      break;
   default:
      goto invalid_pname;
   }
   return;

invalid_pname:
   _mesa_error(ctx, GL_INVALID_ENUM, "glGetSamplerParameteriv(pname=%s)",
               _mesa_lookup_enum_by_nr(pname));
}


void glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params)
{
   struct gl_sampler_object *sampObj;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      /* '3.8.2 Sampler Objects' section of the GL-ES 3.0 specification states:
       *
       *     "An INVALID_OPERATION error is generated if sampler is not the name
       *     of a sampler object previously returned from a call to GenSamplers."
       *
       * In desktop GL, an GL_INVALID_VALUE is returned instead.
       */
      _mesa_error(ctx, (GL_INVALID_OPERATION),
                  "glGetSamplerParameterfv(sampler %u)", sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      *params = (GLfloat) sampObj->WrapS;
      break;
   case GL_TEXTURE_WRAP_T:
      *params = (GLfloat) sampObj->WrapT;
      break;
//   case GL_TEXTURE_WRAP_R:
//      *params = (GLfloat) sampObj->WrapR;
//      break;
   case GL_TEXTURE_MIN_FILTER:
      *params = (GLfloat) sampObj->MinFilter;
      break;
   case GL_TEXTURE_MAG_FILTER:
      *params = (GLfloat) sampObj->MagFilter;
      break;
//   case GL_TEXTURE_MIN_LOD:
//      *params = sampObj->MinLod;
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      *params = sampObj->MaxLod;
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      *params = sampObj->LodBias;
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = (GLfloat) sampObj->CompareMode;
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = (GLfloat) sampObj->CompareFunc;
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      *params = sampObj->MaxAnisotropy;
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      params[0] = sampObj->BorderColor.f[0];
      params[1] = sampObj->BorderColor.f[1];
      params[2] = sampObj->BorderColor.f[2];
      params[3] = sampObj->BorderColor.f[3];
      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      if (!ctx->Extensions.AMD_seamless_cubemap_per_texture)
//         goto invalid_pname;
//      *params = (GLfloat) sampObj->CubeMapSeamless;
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      if (!ctx->Extensions.EXT_texture_sRGB_decode)
//         goto invalid_pname;
//      *params = (GLfloat) sampObj->sRGBDecode;
//      break;
   default:
      goto invalid_pname;
   }
   return;

invalid_pname:
   _mesa_error(ctx, GL_INVALID_ENUM, "glGetSamplerParameterfv(pname=%s)",
               _mesa_lookup_enum_by_nr(pname));
}


void glGetSamplerParameterIiv(GLuint sampler, GLenum pname, GLint *params)
{
   struct gl_sampler_object *sampObj;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "glGetSamplerParameterIiv(sampler %u)",
                  sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      *params = sampObj->WrapS;
      break;
   case GL_TEXTURE_WRAP_T:
      *params = sampObj->WrapT;
      break;
//   case GL_TEXTURE_WRAP_R:
//      *params = sampObj->WrapR;
//      break;
   case GL_TEXTURE_MIN_FILTER:
      *params = sampObj->MinFilter;
      break;
   case GL_TEXTURE_MAG_FILTER:
      *params = sampObj->MagFilter;
      break;
//   case GL_TEXTURE_MIN_LOD:
//      *params = (GLint) sampObj->MinLod;
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      *params = (GLint) sampObj->MaxLod;
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      *params = (GLint) sampObj->LodBias;
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = sampObj->CompareMode;
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = sampObj->CompareFunc;
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      *params = (GLint) sampObj->MaxAnisotropy;
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      params[0] = sampObj->BorderColor.i[0];
      params[1] = sampObj->BorderColor.i[1];
      params[2] = sampObj->BorderColor.i[2];
      params[3] = sampObj->BorderColor.i[3];
      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      if (!ctx->Extensions.AMD_seamless_cubemap_per_texture)
//         goto invalid_pname;
//      *params = sampObj->CubeMapSeamless;
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      if (!ctx->Extensions.EXT_texture_sRGB_decode)
//         goto invalid_pname;
//      *params = (GLenum) sampObj->sRGBDecode;
//      break;
   default:
      goto invalid_pname;
   }
   return;

invalid_pname:
   _mesa_error(ctx, GL_INVALID_ENUM, "glGetSamplerParameterIiv(pname=%s)",
               _mesa_lookup_enum_by_nr(pname));
}


void glGetSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint *params)
{
   struct gl_sampler_object *sampObj;
   GET_CURRENT_CONTEXT(ctx);

   sampObj = _mesa_lookup_samplerobj(ctx, sampler);
   if (!sampObj) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "glGetSamplerParameterIuiv(sampler %u)",
                  sampler);
      return;
   }

   switch (pname) {
   case GL_TEXTURE_WRAP_S:
      *params = sampObj->WrapS;
      break;
   case GL_TEXTURE_WRAP_T:
      *params = sampObj->WrapT;
      break;
//   case GL_TEXTURE_WRAP_R:
//      *params = sampObj->WrapR;
//      break;
   case GL_TEXTURE_MIN_FILTER:
      *params = sampObj->MinFilter;
      break;
   case GL_TEXTURE_MAG_FILTER:
      *params = sampObj->MagFilter;
      break;
//   case GL_TEXTURE_MIN_LOD:
//      *params = (GLuint) sampObj->MinLod;
//      break;
//   case GL_TEXTURE_MAX_LOD:
//      *params = (GLuint) sampObj->MaxLod;
//      break;
//   case GL_TEXTURE_LOD_BIAS:
//      *params = (GLuint) sampObj->LodBias;
//      break;
//   case GL_TEXTURE_COMPARE_MODE:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = sampObj->CompareMode;
//      break;
//   case GL_TEXTURE_COMPARE_FUNC:
//      if (!ctx->Extensions.ARB_shadow)
//         goto invalid_pname;
//      *params = sampObj->CompareFunc;
//      break;
//   case GL_TEXTURE_MAX_ANISOTROPY_EXT:
//      *params = (GLuint) sampObj->MaxAnisotropy;
//      break;
   case GL_TEXTURE_BORDER_COLOR:
      params[0] = sampObj->BorderColor.ui[0];
      params[1] = sampObj->BorderColor.ui[1];
      params[2] = sampObj->BorderColor.ui[2];
      params[3] = sampObj->BorderColor.ui[3];
      break;
//   case GL_TEXTURE_CUBE_MAP_SEAMLESS:
//      if (!ctx->Extensions.AMD_seamless_cubemap_per_texture)
//         goto invalid_pname;
//      *params = sampObj->CubeMapSeamless;
//      break;
//   case GL_TEXTURE_SRGB_DECODE_EXT:
//      if (!ctx->Extensions.EXT_texture_sRGB_decode)
//         goto invalid_pname;
//      *params = (GLenum) sampObj->sRGBDecode;
//      break;
   default:
      goto invalid_pname;
   }
   return;

invalid_pname:
   _mesa_error(ctx, GL_INVALID_ENUM, "glGetSamplerParameterIuiv(pname=%s)",
               _mesa_lookup_enum_by_nr(pname));
}


void
_mesa_init_sampler_object_functions(struct dd_function_table *driver)
{
   driver->NewSamplerObject = _mesa_new_sampler_object;
   driver->DeleteSamplerObject = _mesa_delete_sampler_object;
}
