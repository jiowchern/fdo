#include "StdAfx.h"
#include "ParticleHelper.h"
#include "ParticleSystem.h"
#include "FDO_ParticleObject.h"
#include "J_SoundManager.h"
#include "R_3DInterface.h"
#include "R_EmitterPlayer.h"
#include "R_EmitterManager.h"
#include "R_ParticleRender.h"
#include "R_PRenderManager.h"
#include "R_ParticleEmitter.h"

namespace FDO
{

/**
 *
 */
LPD3DXMATRIX GetPrimitiveMatrix(LPD3DXMATRIX out, ParticleInstPtr particleInst, 
    R_ParticleRenderPtr pRnd, R_ParticleEmitterPtr pEmitter, R_ParticlePtr pPart)
{
    //計算矩陣
    R_MATRIX4 ucs;
    ucs.Identity();

    // 計算縮放大小
    ucs.ScaleX(pPart->m_vScale.x);
    ucs.ScaleY(pPart->m_vScale.y);
    ucs.ScaleX(pEmitter->GetEmitterParameter(EMITTER_UCS_SCALE_X));
    ucs.ScaleY(pEmitter->GetEmitterParameter(EMITTER_UCS_SCALE_Y));

    R_VECTOR3 axis
    (
        pEmitter->GetParticleParameter(PARTICLE_SPIN_AXIS_X),
        pEmitter->GetParticleParameter(PARTICLE_SPIN_AXIS_Y),
        pEmitter->GetParticleParameter(PARTICLE_SPIN_AXIS_Z)
    );

    R_MATRIX4 mat;
    mat.Identity();

    // 計算旋轉
    //  SPIN PARTICLE
    if ( pEmitter->IsEnabledControl(PARTICLE_SPIN_X) )
    {
        mat.RotateXAxis( (int)pPart->m_fSpinDegree );
    }
    else if ( pEmitter->IsEnabledControl(PARTICLE_SPIN_Y) )
    {
        mat.RotateYAxis( (int)pPart->m_fSpinDegree );
    }
    else if ( pEmitter->IsEnabledControl(PARTICLE_SPIN_Z) )
    {
        mat.RotateZAxis( (int)pPart->m_fSpinDegree );
    }
    //	<<	[2003/12/17][ROCKDOG][APPEND]
    else if ( pEmitter->IsEnabledControl(PARTICLE_SPIN_AXIS) )
    {
        if ( pRnd->IsEnabledControl(KEYFRAME_PLAY) )
        {
            RotateUcsByAxis(pRnd->GetUCS().GetPosition(),axis,mat,(float)pPart->m_fSpinDegree/RAD_PER_DEFS);
        }
    }
    //	>>	[2003/12/17][ROCKDOG][APPEND]
    else if ( pEmitter->IsEnabledControl(PARTICLE_SPIN_POSITION_AXIS) ) //  SPIN BY AXIS
    {
        if ( pRnd->IsEnabledControl(KEYFRAME_PLAY) )
        {
            RotatePosByAxis(pRnd->GetUCS().GetPosition(),// RELATED UCS
                            axis,pPart->m_vPosition,(float)pPart->m_fSpinDegree/RAD_PER_DEFS);
        }
    }

    R_MATRIX4 result; // 結合矩陣
    if ( pEmitter->IsEnabledControl(PARTICLE_SPIN_X) ||
         pEmitter->IsEnabledControl(PARTICLE_SPIN_Y) ||
         pEmitter->IsEnabledControl(PARTICLE_SPIN_Z) ||
         pEmitter->IsEnabledControl(PARTICLE_SPIN_AXIS) )
    {
        result = ucs * mat;
    }
    else
    {
        result = ucs;
    }

    if (particleInst->IsControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE))
    {
        R_MATRIX4 rstMat;
        memcpy(&rstMat, &particleInst->GetParticleSystem()->GetInvViewMatrix(), sizeof_MATRIX4);
        result = result * rstMat;
    }

