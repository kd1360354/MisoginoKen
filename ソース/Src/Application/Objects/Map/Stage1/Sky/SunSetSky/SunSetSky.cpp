#include "SunSetSky.h"

void SunSetSky::Init()
{
	SetName("SunSetSky");
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Map/Stage1/Sky/SunSetSky/SunSet.gltf");
	}
}

void SunSetSky::Update()
{
}

void SunSetSky::DrawUnLit()
{
	SkyBase::DrawUnLit();
}
