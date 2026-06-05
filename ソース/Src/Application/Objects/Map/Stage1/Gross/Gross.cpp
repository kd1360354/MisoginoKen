#include "Gross.h"

void Gross::Init()
{
	// ★ MAP_NAME_SAKURA を使用
	SetName(MapParam::GrassParam::NAME);
	{
		m_spModel = std::make_shared<KdModelData>();
		// ★ SAKURA_MODEL_PATH を使用
		m_spModel= KdAssets::Instance().m_modeldatas.GetData(MapParam::GrassParam::MODEL_PATH);//->Load(GROSS_MODEL_PATH);

		Math::Matrix scaleMat = Math::Matrix::CreateScale(rand() % 3 + 1);
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(rand() % 180));
		Math::Matrix transMat = Math::Matrix::CreateTranslation(rand() % 200 - 100, 0, rand() % 200 - 100);
		m_mWorld = scaleMat * rotMat * transMat;
	}
}


void Gross::DrawLit()
{

	MapBase::DrawLit();
}

void Gross::GenerateDepthMapFromLight()
{
	MapBase::GenerateDepthMapFromLight();
}