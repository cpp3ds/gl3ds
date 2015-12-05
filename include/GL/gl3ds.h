#ifndef GL_H
#define GL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <3ds.h>
#include <stdint.h>

// Boolean values
#define GL_FALSE   0
#define GL_TRUE    1

// Image formats
#define GL_RGB                  GPU_RGB8
#define GL_RGBA                 GPU_RGBA8
//#define GL_RGBA8                GPU_RGBA8
#define GL_ETC1_RGB8_OES        GPU_ETC1

// glPush/PopAttrib bits
#define GL_CURRENT_BIT          0x00000001
#define GL_POINT_BIT            0x00000002
#define GL_LINE_BIT             0x00000004
#define GL_POLYGON_BIT          0x00000008
#define GL_POLYGON_STIPPLE_BIT  0x00000010
#define GL_PIXEL_MODE_BIT       0x00000020
#define GL_LIGHTING_BIT         0x00000040
#define GL_FOG_BIT              0x00000080
#define GL_DEPTH_BUFFER_BIT     0x00000100
#define GL_ACCUM_BUFFER_BIT     0x00000200
#define GL_STENCIL_BUFFER_BIT   0x00000400
#define GL_VIEWPORT_BIT         0x00000800
#define GL_TRANSFORM_BIT        0x00001000
#define GL_ENABLE_BIT           0x00002000
#define GL_COLOR_BUFFER_BIT     0x00004000
#define GL_HINT_BIT             0x00008000
#define GL_EVAL_BIT             0x00010000
#define GL_LIST_BIT             0x00020000
#define GL_TEXTURE_BIT          0x00040000
#define GL_SCISSOR_BIT          0x00080000
#define GL_MULTISAMPLE_BIT      0x20000000
#define GL_ALL_ATTRIB_BITS      0xFFFFFFFF

#define GL_CLIENT_PIXEL_STORE_BIT		0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT		0x00000002
#define GL_ALL_CLIENT_ATTRIB_BITS 		0xFFFFFFFF
#define GL_CLIENT_ALL_ATTRIB_BITS 		0xFFFFFFFF

#define GL_DYNAMIC_STORAGE_BIT            0x0100
#define GL_CLIENT_STORAGE_BIT             0x0200

// Map buffer
#define GL_ARRAY_BUFFER                   0x8892
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_MAP_READ_BIT                   0x0001
#define GL_MAP_WRITE_BIT                  0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT         0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT         0x0020
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

// Errors
#define GL_NO_ERROR             0
#define GL_INVALID_ENUM         0x0500
#define GL_INVALID_VALUE        0x0501
#define GL_INVALID_OPERATION    0x0502
#define GL_STACK_OVERFLOW       0x0503
#define GL_STACK_UNDERFLOW      0x0504
#define GL_OUT_OF_MEMORY        0x0505

// Shader
#define GL_GEOMETRY_SHADER        0x8DD9
#define GL_FRAGMENT_SHADER        0x8B30
#define GL_VERTEX_SHADER          0x8B31
#define GL_VERTEX_SHADER_BINARY   0x01
#define GL_GEOMETRY_SHADER_BINARY 0x02

// Texture mapping
#define GL_TEXTURE_ENV              0x2300
#define GL_TEXTURE_ENV_MODE         0x2200
#define GL_TEXTURE_1D               0x0DE0
#define GL_TEXTURE_2D               0x0DE1
#define GL_TEXTURE_WRAP_S			0x2802
#define GL_TEXTURE_WRAP_T			0x2803
#define GL_TEXTURE_WRAP_R           0x8072
#define GL_TEXTURE_DEPTH            0x8071
#define GL_TEXTURE_MAG_FILTER       0x2800
#define GL_TEXTURE_MIN_FILTER       0x2801
#define GL_TEXTURE_ENV_COLOR        0x2201
#define GL_TEXTURE_GEN_S            0x0C60
#define GL_TEXTURE_GEN_T            0x0C61
#define GL_TEXTURE_GEN_R            0x0C62
#define GL_TEXTURE_GEN_Q            0x0C63
#define GL_TEXTURE_GEN_MODE         0x2500
#define GL_TEXTURE_BORDER_COLOR     0x1004
#define GL_TEXTURE_WIDTH            0x1000
#define GL_TEXTURE_HEIGHT           0x1001
#define GL_TEXTURE_BORDER           0x1005
#define GL_TEXTURE_COMPONENTS       0x1003
#define GL_TEXTURE_RED_SIZE         0x805C
#define GL_TEXTURE_GREEN_SIZE       0x805D
#define GL_TEXTURE_BLUE_SIZE        0x805E
#define GL_TEXTURE_ALPHA_SIZE       0x805F
#define GL_TEXTURE_LUMINANCE_SIZE   0x8060
#define GL_TEXTURE_INTENSITY_SIZE   0x8061
#define GL_NEAREST_MIPMAP_NEAREST   0x2700
#define GL_NEAREST_MIPMAP_LINEAR    0x2702
#define GL_LINEAR_MIPMAP_NEAREST    0x2701
#define GL_LINEAR_MIPMAP_LINEAR     0x2703
#define GL_OBJECT_LINEAR            0x2401
#define GL_OBJECT_PLANE             0x2501
#define GL_EYE_LINEAR               0x2400
#define GL_EYE_PLANE                0x2502
#define GL_SPHERE_MAP               0x2402
#define GL_DECAL                    0x2101
#define GL_NEAREST                  0x2600
#define GL_LINEAR                   0x2601
#define GL_REPEAT                   0x2901
#define GL_CLAMP_TO_EDGE            0x812F
#define GL_CLAMP_TO_BORDER          0x812D
#define GL_CLAMP                    0x2900
#define GL_S                        0x2000
#define GL_T                        0x2001
#define GL_R                        0x2002
#define GL_Q                        0x2003
#define GL_SRC0_RGB                 0x8580
#define GL_SRC1_RGB                 0x8581
#define GL_SRC2_RGB                 0x8582

#define GL_COMBINE				0x8570
#define GL_COMBINE_RGB				0x8571
#define GL_COMBINE_ALPHA			0x8572
#define GL_SOURCE0_RGB				0x8580
#define GL_SOURCE1_RGB				0x8581
#define GL_SOURCE2_RGB				0x8582
#define GL_SOURCE0_ALPHA			0x8588
#define GL_SOURCE1_ALPHA			0x8589
#define GL_SOURCE2_ALPHA			0x858A
#define GL_OPERAND0_RGB				0x8590
#define GL_OPERAND1_RGB				0x8591
#define GL_OPERAND2_RGB				0x8592
#define GL_OPERAND0_ALPHA			0x8598
#define GL_OPERAND1_ALPHA			0x8599
#define GL_OPERAND2_ALPHA			0x859A
#define GL_RGB_SCALE				0x8573

#define GL_TEXTURE_FILTER_CONTROL         0x8500

#define GL_DEPTH_STENCIL                  0x84F9
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE   0x9101
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A

#define GL_TEXTURE_1D_ARRAY               0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY         0x8C19
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY         0x8C1B
#define GL_TEXTURE_CUBE_MAP_ARRAY         0x9009

#define GL_LIGHT_MODEL_COLOR_CONTROL      0x81F8
#define GL_SINGLE_COLOR                   0x81F9
#define GL_SEPARATE_SPECULAR_COLOR        0x81FA
#define GL_FIRST_VERTEX_CONVENTION        0x8E4D
#define GL_LAST_VERTEX_CONVENTION         0x8E4E
#define GL_PROVOKING_VERTEX               0x8E4F

