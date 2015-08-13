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


/**
 * \file state.c
 * State management.
 * 
 * This file manages recalculation of derived values in struct gl_context.
 */


#include "glheader.h"
#include "mtypes.h"
#include "arrayobj.h"
#include "context.h"
#include "debug.h"
#include "macros.h"
//#include "ffvertex_prog.h"
#include "framebuffer.h"
//#include "light.h"
#include "matrix.h"
#include "pixel.h"
//#include "program/program.h"
//#include "program/prog_parameter.h"
//#include "shaderobj.h"
#include "state.h"
#include "stencil.h"
//#include "texenvprogram.h"
#include "texobj.h"
#include "texstate.h"
#include "varray.h"
#include "viewport.h"
#include "blend.h"
#include "shader.h"


/**
 * Update the following fields:
 *   ctx->VertexProgram._Enabled
 *   ctx->FragmentProgram._Enabled
 *   ctx->ATIFragmentShader._Enabled
 * This needs to be done before texture state validation.
 */
static void
update_program_enables(struct gl_context *ctx)
{
   /* These _Enabled flags indicate if the user-defined ARB/NV vertex/fragment
    * program is enabled AND valid.  Similarly for ATI fragment shaders.
    * GLSL shaders not relevant here.
    */
//   ctx->VertexProgram._Enabled = ctx->VertexProgram.Enabled
//      && ctx->VertexProgram.Current->Base.Instructions;
//   ctx->FragmentProgram._Enabled = ctx->FragmentProgram.Enabled
//      && ctx->FragmentProgram.Current->Base.Instructions;
//   ctx->ATIFragmentShader._Enabled = ctx->ATIFragmentShader.Enabled
//      && ctx->ATIFragmentShader.Current->Instructions[0];
}



/**
 * Update the ctx->Polygon._FrontBit flag.
 */
static void
update_frontbit(struct gl_context *ctx)
{
//   if (ctx->Transform.ClipOrigin == GL_LOWER_LEFT)
//      ctx->Polygon._FrontBit = (ctx->Polygon.FrontFace == GL_CW);
//   else
      ctx->Polygon._FrontBit = (ctx->Polygon.FrontFace == GL_CCW);
}


/**
 * Update derived multisample state.
 */
static void
update_multisample(struct gl_context *ctx)
{
   ctx->Multisample._Enabled = GL_FALSE;
   if (ctx->Multisample.Enabled &&
       ctx->DrawBuffer &&
       ctx->DrawBuffer->Visual.sampleBuffers)
      ctx->Multisample._Enabled = GL_TRUE;
}


/**
 * Compute derived GL state.
 * If __struct gl_contextRec::NewState is non-zero then this function \b must
 * be called before rendering anything.
 *
 * Calls dd_function_table::UpdateState to perform any internal state
 * management necessary.
 * 
 * \sa _mesa_update_modelview_project(), _mesa_update_texture(),
 * _mesa_update_buffer_bounds(),
 * _mesa_update_lighting() and _mesa_update_tnl_spaces().
 */
