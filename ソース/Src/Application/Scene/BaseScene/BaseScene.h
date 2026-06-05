#pragma once

class CameraBase;

class BaseScene
{
public:
	BaseScene(){}
	virtual ~BaseScene() { Release(); }

	//メイン処理
	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void PostDraw();
	void DrawSprite();

	//派生クラスでの固有処理
	virtual void Init() = 0;
	virtual void SceneUpdate() {}
	virtual void Release(){}

	//ヘルパー関数
	 
	void AddObject(const std::shared_ptr<KdGameObject>& obj);


	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }


	//オブジェクトリスト取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList()const
	{
		return m_objList;
	}
	
	//ファクトリ取得
	KdGameObjectFactory& GetFactory() { return KdGameObjectFactory::Instance(); }

	//アクティブカメラを設定
	void SetActiveCamera(const std::shared_ptr<CameraBase>& camera);
	//アクティブカメラを取得
	std::shared_ptr<CameraBase> GetActiveCamera();
	void LoadPrefab(const std::string& filePath);
protected:
	//シーンが管理する全オブジェクトのリスト
	std::list<std::shared_ptr<KdGameObject>> m_objList;
	////シーンが持つオブジェクト生成工場
	//KdGameObjectFactory m_gameObjectFactory;
	//現在アクティブなカメラへのポインタ
	std::weak_ptr<CameraBase> m_wpActiveCamera;

	std::string m_name;
};