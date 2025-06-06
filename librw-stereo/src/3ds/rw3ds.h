#ifdef RW_3DS
#include <3ds.h>
#include <citro3d.h>
#undef BIT
#endif

namespace rw {

#ifdef RW_3DS
struct EngineOpenParams
{
	void **window;
	bool32 fullscreen;
	int width, height;
	const char *windowtitle;
};
#endif

namespace c3d {

void registerPlatformPlugins(void);

extern Device renderdevice;

struct AttribDesc
{
	uint8 index;
	uint8 type;
	uint8 count;
	uint8 offset;
};
  
enum AttribIndices
{
	ATTRIB_POS = 0,		// v0
	ATTRIB_NORMAL,		// v1
	ATTRIB_COLOR,		// v2
	ATTRIB_TEXCOORDS0,	// v3
	ATTRIB_TEXCOORDS1,	// v4 (not actually used, matfx-env uses a uniform)
	ATTRIB_TEXCOORDS2,	// v5 (not actually used)
	MAX_ATTRIBS
	// ATTRIB_WEIGHTS, /* skinning done on cpu */
	// ATTRIB_INDICES, 
	// ATTRIB_TEXCOORDS3, /* proctex, could be of some use */
	// ATTRIB_TEXCOORDS4,
	// ATTRIB_TEXCOORDS5,
	// ATTRIB_TEXCOORDS6,
	// ATTRIB_TEXCOORDS7,
};

extern void *linearScratch;
  
// default uniform indices
//extern int32 u_matColor;
//extern int32 u_surfProps;

struct InstanceData
{
	uint32    numIndex;
	uint32    minVert;	// not used for rendering
	int32     numVertices;	//
	Material *material;
	bool32    vertexAlpha;
	uint32    program;
	uint16   *indexBuffer;
};

#ifdef RW_3DS
	
struct InstanceDataHeader : rw::InstanceDataHeader
{
	uint32      serialNumber;
	uint32      numMeshes;
	GPU_Primitive_t primType;
	uint8      *vertexBuffer;
	int32       numAttribs;
	AttribDesc  attribDesc[MAX_ATTRIBS];
	uint32      totalNumIndex;
	uint32      totalNumVertex;

	C3D_BufInfo  vbo;
	C3D_AttrInfo vao;
	ptrdiff_t    stride;

	InstanceData *inst;
};

struct Shader;

extern Shader *defaultShader;

struct Im3DVertex
{
	V3d     position;
	uint8   r, g, b, a;
	float32 u, v;

	void setX(float32 x) { this->position.x = x; }
	void setY(float32 y) { this->position.y = y; }
	void setZ(float32 z) { this->position.z = z; }
	void setColor(uint8 r, uint8 g, uint8 b, uint8 a){
		this->r = r; this->g = g; this->b = b; this->a = a;
	}
	void setU(float32 u) { this->u = u; }
	void setV(float32 v) { this->v = v; }

	float getX(void) { return this->position.x; }
	float getY(void) { return this->position.y; }
	float getZ(void) { return this->position.z; }
	RGBA getColor(void) { return makeRGBA(this->r, this->g, this->b, this->a); }
	float getU(void) { return this->u; }
	float getV(void) { return this->v; }
};

struct Im2DVertex
{
	float32 x, y, z, w;
	uint8   r, g, b, a;
	float32 u, v;

	void setScreenX(float32 x) { this->x = x; }
	void setScreenY(float32 y) { this->y = y; }
	void setScreenZ(float32 z) { this->z = z; }
	void setCameraZ(float32 z) { this->w = z; }
	void setRecipCameraZ(float32 recipz) { this->w = 1.0f/recipz; }
	void setColor(uint8 r, uint8 g, uint8 b, uint8 a) {
		this->r = r; this->g = g; this->b = b; this->a = a;
	}
	void setU(float32 u, float recipz) { this->u = u; }
	void setV(float32 v, float recipz) { this->v = v; }

