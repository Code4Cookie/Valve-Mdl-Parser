#include "mdlobj.h"
#include "valve/studio.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace 
{
    void ToLower(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });
    }
}

CModel::CModel(const std::string& filename)
{
    m_BoneMap.reserve(MAXSTUDIOBONES);
    m_vecBoneControllers.reserve(MAXSTUDIOBONECTRLS);
    m_vecTextures.reserve(MAXSTUDIOSKINS);

    if (filename != "")
        LoadFile(filename);
}

const CModelBone* CModel::Bone(int iIndex) const
{
    const CModelBone* pBone;

    try {
        pBone = &(m_BoneMap.at(iIndex));
    }
    catch (const std::out_of_range& e) {
        pBone = nullptr;
    }

    return pBone;
}

const std::string* CModel::Texture(int iIndex) const
{
    const std::string* pMat;

    try {
        pMat = &(m_vecTextures.at(iIndex));
    }
    catch (const std::out_of_range& e) {
        pMat = nullptr;
    }

    return pMat;
}

bool CModel::LoadFile(const std::string& filename)
{
    std::ifstream file;

    file.open(filename, std::ifstream::binary);

    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    std::string contents = buffer.str();
    std::vector<char> cstr(contents.c_str(), contents.c_str() + contents.size() + 1);

    studiohdr_t* pModel = reinterpret_cast<studiohdr_t*>(cstr.data());

    if (!pModel)
        return false;

    m_vecRawData = cstr;

    CacheModelInfo(pModel);

    return true;
}

void CModel::CacheModelInfo(studiohdr_t* pMdl)
{
    m_strModelName = pMdl->name;

    ToLower(m_strModelName);

    m_iVersion = pMdl->version;

    int iMatCount = pMdl->numtextures;

    if (iMatCount >= 1)
    {
        m_vecTextures.reserve(iMatCount);

        for (int i = 0; i < iMatCount; i++)
            m_vecTextures.push_back(pMdl->pTexture(i)->pszName());
    }

    m_iMaterialCount = iMatCount;
    m_flMass = pMdl->mass;

    m_hullMins = pMdl->hull_min;
    m_hullMaxs = pMdl->hull_max;

    m_iBoneCount = pMdl->numbones;

    mstudiobone_t* pBone;
    for (int i = 0; i < m_iBoneCount; i++)
    {
        pBone = pMdl->pBone(i);

        if (!pBone)
            break;

        CModelBone bone;
        bone.Cache(pBone);

        m_BoneMap.insert({ i, bone });
    }

    m_iBoneControllerCount = pMdl->numbonecontrollers;

    mstudiobonecontroller_t* pController;
    for (int i = 0; i < m_iBoneControllerCount; i++)
    {
        pController = pMdl->pBonecontroller(i);

        if (!pController)
            break;

        CBoneController ctrl;
        ctrl.Cache(pController);

        m_vecBoneControllers.push_back(ctrl);
    }
  
    m_iBodyPartsCount = pMdl->numbodyparts;

    mstudiobodyparts_t* pBodyParts;
    for (int i = 0; i < m_iBodyPartsCount; i++)
    {
        pBodyParts = pMdl->pBodypart(i);

        if (!pBodyParts)
            break;

        CModelBodyParts parts;
        parts.Cache(pBodyParts);

        m_vecBodyParts.push_back(parts);
    }

    m_iSequenceCount = pMdl->numlocalseq;
    // TODO: Store sequences!

    m_iHitBoxSetCount = pMdl->numhitboxsets;

    mstudiohitboxset_t* pHitBoxSet;

    for (int i = 0; i < m_iHitBoxSetCount; i++)
    {
        pHitBoxSet = pMdl->pHitboxSet(i);

        if (!pHitBoxSet)
            break;

        CHitBoxSet set;
        set.Cache(pHitBoxSet);

        m_vecHitBoxSets.push_back(set);
    }
}