// Buffer
#define GL_BUFFER_SIZE                    0x8764
#define GL_BUFFER_USAGE                   0x8765
#define GL_QUERY_COUNTER_BITS             0x8864
#define GL_CURRENT_QUERY                  0x8865
#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_ARRAY_BUFFER                   0x8892
#define GL_TEXTURE_BUFFER                 0x8C2A
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9

#define GL_BUFFER_ACCESS_FLAGS            0x911F
#define GL_BUFFER_MAP_LENGTH              0x9120
#define GL_BUFFER_MAP_OFFSET              0x9121
#define GL_BUFFER_IMMUTABLE_STORAGE       0x821F
#define GL_BUFFER_STORAGE_FLAGS           0x8220

/* Buffers, Pixel Drawing/Reading */
#define GL_NONE					0
#define GL_LEFT					0x0406
#define GL_RIGHT				0x0407

#define GL_BITMAP				0x1A00
#define GL_COLOR				0x1800
#define GL_DEPTH				0x1801
#define GL_STENCIL				0x1802
#define GL_DITHER				0x0BD0

#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_COLOR_ATTACHMENT0_EXT          0x8CE0

// Render Mode
//#define GL_FEEDBACK				0x1C01
#define GL_RENDER				0x1C00
//#define GL_SELECT				0x1C02

// Vertex Arrays
#define GL_VERTEX_ARRAY             0x8074
#define GL_NORMAL_ARRAY             0x8075
#define GL_COLOR_ARRAY              0x8076
#define GL_INDEX_ARRAY              0x8077
#define GL_TEXTURE_COORD_ARRAY      0x8078
#define GL_EDGE_FLAG_ARRAY_POINTER  0x8093

#define GL_VERTEX_ATTRIB_BINDING          0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET  0x82D5
#define GL_VERTEX_BINDING_DIVISOR         0x82D6
#define GL_VERTEX_BINDING_OFFSET          0x82D7
#define GL_VERTEX_BINDING_STRIDE          0x82D8
#define GL_VERTEX_BINDING_BUFFER          0x8F4F

#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB   0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB   0x8625
#define GL_CURRENT_VERTEX_ATTRIB_ARB      0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB  0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB    0x8643
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB 0x8645
#define GL_TEXTURE_COORD_ARRAY_SIZE   0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE   0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE 0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT  0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE     0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT      0x808D
#define GL_VERTEX_ARRAY_POINTER       0x808E
#define GL_NORMAL_ARRAY_POINTER       0x808F
#define GL_COLOR_ARRAY_POINTER        0x8090
#define GL_INDEX_ARRAY_POINTER        0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER    0x8093

#define GL_CURRENT_SECONDARY_COLOR_EXT    0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT 0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT 0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT 0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT 0x845D

#define GL_CURRENT_FOG_COORDINATE_EXT     0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT  0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT 0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT 0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT       0x8457

#define GL_DEBUG_CALLBACK_FUNCTION_ARB    0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_ARB  0x8245


#define GL_V2F                    0x2A20
#define GL_V3F                    0x2A21
#define GL_C4UB_V2F               0x2A22
#define GL_C4UB_V3F               0x2A23
#define GL_C3F_V3F                0x2A24
#define GL_N3F_V3F                0x2A25
#define GL_C4F_N3F_V3F            0x2A26
#define GL_T2F_V3F                0x2A27
#define GL_T4F_V4F                0x2A28
#define GL_T2F_C4UB_V3F           0x2A29
#define GL_T2F_C3F_V3F            0x2A2A
#define GL_T2F_N3F_V3F            0x2A2B
#define GL_T2F_C4F_N3F_V3F        0x2A2C
#define GL_T4F_C4F_N3F_V4F        0x2A2D

#define GL_BGR					0x80E0
#define GL_BGRA					0x80E1
#define GL_UNSIGNED_BYTE_3_3_2			0x8032
#define GL_UNSIGNED_BYTE_2_3_3_REV		0x8362
#define GL_UNSIGNED_SHORT_5_6_5			0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV		0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4		0x8033
#define GL_UNSIGNED_SHORT_4_4_4_4_REV		0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1		0x8034
#define GL_UNSIGNED_SHORT_1_5_5_5_REV		0x8366
#define GL_UNSIGNED_INT_8_8_8_8			0x8035
#define GL_UNSIGNED_INT_8_8_8_8_REV		0x8367
#define GL_UNSIGNED_INT_10_10_10_2		0x8036
#define GL_UNSIGNED_INT_2_10_10_10_REV		0x8368
#define GL_INT_2_10_10_10_REV             0x8D9F

/* OpenGL 1.1 */
#define GL_PROXY_TEXTURE_1D			0x8063
#define GL_PROXY_TEXTURE_2D			0x8064
#define GL_TEXTURE_PRIORITY			0x8066
#define GL_TEXTURE_RESIDENT			0x8067
#define GL_TEXTURE_BINDING_1D			0x8068
#define GL_TEXTURE_BINDING_2D			0x8069
#define GL_TEXTURE_INTERNAL_FORMAT		0x1003
#define GL_ALPHA4				0x803B
#define GL_ALPHA8				0x803C
#define GL_ALPHA12				0x803D
#define GL_ALPHA16				0x803E
#define GL_LUMINANCE4				0x803F
#define GL_LUMINANCE8				0x8040
#define GL_LUMINANCE12				0x8041
#define GL_LUMINANCE16				0x8042
#define GL_LUMINANCE4_ALPHA4			0x8043
#define GL_LUMINANCE6_ALPHA2			0x8044
#define GL_LUMINANCE8_ALPHA8			0x8045
#define GL_LUMINANCE12_ALPHA4			0x8046
#define GL_LUMINANCE12_ALPHA12			0x8047
#define GL_LUMINANCE16_ALPHA16			0x8048
#define GL_INTENSITY				0x8049
#define GL_INTENSITY4				0x804A
#define GL_INTENSITY8				0x804B
#define GL_INTENSITY12				0x804C
#define GL_INTENSITY16				0x804D
#define GL_R3_G3_B2				0x2A10
#define GL_RGB4					0x804F
#define GL_RGB5					0x8050
#define GL_RGB8					0x8051
#define GL_RGB10				0x8052
#define GL_RGB12				0x8053
#define GL_RGB16				0x8054
#define GL_RGBA2				0x8055
#define GL_RGBA4				0x8056
#define GL_RGB5_A1				0x8057
#define GL_RGBA8				0x8058
#define GL_RGB10_A2				0x8059
#define GL_RGBA12				0x805A
#define GL_RGBA16				0x805B

#define GL_RED					0x1903
#define GL_GREEN				0x1904
#define GL_BLUE					0x1905
#define GL_ALPHA				0x1906
#define GL_LUMINANCE				0x1909
#define GL_LUMINANCE_ALPHA			0x190A
#define GL_LUMINANCE_INTEGER_EXT          0x8D9C
#define GL_LUMINANCE_ALPHA_INTEGER_EXT    0x8D9D

#define GL_RGBA_INTEGER                   0x8D99
#define GL_SRGB                           0x8C40
#define GL_SRGB8                          0x8C41
#define GL_SRGB_ALPHA                     0x8C42
#define GL_SRGB8_ALPHA8                   0x8C43

#define GL_SEPARATE_SPECULAR_COLOR		0x81FA
#define GL_TEXTURE_MIN_LOD			0x813A
#define GL_TEXTURE_MAX_LOD			0x813B

// Matrix mode
#define GL_MATRIX_MODE          0x0BA0
#define GL_MODELVIEW            0x0001
#define GL_PROJECTION           0x0002
#define GL_TEXTURE              0x0004

// Misc
#define GL_ALPHA_TEST                     0x0BC0
#define GL_SCISSOR_TEST                   0x0C11
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D

