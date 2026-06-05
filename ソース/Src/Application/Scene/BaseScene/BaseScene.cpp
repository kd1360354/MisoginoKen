#include "BaseScene.h"
#include"../../Objects/Camera/CameraBase.h"

void BaseScene::PreUpdate()
{
	//寿命の尽きたオブジェクトをリストから削除
	for (auto it = m_objList.begin(); it != m_objList.end();)
	{
		if ((*it)->IsExpired())
		{
			it = m_objList.erase(it);
		}
		else
		{
			++it;
		}
	}

	//全オブジェクトのPreUpdate
	for (const auto& obj : m_objList)
	{
		obj->PreUpdate();
	}
}

void BaseScene::Update()
{
	//シーン固有の更新処理
	SceneUpdate();

	for (const auto& obj : m_objList)
	{
		obj->Update();
	}

	// エフェクト更新
	KdEffekseerManager::GetInstance().Update();
}

void BaseScene::PostUpdate()
{
	for (const auto& obj : m_objList)
	{
		obj->PostUpdate();
	}
}

void BaseScene::PreDraw()
{
	if (auto spActiveCamera = m_wpActiveCamera.lock())
	{
		if (const auto& kdCam = spActiveCamera->GetCamera())
		{
			kdCam->SetToShader();
		}
	}

	for (const auto& obj : m_objList)
	{
		obj->PreDraw();
	}
}

void BaseScene::Draw()
{
	// 3Dオブジェクトを描画する直前に、光と霧の情報をシェーダーに送る
	KdShaderManager::Instance().WorkAmbientController().Draw();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクトはBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawUnLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawBright();
		}
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();

	// エフェクト描画
	KdEffekseerManager::GetInstance().Draw();
}

void BaseScene::PostDraw()
{
	for (auto& obj : m_objList)
	{
		obj->PostDraw();
	}
}

void BaseScene::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.Begin();
	for (auto& obj : m_objList)
	{
		obj->DrawSprite();
	}
	KdShaderManager::Instance().m_spriteShader.End();
}

void BaseScene::AddObject(const std::shared_ptr<KdGameObject>& obj)
{
	if (obj)
	{
		m_objList.push_back(obj);
	}
}

void BaseScene::SetActiveCamera(const std::shared_ptr<CameraBase>& camera)
{
	if (camera)
	{
		m_wpActiveCamera = camera;
	}
}

std::shared_ptr<CameraBase> BaseScene::GetActiveCamera()
{
	return m_wpActiveCamera.lock();
}
