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
 * \file matrix.c
 * Matrix operations.
 *
 * \note
 * -# 4x4 transformation matrices are stored in memory in column major order.
 * -# Points/vertices are to be thought of as column vectors.
 * -# Transformation of a point p by a matrix M is: p' = M * p
 */


#include "glheader.h"
#include "imports.h"
#include "context.h"
#include "enums.h"
#include "macros.h"
#include "matrix.h"
#include "mtypes.h"
#include "math/m_matrix.h"

/**
 * Apply a perspective projection matrix.
 *
 * \param left left clipping plane coordinate.
 * \param right right clipping plane coordinate.
 * \param bottom bottom clipping plane coordinate.
 * \param top top clipping plane coordinate.
 * \param nearval distance to the near clipping plane.
 * \param farval distance to the far clipping plane.
 *
 * \sa glFrustum().
 *
 * Flushes vertices and validates parameters. Calls _math_matrix_frustum() with
 * the top matrix of the current matrix stack and sets
 * __struct gl_contextRec::NewState.
 */
void glFrustum( GLdouble left, GLdouble right,
			   GLdouble bottom, GLdouble top,
			   GLdouble nearval, GLdouble farval )
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);

   if (nearval <= 0.0 ||
	   farval <= 0.0 ||
	   nearval == farval ||
	   left == right ||
	   top == bottom)
   {
	  _mesa_error( ctx,  GL_INVALID_VALUE, "glFrustum" );
	  return;
   }

   _math_matrix_frustum( ctx->CurrentStack->Top,
						 (GLfloat) left, (GLfloat) right,
			 (GLfloat) bottom, (GLfloat) top,
			 (GLfloat) nearval, (GLfloat) farval );
   ctx->NewState |= ctx->CurrentStack->DirtyFlag;
}


/**
 * Apply an orthographic projection matrix.
 *
 * \param left left clipping plane coordinate.
 * \param right right clipping plane coordinate.
 * \param bottom bottom clipping plane coordinate.
 * \param top top clipping plane coordinate.
 * \param nearval distance to the near clipping plane.
 * \param farval distance to the far clipping plane.
 *
 * \sa glOrtho().
 *
 * Flushes vertices and validates parameters. Calls _math_matrix_ortho() with
 * the top matrix of the current matrix stack and sets
 * __struct gl_contextRec::NewState.
 */
void glOrtho( GLdouble left, GLdouble right,
			 GLdouble bottom, GLdouble top,
			 GLdouble nearval, GLdouble farval )
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);

   if (MESA_VERBOSE & VERBOSE_API)
	  _mesa_debug(ctx, "glOrtho(%f, %f, %f, %f, %f, %f)\n",
				  left, right, bottom, top, nearval, farval);

   if (left == right ||
	   bottom == top ||
	   nearval == farval)
   {
	  _mesa_error( ctx,  GL_INVALID_VALUE, "glOrtho" );
	  return;
   }

   _math_matrix_ortho( ctx->CurrentStack->Top,
					   (GLfloat) left, (GLfloat) right,
			   (GLfloat) bottom, (GLfloat) top,
			   (GLfloat) nearval, (GLfloat) farval );
   ctx->NewState |= ctx->CurrentStack->DirtyFlag;
}


/**
 * Set the current matrix stack.
 *
 * \param mode matrix stack.
 *
 * \sa glMatrixMode().
 *
 * Flushes the vertices, validates the parameter and updates
 * __struct gl_contextRec::CurrentStack and gl_transform_attrib::MatrixMode
 * with the specified matrix stack.
 */
