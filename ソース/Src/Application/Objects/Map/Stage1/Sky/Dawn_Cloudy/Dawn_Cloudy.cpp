#include "Dawn_Cloudy.h"

void Dawn_Cloudy::Init()
{
	SetName("Dawn_Cloudy");
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Map/Stage1/Sky/Dawn_Cloudy/Dawn_Cloudy.gltf");
	}
}

void Dawn_Cloudy::Update()
{
}

void Dawn_Cloudy::DrawUnLit()
{
	SkyBase::DrawUnLit();
}