    // 設定位移
    result.SetPosition(pPart->m_vPosition); 
    // 設定世界座標
	memcpy(out, &result, sizeof_MATRIX4);
    if (!particleInst->IsControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE))
    {
        D3DXMATRIX matTemp;
        D3DXMatrixRotationYawPitchRoll
        (
            &matTemp, 
            -(particleInst->GetRotation().y),
            -(particleInst->GetRotation().x),
            -(particleInst->GetRotation().z)
        );
        // TODO: 插入世界座標(改變整沱位置)
        (*out) *= particleInst->GetParticleSystem()->GetInvViewMatrix() * matTemp * particleInst->GetWorldMatrix();
    }
    return out;
}

/**
 *
 */
D3DMATERIAL9 * GetMaterial(D3DMATERIAL9 * out, R_ParticlePtr pPart)
{
    // 指定分子的顏色
    memset(out, 0, sizeof_D3DMATERIAL9); //將材質質感陣列歸零
    out->Ambient.r = RGB_GETRED  (pPart->m_cColor) * 0.003921f ; // 255.f;
    out->Ambient.g = RGB_GETGREEN(pPart->m_cColor) * 0.003921f ; // 255.f;
    out->Ambient.b = RGB_GETBLUE (pPart->m_cColor) * 0.003921f ; // 255.f;
    out->Ambient.a = RGB_GETALPHA(pPart->m_cColor) * 0.003921f ; // 255.f;
    out->Diffuse = out->Ambient;
    return out;
}

/**
 *
 */
bool RenderWithPrimitive(ParticleInstPtr particleInst, R_ParticleRenderPtr pRnd , unsigned int &outflushedPartCount)
{
    LPDIRECT3DDEVICE9 pd3dDevice = particleInst->GetD3DDevice();
    F_RETURN(pd3dDevice != NULL);

    R_ParticleEmitterPtr pEmitter = pRnd->GetEmitter();
    F_RETURN(pEmitter != NULL);

    D3DXMATRIX worldMatrix;
    D3DXMatrixIdentity(&worldMatrix);
    pd3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix);

    //D3DMATERIAL9 material;
    //memset(&material, 0, sizeof_D3DMATERIAL9); //將材質質感陣列歸零
    //material.Diffuse = material.Ambient = D3DXCOLOR(0xFFFFFFFF);
    //pd3dDevice->SetMaterial(&material);

    //檢查每個分子是否須要做繪製的動作
    int count = static_cast<int>(pEmitter->GetEmitterParameter(EMITTER_PARTICLE_COUNT));
    _LOOP(count, i)
	{
        // 判定是否顯示中分子
        F_CONTINUE(pEmitter->IsParticleValid(i));
        //取得分子資料
		R_ParticlePtr pPart = pEmitter->GetParticleByIndex(i);
        F_CONTINUE(pPart != NULL);

        F_CONTINUE(RGB_GETALPHA(pPart->m_cColor) > 0);
        if (pPart->m_ppTextures.empty())
        {
            //取得貼圖名稱
            F_CONTINUE(pPart->m_ppTextureName[0] != NULL);
            fstring fileName(strupr(pPart->m_ppTextureName[0]));
            F_CONTINUE(!fileName.empty());
            //取得貼圖
            LPDIRECT3DTEXTURE9 pTexture = particleInst->GetTexture(fileName, particleInst->IsControlEnabled(PARTICLE_FLAG::STATIC_RESOURCE));
            F_CONTINUE(pTexture != NULL);

            pPart->m_ppTextures.push_back(pTexture);
        }
        //設定貼圖
        //pd3dDevice->SetTexture(0, pTexture);
		if(pPart->m_ppTextures.size())
		{
			pd3dDevice->SetTexture(0, pPart->m_ppTextures.front());
		}
		else
		{
			pd3dDevice->SetTexture(0, 0);
		}
        

        //設定矩陣
        //D3DXMATRIX worldMatrix;
        //pd3dDevice->SetTransform(D3DTS_WORLD, GetPrimitiveMatrix(&worldMatrix, particleInst, pRnd, pEmitter, pPart));
        GetPrimitiveMatrix(&worldMatrix, particleInst, pRnd, pEmitter, pPart);
        Plane plane
        (
            D3DXVECTOR3(-1.f, -1.f, 0.f),
            D3DXVECTOR3( 1.f, -1.f, 0.f),
            D3DXVECTOR3(-1.f,  1.f, 0.f),
            D3DXVECTOR3( 1.f,  1.f, 0.f)
        );
        for (int i = 0; i < plane.count; ++i)
        {
            D3DXVec3TransformCoord(&plane.pts[i], &plane.pts[i], &worldMatrix);
        }
        //設定質料
        D3DMATERIAL9 material;
        pd3dDevice->SetMaterial(GetMaterial(&material, pPart));
        //繪製平面
        DrawPrimitive(pd3dDevice, plane);

		outflushedPartCount++ ; 
    }
    return true;
}
/**
 *
 */