void glMatrixMode( GLenum mode )
{
   GET_CURRENT_CONTEXT(ctx);

   if (ctx->Transform.MatrixMode == mode && mode != GL_TEXTURE)
	  return;
   FLUSH_VERTICES(ctx, _NEW_TRANSFORM);

   switch (mode) {
   case GL_MODELVIEW:
	  ctx->CurrentStack = &ctx->ModelviewMatrixStack;
	  break;
   case GL_PROJECTION:
	  ctx->CurrentStack = &ctx->ProjectionMatrixStack;
	  break;
   case GL_TEXTURE:
	  /* This error check is disabled because if we're called from
	   * glPopAttrib() when the active texture unit is >= MaxTextureCoordUnits
	   * we'll generate an unexpected error.
	   * From the GL_ARB_vertex_shader spec it sounds like we should instead
	   * do error checking in other places when we actually try to access
	   * texture matrices beyond MaxTextureCoordUnits.
	   */
#if 0
	  if (ctx->Texture.CurrentUnit >= ctx->Const.MaxTextureCoordUnits) {
		 _mesa_error(ctx, GL_INVALID_OPERATION,
					 "glMatrixMode(invalid tex unit %d)",
					 ctx->Texture.CurrentUnit);
		 return;
	  }
#endif
	  assert(ctx->Texture.CurrentUnit < ARRAY_SIZE(ctx->TextureMatrixStack));
	  ctx->CurrentStack = &ctx->TextureMatrixStack[ctx->Texture.CurrentUnit];
	  break;
   default:
	  _mesa_error( ctx,  GL_INVALID_ENUM, "glMatrixMode(mode)" );
	  return;
   }

   ctx->Transform.MatrixMode = mode;
}


/**
 * Push the current matrix stack.
 *
 * \sa glPushMatrix().
 *
 * Verifies the current matrix stack is not full, and duplicates the top-most
 * matrix in the stack.
 * Marks __struct gl_contextRec::NewState with the stack dirty flag.
 */
void glPushMatrix( void )
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_matrix_stack *stack = ctx->CurrentStack;

   if (MESA_VERBOSE&VERBOSE_API)
	  _mesa_debug(ctx, "glPushMatrix %s\n",
				  _mesa_lookup_enum_by_nr(ctx->Transform.MatrixMode));

   if (stack->Depth + 1 >= stack->MaxDepth) {
	  if (ctx->Transform.MatrixMode == GL_TEXTURE) {
		 _mesa_error(ctx,  GL_STACK_OVERFLOW,
					 "glPushMatrix(mode=GL_TEXTURE, unit=%d)",
					  ctx->Texture.CurrentUnit);
	  }
	  else {
		 _mesa_error(ctx,  GL_STACK_OVERFLOW, "glPushMatrix(mode=?)");
	  }
	  return;
   }
   _math_matrix_copy( &stack->Stack[stack->Depth + 1],
					  &stack->Stack[stack->Depth] );
   stack->Depth++;
   stack->Top = &(stack->Stack[stack->Depth]);
   ctx->NewState |= stack->DirtyFlag;
}


/**
 * Pop the current matrix stack.
 *
 * \sa glPopMatrix().
 *
 * Flushes the vertices, verifies the current matrix stack is not empty, and
 * moves the stack head down.
 * Marks __struct gl_contextRec::NewState with the dirty stack flag.
 */
void glPopMatrix( void )
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_matrix_stack *stack = ctx->CurrentStack;

   FLUSH_VERTICES(ctx, 0);

   if (MESA_VERBOSE&VERBOSE_API)
	  _mesa_debug(ctx, "glPopMatrix %s\n",
				  _mesa_lookup_enum_by_nr(ctx->Transform.MatrixMode));

   if (stack->Depth == 0) {
	  if (ctx->Transform.MatrixMode == GL_TEXTURE) {
		 _mesa_error(ctx,  GL_STACK_UNDERFLOW,
					 "glPopMatrix(mode=GL_TEXTURE, unit=%d)",
					  ctx->Texture.CurrentUnit);
	  }
	  else {
		 _mesa_error(ctx,  GL_STACK_UNDERFLOW, "glPopMatrix(mode=?)");
	  }
	  return;
   }
   stack->Depth--;
   stack->Top = &(stack->Stack[stack->Depth]);
   ctx->NewState |= stack->DirtyFlag;
}


