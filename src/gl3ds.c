#include "context.h"
#include "glheader.h"
#include "shader.h"
#include "drivers/driverfuncs.h"
#include "matrix.h"
#include "polygon.h"
#include "viewport.h"
#include "blend.h"
#include "scissor.h"
#include "framebuffer.h"
#include "state.h"
#include "drivers/s_context.h"
#include "texstate.h"
#include "texobj.h"
#include "teximage.h"
#include "texstore.h"
#include "depth.h"
#include "mtypes.h"

#define RGBA8(r, g, b, a) ((((r)&0xFF)<<24) | (((g)&0xFF)<<16) | (((b)&0xFF)<<8) | (((a)&0xFF)<<0))

//Apt hook cookie
static aptHookCookie apt_hook_cookie;


// Called by glClear
void gl3ds_Clear(struct gl_context *ctx, GLbitfield mask) {
	// TODO: implement masks
	u32 color = RGBA8((char)ctx->Color.ClearColor.i[0], (char)ctx->Color.ClearColor.i[1], (char)ctx->Color.ClearColor.i[2], (char)ctx->Color.ClearColor.i[3]);
	GX_SetMemoryFill(NULL, ctx->FrameBuffer, color, &ctx->FrameBuffer[0x2EE00], 0x201,
					 ctx->DepthBuffer, 0x00000000, &ctx->DepthBuffer[0x2EE00], 0x201);
	gspWaitForPSC0();
}


// Called by glFlush
void gl3ds_Flush(struct gl_context *ctx)
{
	// TODO: flush everything
	_mesa_update_state(ctx);
}


static void apt_hook_func(int hook, void* param)
{
	GET_CURRENT_CONTEXT(ctx);
	if (currentContext == ctx) {
		if (hook == APTHOOK_ONSUSPEND) {
			GPUCMD_GetBuffer(&ctx->CommandBuffer, &ctx->CommandBufferSize, &ctx->CommandBufferOffset);
		}
		if (hook == APTHOOK_ONRESTORE) {
			GPU_Reset(NULL, ctx->CommandBuffer, ctx->CommandBufferSize);
			ctx->NewState = _NEW_PROGRAM;
			_gl3ds_update_program(ctx);
			GPUCMD_Finalize();
			GPUCMD_FlushAndRun(NULL);
			gspWaitForP3D();
			GPUCMD_SetBufferOffset(0);
			ctx->NewState = _NEW_VIEWPORT | _NEW_PROJECTION | _NEW_MODELVIEW;
		}
	}
}


static void
gl3ds_update_state( struct gl_context *ctx, GLuint new_state )
{
//	if (new_state & _NEW_PROGRAM) {
		_gl3ds_update_program(ctx);
//	}

	if (new_state & _NEW_VIEWPORT)
		_gl3ds_update_viewport(ctx);

	if (new_state & _NEW_SCISSOR)
		_gl3ds_update_scissor(ctx);

	if (new_state & _NEW_PROJECTION) {
//		printf("new proj");
		_gl3ds_upload_matrix(ctx->ProjectionMatrixStack.Top, ctx->Shared->Shader->ProjectionUniform, true);
	}

	if (new_state & _NEW_MODELVIEW) {
//		printf("new modelview");
		_gl3ds_upload_matrix(ctx->ModelviewMatrixStack.Top, ctx->Shared->Shader->ModelviewUniform, false);
	}

	if (new_state & _NEW_TEXTURE_MATRIX) {
		// TODO: Handle other texunits
		_gl3ds_upload_matrix(ctx->TextureMatrixStack[ctx->Texture.CurrentUnit].Top, ctx->Shared->Shader->TextureUniform, false);
	}
}

void make_current(struct gl_context *ctx)
{
	static bool reset = false;

	if (currentContext == ctx || ctx == NULL)
		return;

	if (currentContext) {
		GPUCMD_GetBuffer(&currentContext->CommandBuffer, &currentContext->CommandBufferSize, &currentContext->CommandBufferOffset);
	}

	if (!reset) {
		reset = true;
		//Register the apt callback hook
		aptHook(&apt_hook_cookie, apt_hook_func, NULL);
		GPU_Init(NULL);
		GPU_Reset(NULL, ctx->CommandBuffer, 0x40000);
	} else {
		GPUCMD_SetBuffer(ctx->CommandBuffer, ctx->CommandBufferSize, ctx->CommandBufferOffset);
	}

	_mesa_make_current(ctx, ctx->DrawBuffer, ctx->ReadBuffer);
}