#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS 0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_DEFAULT            0x8218
#define GL_FRAMEBUFFER_UNDEFINED          0x8219
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_SIGNED_NORMALIZED              0x8F9C
#define GL_UNSIGNED_NORMALIZED            0x8C17
#define GL_INDEX                          0x8222
#define GL_COLOR_INDEX                    0x1900
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_RG                             0x8227
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_DEPTH_COMPONENT			0x1902
#define GL_ALPHA_BITS				0x0D55
#define GL_RED_BITS				0x0D52
#define GL_GREEN_BITS				0x0D53
#define GL_BLUE_BITS				0x0D54
#define GL_INDEX_BITS				0x0D51

#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE

#define GL_CONTEXT_RELEASE_BEHAVIOR       0x82FB
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC
#define GL_UNSIGNED_INT_8_24_REV          0x8752
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_CLAMP_VERTEX_COLOR             0x891A
#define GL_CLAMP_FRAGMENT_COLOR           0x891B
#define GL_CLAMP_READ_COLOR               0x891C
#define GL_FIXED_ONLY                     0x891D

#define GL_VIEW_CLASS_128_BITS            0x82C4
#define GL_VIEW_CLASS_96_BITS             0x82C5
#define GL_VIEW_CLASS_64_BITS             0x82C6
#define GL_VIEW_CLASS_48_BITS             0x82C7
#define GL_VIEW_CLASS_32_BITS             0x82C8
#define GL_VIEW_CLASS_24_BITS             0x82C9
#define GL_VIEW_CLASS_16_BITS             0x82CA
#define GL_VIEW_CLASS_8_BITS              0x82CB

#define GL_UNSIGNED_SHORT_8_8_MESA        0x85BA
#define GL_UNSIGNED_SHORT_8_8_REV_MESA    0x85BB
#define GL_YCBCR_MESA                     0x8757

#define GL_PALETTE4_RGB8_OES              0x8B90
#define GL_PALETTE4_RGBA8_OES             0x8B91

#define GL_HALF_FLOAT                     0x140B

// Pixelstore
#define GL_PACK_ALIGNMENT			0x0D05
#define GL_PACK_LSB_FIRST			0x0D01
#define GL_PACK_ROW_LENGTH			0x0D02
#define GL_PACK_SKIP_PIXELS			0x0D04
#define GL_PACK_SKIP_ROWS			0x0D03
#define GL_PACK_SWAP_BYTES			0x0D00
#define GL_UNPACK_ALIGNMENT			0x0CF5
#define GL_UNPACK_LSB_FIRST			0x0CF1
#define GL_UNPACK_ROW_LENGTH			0x0CF2
#define GL_UNPACK_SKIP_PIXELS			0x0CF4
#define GL_UNPACK_SKIP_ROWS			0x0CF3
#define GL_UNPACK_SWAP_BYTES			0x0CF0
#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH  0x9127
#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT 0x9128
#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH  0x9129
#define GL_UNPACK_COMPRESSED_BLOCK_SIZE   0x912A
#define GL_PACK_COMPRESSED_BLOCK_WIDTH    0x912B
#define GL_PACK_COMPRESSED_BLOCK_HEIGHT   0x912C
#define GL_PACK_COMPRESSED_BLOCK_DEPTH    0x912D
#define GL_PACK_COMPRESSED_BLOCK_SIZE     0x912E
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E

// Pixel map
#define GL_MAP_COLOR                0x0D10
#define GL_MAP_STENCIL              0x0D11
#define GL_INDEX_SHIFT              0x0D12
#define GL_INDEX_OFFSET             0x0D13
#define GL_PIXEL_MAP_S_TO_S         0x0C71
#define GL_PIXEL_MAP_I_TO_I         0x0C70
#define GL_PIXEL_MAP_I_TO_R         0x0C72
#define GL_PIXEL_MAP_I_TO_G         0x0C73
#define GL_PIXEL_MAP_I_TO_B         0x0C74
#define GL_PIXEL_MAP_I_TO_A         0x0C75
#define GL_PIXEL_MAP_R_TO_R         0x0C76
#define GL_PIXEL_MAP_G_TO_G         0x0C77
#define GL_PIXEL_MAP_B_TO_B         0x0C78
#define GL_PIXEL_MAP_A_TO_A         0x0C79
#define GL_RED_SCALE                0x0D14
#define GL_RED_BIAS                 0x0D15
#define GL_GREEN_SCALE              0x0D18
#define GL_GREEN_BIAS               0x0D19
#define GL_BLUE_SCALE               0x0D1A
#define GL_BLUE_BIAS                0x0D1B
#define GL_ALPHA_SCALE              0x0D1C
#define GL_ALPHA_BIAS               0x0D1D
#define GL_DEPTH_SCALE              0x0D1E
#define GL_DEPTH_BIAS               0x0D1F

// Debug
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D

#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002

/* Implementation limits */
#define GL_MAX_LIST_NESTING			0x0B31
#define GL_MAX_EVAL_ORDER			0x0D30
#define GL_MAX_LIGHTS				0x0D31
#define GL_MAX_CLIP_PLANES			0x0D32
#define GL_MAX_TEXTURE_SIZE			0x0D33
#define GL_MAX_PIXEL_MAP_TABLE			0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH		0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH		0x0D36
#define GL_MAX_NAME_STACK_DEPTH			0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH		0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH		0x0D39
#define GL_MAX_VIEWPORT_DIMS			0x0D3A
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH	0x0D3B

/* Gets */
#define GL_ATTRIB_STACK_DEPTH			0x0BB0
#define GL_CLIENT_ATTRIB_STACK_DEPTH		0x0BB1
#define GL_COLOR_CLEAR_VALUE			0x0C22
#define GL_COLOR_WRITEMASK			0x0C23
#define GL_CURRENT_INDEX			0x0B01
#define GL_CURRENT_COLOR			0x0B00
#define GL_CURRENT_NORMAL			0x0B02
#define GL_CURRENT_RASTER_COLOR			0x0B04
#define GL_CURRENT_RASTER_DISTANCE		0x0B09
#define GL_CURRENT_RASTER_INDEX			0x0B05
#define GL_CURRENT_RASTER_POSITION		0x0B07
#define GL_CURRENT_RASTER_TEXTURE_COORDS	0x0B06
#define GL_CURRENT_RASTER_POSITION_VALID	0x0B08
#define GL_CURRENT_TEXTURE_COORDS		0x0B03
#define GL_INDEX_CLEAR_VALUE			0x0C20
#define GL_INDEX_MODE				0x0C30
#define GL_INDEX_WRITEMASK			0x0C21
#define GL_MODELVIEW_MATRIX			0x0BA6
#define GL_MODELVIEW_STACK_DEPTH		0x0BA3
#define GL_NAME_STACK_DEPTH			0x0D70
#define GL_PROJECTION_MATRIX			0x0BA7
#define GL_PROJECTION_STACK_DEPTH		0x0BA4
#define GL_RENDER_MODE				0x0C40
#define GL_RGBA_MODE				0x0C31
#define GL_TEXTURE_MATRIX			0x0BA8
#define GL_TEXTURE_STACK_DEPTH			0x0BA5
#define GL_VIEWPORT				0x0BA2

