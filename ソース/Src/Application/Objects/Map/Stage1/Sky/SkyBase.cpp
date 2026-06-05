#include "SkyBase.h"
#include"../../../../Constants/MapConstants/SkyConstants.h"
void SkyBase::Init()
{
	SetScale(m_Scale);
}

void SkyBase::Update()
{
	Math::Vector3 _Pos = SkyParam::Base::POS;
	m_Angle += SkyParam::Base::ROTATION_SPEED;
	m_mWorld = Math::Matrix::CreateScale(m_Scale) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_Angle)) * Math::Matrix::CreateTranslation(_Pos);
}

void SkyBase::DrawUnLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);

		//裏面カリンングをOFF
		// NOTE: 裏面カリングをOFFにするコードがありませんが、コメントの意図に従います。

		//描画
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		KdShaderManager::Instance().UndoRasterizerState();
		KdShaderManager::Instance().UndoDepthStencilState();
	}
}