LPD3DXMATRIX GetMeshMatrix(LPD3DXMATRIX out, ParticleInstPtr particleInst, 
    R_ParticleRenderPtr pRnd, R_ParticleEmitterPtr pEmitter, R_ParticlePtr pPart)
{
    // 計算轉換的矩陣
    R_MATRIX4 mat;
    mat.Identity();

    R_VECTOR3 vz;

    // 計算模型面向
    if( pEmitter->IsEnabledControl(MESH_FACE_TANGENT) )
    {
        vz = pPart->m_vDirection;
    }
    else if( pEmitter->IsEnabledControl(MESH_FACE_DIRECTION) )
    {
        //  THE FORWARD VECTOR OF UCS OF MESH FOCUS TANGENT VECTOR
        //  CALCULATE UCS
        vz.x = pEmitter->GetParticleParameter(PARTICLE_MESH_DIRECTION_X);
        vz.y = pEmitter->GetParticleParameter(PARTICLE_MESH_DIRECTION_Y);
        vz.z = pEmitter->GetParticleParameter(PARTICLE_MESH_DIRECTION_Z);
    }
    else if( pEmitter->IsEnabledControl(MESH_FACE_CAMERA) )
    {
        // 面向攝影機
        //D3DXMATRIX matTemp;
        //D3DXMatrixRotationYawPitchRoll
        //(
        //    &matTemp,
        //    -(particleInst->GetRotation().y),
        //    -(particleInst->GetRotation().x),
        //    -(particleInst->GetRotation().z)
        //);
        //D3DXMatrixMultiply(&matTemp, &matTemp, &particleInst->GetParticleSystem()->GetInvViewMatrix());
        //memcpy(&mat, &matTemp, sizeof_MATRIX4);

        memcpy(&mat, &particleInst->GetParticleSystem()->GetInvViewMatrix(), sizeof_MATRIX4);
    }
    else if ( pEmitter->IsEnabledControl(MESH_FACE_HORIZONTAL) )
    {
        memcpy(&mat, &particleInst->GetParticleSystem()->GetInvViewMatrix(), sizeof_MATRIX4);
        mat.SetUpVector(R_VECTOR3(0.0f,1.0f,0.0f));
    }
    else // MESH_FACE_NONE
    {
        //  FOCUS WCS X,Y,Z NX,NY,NZ
    }

    // 向量方向計算
    if (pEmitter->IsEnabledControl(MESH_FACE_TANGENT) ||
        pEmitter->IsEnabledControl(MESH_FACE_DIRECTION))
    {
        R_VECTOR3   vx,vy;

        if ( fabs(vz.x) > fabs(vz.y) && fabs(vz.x) > fabs(vz.z) )
        {
            vx.Vec3Cross2V(R_VECTOR3(0.0f,1.0f,0.0f),vz);
            vx.Vec3Normalize();
            vy.Vec3Cross2V(vz,vx);
            vy.Vec3Normalize();
        }
        else if ( fabs(vz.y) > fabs(vz.x) && fabs(vz.y) > fabs(vz.z) )
        {
            vy.Vec3Cross2V(vz,R_VECTOR3(1.0f,0.0f,0.0f));
            vy.Vec3Normalize();
            vx.Vec3Cross2V(vy,vz);
            vx.Vec3Normalize();
        }
        else
        {
            vx.Vec3Cross2V(R_VECTOR3(0.0f,1.0f,0.0f),vz);
            vx.Vec3Normalize();
            vy.Vec3Cross2V(vz,vx);
            vy.Vec3Normalize();
        }
        vz.Vec3Normalize();

        mat.SetForwardVector(vz);
        mat.SetRightVector(vx);
        mat.SetUpVector(vy);
    }

    // 計算模型縮放大小
    mat.ScaleX( pEmitter->GetEmitterParameter(EMITTER_UCS_SCALE_X ) );
    mat.ScaleY( pEmitter->GetEmitterParameter(EMITTER_UCS_SCALE_Y ) );
    mat.ScaleZ( pEmitter->GetEmitterParameter(EMITTER_UCS_SCALE_Z ) );
    mat.ScaleX( pPart->m_vScale.x );
    mat.ScaleY( pPart->m_vScale.y );
    mat.ScaleZ( pPart->m_vScale.z );

    //計算模型旋轉角度
    R_VECTOR3 axis
    (
        pEmitter->GetParticleParameter(PARTICLE_SPIN_AXIS_X),
        pEmitter->GetParticleParameter(PARTICLE_SPIN_AXIS_Y),
        pEmitter->GetParticleParameter(PARTICLE_SPIN_AXIS_Z)
    );

    if( pEmitter->IsEnabledControl(PARTICLE_SPIN_X) )
    {
        mat.RotateXAxis( (int)pPart->m_fSpinDegree );
    }
    else if( pEmitter->IsEnabledControl(PARTICLE_SPIN_Y) )
    {
        mat.RotateYAxis( (int)pPart->m_fSpinDegree );
    }
    else if( pEmitter->IsEnabledControl(PARTICLE_SPIN_Z) )
    {
        mat.RotateZAxis( (int)pPart->m_fSpinDegree );
    }
    else if( pEmitter->IsEnabledControl(PARTICLE_SPIN_AXIS) )
    {
        if( pRnd->IsEnabledControl(KEYFRAME_PLAY) )
        {
            RotateUcsByAxis( pRnd->GetUCS().GetPosition(), axis, mat,
                (float)pPart->m_fSpinDegree/RAD_PER_DEFS );
        }
    }
    else if( pEmitter->IsEnabledControl(PARTICLE_SPIN_POSITION_AXIS) ) // SPIN BY AXIS
    {
        if( pRnd->IsEnabledControl(KEYFRAME_PLAY) )
        {
            RotatePosByAxis( pRnd->GetUCS().GetPosition(), axis, // RELATED UCS
                pPart->m_vPosition, (float)pPart->m_fSpinDegree/RAD_PER_DEFS );
        }
    }

    // 設定模型位移
    mat.SetPosition( pPart->m_vPosition );
    // 設定世界座標
    memcpy(out, &mat, sizeof_MATRIX4);

    if (!particleInst->IsControlEnabled(PARTICLE_FLAG::INTERNAL_UPDATE))
    {
        D3DXMatrixMultiply(out, out, &particleInst->GetWorldMatrix());
    }

    return out;
}
/**
 *
 */
