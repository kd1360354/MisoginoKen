#pragma once
class BaseScene;
class Player;
class NoobEnemy;
class MidBoss;
class LastBoss;
class SceneManager
{
public:
	//モード
	enum class SceneMode
	{
		Game,//ゲームプレイモード
		Title,//タイトル
		Create,//オブジェクト編集モード
		Result,//リザルト
	};

	//シングルトン
	static SceneManager& Instance();
	std::shared_ptr<KdGameObject> FindObjectByType(const std::string& typeName)
	{
		for (const auto& obj : m_objects)
		{
			if (obj && obj->GetTypeName() == typeName)
			{
				return obj;
			}
		}
		return nullptr;
	}

	// オブジェクトを追加するためのヘルパー関数  
	void AddObject(const std::shared_ptr<KdGameObject>& obj)
	{
		m_objects.push_back(obj);
	}

	void Init();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void PreDraw();
	void Draw();
	void PostDraw();
	void DrawSprite();
	void Release();
		
	BaseScene* GetCurrentScene()const { return m_currentScene.get(); }
	SceneMode GetCurrentMode()const { return m_currentMode; }
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();
	template<class T>
	void ChangeScene()
	{
		//TがBaseSceneを継承していることをコンパイル時にチェック
		static_assert(std::is_base_of<BaseScene, T>::value, "T must be a descendant of BaseScene");
		
		//次のシーンのインスタンスを生成し、m_nextSceneにセット
		m_nextScene = std::make_unique<T>();
	}
	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneMode _nextScene)
	{
		m_currentMode = _nextScene;
	}
	void SetPlayer(std::shared_ptr<Player> player) { m_spPlayer = player; }
	std::shared_ptr<Player> GetPlayer() const { return m_spPlayer; }

	    // 敵をリストに追加する関数に変更
    void AddNoobEnemy(std::shared_ptr<NoobEnemy> noobenemy) { m_spNoobEnemies.push_back(noobenemy); }
    // 敵のリスト全体を取得する関数を追加
    std::vector<std::shared_ptr<NoobEnemy>>& GetNoobEnemies() { return m_spNoobEnemies; }

	void ClearNoobEnemies() {
		m_spNoobEnemies.clear();
	}

	void SetMidBoss(std::shared_ptr<MidBoss> midboss) { m_spMidBoss = midboss; }
	std::shared_ptr<MidBoss> GetMidBoss() const { return m_spMidBoss; }

	void SetLastBoss(std::shared_ptr<LastBoss> lastboss) { m_spLastBoss = lastboss; }
	std::shared_ptr<LastBoss> GetLastBoss() const { return m_spLastBoss; }
	void StopCurrentSceneBGM();
private:
	SceneManager();
	~SceneManager();
	//コピー禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	void ToggleMode();

	//実際の切り替え処理
	void ExecuteSceneChange();

	std::unique_ptr<BaseScene> m_currentScene;
	//SceneMode m_currentMode = SceneMode::Create;
	//SceneMode m_currentMode = SceneMode::Game;
	SceneMode m_currentMode = SceneMode::Title;



	std::list<std::shared_ptr<KdGameObject>> m_objects;
	SceneMode m_nextSceneMode = m_currentMode;
	//次に切り替えるシーンを保持
	std::unique_ptr<BaseScene> m_nextScene = nullptr;

	std::shared_ptr<Player> m_spPlayer;
	std::vector<std::shared_ptr<NoobEnemy>> m_spNoobEnemies;
	std::shared_ptr<MidBoss> m_spMidBoss;
	std::shared_ptr<LastBoss> m_spLastBoss;

};