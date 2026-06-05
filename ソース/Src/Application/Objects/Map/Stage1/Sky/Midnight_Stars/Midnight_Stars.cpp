#include "Midnight_Stars.h"

void Midnight_Stars::Init()
{
	SetName("Midnight_Stars");
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Map/Stage1/Sky/Midnight/Midnight_Stars.gltf");
	}
}

void Midnight_Stars::Update()
{
}

void Midnight_Stars::DrawUnLit()
{
	SkyBase::DrawUnLit();
}
