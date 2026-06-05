#include "MidNight.h"
#include"../../../../../Constants/MapConstants/SkyConstants.h"
void MidNight::Init()
{
	SkyBase::Init();
	SetName(SkyParam::MidNightParam::NAME);
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j); // これで json 版が呼ばれる！
	}
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load(m_ModelPath.empty() ? SkyParam::MidNightParam::MODEL_PATH : m_ModelPath);
	}
}

void MidNight::Update()
{
	SkyBase::Update();
}

void MidNight::DrawUnLit()
{
	SkyBase::DrawUnLit();
}

void MidNight::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}