#define GL_ACTIVE_TEXTURE			0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE		0x84E1
#define GL_MAX_TEXTURE_UNITS			0x84E2
#define GL_VERTEX_ARRAY_SIZE			0x807A
#define GL_VERTEX_ARRAY_TYPE			0x807B
#define GL_VERTEX_ARRAY_STRIDE			0x807C
#define GL_NORMAL_ARRAY_TYPE			0x807E
#define GL_NORMAL_ARRAY_STRIDE			0x807F
#define GL_COLOR_ARRAY_SIZE			0x8081
#define GL_COLOR_ARRAY_TYPE			0x8082
#define GL_COLOR_ARRAY_STRIDE			0x8083
#define GL_INDEX_ARRAY_TYPE			0x8085
#define GL_INDEX_ARRAY_STRIDE			0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE		0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE		0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE		0x808A
#define GL_SCISSOR_BOX				0x0C10
#define GL_DEPTH_TEST				0x0B71
#define GL_DEPTH_BITS				0x0D56
#define GL_DEPTH_CLEAR_VALUE			0x0B73
#define GL_DEPTH_FUNC				0x0B74
#define GL_DEPTH_RANGE				0x0B70
#define GL_STENCIL_BITS				0x0D57
#define GL_STENCIL_TEST				0x0B90
#define GL_STENCIL_CLEAR_VALUE			0x0B91
#define GL_STENCIL_FUNC				0x0B92
#define GL_STENCIL_VALUE_MASK			0x0B93
#define GL_STENCIL_FAIL				0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL		0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS		0x0B96
#define GL_STENCIL_REF				0x0B97
#define GL_STENCIL_BACK_REF               0x8CA3
#define GL_STENCIL_WRITEMASK			0x0B98
#define GL_STENCIL_INDEX			0x1901

/* Blending */
#define GL_BLEND_SRC				0x0BE1
#define GL_BLEND_DST				0x0BE0
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_BLEND_EQUATION_RGB             0x8009
#define GL_BLEND_EQUATION_ALPHA           0x883D

#define GL_TEXTURE_BUFFER_ARB             0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE_ARB    0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_ARB     0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_ARB 0x8C2D
#define GL_TEXTURE_BUFFER_FORMAT_ARB      0x8C2E
#define GL_SAMPLER_BINDING                0x8919

#define GL_ALPHA_TEST_REF			0x0BC2
#define GL_ALPHA_TEST_FUNC			0x0BC1
#define GL_BLEND_COLOR                0x8005

#define GL_READ_FRAMEBUFFER_EXT           0x8CA8
#define GL_DRAW_FRAMEBUFFER_EXT           0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_EXT   0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_EXT   0x8CAA
#define GL_MAX_RENDERBUFFER_SIZE_EXT      0x84E8
#define GL_FRAMEBUFFER_BINDING_EXT        0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT       0x8CA7

#define GL_R11F_G11F_B10F_EXT             0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV_EXT 0x8C3B
#define GL_RGBA_SIGNED_COMPONENTS_EXT     0x8C3C

#define GL_PROGRAM_STRING_ARB             0x8628
#define GL_PROGRAM_ERROR_POSITION_ARB     0x864B
#define GL_CURRENT_MATRIX_ARB             0x8641
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB   0x88B7
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB 0x8640
#define GL_MAX_PROGRAM_MATRICES_ARB       0x862F
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E
#define GL_MAX_TEXTURE_COORDS_ARB         0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB    0x8872

#define GL_COMPRESSED_ALPHA_ARB           0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB       0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB 0x84EB
#define GL_COMPRESSED_INTENSITY_ARB       0x84EC
#define GL_COMPRESSED_RGB_ARB             0x84ED
#define GL_COMPRESSED_RGBA_ARB            0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB   0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB 0x86A0
#define GL_TEXTURE_COMPRESSED_ARB         0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3
#define GL_ACTIVE_STENCIL_FACE_EXT        0x8911

#define GL_ARRAY_BUFFER_BINDING_ARB       0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB 0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB 0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB 0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB 0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB 0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB 0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB 0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB 0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB 0x889D

#define GL_IMPLEMENTATION_COLOR_READ_TYPE_OES 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES 0x8B9B
#define GL_MAX_VARYING_FLOATS_ARB         0x8B4B

#define GL_GENERATE_MIPMAP                0x8191

#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_PIXEL_UNPACK_BUFFER            0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING      0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING    0x88EF

#define GL_FRONT_LEFT				0x0400
#define GL_FRONT_RIGHT				0x0401
#define GL_BACK_LEFT				0x0402
#define GL_BACK_RIGHT				0x0403
#define GL_FRONT_AND_BACK			0x0408

#define GL_LOWER_LEFT                     0x8CA1
#define GL_UPPER_LEFT                     0x8CA2

// GL_EXT_texture_sRGB_decode
#define GL_TEXTURE_SRGB_DECODE_EXT        0x8A48
#define GL_DECODE_EXT                     0x8A49
#define GL_SKIP_DECODE_EXT                0x8A4A

// Multisample
#define GL_MULTISAMPLE               0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE   0x809E
#define GL_SAMPLE_ALPHA_TO_ONE        0x809F
#define GL_SAMPLE_COVERAGE            0x80A0
#define GL_SAMPLE_BUFFERS             0x80A8
#define GL_SAMPLES                    0x80A9
#define GL_SAMPLE_COVERAGE_VALUE      0x80AA
#define GL_SAMPLE_COVERAGE_INVERT     0x80AB
#define GL_MULTISAMPLE_BIT            0x20000000

// Hints
#define GL_PERSPECTIVE_CORRECTION_HINT		0x0C50
#define GL_POINT_SMOOTH_HINT			0x0C51
#define GL_LINE_SMOOTH_HINT			0x0C52
#define GL_POLYGON_SMOOTH_HINT			0x0C53
#define GL_FOG_HINT				0x0C54
#define GL_DONT_CARE				0x1100
#define GL_FASTEST				0x1101
#define GL_NICEST				0x1102

#define GL_STENCIL_TEST_TWO_SIDE_EXT      0x8910

#define GL_KEEP                 GPU_STENCIL_KEEP
#define GL_INCR                 0x1E02
#define GL_DECR                 0x1E03
#define GL_AND_NOT              GPU_STENCIL_ZERO

// Polygons
#define GL_POINT				0x1B00
#define GL_LINE					0x1B01
#define GL_FILL					0x1B02
#define GL_CW					0x0900
#define GL_CCW					0x0901
#define GL_FRONT				0x0404
#define GL_BACK					0x0405
#define GL_POLYGON_MODE				0x0B40
#define GL_POLYGON_SMOOTH			0x0B41
#define GL_POLYGON_STIPPLE			0x0B42
#define GL_EDGE_FLAG				0x0B43
#define GL_CULL_FACE				0x0B44
#define GL_CULL_FACE_MODE			0x0B45
#define GL_FRONT_FACE				0x0B46
#define GL_POLYGON_OFFSET_FACTOR		0x8038
#define GL_POLYGON_OFFSET_UNITS			0x2A00
#define GL_POLYGON_OFFSET_POINT			0x2A01
#define GL_POLYGON_OFFSET_LINE			0x2A02
#define GL_POLYGON_OFFSET_FILL			0x8037

// Lighting
#define GL_LIGHTING				0x0B50
#define GL_LIGHT0				0x4000
#define GL_LIGHT1				0x4001
#define GL_LIGHT2				0x4002
#define GL_LIGHT3				0x4003
#define GL_LIGHT4				0x4004
#define GL_LIGHT5				0x4005
#define GL_LIGHT6				0x4006
#define GL_LIGHT7				0x4007
#define GL_SPOT_EXPONENT			0x1205
#define GL_SPOT_CUTOFF				0x1206
#define GL_CONSTANT_ATTENUATION			0x1207
#define GL_LINEAR_ATTENUATION			0x1208
#define GL_QUADRATIC_ATTENUATION		0x1209
#define GL_AMBIENT				0x1200
#define GL_DIFFUSE				0x1201
#define GL_SPECULAR				0x1202
#define GL_SHININESS				0x1601
#define GL_EMISSION				0x1600
#define GL_POSITION				0x1203
#define GL_SPOT_DIRECTION			0x1204
#define GL_AMBIENT_AND_DIFFUSE			0x1602
#define GL_COLOR_INDEXES			0x1603
#define GL_LIGHT_MODEL_TWO_SIDE			0x0B52
#define GL_LIGHT_MODEL_LOCAL_VIEWER		0x0B51
#define GL_LIGHT_MODEL_AMBIENT			0x0B53
//#define GL_FRONT_AND_BACK			0x0408
#define GL_SHADE_MODEL				0x0B54
#define GL_FLAT					0x1D00
#define GL_SMOOTH				0x1D01
#define GL_COLOR_MATERIAL			0x0B57
#define GL_COLOR_MATERIAL_FACE			0x0B55
#define GL_COLOR_MATERIAL_PARAMETER		0x0B56
#define GL_NORMALIZE				0x0BA1


