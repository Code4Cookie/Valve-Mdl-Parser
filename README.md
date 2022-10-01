# Valve-Mdl-Parser
A modern and external parser for Valve's studio model format.
 
## Features
- Wraps Valve's antiquated headers with modern C++
- External parser (Source SDK not required)
- Tested and successfully parsed models from the following games:
	- CS:GO
	- Garry's Mod
	- Half Life 2
	- Left 4 Dead 2

## Usage
```
CModel::CModel(const std::string& filename)

const CModelBone* CModel::Bone(int iIndex) const
const std::string* CModel::Texture(int iIndex) const

inline const std::vector<std::string>& CModel::GetMaterials() const
inline const std::vector<CBoneController>& CModel::GetBoneControllers() const
inline const std::vector<CModelBodyParts>& CModel::GetBodyParts() const
inline const std::vector<CHitBoxSet>& CModel::GetHitBoxSets() const
inline const std::vector<char>& CModel::GetRawData() const

inline const std::string& CModel::Name() const

inline const Vector3D& CModel::HullMins() const
inline const Vector3D& CModel::HullMaxs() const

inline int CModel::MaterialCount() const
inline CModel::float Mass() const
```

View the header file for more information on the additional structs.