void CStudioEyeBall::Cache(mstudioeyeball_t* pEyeBall)
{
    m_strName = pEyeBall->pszName();

    m_iBone = pEyeBall->bone;
    m_iTexture = pEyeBall->texture;

    m_flZOffset = pEyeBall->zoffset;
    m_flRadius = pEyeBall->radius;

    /*
    m_Origin.x = pEyeBall->org.x;
    m_Origin.y = pEyeBall->org.y;
    m_Origin.z = pEyeBall->org.z;

    m_dirUp.x = pEyeBall->up.x;
    m_dirUp.y = pEyeBall->up.y;
    m_dirUp.z = pEyeBall->up.z;

    m_dirForward.x = pEyeBall->forward.x;
    m_dirForward.y = pEyeBall->forward.y;
    m_dirForward.z = pEyeBall->forward.z;
    */

    m_Origin = pEyeBall->org;
    m_dirUp = pEyeBall->up;
    m_dirForward = pEyeBall->forward;
};

void CStudioModel::Cache(mstudiomodel_t* pModel)
{
    m_strName = pModel->pszName();

    m_iAttachmentCount = pModel->numattachments;
    m_iAttachmentIndex = pModel->attachmentindex;
    
    m_iEyeBallCount = pModel->numeyeballs;
    m_iEyeBallIndex = pModel->eyeballindex;

    m_iMeshCount = pModel->nummeshes;
    m_iMeshIndex = pModel->meshindex;

    m_iTangentsIndex = pModel->tangentsindex;

    m_VertexCount = pModel->numvertices;
    m_iVertexIndex = pModel->vertexindex;

    m_iType = pModel->type;

    m_flBoundingRadius = pModel->boundingradius;

    for (int i = 0; i < m_iEyeBallCount; i++)
    {
        auto ptr = pModel->pEyeball(i);

        if (!ptr)
            break;

        CStudioEyeBall eyeball;
        eyeball.Cache(ptr);

        m_vecEyeBalls.push_back(eyeball);
    }
}

void CModelBone::Cache(mstudiobone_t* pBone)
{
    m_strName = pBone->pszName();

    m_iContents = pBone->contents;
    m_iFlags = pBone->flags;
    m_iParent = pBone->parent;

    /*
    m_vecPosition.x = pBone->pos.x;
    m_vecPosition.y = pBone->pos.y;
    m_vecPosition.z = pBone->pos.z;
    */

    m_vecPosition = pBone->pos;
}

void CModelBodyParts::Cache(mstudiobodyparts_t* pBodyPart)
{
    m_strName = pBodyPart->pszName();

    m_iBase = pBodyPart->base;
    m_iModelCount = pBodyPart->nummodels;
    m_iModelIndex = pBodyPart->modelindex;

    for (int i = 0; i < m_iModelCount; i++)
    {
        auto ptr = pBodyPart->pModel(i);

        if (!ptr)
            break;

        CStudioModel mdl;
        mdl.Cache(ptr);

        m_vecStudioModels.push_back(mdl);
    }

}

void CBoneController::Cache(mstudiobonecontroller_t* pController)
{
    m_iBone = pController->bone;
    m_iInputField = pController->inputfield;
    m_iRest = pController->rest;
    m_iType = pController->type;

    m_flEnd = pController->end;
    m_flStart = pController->start;
}

void CHitBoxSet::Cache(mstudiohitboxset_t* pPtr)
{
    m_strName = pPtr->pszName();

    m_iHitBoxCount = pPtr->numhitboxes;
    m_iHitBoxIndex = pPtr->hitboxindex;
    
    for (int i = 0; i < m_iHitBoxCount; i++)
    {
        auto ptr = pPtr->pHitbox(i);

        if (!ptr)
            break;

        CBBox box;
        box.Cache(ptr);

        m_vecHitBoxes.push_back(box);
    }
}

void CBBox::Cache(mstudiobbox_t* pPtr)
{
    m_strName = pPtr->pszHitboxName();

    m_iBone = pPtr->bone;
    m_iGroup = pPtr->group;

    m_bbMin = pPtr->bbmin;
    m_bbMax = pPtr->bbmax;
}

Vector3D& Vector3D::operator=(Vector other)
{
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
}