/*
 * CTRULIB specific constants below
 */

// Primitives
#define GL_TRIANGLES            GPU_TRIANGLES
#define GL_TRIANGLE_STRIP       GPU_TRIANGLE_STRIP
#define GL_TRIANGLE_FAN         GPU_TRIANGLE_FAN
#define GL_POINTS               0x0400
#define GL_LINES                0x0800
#define GL_LINE_STRIP           0x1600

#define GL_POLYGON				0x0009

// Logic Ops
#define GL_LOGIC_OP                 0x0BF1
#define GL_INDEX_LOGIC_OP           0x0BF1
#define GL_COLOR_LOGIC_OP           0x0BF2
#define GL_LOGIC_OP_MODE            0x0BF0
#define GL_CLEAR             GPU_LOGICOP_CLEAR
#define GL_SET               GPU_LOGICOP_SET
#define GL_COPY              GPU_LOGICOP_COPY
#define GL_COPY_INVERTED     GPU_LOGICOP_COPY_INVERTED
#define GL_NOOP              GPU_LOGICOP_NOOP
#define GL_INVERT            GPU_LOGICOP_INVERT
#define GL_AND               GPU_LOGICOP_AND
#define GL_NAND              GPU_LOGICOP_NAND
#define GL_OR                GPU_LOGICOP_OR
#define GL_NOR               GPU_LOGICOP_NOR
#define GL_XOR               GPU_LOGICOP_XOR
#define GL_EQUIV             GPU_LOGICOP_EQUIV
#define GL_AND_REVERSE       GPU_LOGICOP_AND_REVERSE
#define GL_AND_INVERTED      GPU_LOGICOP_AND_INVERTED
#define GL_OR_REVERSE        GPU_LOGICOP_OR_REVERSE
#define GL_OR_INVERTED       GPU_LOGICOP_OR_INVERTED

// Test funcs
#define GL_NEVER                GPU_NEVER
#define GL_LESS                 GPU_LESS
#define GL_EQUAL                GPU_EQUAL
#define GL_LEQUAL               GPU_LEQUAL
#define GL_GREATER              GPU_GREATER
#define GL_NOTEQUAL             GPU_NOTEQUAL
#define GL_GEQUAL               GPU_GEQUAL
#define GL_ALWAYS               GPU_ALWAYS

// Blending
#define GL_BLEND                 0x0BE2
#define GL_ZERO                  0
#define GL_ONE                   1
#define GL_SRC_COLOR             GPU_SRC_COLOR
#define GL_ONE_MINUS_SRC_COLOR   GPU_ONE_MINUS_SRC_COLOR
#define GL_SRC_ALPHA             GPU_SRC_ALPHA
#define GL_ONE_MINUS_SRC_ALPHA   GPU_ONE_MINUS_SRC_ALPHA
#define GL_DST_ALPHA             GPU_DST_ALPHA
#define GL_ONE_MINUS_DST_ALPHA   GPU_ONE_MINUS_DST_ALPHA
#define GL_DST_COLOR             GPU_DST_COLOR
#define GL_ONE_MINUS_DST_COLOR   GPU_ONE_MINUS_DST_COLOR
#define GL_SRC_ALPHA_SATURATE    GPU_SRC_ALPHA_SATURATE
#define GL_CONSTANT_COLOR            GPU_CONSTANT_COLOR
#define GL_ONE_MINUS_CONSTANT_COLOR  GPU_ONE_MINUS_CONSTANT_COLOR
#define GL_CONSTANT_ALPHA            GPU_CONSTANT_ALPHA
#define GL_ONE_MINUS_CONSTANT_ALPHA  GPU_ONE_MINUS_CONSTANT_ALPHA

#define GL_MIN                   GPU_BLEND_MIN
#define GL_MAX                   GPU_BLEND_MAX
#define GL_FUNC_ADD              GPU_BLEND_ADD
#define GL_FUNC_SUBTRACT         GPU_BLEND_SUBTRACT
#define GL_FUNC_REVERSE_SUBTRACT GPU_BLEND_REVERSE_SUBTRACT

// Data types
#define GL_BYTE                 0x0000  // GPU_BYTE
#define GL_UNSIGNED_BYTE        0x0001  // GPU_UNSIGNED_BYTE
#define GL_SHORT                0x0002  // ...
#define GL_FLOAT                0x0003

#define GL_UNSIGNED_SHORT       0x1403  // Not in ctrulib
#define GL_INT                  0x1404
#define GL_UNSIGNED_INT         0x1405

// GPU_TEVSRC
#define GL_PRIMARY_COLOR        GPU_PRIMARY_COLOR
#define GL_TEXTURE0             GPU_TEXTURE0
#define GL_TEXTURE1             GPU_TEXTURE1
#define GL_TEXTURE2             GPU_TEXTURE2
#define GL_TEXTURE3             GPU_TEXTURE3
#define GL_CONSTANT             GPU_CONSTANT
#define GL_PREVIOUS             GPU_PREVIOUS

// GPU_COMBINEFUNC
#define GL_REPLACE              GPU_REPLACE
#define GL_MODULATE             GPU_MODULATE
#define GL_ADD                  GPU_ADD
#define GL_ADD_SIGNED           GPU_ADD_SIGNED
#define GL_INTERPOLATE          GPU_INTERPOLATE
#define GL_SUBTRACT             GPU_SUBTRACT
#define GL_DOT3_RGB             GPU_DOT3_RGB


// Type declarations

typedef char        GLchar;
typedef int32_t     GLintptr;
typedef int32_t     GLsizeiptr;
typedef uint64_t    GLuint64;
typedef int64_t     GLint64;

typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef void            GLvoid;
typedef signed char     GLbyte;         /* 1-byte signed */
typedef short           GLshort;        /* 2-byte signed */
typedef int             GLint;          /* 4-byte signed */
typedef unsigned char   GLubyte;        /* 1-byte unsigned */
typedef unsigned short  GLushort;       /* 2-byte unsigned */
typedef unsigned int    GLuint;         /* 4-byte unsigned */
typedef int             GLsizei;        /* 4-byte signed */
typedef float           GLfloat;        /* single precision float */
typedef float           GLclampf;       /* single precision float in [0,1] */
typedef double          GLdouble;       /* double precision float */
typedef double          GLclampd;       /* double precision float in [0,1] */

typedef GLint GLfixed;

