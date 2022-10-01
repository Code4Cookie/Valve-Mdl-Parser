//========= Copyright Valve Corporation, All rights reserved. ============//

#pragma once

#include "compressed_vector.h"
#include "vector4d.h"
#include <cassert>

#define MAX_NUM_LODS 8


#define STUDIO_VERSION		48

#ifndef _XBOX
#define MAXSTUDIOTRIANGLES	65536	// TODO: tune this
#define MAXSTUDIOVERTS		65536	// TODO: tune this
#define	MAXSTUDIOFLEXVERTS	10000	// max number of verts that can be flexed per mesh.  TODO: tune this
#else
#define MAXSTUDIOTRIANGLES	25000
#define MAXSTUDIOVERTS		10000
#define	MAXSTUDIOFLEXVERTS	1000
#endif
#define MAXSTUDIOSKINS		32		// total textures
#define MAXSTUDIOBONES		128		// total bones actually used
#define MAXSTUDIOFLEXDESC	1024	// maximum number of low level flexes (actual morph targets)
#define MAXSTUDIOFLEXCTRL	96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONECTRLS	4
#define MAXSTUDIOANIMBLOCKS 256

#define MAXSTUDIOBONEBITS	7		// NOTE: MUST MATCH MAXSTUDIOBONES

// NOTE!!! : Changing this number also changes the vtx file format!!!!!
#define MAX_NUM_BONES_PER_VERT 3

//Adrian - Remove this when we completely phase out the old event system.
#define NEW_EVENT_STYLE ( 1 << 10 )

typedef unsigned char byte;

struct mstudiodata_t
{
	int		count;
	int		offset;
};

namespace
{
	template< class T >
	inline T clamp(T const& val, T const& minVal, T const& maxVal)
	{
		if (maxVal < minVal)
			return maxVal;
		else if (val < minVal)
			return minVal;
		else if (val > maxVal)
			return maxVal;
		else
			return val;
	}
}

enum StudioVertAnimType_t
{
	STUDIO_VERT_ANIM_NORMAL = 0,
	STUDIO_VERT_ANIM_WRINKLE,
};

struct mstudioiface_t
{
	unsigned short a, b, c;		// Indices to vertices
};

struct mstudio_modelvertexdata_t
{
	// base of external vertex data stores
	const void* pVertexData;
	const void* pTangentData;
};

struct mstudio_meshvertexdata_t
{
	// indirection to this mesh's model's vertex data
	const mstudio_modelvertexdata_t* modelvertexdata;

	// used for fixup calcs when culling top level lods
	// expected number of mesh verts at desired lod
	int					numLODVertexes[MAX_NUM_LODS];
};

struct mstudiobonecontroller_t
{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					inputfield;	// 0-3 user set controller, 4 mouth
	int					unused[8];
};

struct mstudiobbox_t
{
	int					bone;
	int					group;				// intersection group
	Vector				bbmin;				// bounding box
	Vector				bbmax;
	int					szhitboxnameindex;	// offset to the name of the hitbox.
	int					unused[8];

	const char* pszHitboxName()
	{
		if (szhitboxnameindex == 0)
			return "";

		return ((const char*)this) + szhitboxnameindex;
	}

	mstudiobbox_t() {}

private:
	// No copy constructors allowed
	mstudiobbox_t(const mstudiobbox_t& vOther);
};

struct mstudiohitboxset_t
{
	int					sznameindex;
	inline char* const	pszName(void) const { return ((char*)this) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t* pHitbox(int i) const { return (mstudiobbox_t*)(((byte*)this) + hitboxindex) + i; };
};

struct mstudiovertanim_t
{
	unsigned short		index;
	byte				speed;	// 255/max_length_in_flex
	byte				side;	// 255/left_right

protected:
	// JasonM changing this type a lot, to prefer fixed point 16 bit...
	union
	{
		short			delta[3];
		float16			flDelta[3];
	};

