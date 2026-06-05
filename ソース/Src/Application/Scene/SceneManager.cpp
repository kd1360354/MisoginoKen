#include "SceneManager.h"
#include"GameScene/GameScene.h"
#include"BaseScene/BaseScene.h"
#include"TitleScene/TitleScene.h"
#include"ResultScene/ResultScene.h"

SceneManager& SceneManager::Instance()
{
	static SceneManager instance; 
	return instance;
}

void SceneManager::Init()
{
	//最初のシーン生成
//	m_currentScene = std::make_unique<GameScene>();
	m_currentScene = std::make_unique<TItleScene>();
	//m_currentScene = std::make_unique<ReslutScene>();
	m_currentScene->Init();

	ShowCursor(m_currentMode == SceneMode::Create);
}

void SceneManager::PreUpdate()
{
	//m_nextSceneに予約が入っていれば切り替え
	if (m_nextScene)
	{
		ExecuteSceneChange();
	}

	// 空間環境の更新
	KdShaderManager::Instance().WorkAmbientController().Update();


	if (m_currentScene)
	{
		m_currentScene->PreUpdate();
	}
}

void SceneManager::Update()
{
	if (KdInputManager::Instance().IsPress(APP_BTN_MODE_CHANGE))
	{
		ToggleMode();
	}

	if (m_currentScene)
	{
		m_currentScene->Update();
	}
}

void SceneManager::PostUpdate()
{
	if (m_currentScene)
	{
		m_currentScene->PostUpdate();
	}
}

void SceneManager::PreDraw()
{
	if (m_currentScene)
	{
		m_currentScene->PreDraw();
	}
}

void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->Draw();
	}
}

void SceneManager::PostDraw()
{
	// 画面のぼかしや被写界深度処理の実施
	KdShaderManager::Instance().m_postProcessShader.PostEffectProcess();

	if (m_currentScene)
	{
		m_currentScene->PostDraw();
	}
}

void SceneManager::DrawSprite()
{
	if (m_currentScene)
	{
		m_currentScene->DrawSprite();
	}
}

void SceneManager::Release()
{
	if (m_currentScene)
	{
		m_currentScene->Release();
	}
	//管理しているオブジェクトを安全に破棄
	m_currentScene.reset();
}

void SceneManager::StopCurrentSceneBGM()
{
	// 1. 現在のシーンの生ポインタを取得 (unique_ptrなのでget()を使用)
	BaseScene* currentBaseScene = m_currentScene.get();

	// 2. GameSceneクラスにダウンキャストを試みる
	// (現在のシーンがGameSceneでなければ nullptr が返る)
	GameScene* gameScene = dynamic_cast<GameScene*>(currentBaseScene);

	// 3. ダウンキャストに成功した場合 (現在のシーンがGameSceneの場合) のみ、BGM停止メソッドを呼び出す
	if (gameScene)
	{
		gameScene->StopAllBGM();
	}
}

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}
const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}
void SceneManager::ToggleMode()
{
	m_currentMode = (m_currentMode == SceneMode::Create) ? SceneMode::Game : SceneMode::Create;

	//モードに合わせてマウスカーソルの表示/非表示
	ShowCursor(m_currentMode == SceneMode::Create);
}

void SceneManager::ExecuteSceneChange()
{
	//現在のシーンがあれば解放
	if (m_currentScene)
	{
		m_currentScene->Release();
	}

	//現在のシーンを次のシーンに差し替える
	m_currentScene = std::move(m_nextScene);

	//新しいシーンの初期化処理
	if (m_currentScene)
	{
		m_currentScene->Init();
	}
}