/**
 * Replace the current matrix with the identity matrix.
 *
 * \sa glLoadIdentity().
 *
 * Flushes the vertices and calls _math_matrix_set_identity() with the
 * top-most matrix in the current stack.
 * Marks __struct gl_contextRec::NewState with the stack dirty flag.
 */
void glLoadIdentity( void )
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);

   if (MESA_VERBOSE & VERBOSE_API)
	  _mesa_debug(ctx, "glLoadIdentity()\n");

   _math_matrix_set_identity( ctx->CurrentStack->Top );
   ctx->NewState |= ctx->CurrentStack->DirtyFlag;
}


/**
 * Replace the current matrix with a given matrix.
 *
 * \param m matrix.
 *
 * \sa glLoadMatrixf().
 *
 * Flushes the vertices and calls _math_matrix_loadf() with the top-most
 * matrix in the current stack and the given matrix.
 * Marks __struct gl_contextRec::NewState with the dirty stack flag.
 */
void glLoadMatrixf( const GLfloat *m )
{
   GET_CURRENT_CONTEXT(ctx);
   if (!m) return;
   if (MESA_VERBOSE & VERBOSE_API)
	  _mesa_debug(ctx,
		  "glLoadMatrix(%f %f %f %f, %f %f %f %f, %f %f %f %f, %f %f %f %f\n",
		  m[0], m[4], m[8], m[12],
		  m[1], m[5], m[9], m[13],
		  m[2], m[6], m[10], m[14],
		  m[3], m[7], m[11], m[15]);

   FLUSH_VERTICES(ctx, 0);
   _math_matrix_loadf( ctx->CurrentStack->Top, m );
   ctx->NewState |= ctx->CurrentStack->DirtyFlag;
}


/**
 * Multiply the current matrix with a given matrix.
 *
 * \param m matrix.
 *
 * \sa glMultMatrixf().
 *
 * Flushes the vertices and calls _math_matrix_mul_floats() with the top-most
 * matrix in the current stack and the given matrix. Marks
 * __struct gl_contextRec::NewState with the dirty stack flag.
 */
void glMultMatrixf( const GLfloat *m )
{
   GET_CURRENT_CONTEXT(ctx);
   if (!m) return;
   if (MESA_VERBOSE & VERBOSE_API)
	  _mesa_debug(ctx,
		  "glMultMatrix(%f %f %f %f, %f %f %f %f, %f %f %f %f, %f %f %f %f\n",
		  m[0], m[4], m[8], m[12],
		  m[1], m[5], m[9], m[13],
		  m[2], m[6], m[10], m[14],
		  m[3], m[7], m[11], m[15]);

   FLUSH_VERTICES(ctx, 0);
   _math_matrix_mul_floats( ctx->CurrentStack->Top, m );
   ctx->NewState |= ctx->CurrentStack->DirtyFlag;
}


/**
 * Multiply the current matrix with a rotation matrix.
 *
 * \param angle angle of rotation, in degrees.
 * \param x rotation vector x coordinate.
 * \param y rotation vector y coordinate.
 * \param z rotation vector z coordinate.
 *
 * \sa glRotatef().
 *
 * Flushes the vertices and calls _math_matrix_rotate() with the top-most
 * matrix in the current stack and the given parameters. Marks
 * __struct gl_contextRec::NewState with the dirty stack flag.
 */
void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);
   if (angle != 0.0F) {
	  _math_matrix_rotate( ctx->CurrentStack->Top, angle, x, y, z);
	  ctx->NewState |= ctx->CurrentStack->DirtyFlag;
   }
}


/**
 * Multiply the current matrix with a general scaling matrix.
 *
 * \param x x axis scale factor.
 * \param y y axis scale factor.
 * \param z z axis scale factor.
 *
 * \sa glScalef().
 *
 * Flushes the vertices and calls _math_matrix_scale() with the top-most
 * matrix in the current stack and the given parameters. Marks
 * __struct gl_contextRec::NewState with the dirty stack flag.
 */
void glScalef( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);
   _math_matrix_scale( ctx->CurrentStack->Top, x, y, z);
   ctx->NewState |= ctx->CurrentStack->DirtyFlag;
}