	union
	{
		short			ndelta[3];
		float16			flNDelta[3];
	};

public:
	inline void ConvertToFixed(float flVertAnimFixedPointScale)
	{
		delta[0] = flDelta[0].GetFloat() / flVertAnimFixedPointScale;
		delta[1] = flDelta[1].GetFloat() / flVertAnimFixedPointScale;
		delta[2] = flDelta[2].GetFloat() / flVertAnimFixedPointScale;
		ndelta[0] = flNDelta[0].GetFloat() / flVertAnimFixedPointScale;
		ndelta[1] = flNDelta[1].GetFloat() / flVertAnimFixedPointScale;
		ndelta[2] = flNDelta[2].GetFloat() / flVertAnimFixedPointScale;
	}

	inline Vector GetDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector(delta[0] * flVertAnimFixedPointScale, delta[1] * flVertAnimFixedPointScale, delta[2] * flVertAnimFixedPointScale);
	}
	inline Vector GetNDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector(ndelta[0] * flVertAnimFixedPointScale, ndelta[1] * flVertAnimFixedPointScale, ndelta[2] * flVertAnimFixedPointScale);
	}
	inline void GetDeltaFixed4DAligned(Vector4DAligned* vFillIn, float flVertAnimFixedPointScale)
	{
		vFillIn->Set(delta[0] * flVertAnimFixedPointScale, delta[1] * flVertAnimFixedPointScale, delta[2] * flVertAnimFixedPointScale, 0.0f);
	}
	inline void GetNDeltaFixed4DAligned(Vector4DAligned* vFillIn, float flVertAnimFixedPointScale)
	{
		vFillIn->Set(ndelta[0] * flVertAnimFixedPointScale, ndelta[1] * flVertAnimFixedPointScale, ndelta[2] * flVertAnimFixedPointScale, 0.0f);
	}
	inline Vector GetDeltaFloat()
	{
		return Vector(flDelta[0].GetFloat(), flDelta[1].GetFloat(), flDelta[2].GetFloat());
	}
	inline Vector GetNDeltaFloat()
	{
		return Vector(flNDelta[0].GetFloat(), flNDelta[1].GetFloat(), flNDelta[2].GetFloat());
	}
	inline void SetDeltaFixed(const Vector& vInput, float flVertAnimFixedPointScale)
	{
		delta[0] = vInput.x / flVertAnimFixedPointScale;
		delta[1] = vInput.y / flVertAnimFixedPointScale;
		delta[2] = vInput.z / flVertAnimFixedPointScale;
	}
	inline void SetNDeltaFixed(const Vector& vInputNormal, float flVertAnimFixedPointScale)
	{
		ndelta[0] = vInputNormal.x / flVertAnimFixedPointScale;
		ndelta[1] = vInputNormal.y / flVertAnimFixedPointScale;
		ndelta[2] = vInputNormal.z / flVertAnimFixedPointScale;
	}

	// Ick...can also force fp16 data into this structure for writing to file in legacy format...
	inline void SetDeltaFloat(const Vector& vInput)
	{
		flDelta[0].SetFloat(vInput.x);
		flDelta[1].SetFloat(vInput.y);
		flDelta[2].SetFloat(vInput.z);
	}
	inline void SetNDeltaFloat(const Vector& vInputNormal)
	{
		flNDelta[0].SetFloat(vInputNormal.x);
		flNDelta[1].SetFloat(vInputNormal.y);
		flNDelta[2].SetFloat(vInputNormal.z);
	}

	class CSortByIndex
	{
	public:
		bool operator()(const mstudiovertanim_t& left, const mstudiovertanim_t& right)const
		{
			return left.index < right.index;
		}
	};
	friend class CSortByIndex;

	mstudiovertanim_t() {}
	//private:
	// No copy constructors allowed, but it's needed for std::sort()
	//	mstudiovertanim_t(const mstudiovertanim_t& vOther);
};

struct mstudiovertanim_wrinkle_t : public mstudiovertanim_t
{
	short	wrinkledelta;