	float getScreenX(void) { return this->x; }
	float getScreenY(void) { return this->y; }
	float getScreenZ(void) { return this->z; }
	float getCameraZ(void) { return this->w; }
	float getRecipCameraZ(void) { return 1.0f/this->w; }
	RGBA getColor(void) { return makeRGBA(this->r, this->g, this->b, this->a); }
	float getU(void) { return this->u; }
	float getV(void) { return this->v; }
};


void genAttribPointers(InstanceDataHeader *header);
void setAttribPointers(InstanceDataHeader *header);
void setAttribsFixed(void);
  
// Render state

// Vertex shader bits
enum
{
	// These should be low so they could be used as indices
	VSLIGHT_DIRECT	= 1,
	VSLIGHT_POINT	= 2,
	VSLIGHT_SPOT	= 4,
	VSLIGHT_MASK	= 7,	// all the above
	// less critical
	VSLIGHT_AMBIENT = 8,
};

//extern const char *shaderDecl;	// #version stuff
//extern const char *header_vert_src;
//extern const char *header_frag_src;

extern Shader *im2dOverrideShader;

// per Scene
void setProjectionMatrix(float32*);
void setViewMatrix(float32*);

// per Object
void setWorldMatrix(Matrix*);
int32 setLights(WorldLights *lightData);

// per Mesh
void setTexture(int32 n, Texture *tex);
void setMaterial(const RGBA &color, const SurfaceProperties &surfaceprops, float extraSurfProp = 0.0f);
inline void setMaterial(uint32 flags, const RGBA &color, const SurfaceProperties &surfaceprops, float extraSurfProp = 0.0f)
{
	static RGBA white = { 255, 255, 255, 255 };
	if(flags & Geometry::MODULATE)
		setMaterial(color, surfaceprops, extraSurfProp);
	else
		setMaterial(white, surfaceprops, extraSurfProp);
}

void setAlphaBlend(bool32 enable);
bool32 getAlphaBlend(void);

void bindFramebuffer(uint32 fbo);
void bindTexture(C3D_Tex *tex);

void flushCache(void);

int cameraTilt(Camera *cam);
  
#endif

class ObjPipeline : public rw::ObjPipeline
{
public:
	void init(void);
	static ObjPipeline *create(void);

	void (*instanceCB)(Geometry *geo, InstanceDataHeader *header, bool32 reinstance);
	void (*uninstanceCB)(Geometry *geo, InstanceDataHeader *header);
	void (*renderCB)(Atomic *atomic, InstanceDataHeader *header);
};

void defaultInstanceCB(Geometry *geo, InstanceDataHeader *header, bool32 reinstance);
void defaultUninstanceCB(Geometry *geo, InstanceDataHeader *header);
void defaultRenderCB(Atomic *atomic, InstanceDataHeader *header);
int32 lightingCB(Atomic *atomic);

void drawInst_simple(InstanceDataHeader *header, InstanceData *inst);
// Emulate PS2 GS alpha test FB_ONLY case: failed alpha writes to frame- but not to depth buffer
void drawInst_GSemu(InstanceDataHeader *header, InstanceData *inst);
// This one switches between the above two depending on render state;
void drawInst(InstanceDataHeader *header, InstanceData *inst);


void *destroyNativeData(void *object, int32, int32);

ObjPipeline *makeDefaultPipeline(void);

// Native Texture and Raster

struct C3DRaster
{
	int32 bpp;		// bytes per pixel
	GPU_TEXCOLOR format;
	GX_TRANSFER_FORMAT transfer;
	int32 totalSize;
	
	// texture object
	union
	{
		C3D_Tex *tex;
		void *zbuf;
	};
		
	bool  onVram;
	bool  isCompressed;
	bool  hasAlpha;
	bool  autogenMipmap;
	int8  numLevels;
	
	// cached filtermode and addressing
	uint8 filterMode;
	uint8 addressU;
	uint8 addressV;

	bool tilt;
	C3D_FrameBuf *fbo;
	Raster *fboMate;	// color or zbuffer raster mate of this one
};

// void allocateETC(Raster *raster, bool hasAlpha);
Raster *allocateETC(Raster *raster);
void rasterFromImage_etc1(rw::Raster *ras, rw::Image *img);

Texture *readNativeTexture(Stream *stream);
void writeNativeTexture(Texture *tex, Stream *stream);
uint32 getSizeNativeTexture(Texture *tex);

void cameraLHS(void);
void cameraRHS(void);
  
#ifdef RW_3DS
void *safeLinearAlloc(size_t size);
void TexFree(C3DRaster *natras);
  int TexAlloc(Raster *raster, int w, int h, GPU_TEXCOLOR fmt, int mipmap);
#endif
  
extern int32 nativeRasterOffset;
void registerNativeRaster(void);
#define GETC3DRASTEREXT(raster) PLUGINOFFSET(c3d::C3DRaster, raster, rw::c3d::nativeRasterOffset)

}
}