typedef void (*GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void* GLeglImageOES;
typedef unsigned short GLhalfARB;
typedef struct __GLsync *GLsync;

/* Non-standard GL functions specific to the needs of the 3DS and ctrulib */
GLuint gl3ds_createContext(GLuint sharedContext, gfxScreen_t screen);
GLboolean gl3ds_makeCurrent(GLuint context);
void gl3ds_deleteContext(GLuint context);
void gl3ds_flushContext(GLuint context);
void gl3ds_swapBuffers();

// arrayobj.c
void glBindVertexArray( GLuint id );
//void glBindVertexArrayAPPLE( GLuint id );
void glDeleteVertexArrays(GLsizei n, const GLuint *ids);
void glGenVertexArrays(GLsizei n, GLuint *arrays);
//void glGenVertexArraysAPPLE(GLsizei n, GLuint *buffer);
void glCreateVertexArrays(GLsizei n, GLuint *arrays);
GLboolean glIsVertexArray( GLuint id );
void glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
void glGetVertexArrayiv(GLuint vaobj, GLenum pname, GLint *param);

// attrib.c
void glPushAttrib(GLbitfield mask);
void glPopAttrib(void);
void glPushClientAttrib(GLbitfield mask);
void glPopClientAttrib(void);

// blend.c
void glBlendFunc(GLenum sfactor, GLenum dfactor);
void glBlendFunciARB(GLuint buf, GLenum sfactor, GLenum dfactor);
void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
void glBlendFuncSeparateiARB(GLuint buf, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorA, GLenum dfactorA);
void glBlendEquation( GLenum mode );
void glBlendEquationiARB(GLuint buf, GLenum mode);
void glBlendEquationSeparate( GLenum modeRGB, GLenum modeA );
void glBlendEquationSeparateiARB(GLuint buf, GLenum modeRGB, GLenum modeA);
void glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glAlphaFunc( GLenum func, GLclampf ref );
void glLogicOp( GLenum opcode );
void glIndexMask( GLuint mask );
void glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
void glColorMaski( GLuint buf, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
void glClampColor(GLenum target, GLenum clamp);

// buffers.c
void glDrawBuffer( GLenum mode );
void glNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf);
void glDrawBuffers(GLsizei n, const GLenum *buffers);
void glNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum *bufs);
void glReadBuffer( GLenum mode );
void glNamedFramebufferReadBuffer(GLuint framebuffer, GLenum src);

// bufferobj.c
void glBindBuffer(GLenum target, GLuint buffer);
void glDeleteBuffers(GLsizei n, const GLuint * buffer);
void glGenBuffers(GLsizei n, GLuint *buffers);
void glCreateBuffers(GLsizei n, GLuint *buffers);
GLboolean glIsBuffer(GLuint buffer);
void glBufferStorage(GLenum target, GLsizeiptr size, const GLvoid *data, GLbitfield flags);
void glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const GLvoid *data, GLbitfield flags);
void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
void glNamedBufferData(GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage);
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid *data);
void glClearBufferData(GLenum target, GLenum internalformat, GLenum format, GLenum type, const GLvoid *data);
void glClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const GLvoid *data);
void glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const GLvoid *data);
void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const GLvoid *data);
GLboolean glUnmapBuffer(GLenum target);
GLboolean glUnmapNamedBuffer(GLuint buffer);
void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params);
void glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params);
void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint *params);
void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64 *params);
void glGetBufferPointerv(GLenum target, GLenum pname, GLvoid **params);
void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, GLvoid **params);
void glCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
void glCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
void * glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
void * glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
void * glMapBuffer(GLenum target, GLenum access);
void * glMapNamedBuffer(GLuint buffer, GLenum access);
void glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length);
void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length);
//GLenum glObjectPurgeableAPPLE(GLenum objectType, GLuint name, GLenum option);
//GLenum glObjectUnpurgeableAPPLE(GLenum objectType, GLuint name, GLenum option);
//void glGetObjectParameterivAPPLE(GLenum objectType, GLuint name, GLenum pname, GLint* params);
//void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
//void glBindBufferBase(GLenum target, GLuint index, GLuint buffer);
//void glBindBuffersRange(GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizeiptr *sizes);
//void glBindBuffersBase(GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
//void glInvalidateBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr length);
//void glInvalidateBufferData(GLuint buffer);

// pixelstore.c
void glPixelStorei( GLenum pname, GLint param );
void glPixelStoref( GLenum pname, GLfloat param );

// polygon.c
void glCullFace(GLenum mode);
void glFrontFace(GLenum mode);
void glPolygonMode(GLenum face, GLenum mode);
void glPolygonOffset(GLfloat factor, GLfloat units);

// depth.c
void glDepthFunc(GLenum func);
void glDepthMask(GLboolean flag);
void glClearDepth( GLclampd depth );
void glClearDepthf( GLclampf depth );
void glDepthBoundsEXT( GLclampd zmin, GLclampd zmax );

// context.c
void glFinish( void );
void glFlush( void );

// clear.c
void glClearIndex( GLfloat c );
void glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
void glClearColorIiEXT(GLint r, GLint g, GLint b, GLint a);
void glClearColorIuiEXT(GLuint r, GLuint g, GLuint b, GLuint a);
void glClear( GLbitfield mask );
void glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value);
//void glClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint *value);
void glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value);
//void glClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint *value);
void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value);
//void glClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat *value);
void glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
//void glClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, GLfloat depth, GLint stencil);

// enable.c
void glEnable(GLenum cap);
void glDisable(GLenum cap);
GLboolean glIsEnabled (GLenum cap);
//GLboolean glIsEnabledi( GLenum cap, GLuint index );
void glEnablei(GLenum cap, GLuint index);
void glDisablei(GLenum cap, GLuint index);
void glEnableClientState(GLenum cap);
void glDisableClientState(GLenum cap);

void glClear(GLbitfield mask);
void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glClearColori(GLint red, GLint green, GLint blue, GLint alpha);

// errors.c
GLenum glGetError (void);

// fbobject.c
GLboolean glIsRenderbuffer(GLuint renderbuffer);
void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
void glBindRenderbufferEXT(GLenum target, GLuint renderbuffer);
void glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
void glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
void glCreateRenderbuffers(GLsizei n, GLuint *renderbuffers);
void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
//void GLAPIENTRY _es_RenderbufferStorageEXT(GLenum target, GLenum internalFormat, GLsizei width, GLsizei height);
void glNamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
void glNamedRenderbufferStorageMultisample(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
void glEGLImageTargetRenderbufferStorageOES(GLenum target, GLeglImageOES image);
void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params);
void glGetNamedRenderbufferParameteriv(GLuint renderbuffer, GLenum pname, GLint *params);
GLboolean glIsFramebuffer(GLuint framebuffer);
void glBindFramebuffer(GLenum target, GLuint framebuffer);
void glBindFramebufferEXT(GLenum target, GLuint framebuffer);
void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
void glGenFramebuffers(GLsizei n, GLuint *framebuffers);
void glCreateFramebuffers(GLsizei n, GLuint *framebuffers);
GLenum glCheckFramebufferStatus(GLenum target);
GLenum glCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target);
void glFramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer);
void glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
void glNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
void glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level);
void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params);
void glGetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment,  GLenum pname, GLint *params);
void glNamedFramebufferParameteri(GLuint framebuffer, GLenum pname, GLint param);
void glGetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint *param);
void glInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
void glInvalidateNamedFramebufferSubData(GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
void glInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments);
void glInvalidateNamedFramebufferData(GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments);
void glDiscardFramebufferEXT(GLenum target, GLsizei numAttachments, const GLenum *attachments);