void
_mesa_update_state_locked( struct gl_context *ctx )
{
   GLbitfield new_state = ctx->NewState;
   GLbitfield prog_flags = _NEW_PROGRAM;
   GLbitfield new_prog_state = 0x0;

   if (new_state == _NEW_CURRENT_ATTRIB) 
      goto out;

   if (MESA_VERBOSE & VERBOSE_STATE)
      _mesa_print_state("_mesa_update_state", new_state);

   /* Determine which state flags effect vertex/fragment program state */
//   if (ctx->FragmentProgram._MaintainTexEnvProgram) {
//      prog_flags |= (_NEW_BUFFERS | _NEW_TEXTURE | _NEW_FOG |
//		     _NEW_VARYING_VP_INPUTS | _NEW_LIGHT | _NEW_POINT |
//		     _NEW_RENDERMODE | _NEW_PROGRAM | _NEW_FRAG_CLAMP |
//		     _NEW_COLOR);
//   }
//   if (ctx->VertexProgram._MaintainTnlProgram) {
//      prog_flags |= (_NEW_VARYING_VP_INPUTS | _NEW_TEXTURE |
//                     _NEW_TEXTURE_MATRIX | _NEW_TRANSFORM | _NEW_POINT |
//                     _NEW_FOG | _NEW_LIGHT |
//                     _MESA_NEW_NEED_EYE_COORDS);
//   }

   /*
    * Now update derived state info
    */

//   if (new_state & prog_flags)
//	   _gl3ds_update_program(ctx);

   if (new_state & (_NEW_MODELVIEW|_NEW_PROJECTION))
      _mesa_update_modelview_project( ctx, new_state );

   if (new_state & (_NEW_PROGRAM|_NEW_TEXTURE|_NEW_TEXTURE_MATRIX))
      _mesa_update_texture( ctx, new_state );

   if (new_state & _NEW_POLYGON)
      update_frontbit( ctx );

   if (new_state & _NEW_BUFFERS)
      _mesa_update_framebuffer(ctx, ctx->ReadBuffer, ctx->DrawBuffer);

   if (new_state & (_NEW_SCISSOR | _NEW_BUFFERS | _NEW_VIEWPORT))
      _mesa_update_draw_buffer_bounds(ctx, ctx->DrawBuffer);

//   if (new_state & _NEW_LIGHT)
//      _mesa_update_lighting( ctx );

//   if (new_state & (_NEW_LIGHT | _NEW_PROGRAM))
//      update_twoside( ctx );

   if (new_state & (_NEW_STENCIL | _NEW_BUFFERS))
      _mesa_update_stencil( ctx );

   if (new_state & _NEW_PIXEL)
      _mesa_update_pixel( ctx, new_state );

   if (new_state & (_NEW_MULTISAMPLE | _NEW_BUFFERS))
      update_multisample( ctx );

   /* ctx->_NeedEyeCoords is now up to date.
    *
    * If the truth value of this variable has changed, update for the
    * new lighting space and recompute the positions of lights and the
    * normal transform.
    *
    * If the lighting space hasn't changed, may still need to recompute
    * light positions & normal transforms for other reasons.
    */
//   if (new_state & _MESA_NEW_NEED_EYE_COORDS)
//      _mesa_update_tnl_spaces( ctx, new_state );

//   if (new_state & prog_flags) {
      /* When we generate programs from fixed-function vertex/fragment state
       * this call may generate/bind a new program.  If so, we need to
       * propogate the _NEW_PROGRAM flag to the driver.
       */
//      new_prog_state |= update_program( ctx );
//   }

   if (new_state & _NEW_ARRAY)
      _mesa_update_vao_client_arrays(ctx, ctx->Array.VAO);

 out:
//   new_prog_state |= update_program_constants(ctx);

   /*
    * Give the driver a chance to act upon the new_state flags.
    * The driver might plug in different span functions, for example.
    * Also, this is where the driver can invalidate the state of any
    * active modules (such as swrast_setup, swrast, tnl, etc).
    *
    * Set ctx->NewState to zero to avoid recursion if
    * Driver.UpdateState() has to call FLUSH_VERTICES().  (fixed?)
    */
   new_state = ctx->NewState | new_prog_state;
   ctx->NewState = 0;
   ctx->Driver.UpdateState(ctx, new_state);
   ctx->Array.VAO->NewArrays = 0x0;
}


/* This is the usual entrypoint for state updates:
 */
void
_mesa_update_state( struct gl_context *ctx )
{
   _mesa_lock_context_textures(ctx);
   _mesa_update_state_locked(ctx);
   _mesa_unlock_context_textures(ctx);
}




/**
 * Want to figure out which fragment program inputs are actually
 * constant/current values from ctx->Current.  These should be
 * referenced as a tracked state variable rather than a fragment
 * program input, to save the overhead of putting a constant value in
 * every submitted vertex, transferring it to hardware, interpolating
 * it across the triangle, etc...
 *
 * When there is a VP bound, just use vp->outputs.  But when we're
 * generating vp from fixed function state, basically want to
 * calculate:
 *
 * vp_out_2_fp_in( vp_in_2_vp_out( varying_inputs ) | 
 *                 potential_vp_outputs )
 *
 * Where potential_vp_outputs is calculated by looking at enabled
 * texgen, etc.
 * 
 * The generated fragment program should then only declare inputs that
 * may vary or otherwise differ from the ctx->Current values.
 * Otherwise, the fp should track them as state values instead.
 */
void
_mesa_set_varying_vp_inputs( struct gl_context *ctx,
                             GLbitfield64 varying_inputs )
{
   if (ctx->varying_vp_inputs != varying_inputs) {
      ctx->varying_vp_inputs = varying_inputs;

      /* Only the fixed-func generated programs need to use the flag
       * and the fixed-func fragment program uses it only if there is also
       * a fixed-func vertex program, so this only depends on the latter.
       *
       * It's okay to check the VP pointer here, because this is called after
       * _mesa_update_state in the vbo module. */
//      if (ctx->VertexProgram._TnlProgram ||
//          ctx->FragmentProgram._TexEnvProgram) {
//         ctx->NewState |= _NEW_VARYING_VP_INPUTS;
//      }
      /*printf("%s %x\n", __func__, varying_inputs);*/
   }
}


/**
 * Used by drivers to tell core Mesa that the driver is going to
 * install/ use its own vertex program.  In particular, this will
 * prevent generated fragment programs from using state vars instead
 * of ordinary varyings/inputs.
 */
void
_mesa_set_vp_override(struct gl_context *ctx, GLboolean flag)
{
//   if (ctx->VertexProgram._Overriden != flag) {
//      ctx->VertexProgram._Overriden = flag;
//
//      /* Set one of the bits which will trigger fragment program
//       * regeneration:
//       */
//      ctx->NewState |= _NEW_PROGRAM;
//   }
}