void DrawMesh(ParticleInstPtr particleInst, R_ParticlePtr pPart, LPD3DXMESH pMesh)
{
    if (pPart->m_ppTextures.empty())
    {
        pPart->m_ppTextures.resize(pPart->m_uiNumOfMaterial, NULL);
    }

    _ULOOP(pPart->m_uiNumOfMaterial, i)
    {
        //if (pPart->m_ppTextures[i] == NULL)
        //{
            F_CONTINUE(pPart->m_ppTextureName[i] != NULL);

            fstring fileName(strupr(pPart->m_ppTextureName[i]));
            F_CONTINUE(!fileName.empty());

            LPDIRECT3DTEXTURE9 pTexture = particleInst->GetTexture(fileName, particleInst->IsControlEnabled(PARTICLE_FLAG::STATIC_RESOURCE));
            F_CONTINUE(pTexture != NULL);

            //pPart->m_ppTextures[i] = pTexture;
        //}

        particleInst->GetD3DDevice()->SetTexture(0, pTexture); // 設定貼圖
        //particleInst->GetD3DDevice()->SetTexture(0, pPart->m_ppTextures[i]); // 設定貼圖
        //m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        //m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

        // 繪製模型
        pMesh->DrawSubset(i); //  RENDER OBJECT
    }
}
/**
 *
 */
