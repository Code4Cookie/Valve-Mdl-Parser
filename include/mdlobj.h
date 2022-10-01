#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct studiohdr_t;
struct mstudioeyeball_t;
struct mstudiomodel_t;
struct mstudiobonecontroller_t;
struct mstudiobodyparts_t;
struct mstudiobone_t;
struct mstudiohitboxset_t;
struct mstudiobbox_t;

class Vector;

struct Vector3D
{
	float x, y, z;

	Vector3D& operator=(Vector other);
};

template<typename T>
struct ICacheable
{
	virtual void Cache(T* pPtr) = 0;
};

// bounding box
struct CBBox : ICacheable<mstudiobbox_t>
{
	std::string m_strName;

	Vector3D m_bbMin;
	Vector3D m_bbMax;

	int m_iBone;
	int m_iGroup;

	virtual void Cache(mstudiobbox_t* pPtr) override;
};

struct CHitBoxSet : ICacheable<mstudiohitboxset_t>
{
	std::vector<CBBox> m_vecHitBoxes;

	std::string m_strName;

	int m_iHitBoxCount;
	int m_iHitBoxIndex;

	virtual void Cache(mstudiohitboxset_t* pPtr) override;
};

struct CStudioEyeBall : ICacheable<mstudioeyeball_t>
{
	Vector3D m_Origin; // "org" in mstudioeyeball_t. pretty sure it means "origin"
	Vector3D m_dirUp;
	Vector3D m_dirForward;

	std::string m_strName;

	int m_iBone;
	int m_iTexture;

	float m_flZOffset;
	float m_flRadius;

	virtual void Cache(mstudioeyeball_t* pEyeBall) override;
};

struct CStudioModel : ICacheable<mstudiomodel_t>
{
	std::string m_strName;

	int m_iMeshCount;
	int m_iMeshIndex;

	int m_iType;

	int m_VertexCount;
	int m_iVertexIndex;
	int m_iTangentsIndex;

	int m_iAttachmentCount;
	int m_iAttachmentIndex;

	int m_iEyeBallCount;
	int m_iEyeBallIndex;

	float m_flBoundingRadius;

	std::vector<CStudioEyeBall> m_vecEyeBalls;
	
	virtual void Cache(mstudiomodel_t* pModel) override;
};

struct CModelBodyParts : ICacheable<mstudiobodyparts_t>
{
	std::string m_strName;

	int m_iModelCount;
	int m_iBase;
	int m_iModelIndex;

	std::vector<CStudioModel> m_vecStudioModels;

	virtual void Cache(mstudiobodyparts_t* pBodyPart) override;
};

struct CBoneController : ICacheable<mstudiobonecontroller_t>
{
	int m_iBone;
	int m_iType;

	int m_iRest;
	int m_iInputField;

	float m_flStart;
	float m_flEnd;

	virtual void Cache(mstudiobonecontroller_t* pController) override;
};

struct CModelBone : ICacheable<mstudiobone_t>
{
	std::string m_strName;

	int m_iParent;
	int m_iFlags;
	int m_iContents;

	Vector3D m_vecPosition;

	virtual void Cache(mstudiobone_t* pBone) override;
};

class CModel
{
public:
	CModel(const std::string& filename);

	const CModelBone* Bone(int iIndex) const;
	const std::string* Texture(int iIndex) const;

	inline const std::vector<std::string>& GetMaterials() const;
	inline const std::vector<CBoneController>& GetBoneControllers() const;
	inline const std::vector<CModelBodyParts>& GetBodyParts() const;
	inline const std::vector<CHitBoxSet>& GetHitBoxSets() const;
	inline const std::vector<char>& GetRawData() const;

	inline const std::string& Name() const;

	inline const Vector3D& HullMins() const;
	inline const Vector3D& HullMaxs() const;
	
	inline int MaterialCount() const;
	inline float Mass() const;

private:
	std::unordered_map<int, CModelBone> m_BoneMap{};

	std::vector<CBoneController> m_vecBoneControllers{};
	std::vector<CModelBodyParts> m_vecBodyParts{};
	std::vector<CHitBoxSet> m_vecHitBoxSets{};
	std::vector<std::string> m_vecTextures{};
	std::vector<char> m_vecRawData{};

	std::string m_strModelName{};

	Vector3D m_hullMins{};
	Vector3D m_hullMaxs{};

	float m_flMass = 0.0f;

	int m_iVersion = -1;

	int m_iBoneCount = 0;
	int m_iMaterialCount = 0;
	int m_iBoneControllerCount = 0;
	int m_iBodyPartsCount = 0;
	int m_iSequenceCount = 0;
	int m_iHitBoxSetCount = 0;

	bool LoadFile(const std::string& filename);
	void CacheModelInfo(studiohdr_t* pMdl);

	CModel() {}
};

inline int CModel::MaterialCount() const
{
	return m_iMaterialCount;
}

inline const std::vector<std::string>& CModel::GetMaterials() const
{
	return m_vecTextures;
}

inline const std::vector<CBoneController>& CModel::GetBoneControllers() const
{
	return m_vecBoneControllers;
}

inline const std::vector<CModelBodyParts>& CModel::GetBodyParts() const
{
	return m_vecBodyParts;
}

inline const std::vector<CHitBoxSet>& CModel::GetHitBoxSets() const
{
	return m_vecHitBoxSets;
}

inline const std::vector<char>& CModel::GetRawData() const
{
	return m_vecRawData;
}

inline const std::string& CModel::Name() const
{
	return m_strModelName;
}

inline const Vector3D& CModel::HullMins() const
{
	return m_hullMins;
}

inline const Vector3D& CModel::HullMaxs() const
{
	return m_hullMaxs;
}

inline float CModel::Mass() const
{
	return m_flMass;
}