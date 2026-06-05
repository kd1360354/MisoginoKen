#pragma once
#include"../BaseScene/BaseScene.h"

class TPSCamera;
class FPSCamera;
class NoobEnemy;
class MidBoss;
class LastBoss;
class Lantern;
class BonFire;
class Wall;
struct ObjTransForms
{
	Math::Vector3 pos = Math::Vector3::Zero;
	Math::Vector3 rot = Math::Vector3::Zero;
	Math::Vector3 scale = Math::Vector3::One;

};

class GameScene:public BaseScene
{
public:
	GameScene(){}
	~GameScene()override { Release(); }

	void Init()override;
	void SceneUpdate()override;
	void Release()override;

	//オブジェクトにユニークな名前を与える
	void GenerateUniqueName(const std::shared_ptr<KdGameObject>& obj);

	void OnEnemyDefeated();

	void SpawnPhase2Enemies();

	void StopAllBGM();


	void UpdateCommon();        // 共通（ランタン、お掃除、環境音）
	void UpdateMidBossPhase();   // 中ボスの出現・起動・撃破管理
	void UpdateLastBossPhase();  // ラスボスの出現・起動・撃破管理
	void ForceStopBattleWall();
private:
	// 💡 ランタンのポインタを保持するためのリスト (要素数3と仮定)
	std::vector<std::shared_ptr<Lantern>> m_lanterns;

	// 💡 全てのオブジェクトのUpdate後に点光源を更新する関数を定義
	void UpdatePointLights();

	bool m_midBossActivated = false; // 中ボスUI/BGM処理が完了したか
	bool m_lastBossActivated = false;  // ラスボスUI/BGM処理が完了したか
	std::shared_ptr<MidBoss> m_MidBoss = nullptr; // 中ボス
	std::shared_ptr<LastBoss> m_LastBoss = nullptr; // ラスボス
	//オブジェクトの種類ごとに次に使うIDを管理
	std::map<std::string, int> m_objNameCounter;

	std::shared_ptr<KdSoundInstance>_NoobEnemyBgm;
	std::shared_ptr<KdSoundInstance>_BossBgm;
	std::shared_ptr<KdSoundInstance>_MidBossBgm;
	std::shared_ptr<KdSoundInstance>_WindSe;

	std::shared_ptr<BonFire> m_BonFire ;

	std::shared_ptr<TPSCamera> m_spTPSCamera = nullptr;
	std::shared_ptr<FPSCamera> m_spFPSCamera = nullptr;
	std::vector<std::weak_ptr<KdGameObject>> m_killedObjects;
	int m_enemyCount = 0;
	int m_defeatedEnemyCount = 0;
	bool m_isGameClear = false;
	int m_midBossSpawnCount = 3;  // 中ボスを出現させるのに必要な敵の撃破数
	int m_lastBossSpawnCount = 10;  // ラスボスを出現させるのに必要な敵の撃破数
	bool m_midBossSpawned = false;  // 中ボスが既に出現しているか
	bool m_lastBossSpawned = false; // ラスボスが既に出現しているか

	bool m_midBossDefeated = false; // 中ボスが倒されたか
	bool m_phase2Spawned = false;   // フェーズ2の敵が出現済みか
	bool m_isWaveCombatStarted = false; // そのウェーブで戦闘が一度でも始まったか
	// フェーズ2の敵数 (Init()でセットされる想定)
	int m_phase2EnemyCount = 6;
	float m_AudioVolume = 0.2f;
	// 中ボス/ラスボスの出現条件
	const int PHASE1_DEFEAT_COUNT_FOR_MIDBOSS = 8;
	const int PHASE2_DEFEAT_COUNT_FOR_LASTBOSS = 6; // フェーズ2の敵の総数に合わせる
	std::weak_ptr<KdEffekseerObject> m_wpMidBossToriiEffect;  // 中ボス用
	std::weak_ptr<KdEffekseerObject> m_wpLastBossToriiEffect; // ラスボス用

	bool m_isMidToriiEffectPlayed = false;   // 炊きっぱなし防止用
	bool m_isLastToriiEffectPlayed = false;  // 炊きっぱなし防止用

	std::shared_ptr<Wall> m_midBossBarrier;
	std::shared_ptr<Wall> m_lastBossBarrier;

	// 静的変数でバトル中かどうかを保持
	bool isBgmActive = false;
};