/**
 * Multiply the current matrix with a translation matrix.
 *
 * \param x translation vector x coordinate.
 * \param y translation vector y coordinate.
 * \param z translation vector z coordinate.
 *
 * \sa glTranslatef().
 *
 * Flushes the vertices and calls _math_matrix_translate() with the top-most
 * matrix in the current stack and the given parameters. Marks
 * __struct gl_contextRec::NewState with the dirty stack flag.
 */
void glTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);
   _math_matrix_translate( ctx->CurrentStack->Top, x, y, z);
   ctx->NewState |= ctx->CurrentStack->DirtyFlag;
}


void glLoadMatrixd( const GLdouble *m )
{
   GLint i;
   GLfloat f[16];
   if (!m) return;
   for (i = 0; i < 16; i++)
	  f[i] = (GLfloat) m[i];
   glLoadMatrixf(f);
}

void glMultMatrixd( const GLdouble *m )
{
   GLint i;
   GLfloat f[16];
   if (!m) return;
   for (i = 0; i < 16; i++)
	  f[i] = (GLfloat) m[i];
   glMultMatrixf( f );
}


void glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
   glRotatef((GLfloat) angle, (GLfloat) x, (GLfloat) y, (GLfloat) z);
}


void glScaled( GLdouble x, GLdouble y, GLdouble z )
{
   glScalef((GLfloat) x, (GLfloat) y, (GLfloat) z);
}


void glTranslated( GLdouble x, GLdouble y, GLdouble z )
{
   glTranslatef((GLfloat) x, (GLfloat) y, (GLfloat) z);
}


void glLoadTransposeMatrixf( const GLfloat *m )
{
   GLfloat tm[16];
   if (!m) return;
   _math_transposef(tm, m);
   glLoadMatrixf(tm);
}


void glLoadTransposeMatrixd( const GLdouble *m )
{
   GLfloat tm[16];
   if (!m) return;
   _math_transposefd(tm, m);
   glLoadMatrixf(tm);
}


void glMultTransposeMatrixf( const GLfloat *m )
{
   GLfloat tm[16];
   if (!m) return;
   _math_transposef(tm, m);
   glMultMatrixf(tm);
}


void glMultTransposeMatrixd( const GLdouble *m )
{
   GLfloat tm[16];
   if (!m) return;
   _math_transposefd(tm, m);
   glMultMatrixf(tm);
}

/**********************************************************************/
/** \name State management */
/*@{*/


/**
 * Update the projection matrix stack.
 *
 * \param ctx GL context.
 *
 * Calls _math_matrix_analyse() with the top-matrix of the projection matrix
 * stack, and recomputes user clip positions if necessary.
 *
 * \note This routine references __struct gl_contextRec::Tranform attribute
 * values to compute userclip positions in clip space, but is only called on
 * _NEW_PROJECTION.  The _mesa_ClipPlane() function keeps these values up to
 * date across changes to the __struct gl_contextRec::Transform attributes.
 */
static void
update_projection( struct gl_context *ctx )
{
   _math_matrix_analyse( ctx->ProjectionMatrixStack.Top );

   /* Recompute clip plane positions in clipspace.  This is also done
	* in _mesa_ClipPlane().
	*/
   if (ctx->Transform.ClipPlanesEnabled) {
	  GLuint p;
	  for (p = 0; p < ctx->Const.MaxClipPlanes; p++) {
	 if (ctx->Transform.ClipPlanesEnabled & (1 << p)) {
		_mesa_transform_vector( ctx->Transform._ClipUserPlane[p],
				 ctx->Transform.EyeUserPlane[p],
				 ctx->ProjectionMatrixStack.Top->inv );
	 }
	  }
   }
}


/**
 * Calculate the combined modelview-projection matrix.
 *
 * \param ctx GL context.
 *
 * Multiplies the top matrices of the projection and model view stacks into
 * __struct gl_contextRec::_ModelProjectMatrix via _math_matrix_mul_matrix()
 * and analyzes the resulting matrix via _math_matrix_analyse().
 */