// get.c
void glGetBooleanv( GLenum pname, GLboolean *params );
void glGetDoublev( GLenum pname, GLdouble *params );
void glGetFloatv( GLenum pname, GLfloat *params );
void glGetIntegerv( GLenum pname, GLint *params );
void glGetInteger64v( GLenum pname, GLint64 *params );
void glGetFixedv(GLenum pname, GLfixed *params);
void glGetBooleani_v( GLenum pname, GLuint index, GLboolean *params );
void glGetIntegeri_v( GLenum pname, GLuint index, GLint *params );
void glGetInteger64i_v( GLenum pname, GLuint index, GLint64 *params );
void glGetPointerv( GLenum pname, GLvoid **params );
void glGetFloati_v(GLenum target, GLuint index, GLfloat *data);
void glGetDoublei_v(GLenum target, GLuint index, GLdouble *data);
const GLubyte * glGetString( GLenum name );
const GLubyte * glGetStringi(GLenum name, GLuint index);
GLenum glGetError( void );
/* GL_ARB_robustness */
//GLenum glGetGraphicsResetStatusARB( void );

// matrix.c
void glLoadIdentity(void);
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearval, GLdouble farval);
void glTranslatef( GLfloat x, GLfloat y, GLfloat z );
void glMatrixMode (GLenum mode);
void glLoadMatrixf (const GLfloat *m);


void glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearval, GLdouble farval );
void glPushMatrix( void );
void glPopMatrix( void );
void glLoadMatrixd( const GLdouble *m );
void glMultMatrixf( const GLfloat *m );
void glMultMatrixd( const GLdouble *m );
void glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
void glScalef( GLfloat x, GLfloat y, GLfloat z );
void glScaled( GLdouble x, GLdouble y, GLdouble z );
void glTranslated( GLdouble x, GLdouble y, GLdouble z );
void glLoadTransposeMatrixf( const GLfloat *m );
void glLoadTransposeMatrixd( const GLdouble *m );
void glMultTransposeMatrixf( const GLfloat *m );
void glMultTransposeMatrixd( const GLdouble *m );

// light.c
void glShadeModel( GLenum mode );
void glProvokingVertex(GLenum mode);
void glColorMaterial( GLenum face, GLenum mode );
void glLightf( GLenum light, GLenum pname, GLfloat param );
void glLightfv( GLenum light, GLenum pname, const GLfloat *params );
void glLightiv( GLenum light, GLenum pname, const GLint *params );
void glLighti( GLenum light, GLenum pname, GLint param );
void glLightModelf( GLenum pname, GLfloat param );
void glLightModelfv( GLenum pname, const GLfloat *params );
void glLightModeli( GLenum pname, GLint param );
void glLightModeliv( GLenum pname, const GLint *params );
void glGetLightfv( GLenum light, GLenum pname, GLfloat *params );
void glGetLightiv( GLenum light, GLenum pname, GLint *params );
void glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params );
void glGetMaterialiv( GLenum face, GLenum pname, GLint *params );

// multisample.c
void glSampleCoverage(GLclampf value, GLboolean invert);
void glGetMultisamplefv(GLenum pname, GLuint index, GLfloat* val);
void glSampleMaski(GLuint index, GLbitfield mask);
void glMinSampleShading(GLclampf value);

// pixel.c
void glPixelZoom( GLfloat xfactor, GLfloat yfactor );
void glPixelMapfv( GLenum map, GLsizei mapsize, const GLfloat *values );
void glPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values );
void glPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values );
void glGetnPixelMapfvARB( GLenum map, GLsizei bufSize, GLfloat *values );
void glGetPixelMapfv( GLenum map, GLfloat *values );
void glGetnPixelMapuivARB( GLenum map, GLsizei bufSize, GLuint *values );
void glGetPixelMapuiv( GLenum map, GLuint *values );
void glGetnPixelMapusvARB( GLenum map, GLsizei bufSize, GLushort *values );
void glGetPixelMapusv( GLenum map, GLushort *values );
void glPixelTransferf(GLenum pname, GLfloat param);
void glPixelTransferi( GLenum pname, GLint param );

// readpix.c
void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );

// samplerobj.c
void glGenSamplers(GLsizei count, GLuint *samplers);
void glCreateSamplers(GLsizei count, GLuint *samplers);
void glDeleteSamplers(GLsizei count, const GLuint *samplers);
GLboolean glIsSampler(GLuint sampler);
void glBindSampler(GLuint unit, GLuint sampler);
void glBindSamplers(GLuint first, GLsizei count, const GLuint *samplers);
void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param);
void glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param);
void glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint *params);
void glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *params);
void glSamplerParameterIiv(GLuint sampler, GLenum pname, const GLint *params);
void glSamplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint *params);
void glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params);
void glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params);
void glGetSamplerParameterIiv(GLuint sampler, GLenum pname, GLint *params);
void glGetSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint *params);

// scissor.c
void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void glScissorArrayv(GLuint first, GLsizei count, const GLint * v);
void glScissorIndexed(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
void glScissorIndexedv(GLuint index, const GLint * v);

// shader.c
GLuint glCreateProgram(void);
void glDeleteProgram(GLuint program);
void glUseProgram(GLuint program);
void glProgramBinary(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
GLuint glCreateShader(GLenum shaderType);
void glAttachShader(GLuint program, GLuint shader);
//void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);

// stencil.c
void glClearStencil( GLint s );
void glStencilFunc( GLenum func, GLint ref, GLuint mask );
void glStencilMask( GLuint mask );
void glStencilOp( GLenum fail, GLenum zfail, GLenum zpass );
void glActiveStencilFaceEXT(GLenum face);
void glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
void glStencilFuncSeparateATI(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
void glStencilMaskSeparate(GLenum face, GLuint mask);

// texture.c
//void glGenTextures(GLsizei n, GLuint* textures);
//void glDeleteTextures(GLsizei n, const GLuint* textures);
//void glBindTexture(GLenum target, GLuint texture);  /* target must be GL_TEXTURE_2D */
//void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data);
//void glTexEnvi(GLenum target, GLenum pname, GLint param);
//void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
//void glTexParameteri (GLenum target, GLenum pname, GLint param);
//void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
//void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
//void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

// texstate.c
void glActiveTexture( GLenum target );
void glClientActiveTexture( GLenum target );

// texstoreage.c
void glTexStorage1D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
void glTextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void glTextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
void glTextureStorage1DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
void glTextureStorage2DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void glTextureStorage3DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

// texenv.c
void glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );
void glGetTexEnviv( GLenum target, GLenum pname, GLint *params );
void glTexEnvf( GLenum target, GLenum pname, GLfloat param );
void glTexEnvfv( GLenum target, GLenum pname, const GLfloat *param );
void glTexEnvi( GLenum target, GLenum pname, GLint param );
void glTexEnviv( GLenum target, GLenum pname, const GLint *param );

// texgetimage.c
void glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels );
void glGetnTexImageARB( GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid *pixels );
void glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid *pixels);
void glGetCompressedTexImage(GLenum target, GLint lod, GLvoid *img);
void glGetnCompressedTexImageARB(GLenum target, GLint level, GLsizei bufSize, GLvoid *img);
void glGetCompressedTextureImage(GLuint texture, GLint level, GLsizei bufSize, GLvoid *pixels);

// teximage.c
void glTexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
void glTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
void glTexImage3D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
void glTexImage3DEXT( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
//void glEGLImageTargetTexture2DOES( GLenum target, GLeglImageOES image );
void glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels );
void glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
void glTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels );
void glTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void glTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
void glCopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
void glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
void glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
void glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
void glCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height );
void glCopyTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
void glCopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void glCopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void glClearTexSubImage( GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data );
void glClearTexImage( GLuint texture, GLint level, GLenum format, GLenum type, const void *data );
void glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
void glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
void glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
void glCompressedTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
void glCompressedTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
void glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
void glCompressedTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
void glTexBuffer(GLenum target, GLenum internalFormat, GLuint buffer);
void glTexBufferRange(GLenum target, GLenum internalFormat, GLuint buffer, GLintptr offset, GLsizeiptr size);
void glTextureBuffer(GLuint texture, GLenum internalFormat, GLuint buffer);
void glTextureBufferRange(GLuint texture, GLenum internalFormat, GLuint buffer, GLintptr offset, GLsizeiptr size);
void glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
void glTexImage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
void glTexStorage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
void glTexStorage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
void glTextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
void glTextureStorage3DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);