void update_context(struct gl_context *ctx)
{
	_mesa_update_state(ctx); // Calls gl3ds_update_state

	// TODO: use functions from ctx->Driver to follow mesa's format?
//	_mesa_init_driver_state(ctx);

//	GPU_SetFaceCulling(GPU_CULL_BACK_CCW);
	GPU_SetFaceCulling(GPU_CULL_NONE);


	GPU_SetBlendingColor(ctx->Color.BlendColor[0],
						 ctx->Color.BlendColor[1],
						 ctx->Color.BlendColor[2],
						 ctx->Color.BlendColor[3]);

	_gl3ds_update_depth(ctx);

//	GPU_SetAlphaBlending(
//			GPU_BLEND_ADD,
//			GPU_BLEND_ADD,
//			GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA,
//			GPU_ONE, GPU_ZERO
//	);
	GPU_SetAlphaBlending(
			ctx->Color.Blend[0].EquationRGB,
			ctx->Color.Blend[0].EquationA,
			ctx->Color.Blend[0].SrcRGB, ctx->Color.Blend[0].DstRGB,
			ctx->Color.Blend[0].SrcA, ctx->Color.Blend[0].DstA
	);

	GPU_SetAlphaTest(ctx->Color.AlphaEnabled, ctx->Color.AlphaFunc, ctx->Color.AlphaRef);

	_gl3ds_update_polygon(ctx);

	int i;

	GPU_SetTexEnv(
			0,
			GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
			GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
			GPU_TEVOPERANDS(0, 0, 0),
			GPU_TEVOPERANDS(0, 0, 0),
//			GPU_MODULATE, GPU_MODULATE,
			GPU_REPLACE, GPU_REPLACE,
			0xFFFFFFFF
	);

//	GPU_SetTexEnv(
//			0,
//			GPU_TEVSOURCES(GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_CONSTANT),
//			GPU_TEVSOURCES(GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_CONSTANT),
//			GPU_TEVOPERANDS(GPU_SRC_COLOR, GPU_SRC_COLOR, GPU_SRC_ALPHA),
//			GPU_TEVOPERANDS(GPU_SRC_ALPHA, GPU_SRC_ALPHA, GPU_SRC_ALPHA),
//			GPU_MODULATE, GPU_MODULATE,
////			GPU_REPLACE, GPU_REPLACE,
//			0xFFFFFFFF
//	);

//	GPU_SetTexEnv(0,
//				  GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR), // RGB channels
//				  GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR), // Alpha
//				  GPU_TEVOPERANDS(0, 0, 0), // RGB
//				  GPU_TEVOPERANDS(0, 0, 0), // Alpha
//				  GPU_REPLACE, GPU_REPLACE, // RGB, Alpha
//				  0xFFFFFFFF);

	for (i = 1; i < 6; i++) {
//		struct gl_texture_unit *unit = &ctx->Texture.Unit[i];
//		struct gl_tex_env_combine_state* env = &unit->Combine;
//		GPU_SetTexEnv(i,
//					  GPU_TEVSOURCES(env->SourceRGB[0], env->SourceRGB[1], env->SourceRGB[2]),
//					  GPU_TEVSOURCES(env->SourceA[0], env->SourceA[1], env->SourceA[2]),
//					  GPU_TEVOPERANDS(env->OperandRGB[0], env->OperandRGB[1], env->OperandRGB[2]),
//					  GPU_TEVOPERANDS(env->OperandA[0], env->OperandA[1], env->OperandA[2]),
//					  env->ModeRGB,
//					  env->ModeA,
//					  0xFFFFFFFF); // TODO: Use unit->EnvColor[0]
		GPU_SetTexEnv(i,
					  GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
					  GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
					  GPU_TEVOPERANDS(0,0,0),
					  GPU_TEVOPERANDS(0,0,0),
					  GPU_REPLACE,
					  GPU_REPLACE,
					  0xFFFFFFFF);
	}

	GLubyte enabledTexUnits = 0x0;

	for (i = 0; i < 3; i++) {
//	for (i = 0; i < ctx->Const.MaxTextureUnits; i++) {
		struct gl_texture_unit *texUnit = &ctx->Texture.Unit[i];
		struct gl_texture_object *texObj = texUnit->CurrentTex[TEXTURE_2D_INDEX];
		struct swrast_texture_image *swImage = swrast_texture_image(_mesa_select_tex_image(texObj, GL_TEXTURE_2D, 0));
		if (swImage) {
			if (swImage->NeedsTiling) {
				swImage->NeedsTiling = GL_FALSE;
				imageTile32(swImage->TiledBuffer, swImage->Buffer, swImage->Base.Width, swImage->Base.Height);
			}
			GPU_SetTexture(
					(GPU_TEXUNIT) (1 << i),
					(u32 *) osConvertVirtToPhys((u32) (swImage->TiledBuffer)),
					(u16)swImage->Base.Width,
					(u16)swImage->Base.Height,
					GPU_TEXTURE_MAG_FILTER(GPU_NEAREST) | GPU_TEXTURE_MIN_FILTER(GPU_NEAREST) | GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE),
					GPU_RGBA8);
			enabledTexUnits |= (1 << i);
			printf("texunit(%d w:%d h:%d)\n", i, swImage->Base.Width, swImage->Base.Height);
		}

	}
	GPU_SetTextureEnable((GPU_TEXUNIT)enabledTexUnits);

	ctx->NewState = _NEW_VIEWPORT;
}