	inline void SetWrinkleFixed(float flWrinkle, float flVertAnimFixedPointScale)
	{
		int nWrinkleDeltaInt = flWrinkle / flVertAnimFixedPointScale;
		wrinkledelta = clamp(nWrinkleDeltaInt, -32767, 32767);
	}

	inline Vector4D GetDeltaFixed(float flVertAnimFixedPointScale)
	{
		return Vector4D(delta[0] * flVertAnimFixedPointScale, delta[1] * flVertAnimFixedPointScale, delta[2] * flVertAnimFixedPointScale, wrinkledelta * flVertAnimFixedPointScale);
	}

	inline void GetDeltaFixed4DAligned(Vector4DAligned* vFillIn, float flVertAnimFixedPointScale)
	{
		vFillIn->Set(delta[0] * flVertAnimFixedPointScale, delta[1] * flVertAnimFixedPointScale, delta[2] * flVertAnimFixedPointScale, wrinkledelta * flVertAnimFixedPointScale);
	}

	inline float GetWrinkleDeltaFixed(float flVertAnimFixedPointScale)
	{
		return wrinkledelta * flVertAnimFixedPointScale;
	}
};

struct mstudioflex_t
{
	int					flexdesc;	// input value

	float				target0;	// zero
	float				target1;	// one
	float				target2;	// one
	float				target3;	// zero

	int					numverts;
	int					vertindex;

	inline	mstudiovertanim_t* pVertanim(int i) const { assert(vertanimtype == STUDIO_VERT_ANIM_NORMAL); return (mstudiovertanim_t*)(((byte*)this) + vertindex) + i; };
	inline	mstudiovertanim_wrinkle_t* pVertanimWrinkle(int i) const { assert(vertanimtype == STUDIO_VERT_ANIM_WRINKLE); return  (mstudiovertanim_wrinkle_t*)(((byte*)this) + vertindex) + i; };

	inline	byte* pBaseVertanim() const { return ((byte*)this) + vertindex; };
	inline	int	VertAnimSizeBytes() const { return (vertanimtype == STUDIO_VERT_ANIM_NORMAL) ? sizeof(mstudiovertanim_t) : sizeof(mstudiovertanim_wrinkle_t); }

	int					flexpair;	// second flex desc
	unsigned char		vertanimtype;	// See StudioVertAnimType_t
	unsigned char		unusedchar[3];
	int					unused[6];
};

struct mstudiomodel_t;

struct mstudiomesh_t
{
	int					material;

	int					modelindex;
	mstudiomodel_t* pModel() const { return (mstudiomodel_t*)(((byte*)this) + modelindex); };

	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexoffset;		// vertex mstudiovertex_t

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)

	int					numflexes;			// vertex animation
	int					flexindex;
	inline mstudioflex_t* pFlex(int i) const { return (mstudioflex_t*)(((byte*)this) + flexindex) + i; };

	// special codes for material operations
	int					materialtype;
	int					materialparam;

	// a unique ordinal for this mesh
	int					meshid;

	Vector				center;

	mstudio_meshvertexdata_t vertexdata;

	int					unused[8]; // remove as appropriate

	mstudiomesh_t() {}
private:
	// No copy constructors allowed
//	mstudiomesh_t(const mstudiomesh_t& vOther);
};

struct mstudioeyeball_t
{
	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int		bone;
	Vector	org;
	float	zoffset;
	float	radius;
	Vector	up;
	Vector	forward;
	int		texture;

	int		unused1;
	float	iris_scale;
	int		unused2;

	int		upperflexdesc[3];	// index of raiser, neutral, and lowerer flexdesc that is set by flex controllers
	int		lowerflexdesc[3];
	float	uppertarget[3];		// angle (radians) of raised, neutral, and lowered lid positions
	float	lowertarget[3];

	int		upperlidflexdesc;	// index of flex desc that actual lid flexes look to
	int		lowerlidflexdesc;
	int		unused[4];			// These were used before, so not guaranteed to be 0
	bool	m_bNonFACS;			// Never used before version 44
	char	unused3[3];
	int		unused4[7];