// texobj.c
void glGenTextures(GLsizei n, GLuint *textures);
void glCreateTextures(GLenum target, GLsizei n, GLuint *textures);
void glDeleteTextures( GLsizei n, const GLuint *textures );
void glBindTexture( GLenum target, GLuint texture );
void glBindTextureUnit(GLuint unit, GLuint texture);
void glBindTextures( GLuint first, GLsizei count, const GLuint *textures );
void glPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities );
GLboolean glAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences );
GLboolean glIsTexture( GLuint texture );
void glInvalidateTexSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
void glInvalidateTexImage(GLuint texture, GLint level);

// texparam.c
void glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );
void glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );
void glGetTextureLevelParameterfv(GLuint texture, GLint level, GLenum pname, GLfloat *params);
void glGetTextureLevelParameteriv(GLuint texture, GLint level, GLenum pname, GLint *params);
void glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params );
void glGetTexParameteriv( GLenum target, GLenum pname, GLint *params );
void glGetTexParameterIiv(GLenum target, GLenum pname, GLint *params);
void glGetTexParameterIuiv(GLenum target, GLenum pname, GLuint *params);
void glGetTextureParameterfv(GLuint texture, GLenum pname, GLfloat *params);
void glGetTextureParameteriv(GLuint texture, GLenum pname, GLint *params);
void glGetTextureParameterIiv(GLuint texture, GLenum pname, GLint *params);
void glGetTextureParameterIuiv(GLuint texture, GLenum pname, GLuint *params);
void glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
void glTexParameterf( GLenum target, GLenum pname, GLfloat param );
void glTexParameteri( GLenum target, GLenum pname, GLint param );
void glTexParameteriv( GLenum target, GLenum pname, const GLint *params );
void glTexParameterIiv(GLenum target, GLenum pname, const GLint *params);
void glTexParameterIuiv(GLenum target, GLenum pname, const GLuint *params);
void glTextureParameterfv(GLuint texture, GLenum pname, const GLfloat *params);
void glTextureParameterf(GLuint texture, GLenum pname, GLfloat param);
void glTextureParameteri(GLuint texture, GLenum pname, GLint param);
void glTextureParameteriv(GLuint texture, GLenum pname, const GLint *params);
void glTextureParameterIiv(GLuint texture, GLenum pname, const GLint *params);
void glTextureParameterIuiv(GLuint texture, GLenum pname, const GLuint *params);

// textureview.c
void glTextureView(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);

// uniform.c
GLint glGetUniformLocation(GLuint program, const GLchar* name);
void glUniform1f(GLint location, GLfloat v0);
void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void glUniform1i(GLint location, GLint v0);
void glUniform2i(GLint location, GLint v0, GLint v1);
void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
//void glUniform1ui(GLint location, GLuint v0);
//void glUniform2ui(GLint location, GLuint v0, GLuint v1);
//void glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2);
//void glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
//void glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
//void glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
//void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
//void glUniform1iv(GLint location, GLsizei count, const GLint *value);
//void glUniform2iv(GLint location, GLsizei count, const GLint *value);
//void glUniform3iv(GLint location, GLsizei count, const GLint *value);
void glUniform4iv(GLint location, GLsizei count, const GLint *value);
//void glUniform1uiv(GLint location, GLsizei count, const GLuint *value);
//void glUniform2uiv(GLint location, GLsizei count, const GLuint *value);
//void glUniform3uiv(GLint location, GLsizei count, const GLuint *value);
//void glUniform4uiv(GLint location, GLsizei count, const GLuint *value);
//void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
//void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//void glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
//void glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
//void glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
//void glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
//void glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
//void glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

// varray.c
void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *ptr);
void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
void glIndexPointer(GLenum type, GLsizei stride, const GLvoid *ptr);
void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
void glEdgeFlagPointer(GLsizei stride, const GLvoid *ptr);
void glVertexPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr);
void glNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr);
void glColorPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr);
void glIndexPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr);
void glTexCoordPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr);
void glEdgeFlagPointerEXT(GLsizei stride, GLsizei count, const GLboolean *ptr);
void glFogCoordPointer(GLenum type, GLsizei stride, const GLvoid *ptr);
void glSecondaryColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
void glPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid *ptr);
void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
void glVertexAttribLPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glEnableVertexAttribArray(GLuint index);
void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
void glDisableVertexAttribArray(GLuint index);
void glDisableVertexArrayAttrib(GLuint vaobj, GLuint index);
void glGetVertexAttribdv(GLuint index, GLenum pname, GLdouble *params);
void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params);
void glGetVertexAttribLdv(GLuint index, GLenum pname, GLdouble *params);
void glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params);
void glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params);
void glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params);
void glGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid **pointer);
void glGetVertexArrayIndexediv(GLuint vaobj, GLuint index, GLenum pname, GLint *param);
void glGetVertexArrayIndexed64iv(GLuint vaobj, GLuint index, GLenum pname, GLint64 *param);
void glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer);
void glMultiDrawArrays( GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount );
void glMultiDrawElementsEXT( GLenum mode, const GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount );
void glMultiDrawElementsBaseVertex( GLenum mode, const GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount, const GLint *basevertex);
void glMultiModeDrawArraysIBM( const GLenum * mode, const GLint * first, const GLsizei * count, GLsizei primcount, GLint modestride );
void glMultiModeDrawElementsIBM( const GLenum * mode, const GLsizei * count, GLenum type, const GLvoid * const * indices, GLsizei primcount, GLint modestride );
void glLockArraysEXT(GLint first, GLsizei count);
void glUnlockArraysEXT( void );
void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex);
void glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex);
void glDrawTransformFeedback(GLenum mode, GLuint name);
void glPrimitiveRestartIndex(GLuint index);
void glVertexAttribDivisor(GLuint index, GLuint divisor);

void glBindVertexBuffer(GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride);
void glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride);
void glBindVertexBuffers(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
void glVertexArrayVertexBuffers(GLuint vaobj, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
void glVertexAttribFormat(GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset);
void glVertexArrayAttribFormat(GLuint vaobj, GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset);
void glVertexAttribIFormat(GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset);
void glVertexArrayAttribIFormat(GLuint vaobj, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset);
void glVertexAttribLFormat(GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset);
void glVertexArrayAttribLFormat(GLuint vaobj, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset);
void glVertexAttribBinding(GLuint attribIndex, GLuint bindingIndex);
void glVertexArrayAttribBinding(GLuint vaobj, GLuint attribIndex, GLuint bindingIndex);
void glVertexBindingDivisor(GLuint bindingIndex, GLuint divisor);
void glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingIndex, GLuint divisor);


// viewport.c
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void glViewportArrayv(GLuint first, GLsizei count, const GLfloat * v);
void glViewportIndexedf(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
void glViewportIndexedfv(GLuint index, const GLfloat * v);
void glDepthRange(GLclampd nearval, GLclampd farval);
void glDepthRangef(GLclampf nearval, GLclampf farval);
void glDepthRangeArrayv(GLuint first, GLsizei count, const GLclampd * v);
void glDepthRangeIndexed(GLuint index, GLclampd n, GLclampd f);
//void glClipControl(GLenum origin, GLenum depth);

#ifdef __cplusplus
}
#endif

#endif