static void
calculate_model_project_matrix( struct gl_context *ctx )
{
   _math_matrix_mul_matrix( &ctx->_ModelProjectMatrix,
							ctx->ProjectionMatrixStack.Top,
							ctx->ModelviewMatrixStack.Top );

   _math_matrix_analyse( &ctx->_ModelProjectMatrix );
}


/**
 * Updates the combined modelview-projection matrix.
 *
 * \param ctx GL context.
 * \param new_state new state bit mask.
 *
 * If there is a new model view matrix then analyzes it. If there is a new
 * projection matrix, updates it. Finally calls
 * calculate_model_project_matrix() to recalculate the modelview-projection
 * matrix.
 */
void _mesa_update_modelview_project( struct gl_context *ctx, GLuint new_state )
{
   if (new_state & _NEW_MODELVIEW)
	  _math_matrix_analyse( ctx->ModelviewMatrixStack.Top );

   if (new_state & _NEW_PROJECTION)
	  update_projection( ctx );

   /* Keep ModelviewProject up to date always to allow tnl
	* implementations that go model->clip even when eye is required.
	*/
   calculate_model_project_matrix(ctx);
}

/*@}*/

/**********************************************************************/
/** Matrix stack initialization */
/*@{*/


/**
 * Initialize a matrix stack.
 *
 * \param stack matrix stack.
 * \param maxDepth maximum stack depth.
 * \param dirtyFlag dirty flag.
 *
 * Allocates an array of \p maxDepth elements for the matrix stack and calls
 * _math_matrix_ctr() for each element to initialize it.
 */
static void
init_matrix_stack( struct gl_matrix_stack *stack,
				   GLuint maxDepth, GLuint dirtyFlag )
{
   GLuint i;

   stack->Depth = 0;
   stack->MaxDepth = maxDepth;
   stack->DirtyFlag = dirtyFlag;
   /* The stack */
   stack->Stack = calloc(maxDepth, sizeof(GLmatrix));
   for (i = 0; i < maxDepth; i++) {
	  _math_matrix_ctr(&stack->Stack[i]);
   }
   stack->Top = stack->Stack;
}

/**
 * Free matrix stack.
 *
 * \param stack matrix stack.
 *
 * Calls _math_matrix_dtr() for each element of the matrix stack and
 * frees the array.
 */
static void
free_matrix_stack( struct gl_matrix_stack *stack )
{
   GLuint i;
   for (i = 0; i < stack->MaxDepth; i++) {
	  _math_matrix_dtr(&stack->Stack[i]);
   }
   free(stack->Stack);
   stack->Stack = stack->Top = NULL;
}

/*@}*/

/**********************************************************************/
/** \name Initialization */
/*@{*/


/**
 * Initialize the context matrix data.
 *
 * \param ctx GL context.
 *
 * Initializes each of the matrix stacks and the combined modelview-projection
 * matrix.
 */
void _mesa_init_matrix( struct gl_context * ctx )
{
	GLuint i;

	/* Initialize matrix stacks */
	init_matrix_stack(&ctx->ModelviewMatrixStack, MAX_MODELVIEW_STACK_DEPTH,
					  _NEW_MODELVIEW);
	init_matrix_stack(&ctx->ProjectionMatrixStack, MAX_PROJECTION_STACK_DEPTH,
					  _NEW_PROJECTION);
	for (i = 0; i < ARRAY_SIZE(ctx->TextureMatrixStack); i++)
		init_matrix_stack(&ctx->TextureMatrixStack[i], MAX_TEXTURE_STACK_DEPTH,
						  _NEW_TEXTURE_MATRIX);
//	for (i = 0; i < ARRAY_SIZE(ctx->ProgramMatrixStack); i++)
//		init_matrix_stack(&ctx->ProgramMatrixStack[i],
//						  MAX_PROGRAM_MATRIX_STACK_DEPTH, _NEW_TRACK_MATRIX);
	ctx->CurrentStack = &ctx->ModelviewMatrixStack;

	/* Init combined Modelview*Projection matrix */
	_math_matrix_ctr( &ctx->_ModelProjectMatrix );
}