	mstudioeyeball_t() {}
private:
	// No copy constructors allowed
	mstudioeyeball_t(const mstudioeyeball_t& vOther);
};

struct mstudiomodel_t
{
	inline const char* pszName(void) const { return name; }
	char				name[64];

	int					type;

	float				boundingradius;

	int					nummeshes;
	int					meshindex;
	mstudiomesh_t* pMesh(int i) const { return (mstudiomesh_t*)(((byte*)this) + meshindex) + i; };

	// cache purposes
	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexindex;		// vertex Vector
	int					tangentsindex;		// tangents Vector

	// These functions are defined in application-specific code:
//	const vertexFileHeader_t* CacheVertexData(void* pModelData);

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)
//	const mstudio_modelvertexdata_t* GetVertexData(void* pModelData = NULL);
//	const thinModelVertices_t* GetThinVertexData(void* pModelData = NULL);

	int					numattachments;
	int					attachmentindex;

	int					numeyeballs;
	int					eyeballindex;
	inline  mstudioeyeball_t* pEyeball(int i) { return (mstudioeyeball_t*)(((byte*)this) + eyeballindex) + i; };

	mstudio_modelvertexdata_t vertexdata;

	int					unused[8];		// remove as appropriate
};

struct mstudiobodyparts_t
{
	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int					nummodels;
	int					base;
	int					modelindex; // index into models array
	inline mstudiomodel_t* pModel(int i) const { return (mstudiomodel_t*)(((byte*)this) + modelindex) + i; };
};

struct mstudioanimsections_t
{
	int					animblock;
	int					animindex;
};

struct mstudioevent_t
{
	float				cycle;
	int					event;
	int					type;
	inline const char* pszOptions(void) const { return options; }
	char				options[64];

	int					szeventindex;
	inline char* const pszEventName(void) const { return ((char*)this) + szeventindex; }
};

struct mstudioautolayer_t
{
	//private:
	short				iSequence;
	short				iPose;
	//public:
	int					flags;
	float				start;	// beginning of influence
	float				peak;	// start of full influence
	float				tail;	// end of full influence
	float				end;	// end of all influence
};

struct mstudioiklock_t
{
	int			chain;
	float		flPosWeight;
	float		flLocalQWeight;
	int			flags;

	int			unused[4];
};

struct mstudioactivitymodifier_t
{
	int					sznameindex;
	inline char* pszName() { return (sznameindex) ? (char*)(((byte*)this) + sznameindex) : NULL; }
};

struct mstudiomovement_t
{
	int					endframe;
	int					motionflags;
	float				v0;			// velocity at start of block
	float				v1;			// velocity at end of block
	float				angle;		// YAW rotation at end of this blocks movement
	Vector				vector;		// movement vector relative to this blocks initial angle
	Vector				position;	// relative to start of animation???

	mstudiomovement_t() {}
private:
	// No copy constructors allowed
	mstudiomovement_t(const mstudiomovement_t& vOther);
};

struct mstudioanimdesc_t
{
	int					baseptr;
	inline studiohdr_t* pStudiohdr(void) const { return (studiohdr_t*)(((byte*)this) + baseptr); }

	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					numframes;

	// piecewise movement
	int					nummovements;
	int					movementindex;
	inline mstudiomovement_t* const pMovement(int i) const { return (mstudiomovement_t*)(((byte*)this) + movementindex) + i; };

	int					unused1[6];			// remove as appropriate (and zero if loading older versions)	

	int					animblock;
	int					animindex;	 // non-zero when anim data isn't in sections
//	mstudioanim_t* pAnimBlock(int block, int index) const; // returns pointer to a specific anim block (local or external)
//	mstudioanim_t* pAnim(int* piFrame, float& flStall) const; // returns pointer to data and new frame index
//	mstudioanim_t* pAnim(int* piFrame) const; // returns pointer to data and new frame index