GLuint gl3ds_createContext(GLuint sharedContext, gfxScreen_t screen)
{
	struct gl_context* ctx = CALLOC_STRUCT(gl_context);
	struct gl_context* shared_ctx = NULL;
	struct dd_function_table* driverFunctions = CALLOC_STRUCT(dd_function_table);

	if (!ctx)
		return NULL;
	if (sharedContext > 0)
		shared_ctx = (struct gl_context*) sharedContext;

	struct gl_config *vis = _mesa_create_visual(GL_TRUE, GL_FALSE, 8, 8, 8, 8, 32, 8, 0, 0, 0, 0, 1);

	_mesa_init_driver_functions(driverFunctions);

	driverFunctions->GetString =   NULL;
	driverFunctions->UpdateState = gl3ds_update_state;
	driverFunctions->Clear =       gl3ds_Clear;
	driverFunctions->Flush =       gl3ds_Flush;

	if (_mesa_initialize_context(ctx, vis, shared_ctx, driverFunctions)) {
		ctx->ReadBuffer = _mesa_create_framebuffer(vis);
		ctx->DrawBuffer = _mesa_create_framebuffer(vis);
		ctx->RenderMode = GL_RENDER; // From feedback.c
		ctx->Screen = screen;
		return (GLuint) ctx;
	} else {
		free(ctx);
		return NULL;
	}

}


GLboolean gl3ds_makeCurrent(GLuint context)
{
	struct gl_context* ctx = (struct gl_context*) context;
	if (ctx) {
//		update_context(ctx);
		make_current(ctx);
//		ctx->NewState = _NEW_ALL;
		return GL_TRUE;
	} else
		return GL_FALSE;
}


void gl3ds_deleteContext(GLuint context)
{
	struct gl_context* ctx = (struct gl_context*) context;
	if (ctx) {
		if (currentContext == ctx)
			currentContext = NULL;

		// TODO: don't unhook if other contexts exist?
		aptUnhook(&apt_hook_cookie);
		_mesa_free_context_data(ctx);
		free((void*)ctx);
	}
}


#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

void gl3ds_flushContext(GLuint context)
{
	struct gl_context* ctx = (struct gl_context*) context;
//	ctx->NewState = _NEW_ALL;
//	update_context(ctx);
	GPU_FinishDrawing();
	GPUCMD_Finalize();
	GPUCMD_FlushAndRun(NULL);
	gspWaitForP3D();

	u32 dim = (ctx->Screen == GFX_TOP) ? GX_BUFFER_DIM(240, 400) : GX_BUFFER_DIM(240, 320);
	GX_SetDisplayTransfer(NULL, ctx->FrameBuffer, dim, (u32*)gfxGetFramebuffer(ctx->Screen, GFX_LEFT, NULL, NULL), dim, DISPLAY_TRANSFER_FLAGS);
	gspWaitForPPF();

	GPUCMD_SetBufferOffset(0);
}

void gl3ds_swapBuffers()
{
	gfxSwapBuffersGpu();
	gspWaitForEvent(GSPEVENT_VBlank0, true);
}