/**
 * Free the context matrix data.
 *
 * \param ctx GL context.
 *
 * Frees each of the matrix stacks and the combined modelview-projection
 * matrix.
 */
void _mesa_free_matrix_data( struct gl_context *ctx )
{
	GLuint i;

	free_matrix_stack(&ctx->ModelviewMatrixStack);
	free_matrix_stack(&ctx->ProjectionMatrixStack);
	for (i = 0; i < ARRAY_SIZE(ctx->TextureMatrixStack); i++)
		free_matrix_stack(&ctx->TextureMatrixStack[i]);
//	for (i = 0; i < ARRAY_SIZE(ctx->ProgramMatrixStack); i++)
//		free_matrix_stack(&ctx->ProgramMatrixStack[i]);
	/* combined Modelview*Projection matrix */
	_math_matrix_dtr( &ctx->_ModelProjectMatrix );

}


/**
 * Initialize the context transform attribute group.
 *
 * \param ctx GL context.
 *
 * \todo Move this to a new file with other 'transform' routines.
 */
void _mesa_init_transform( struct gl_context *ctx )
{
	GLuint i;

	/* Transformation group */
	ctx->Transform.MatrixMode = GL_MODELVIEW;
	ctx->Transform.Normalize = GL_FALSE;
	ctx->Transform.RescaleNormals = GL_FALSE;
	ctx->Transform.RasterPositionUnclipped = GL_FALSE;
//	for (i=0;i<ctx->Const.MaxClipPlanes;i++) {
//		ASSIGN_4V( ctx->Transform.EyeUserPlane[i], 0.0, 0.0, 0.0, 0.0 );
//	}
	ctx->Transform.ClipPlanesEnabled = 0;
}

static void rotate_projection(GLmatrix *mat)
{
	// Rotate 90 degree clockwise (3DS screens are sideways)
	GLfloat fix[16] = {
			0, -1,  0,  0,
			1,  0,  0,  0,
			0,  0,  1,  0,
			0,  0,  0,  1
	};
	_math_matrix_mul_floats(mat, fix);
}

void _gl3ds_upload_matrix(GLmatrix *mat, GLint uniform, bool rotate)
{
	if (rotate)
		rotate_projection(mat);

	if (mat->flags & MAT_NEED_TRANSPOSE) {
		GLfloat copy[16];
		_math_transposef(copy, mat->m);
		memcpy(mat->m, copy, sizeof(GLfloat)*16);
		mat->flags &= ~MAT_NEED_TRANSPOSE;
	}

	// TODO: Flip xyzw positions? Why not in shader?
	GLfloat swapped[16];
	swapped[0x0] = mat->m[3];
	swapped[0x1] = mat->m[2];
	swapped[0x2] = mat->m[1];
	swapped[0x3] = mat->m[0];

	swapped[0x4] = mat->m[7];
	swapped[0x5] = mat->m[6];
	swapped[0x6] = mat->m[5];
	swapped[0x7] = mat->m[4];

	swapped[0x8] = mat->m[11];
	swapped[0x9] = mat->m[10];
	swapped[0xa] = mat->m[9];
	swapped[0xb] = mat->m[8];

	swapped[0xc] = mat->m[15];
	swapped[0xd] = mat->m[14];
	swapped[0xe] = mat->m[13];
	swapped[0xf] = mat->m[12];

//	printf("\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
//			mat->m[0], mat->m[1], mat->m[2], mat->m[3],
//		   mat->m[4], mat->m[5], mat->m[6], mat->m[7],
//		   mat->m[8], mat->m[9], mat->m[10], mat->m[11],
//		   mat->m[12], mat->m[13], mat->m[14], mat->m[15]);

//	glUniformMatrix4fv(uniform, 1, GL_FALSE, swapped);
	glUniformMatrix4fv(uniform, 1, GL_TRUE, mat->m);
}