	int					numikrules;
	int					ikruleindex;	// non-zero when IK data is stored in the mdl
	int					animblockikruleindex; // non-zero when IK data is stored in animblock file
//	mstudioikrule_t* pIKRule(int i) const;

	int					numlocalhierarchy;
	int					localhierarchyindex;
//	mstudiolocalhierarchy_t* pHierarchy(int i) const;

	int					sectionindex;
	int					sectionframes; // number of frames used in each fast lookup section, zero if not used
	inline mstudioanimsections_t* const pSection(int i) const { return (mstudioanimsections_t*)(((byte*)this) + sectionindex) + i; }

	short				zeroframespan;	// frames per span
	short				zeroframecount; // number of spans
	int					zeroframeindex;
	byte* pZeroFrameData() const { if (zeroframeindex) return (((byte*)this) + zeroframeindex); else return NULL; };
	mutable float		zeroframestalltime;		// saved during read stalls

	mstudioanimdesc_t() {}
private:
	// No copy constructors allowed
	mstudioanimdesc_t(const mstudioanimdesc_t& vOther);
};

struct mstudioseqdesc_t
{
	int					baseptr;
	inline studiohdr_t* pStudiohdr(void) const { return (studiohdr_t*)(((byte*)this) + baseptr); }

	int					szlabelindex;
	inline char* const pszLabel(void) const { return ((char*)this) + szlabelindex; }

	int					szactivitynameindex;
	inline char* const pszActivityName(void) const { return ((char*)this) + szactivitynameindex; }

	int					flags;		// looping/non-looping flags

	int					activity;	// initialized at loadtime to game DLL values
	int					actweight;

	int					numevents;
	int					eventindex;
	inline mstudioevent_t* pEvent(int i) const { assert(i >= 0 && i < numevents); return (mstudioevent_t*)(((byte*)this) + eventindex) + i; };

	Vector				bbmin;		// per sequence bounding box
	Vector				bbmax;

	int					numblends;

	// Index into array of shorts which is groupsize[0] x groupsize[1] in length
	int					animindexindex;

	inline int			anim(int x, int y) const
	{
		if (x >= groupsize[0])
		{
			x = groupsize[0] - 1;
		}

		if (y >= groupsize[1])
		{
			y = groupsize[1] - 1;
		}

		int offset = y * groupsize[0] + x;
		short* blends = (short*)(((byte*)this) + animindexindex);
		int value = (int)blends[offset];
		return value;
	}

	int					movementindex;	// [blend] float array for blended movement
	int					groupsize[2];
	int					paramindex[2];	// X, Y, Z, XR, YR, ZR
	float				paramstart[2];	// local (0..1) starting value
	float				paramend[2];	// local (0..1) ending value
	int					paramparent;

	float				fadeintime;		// ideal cross fate in time (0.2 default)
	float				fadeouttime;	// ideal cross fade out time (0.2 default)

	int					localentrynode;		// transition node at entry
	int					localexitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	float				entryphase;		// used to match entry gait
	float				exitphase;		// used to match exit gait

	float				lastframe;		// frame that should generation EndOfSequence

	int					nextseq;		// auto advancing sequences
	int					pose;			// index of delta animation between end and nextseq

	int					numikrules;

	int					numautolayers;	//
	int					autolayerindex;
	inline mstudioautolayer_t* pAutolayer(int i) const { assert(i >= 0 && i < numautolayers); return (mstudioautolayer_t*)(((byte*)this) + autolayerindex) + i; };

	int					weightlistindex;
	inline float* pBoneweight(int i) const { return ((float*)(((byte*)this) + weightlistindex) + i); };
	inline float		weight(int i) const { return *(pBoneweight(i)); };