bool RenderWithMesh(ParticleInstPtr particleInst, R_ParticleRenderPtr pRnd , unsigned int &outflushedPartCount)
{
    LPDIRECT3DDEVICE9 pd3dDevice = particleInst->GetD3DDevice();
    F_RETURN(pd3dDevice != NULL);

    R_ParticleEmitterPtr pEmitter = pRnd->GetEmitter();
    F_RETURN(pEmitter != NULL);

    //檢查每個分子是否須要做繪製的動作
    int count = static_cast<int>(pEmitter->GetEmitterParameter(EMITTER_PARTICLE_COUNT));
    _LOOP(count, i)
	{
        F_CONTINUE(pEmitter->IsParticleValid(i));

		R_ParticlePtr pPart = pEmitter->GetParticleByIndex(i);
        F_CONTINUE(pPart != NULL);

        F_CONTINUE(RGB_GETALPHA(pPart->m_cColor) > 0);

        if (pPart->m_pMesh == NULL)
        {
            F_CONTINUE(pPart->m_iMeshID >= 0);
            F_CONTINUE(pPart->m_pMeshName != NULL);

            fstring fileName(strupr(pPart->m_pMeshName));
            F_CONTINUE(!fileName.empty());

            LPD3DXMESH pMesh = particleInst->GetMesh(fileName, particleInst->IsControlEnabled(PARTICLE_FLAG::STATIC_RESOURCE));
            F_CONTINUE(pMesh != NULL)

            pPart->m_pMesh = pMesh;
        }

        //設定矩陣
        D3DXMATRIX worldMatrix;
        pd3dDevice->SetTransform(D3DTS_WORLD, GetMeshMatrix(&worldMatrix, particleInst, pRnd, pEmitter, pPart));
        //設定質料
        D3DMATERIAL9 material;
        pd3dDevice->SetMaterial(GetMaterial(&material, pPart));

        DrawMesh(particleInst, pPart, pPart->m_pMesh);
        //DrawMesh(particleInst, pPart, pMesh);

		outflushedPartCount++ ;
    }
    return true;
}
/**
 *
 */
bool RenderParticle(ParticleInstPtr particleInst , unsigned int &outflushedPartCount)
{
    R_PRenderManagerPtr pPRenderManager = particleInst->GetPRenderManager();
    F_RETURN(pPRenderManager != NULL);

    _LOOP(pPRenderManager->GetPRenderCount(), i)
    {
        F_CONTINUE(pPRenderManager->IsPRenderValid(i));

        R_ParticleRenderPtr pRnd = pPRenderManager->GetPRenderByIndex(i);
        F_CONTINUE(pRnd != NULL);
        F_CONTINUE(pRnd->BeginRender())
        
        if (pRnd->GetRenderStatus(RENDER_WITH_PRIMITIVE))
        {
            RenderWithPrimitive(particleInst, pRnd , outflushedPartCount);
        } else
        if (pRnd->GetRenderStatus(RENDER_WITH_MESH))
        {
            RenderWithMesh(particleInst, pRnd , outflushedPartCount);
        } else
        if (pRnd->GetRenderStatus(RENDER_WITH_POINTSPRITE))
        {
            //RenderWithPointSprite();
        }
        pRnd->EndRender();

        R_ParticleEmitterPtr pEmt = pRnd->GetEmitter();
        _LOOP(pEmt->GetSoundItemCount(), j)
        {
            R_EMITTER_SOUNDITEM *pItem = pEmt->GetSoundItemByIndex(j);
            F_CONTINUE(pItem != NULL);

            D3DXVECTOR3 position;

            if (particleInst->IsUseFixSoundPosition())
            {
                position = particleInst->GetFixSoundPosition();
            }
            else
            {
                R_MATRIX4 m(pEmt->GetUCS());
                position.x = particleInst->GetPosition().x + m.GetPosition().x;
                position.y = particleInst->GetPosition().y + 0.f;
                position.z = particleInst->GetPosition().z + m.GetPosition().z;
            }

            g_SoundManager._Play3DSoundInMemory
            (
                std::string(PARTICLE_SOUND_PATH) + std::string(pItem->m_szMediaName),
                position
            );
        }
        pEmt->ClearAllSoundItem();
		
    }

    return true;
}

} // namespace FDO