	// FIXME: make this 2D instead of 2x1D arrays
	int					posekeyindex;
	float* pPoseKey(int iParam, int iAnim) const { return (float*)(((byte*)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
	float				poseKey(int iParam, int iAnim) const { return *(pPoseKey(iParam, iAnim)); }

	int					numiklocks;
	int					iklockindex;
	inline mstudioiklock_t* pIKLock(int i) const { assert(i >= 0 && i < numiklocks); return (mstudioiklock_t*)(((byte*)this) + iklockindex) + i; };

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char* KeyValueText(void) const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL; }

	int					cycleposeindex;		// index of pose parameter to use as cycle index

	int					activitymodifierindex;
	int					numactivitymodifiers;
	inline mstudioactivitymodifier_t* pActivityModifier(int i) const { assert(i >= 0 && i < numactivitymodifiers); return activitymodifierindex != 0 ? (mstudioactivitymodifier_t*)(((byte*)this) + activitymodifierindex) + i : NULL; };

	int					unused[5];		// remove/add as appropriate (grow back to 8 ints on version change!)

	mstudioseqdesc_t() {}
private:
	// No copy constructors allowed
	mstudioseqdesc_t(const mstudioseqdesc_t& vOther);
};

struct mstudiobone_t
{
	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	Vector				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector				posscale;
	Vector				rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void* pProcedure() const { if (procindex == 0) return NULL; else return  (void*)(((byte*)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropidx; }
	int					contents;		// See BSPFlags.h for the contents flags

	int					unused[8];		// remove as appropriate

	mstudiobone_t() {}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};

struct mstudiotexture_t
{
	int						sznameindex;
	inline char* const		pszName(void) const { return ((char*)this) + sznameindex; }
	int						flags;
	int						used;
	int						unused1;
	mutable void* material;  // fixme: this needs to go away . .isn't used by the engine, but is used by studiomdl
	mutable void* clientmaterial;	// gary, replace with client material pointer if used

	int						unused[10];
};

struct studiohdr2_t
{
	// NOTE: For forward compat, make sure any methods in this struct
	// are also available in studiohdr_t so no leaf code ever directly references
	// a studiohdr2_t structure

	int numsrcbonetransform;
	int srcbonetransformindex;

	int	illumpositionattachmentindex;
	inline int			IllumPositionAttachmentIndex() const { return illumpositionattachmentindex; }

	float flMaxEyeDeflection;
	inline float		MaxEyeDeflection() const { return flMaxEyeDeflection != 0.0f ? flMaxEyeDeflection : 0.866f; } // default to cos(30) if not set

	int linearboneindex;
	
	int sznameindex;
	inline char* pszName() { return (sznameindex) ? (char*)(((byte*)this) + sznameindex) : NULL; }

	int m_nBoneFlexDriverCount;
	int m_nBoneFlexDriverIndex;

	int reserved[56];
};

struct studiohdr_t
{
	int					id;
	int					version;

	int					checksum;		// this has to be the same in the phy and vtx files to load!

	char				name[64];
	int					length;


	Vector				eyeposition;	// ideal eye position

	Vector				illumposition;	// illumination center

	Vector				hull_min;		// ideal movement hull size
	Vector				hull_max;

	Vector				view_bbmin;		// clipping bounding box
	Vector				view_bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;
	inline mstudiobone_t* pBone(int i) const { assert(i >= 0 && i < numbones); return (mstudiobone_t*)(((byte*)this) + boneindex) + i; };

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;
	inline mstudiobonecontroller_t* pBonecontroller(int i) const { assert(i >= 0 && i < numbonecontrollers); return (mstudiobonecontroller_t*)(((byte*)this) + bonecontrollerindex) + i; };

	int					numhitboxsets;
	int					hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t* pHitboxSet(int i) const
	{
		assert(i >= 0 && i < numhitboxsets);
		return (mstudiohitboxset_t*)(((byte*)this) + hitboxsetindex) + i;
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t* pHitbox(int i, int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return NULL;

		return s->pHitbox(i);
	};

	// Calls through to set to get hitbox count for set
	inline int			iHitboxCount(int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	};




	// file local animations? and sequences
//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions
	inline mstudioanimdesc_t* pLocalAnimdesc(int i) const { if (i < 0 || i >= numlocalanim) i = 0; return (mstudioanimdesc_t*)(((byte*)this) + localanimindex) + i; };

	int					numlocalseq;				// sequences
	int					localseqindex;
	inline mstudioseqdesc_t* pLocalSeqdesc(int i) const { if (i < 0 || i >= numlocalseq) i = 0; return (mstudioseqdesc_t*)(((byte*)this) + localseqindex) + i; };

	//public:

//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;
	//public:

	// raw textures
	int					numtextures;
	int					textureindex;
	inline mstudiotexture_t* pTexture(int i) const { assert(i >= 0 && i < numtextures); return (mstudiotexture_t*)(((byte*)this) + textureindex) + i; };


	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char* pCdtexture(int i) const { return (((char*)this) + *((int*)(((byte*)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short* pSkinref(int i) const { return (short*)(((byte*)this) + skinindex) + i; };

	int					numbodyparts;
	int					bodypartindex;
	inline mstudiobodyparts_t* pBodypart(int i) const { return (mstudiobodyparts_t*)(((byte*)this) + bodypartindex) + i; };

	// queryable attachable points
//private:
	int					numlocalattachments;
	int					localattachmentindex;
	//public:

	// animation node to animation node transition graph
//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	inline char* pszLocalNodeName(int iNode) const { assert(iNode >= 0 && iNode < numlocalnodes); return (((char*)this) + *((int*)(((byte*)this) + localnodenameindex) + iNode)); }
	inline byte* pLocalTransition(int i) const { assert(i >= 0 && i < (numlocalnodes* numlocalnodes)); return (byte*)(((byte*)this) + localnodeindex) + i; };

	//public:
	int					EntryNode(int iSequence);
	int					ExitNode(int iSequence);
	char* pszNodeName(int iNode);
	int					GetTransition(int iFrom, int iTo) const;

	int					numflexdesc;
	int					flexdescindex;

	int					numflexcontrollers;
	int					flexcontrollerindex;

	int					numflexrules;
	int					flexruleindex;

	int					numikchains;
	int					ikchainindex;

	int					nummouths;
	int					mouthindex;

	//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	//public:

	int					surfacepropindex;
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropindex; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char* KeyValueText(void) const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;


	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;

	// implementation specific back pointer to virtual data
	mutable void* virtualModel;

	// for demand loaded animation blocks
	int					szanimblocknameindex;
	inline char* const pszAnimBlockName(void) const { return ((char*)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;
	mutable void* animblockModel;
//	byte* GetAnimBlock(int i) const;

	int					bonetablebynameindex;
	inline const byte* GetBoneTableSortedByName() const { return (byte*)this + bonetablebynameindex; }

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void* pVertexBase;
	void* pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte				numAllowedRootLODs;

	byte				unused[1];

	int					unused4; // zero out if version < 47

	int					numflexcontrollerui;
	int					flexcontrolleruiindex;

	float				flVertAnimFixedPointScale;

	int					unused3[1];

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int					studiohdr2index;
	studiohdr2_t* pStudioHdr2() const { return (studiohdr2_t*)(((byte*)this) + studiohdr2index); }

	// Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
	int					NumSrcBoneTransforms() const { return studiohdr2index ? pStudioHdr2()->numsrcbonetransform : 0; }

	inline int			IllumPositionAttachmentIndex() const { return studiohdr2index ? pStudioHdr2()->IllumPositionAttachmentIndex() : 0; }

	inline float		MaxEyeDeflection() const { return studiohdr2index ? pStudioHdr2()->MaxEyeDeflection() : 0.866f; } // default to cos(30) if not set


	inline int			BoneFlexDriverCount() const { return studiohdr2index ? pStudioHdr2()->m_nBoneFlexDriverCount : 0; }

	// NOTE: No room to add stuff? Up the .mdl file format version 
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int					unused2[1];

	studiohdr_t() {}

private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};
