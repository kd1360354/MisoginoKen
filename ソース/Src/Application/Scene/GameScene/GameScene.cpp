#include "GameScene.h"
#include"../Src/Application/main.h"
#include"../../Scene/SceneManager.h"
#include"../Src/Application/Objects/Character/Player/Player.h"
#include"../Src/Application/Objects/Character/Enemy/NoobEnemy/NoobEnemy.h"
#include"../../Objects/Map/Stage1/Ground/Ground.h"
#include"../../Objects/Map/Stage1/Torii/Torii.h"
#include"../Src/Application/Objects/Camera/TPSCamera/TPSCamera.h"
#include"../ResultScene/ResultScene.h"
#include"../../Objects/Map/Stage1/GojunoTou/GojunoTou.h"
#include"../../Objects/Map/Stage1/SanjunoTou/SanjunoTou.h"
#include"../../Objects/Character/Enemy/Boss/LastBoss.h"
#include"../../Objects/Character/Enemy/MidBoss/MidBoss.h"
#include"../../Objects/Map/Stage1/Sky/SunSetSky/SunSetSky.h"
#include"../../Objects/Map/Stage1/Sky/MidNight/MidNight.h"
#include"../../Objects/Map/Stage1/Chozuya/Chozuya.h"
#include"../../Objects/Map/Stage1/Honden/Honden.h"
#include"../../Objects/Map/Stage1/Approach/Approach.h"
#include"../../Objects/Map/Stage1/lantern/lantern.h"
#include"../../../Framework/Loader/GameObjectLoader.h"
#include"../../Objects/Ui/PlayerUi/PlayerUi.h"
#include"../../Objects/Map/Stage1/Wall/Wall.h"
#include"../../Objects/Map/Stage1/Sakura/Sakura.h"
#include"../../Objects/Ui/EnemyUi/EnemyUi.h"
#include"../../Objects/Map/Stage1/Bonfire/Bonfire.h"
#include"../../Objects/Map/Stage1/Fence/Fence.h"
#include "../../Constants/GameScenenConstsnts/GameScenenConstsnts.h"
#include"../SoundManager.h"
using json = nlohmann::json;

void GameScene::Init()
{
	// --- 💻 システム・エフェクト設定 ---
	KdEffekseerManager::GetInstance().Create(
		SceneParam::System::EFFECT_VIEW_W,
		SceneParam::System::EFFECT_VIEW_H
	);
	KdAudioManager::Instance().StopAllSound();
	SceneManager::Instance().ClearNoobEnemies();

	// フラグ・カウンタの初期化
	m_midBossSpawned = false;
	m_lastBossSpawned = false;
	m_midBossActivated = false;
	m_lastBossActivated = false;
	m_midBossDefeated = false;
	m_phase2Spawned = false;
	m_enemyCount = 0;
	m_AudioVolume = SceneParam::System::BGM_VOL_DEF; // 0.2f
	isBgmActive = false;

	m_MidBoss = nullptr;
	m_LastBoss = nullptr;

	// --- 🌌 夜間環境演出 ---
	namespace Night = SceneParam::Environment::Night;
	KdShaderManager::Instance().WorkAmbientController().SetDirLight(Night::DIR_LIGHT_DIR, Night::DIR_LIGHT_COLOR);
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(Night::AMBIENT_COLOR);
	KdShaderManager::Instance().WriteCBPointLight({});

	// レイアウトロード
	GameObjectLoader::LoadJson("Asset/Data/GameObjectLayout.json", this);

	// --- 📸 カメラの生成 ---
	namespace Cam = SceneParam::Camera;
	m_spTPSCamera = std::make_shared<TPSCamera>();
	m_spTPSCamera->Init();
	// カメラのプロジェクション行列設定
	m_spTPSCamera->GetCamera()->SetProjectionMatrix(Cam::FOV, Cam::CLIP_FAR, Cam::CLIP_NEAR);

	// --- 🕹️ プレイヤーの生成 ---
	std::shared_ptr<Player> _Player = std::make_shared<Player>();
	_Player->Init();
	AddObject(_Player);
	SceneManager::Instance().SetPlayer(_Player);

	// プレイヤーUI
	std::shared_ptr<PlayerUi> _PlayerUi = std::make_shared<PlayerUi>();
	_PlayerUi->Init();
	_PlayerUi->SetPlayer(_Player);
	AddObject(_PlayerUi);

	// 地面
	std::shared_ptr<Ground> _Ground = std::make_shared<Ground>();
	_Ground->Init();
	AddObject(_Ground);

	// 夜空背景
	std::shared_ptr<MidNight> _MidNight = std::make_shared<MidNight>();
	_MidNight->Init();
	AddObject(_MidNight);

	// --- 🧱 壁 (Wall) の生成：WallParam 名前空間を使用 ---
	namespace WallP = SceneParam::Map::WallParam;
	std::vector<ObjTransForms> wallForms = {
		{ WallP::POS_F, {0.0f, WallP::ROT_Y_F, 0.0f}, WallP::SCALE_FB },
		{ WallP::POS_L, {0.0f, WallP::ROT_Y_L, 0.0f}, WallP::SCALE_L },
		{ WallP::POS_B, {0.0f, WallP::ROT_Y_B, 0.0f}, WallP::SCALE_FB },
		{ WallP::POS_R, {0.0f, WallP::ROT_Y_R, 0.0f}, WallP::SCALE_R }
	};
	for (auto& data : wallForms)
	{
		std::shared_ptr<Wall> _Wall = std::make_shared<Wall>();
		_Wall->Init();
		_Wall->SetPos(data.pos);
		_Wall->SetRot(data.rot);
		_Wall->SetScale(data.scale);
		m_spTPSCamera->RegistHitObject(_Wall);
		AddObject(_Wall);
	}

	// --- ⛩️ 鳥居周りの見えない壁（結界）：ToriiParam 名前空間を使用 ---
	namespace ToriiP = SceneParam::Map::ToriiParam;
	// 中ボス用
	m_midBossBarrier = std::make_shared<Wall>();
	m_midBossBarrier->Init();
	m_midBossBarrier->SetPos({ ToriiP::X_RIGHT, ToriiP::Y_POS, ToriiP::Z_FRONT });
	m_midBossBarrier->SetScale({ 10.0f, 15.0f, 1.0f });
	m_midBossBarrier->SetInvisible(true);
	if (m_midBossBarrier->GetCollider()) { m_midBossBarrier->GetCollider()->SetEnableAll(false); }
	m_spTPSCamera->RegistHitObject(m_midBossBarrier);
	AddObject(m_midBossBarrier);

	// ラスボス用
	m_lastBossBarrier = std::make_shared<Wall>();
	m_lastBossBarrier->Init();
	m_lastBossBarrier->SetPos({ ToriiP::X_CENTER, ToriiP::Y_POS, ToriiP::Z_FRONT });
	m_lastBossBarrier->SetScale({ 10.0f, 15.0f, 1.0f });
	m_lastBossBarrier->SetInvisible(true);
	if (m_lastBossBarrier->GetCollider()) { m_lastBossBarrier->GetCollider()->SetEnableAll(false); }
	m_spTPSCamera->RegistHitObject(m_lastBossBarrier);
	AddObject(m_lastBossBarrier);

	// --- 🚧 柵 (Fence) の配置：FenceParam 名前空間を使用 ---
	namespace FenceP = SceneParam::Map::FenceParam;
	{
		const float F_INT = 2.5f;
		auto SpawnFences = [&](const Math::Vector3& start, int count, float rotY, float dx, float dz) {
			for (int i = 0; i < count; ++i) {
				auto fence = std::make_shared<Fence>();
				fence->Init();
				fence->SetPos({ start.x + (i * dx), start.y, start.z + (i * dz) });
				fence->SetRot({ 0.0f, rotY, 0.0f });
				fence->SetScale(2.0f);
				AddObject(fence);
			}
			};
		// 三重塔・五重塔周辺の柵 (定数座標を適用)
		SpawnFences(FenceP::POS_F1, 8, FenceP::ROT_Y_F, -F_INT, 0.0f);
		SpawnFences(FenceP::POS_F2, 30, FenceP::ROT_Y_F, F_INT, 0.0f);
		SpawnFences({ 43.305f, ToriiP::Y_POS, 37.0f }, 50, 90.0f, 0.0f, F_INT);
		SpawnFences(FenceP::POS_F3, 10, FenceP::ROT_Y_F, -F_INT, 0.0f);
		SpawnFences(FenceP::POS_F4, 9, FenceP::ROT_Y_F, F_INT, 0.0f);
		SpawnFences({ 29.0f, ToriiP::Y_POS, 37.0f }, 50, 90.0f, 0.0f, F_INT);
		SpawnFences({ -30.305f, ToriiP::Y_POS, 37.0f }, 50, 90.0f, 0.0f, F_INT);
	}

	// --- 🛤️ 参道 (Approach)：ApproachParam 名前空間を使用 ---
	namespace AppP = SceneParam::Map::ApproachParam;
	std::vector<AppP::Data> approachForms = { AppP::SET_1, AppP::SET_2, AppP::SET_3 };
	for (auto& data : approachForms)
	{
		std::shared_ptr<Approach> _Approach = std::make_shared<Approach>();
		_Approach->Init();
		_Approach->SetPos(data.Pos);
		_Approach->SetRot(data.Rot);
		_Approach->SetScale(data.Scale);
		AddObject(_Approach);
	}

	// --- ⛩️ 鳥居 (Torii) の配置 ---
	std::vector<ObjTransForms> toriiForms = {
		{ { ToriiP::X_CENTER, ToriiP::Y_POS, ToriiP::Z_FRONT }, {0,0,0}, { ToriiP::SCALE, ToriiP::SCALE, ToriiP::SCALE } },
		{ { ToriiP::X_RIGHT,  ToriiP::Y_POS, ToriiP::Z_FRONT }, {0,0,0}, { ToriiP::SCALE, ToriiP::SCALE, ToriiP::SCALE } },
		{ { ToriiP::X_CENTER, ToriiP::Y_POS, ToriiP::Z_BACK  }, {0,0,0}, { ToriiP::SCALE, ToriiP::SCALE, ToriiP::SCALE } }
	};
	for (auto& data : toriiForms)
	{
		std::shared_ptr<Torii> _Torii = std::make_shared<Torii>();
		_Torii->Init();
		_Torii->SetPos(data.pos);
		AddObject(_Torii);
	}

	// 本殿・手水舎などの配置 (略)
	auto honden = std::make_shared<Honden>(); honden->Init(); AddObject(honden);
	auto chozuya = std::make_shared<Chozuya>(); chozuya->Init(); AddObject(chozuya);
	auto sanju = std::make_shared<SanjunoTou>(); sanju->Init(); AddObject(sanju);
	auto goju = std::make_shared<GojunoTou>(); goju->Init(); AddObject(goju);
	auto sakura = std::make_shared<Sakura>(); sakura->Init(); AddObject(sakura);

	// --- 🏮 ランタン (Lantern)：LanternParam 名前空間を使用 ---
	namespace LanP = SceneParam::Map::LanternParam;
	std::vector<ObjTransForms> lanternForms = {
		{ { LanP::X_R1, ToriiP::Y_POS, LanP::Z_FRONT }, {0,0,0}, { ToriiP::SCALE, ToriiP::SCALE, ToriiP::SCALE } },
		{ { LanP::X_CR, ToriiP::Y_POS, LanP::Z_FRONT }, {0,0,0}, { ToriiP::SCALE, ToriiP::SCALE, ToriiP::SCALE } },
		{ { LanP::X_CL, ToriiP::Y_POS, LanP::Z_FRONT }, {0,0,0}, { ToriiP::SCALE, ToriiP::SCALE, ToriiP::SCALE } }
	};
	m_lanterns.clear();
	for (auto& data : lanternForms)
	{
		auto _lan = std::make_shared<Lantern>(); _lan->Init();
		_lan->SetPos(data.pos); _lan->SetRot(data.rot); _lan->SetScale(data.scale);
		AddObject(_lan); m_lanterns.push_back(_lan);
	}

	// 新規ランタン列生成
	for (int i = 0; i < LanP::ROW_COUNT; ++i)
	{
		float currentZ = LanP::Z_START_NEW - (i * LanP::Z_STEP);
		for (int j = 0; j < 2; ++j)
		{
			float currentX = (i % 2 == 0) ? ((j == 0) ? LanP::SIDE_X_ABS : -LanP::SIDE_X_ABS)
				: ((j == 0) ? -LanP::SIDE_X_ABS : LanP::SIDE_X_ABS);
			auto _lan = std::make_shared<Lantern>(); _lan->Init();
			_lan->SetPos({ currentX, ToriiP::Y_POS, currentZ });
			_lan->Setparam(LanP::LIGHT_COLOR, LanP::LIGHT_RADIUS);
			_lan->SetLightActive(true);
			AddObject(_lan); m_lanterns.push_back(_lan);
		}
	}

	m_BonFire = std::make_shared<BonFire>(); m_BonFire->Init(); AddObject(m_BonFire);
	UpdatePointLights();

	// --- 👾 敵の初期配置：EnemySpawn / Progression 名前空間を使用 ---
	namespace Spawn = SceneParam::EnemySpawn;
	const Math::Vector3 ENEMY_POSITIONS[] = {
		Spawn::P1_CENTER, Spawn::P1_FRONT, Spawn::P1_RIGHT, Spawn::P1_RF,
		Spawn::P1_LF,     Spawn::P1_LEFT,  Spawn::P1_LO,    Spawn::P1_RO
	};
	m_enemyCount = SceneParam::Progression::P1_ENEMY_MAX;

	for (int i = 0; i < m_enemyCount; i++) {
		auto enemy = std::make_shared<NoobEnemy>(); enemy->Init();
		char nameBuf[32]; sprintf_s(nameBuf, "NoobEnemy_%02d", i); enemy->SetName(nameBuf);
		enemy->SetPos(ENEMY_POSITIONS[i]);
		enemy->SetTarget(_Player);
		auto ui = std::make_shared<EnemyUi>(); ui->Init();
		ui->SetEnemy(enemy); ui->SetPlayer(_Player); enemy->SetEnemyUi(ui);
		AddObject(enemy); AddObject(ui);
		SceneManager::Instance().AddNoobEnemy(enemy);
	}

	// --- 📸 カメラの最終設定 (修正済) ---
	m_spTPSCamera->SetTarget(_Player);
	m_spTPSCamera->RegistHitObject(_Ground);
	m_spTPSCamera->SetPos(Cam::POS_DEFAULT); // { 0.0f, 0.0f, 15.0f }
	AddObject(m_spTPSCamera);
	_Player->SetCamera(m_spTPSCamera);
	SetActiveCamera(m_spTPSCamera);
	KdEffekseerManager::GetInstance().SetCamera(m_spTPSCamera->GetCamera());
}
void GameScene::SceneUpdate()
{
	// 1. 全フェーズ共通の更新（プレイヤーチェック、お掃除、環境演出）
	UpdateCommon();

	// 2. 中ボスフェーズの管理
	// まだ中ボスを倒していない（m_midBossDefeated == false）間だけ実行される
	if (!m_midBossDefeated)
	{
		UpdateMidBossPhase();
	}
	// 3. ラスボスフェーズの管理
	// 中ボスを倒した後にのみ、ここが実行されるようになる
	else
	{
		UpdateLastBossPhase();
	}
}
void GameScene::Release()
{
	//シーンの終了時の処理(BGMの停止など)

	StopAllBGM();

	ForceStopBattleWall();

	// その他、シーン終了時のお掃除
	KdEffekseerManager::GetInstance().StopAllEffect(); // 念のため全エフェクト停止
}

void GameScene::GenerateUniqueName(const std::shared_ptr<KdGameObject>& obj)
{
	if (!obj)return;
	//カウンターから次のIDを取得、カウンターを１増やす
	int& id = m_objNameCounter[obj->GetTypeName()];
	obj->SetName(std::string(obj->GetTypeName()) + "_" + std::to_string(id++));
}

void GameScene::OnEnemyDefeated()
{

	// ゲームクリア判定
	if (m_lastBossSpawned && m_defeatedEnemyCount >= m_lastBossSpawnCount) {
		// ラスボス撃破後、ResultSceneへ移行するロジックを記述

		StopAllBGM();
		SceneManager::Instance().ChangeScene<ResultScene>();
	}
}

void GameScene::SpawnPhase2Enemies()
{
	// フェーズ2の座標を定義 (六芒星風のバラけた配置)
	const Math::Vector3 ENEMY_POSITIONS_PHASE2[] = {
		{ -15.0f, 0.0f, 20.0f },
		{ 15.0f, 0.0f, 20.0f },
		{ 0.0f, 0.0f, 30.0f },
		{ -10.0f, 0.0f, 10.0f },
		{ 10.0f, 0.0f, 10.0f },
		{ 0.0f, 0.0f, 5.0f }
	};
	const int ENEMY_COUNT_PHASE2 = SceneParam::Progression::P2_ENEMY_ADD; // 6

	// プレイヤーを取得 (UIに必要)
	std::shared_ptr<Player> spPlayer = SceneManager::Instance().GetPlayer();

	// フェーズ2の敵を生成し配置
	for (int i = 0; i < ENEMY_COUNT_PHASE2; i++) {

		std::shared_ptr<NoobEnemy> enemy = std::make_shared<NoobEnemy>();
		enemy->Init();

		// ★ 修正箇所：フェーズ1の数（8）を足して、08, 09, 10... と続くようにする
		char nameBuf[32];
		sprintf_s(nameBuf, "NoobEnemy_%02d", i + SceneParam::Progression::P1_ENEMY_MAX);
		enemy->SetName(nameBuf);

		enemy->SetPos(ENEMY_POSITIONS_PHASE2[i]);

		// --- 以下、UI生成と登録処理 (既存のまま) ---
		std::shared_ptr<EnemyUi> enemyUi = std::make_shared<EnemyUi>();
		enemyUi->Init();
		enemyUi->SetEnemy(enemy);
		enemyUi->SetPlayer(spPlayer);
		enemy->SetEnemyUi(enemyUi);
		enemy->SetTarget(spPlayer);

		AddObject(enemy);
		AddObject(enemyUi);
		SceneManager::Instance().AddNoobEnemy(enemy);
	}

	
	m_phase2Spawned = true;

}

void GameScene::UpdatePointLights()
{
	std::list<PointLight> currentLights;

	// 2. ランタンの光をリストに追加 (状態が有効なもののみ)
	for (const auto& lantern : m_lanterns)
	{
		if (lantern && lantern->GetPointLight().Radius > 0.0f) // Radius > 0.0f は点灯状態を意味すると仮定
		{
			currentLights.push_back(lantern->GetPointLight());
		}
	}

	// 3. 焚き火の光をリストに追加 (常に光るものとして扱う)
	if (m_BonFire) // m_BonFire は GameScene::Init() で設定した Bonfireのポインタ
	{
		// BonFire の PointLight 情報を取得して追加
		currentLights.push_back(m_BonFire->GetPointLight());
	}

	// 4. KdShaderManagerに最新の点光源リストを書き込む
	KdShaderManager::Instance().WriteCBPointLight(currentLights);
}

void GameScene::StopAllBGM()
{
	// ノーマル敵BGMを停止
	if (_NoobEnemyBgm) {
		_NoobEnemyBgm->Stop();
		_NoobEnemyBgm = nullptr; // 停止後、ポインタをリセット
	}
	// 中ボスBGMを停止
	if (_MidBossBgm) {
		_MidBossBgm->Stop();
		_MidBossBgm = nullptr; // 停止後、ポインタをリセット
	}
	// ラスボスBGMを停止
	if (_BossBgm) {
		_BossBgm->Stop();
		_BossBgm = nullptr; // 停止後、ポインタをリセット
	}
	if (_WindSe) {
		_WindSe->Stop();
		_WindSe = nullptr;
	}
}

void GameScene::UpdateCommon()
{
	std::shared_ptr<Player> spPlayer = SceneManager::Instance().GetPlayer();
	if (!spPlayer) return;

	Math::Vector3 playerPos = spPlayer->GetPos();

	// 1. 環境演出：ランタン消灯
	if (playerPos.z > SceneParam::Map::ToriiParam::Z_BACK)
	{
		if (!m_lanterns.empty() && m_lanterns.size() > 3 && m_lanterns[3]->GetPointLight().Radius > 0.0f)
		{
			for (const auto& lantern : m_lanterns) { if (lantern) lantern->SetLightActive(false); }
			UpdatePointLights();
		}
	}

	// ★ 2. SceneManager側の雑魚リストをお掃除（ここが重要！）
	// これをしないと、倒したフェーズ1の敵がカウントされ続けてラスボスが湧きません
	SceneManager::Instance().GetNoobEnemies().erase(
		std::remove_if(
			SceneManager::Instance().GetNoobEnemies().begin(),
			SceneManager::Instance().GetNoobEnemies().end(),
			[](const std::shared_ptr<NoobEnemy>& enemy) {
				return !enemy || enemy->IsExpired(); // 寿命が切れた敵をリストから外す
			}
		),
		SceneManager::Instance().GetNoobEnemies().end()
	);

	// 3. 雑魚敵の生存カウント（既にあるロジックを活用）
	auto& enemies = SceneManager::Instance().GetNoobEnemies();
	int aliveNoobEnemyCount = 0;
	bool isAnyEnemyAware = false; // ★ ここで宣言！

	for (const auto& enemy : enemies) {
		if (enemy && !enemy->IsDefeated() && !enemy->IsExpired()) {
			aliveNoobEnemyCount++;

			// 誰か一人がプレイヤーを捕捉しているか（ステートで判定）
			// Idle以外の「Walk/Run/Attack/Hit」などの時は「気づいている」とみなす
			if (enemy->GetState() != EnemyAnimState::Idle) {
				isAnyEnemyAware = true;
			}
		}
	}

	// --- 4. サウンド制御 ---
	bool isMidBossFighting = (m_midBossActivated && !m_midBossDefeated);
	bool isLastBossFighting = m_lastBossActivated;

	if (!isMidBossFighting && !isLastBossFighting)
	{
		

		// バトル開始判定：まだバトル中でなく、かつ誰かが気づいたらON
		if (!isBgmActive && isAnyEnemyAware)
		{
			isBgmActive = true;
		}

		// バトル終了判定：敵が全滅したらOFF
		if (isBgmActive && aliveNoobEnemyCount == 0)
		{
			isBgmActive = false;
		}

		// 最終的な再生指示
		if (isBgmActive)
		{
			SoundManager::Instance().ChangeBGM(SoundManager::BgmState::NoobBattle);
		}
		else
		{
			// バトル中でないなら常に環境音（ゲーム開始時もここを通る）
			SoundManager::Instance().ChangeBGM(SoundManager::BgmState::Ambient);
		}
	}

	// 5. オブジェクトリストのお掃除
	for (auto it = m_objList.begin(); it != m_objList.end(); )
	{
		if ((*it)->IsExpired()) { it = m_objList.erase(it); }
		else { ++it; }
	}
}

void GameScene::UpdateMidBossPhase()
{
	std::shared_ptr<Player> spPlayer = SceneManager::Instance().GetPlayer();
	if (!spPlayer) return;
	Math::Vector3 playerPos = spPlayer->GetPos();
	auto& enemies = SceneManager::Instance().GetNoobEnemies();

	// ★ 修正：生きている雑魚敵を数える
	int aliveNoobEnemyCount = 0;
	for (const auto& enemy : enemies) {
		if (enemy && !enemy->IsDefeated()) {
			aliveNoobEnemyCount++;
		}
	}

	// 2. 中ボスのスポーン（雑魚が全滅したら出現）
	if (!m_midBossSpawned && aliveNoobEnemyCount == 0) {
		m_MidBoss = std::make_shared<MidBoss>();
		m_MidBoss->Init();
		m_MidBoss->SetTarget(spPlayer);

		//m_MidBoss->SetPos(MIDBOSS_SPAWN_POS);
		AddObject(m_MidBoss);
		SceneManager::Instance().SetMidBoss(m_MidBoss);
		m_midBossSpawned = true;

		// 演出：道しるべ点灯
		if (!m_lanterns.empty()) { m_lanterns[0]->SetLightActive(true); }
		UpdatePointLights();
	}

	// 3. 起動トリガー（鳥居をくぐった時の処理）
	// 右側の鳥居（TORII_X_RIGHT）の座標を基準に、手前1.0f、左右5.0fの範囲で判定
	const float TRIGGER_WIDTH = 5.0f;
	const float TRIGGER_MARGIN_Z = 0.0f;

	bool isInsideMidToriiX = std::abs(playerPos.x - SceneParam::Map::ToriiParam::X_RIGHT) < TRIGGER_WIDTH;
	bool isPastMidToriiZ = (playerPos.z > SceneParam::Map::ToriiParam::Z_FRONT - TRIGGER_MARGIN_Z);

	if (m_midBossSpawned && !m_midBossActivated) {
		if (isPastMidToriiZ && isInsideMidToriiX) {
			// BGM切り替え（GameSoundManagerを使用）
			SoundManager::Instance().ChangeBGM(SoundManager::BgmState::MidBoss);

			// 中ボスUIの表示
			std::shared_ptr<EnemyUi> midBossUi = std::make_shared<EnemyUi>();
			midBossUi->Init();
			midBossUi->SetEnemy(m_MidBoss);
			midBossUi->SetPlayer(spPlayer);
			m_MidBoss->SetEnemyUi(midBossUi);
			AddObject(midBossUi);

			// 結界（壁）とエフェクトの出現
			if (!m_isMidToriiEffectPlayed) {
				Math::Vector3 effectPos = { SceneParam::Map::ToriiParam::X_RIGHT, SceneParam::Map::ToriiParam::Y_POS, SceneParam::Map::ToriiParam::Z_FRONT };
				auto wpEff = KdEffekseerManager::GetInstance().Play("BattleWall.efk", effectPos, 2.0f);
				m_wpMidBossToriiEffect = wpEff;
				if (auto spEff = m_wpMidBossToriiEffect.lock()) { spEff->SetLoop(true); }

				// 物理的な壁を有効化
				if (m_midBossBarrier && m_midBossBarrier->GetCollider()) {
					m_midBossBarrier->GetCollider()->SetEnableAll(true);
				}
				m_isMidToriiEffectPlayed = true;
			}
			m_midBossActivated = true;
		}
	}

	// 4. 中ボス撃破時の後始末
	if (m_MidBoss && m_MidBoss->IsDefeated() && !m_midBossDefeated) {
		// BGMを止める（または環境音に戻す）
		SoundManager::Instance().ChangeBGM(SoundManager::BgmState::Ambient);

		// 結界エフェクト停止
		if (auto spEff = m_wpMidBossToriiEffect.lock()) { spEff->StopEffect(); }
		m_wpMidBossToriiEffect.reset();

		// 物理的な壁を無効化（通れるようにする）
		if (m_midBossBarrier && m_midBossBarrier->GetCollider()) {
			m_midBossBarrier->GetCollider()->SetEnableAll(false);
		}

		// 次の道しるべランタンを点灯
		if (m_lanterns.size() > 1 && !m_lanterns[1]->GetPointLight().Radius) {
			m_lanterns[1]->SetLightActive(true);
			UpdatePointLights();
		}

		// フェーズ2（雑魚敵の追加スポーン）へ
		SpawnPhase2Enemies();
		m_midBossDefeated = true;
	}
}

void GameScene::UpdateLastBossPhase()
{
	std::shared_ptr<Player> spPlayer = SceneManager::Instance().GetPlayer();
	if (!spPlayer) return;

	Math::Vector3 playerPos = spPlayer->GetPos();

	// ★ ここが超重要！ SceneManagerから最新のリストを直接取得する
	auto& enemies = SceneManager::Instance().GetNoobEnemies();

	// 1. 生存している雑魚敵をカウント
	int aliveNoobEnemyCount = 0;
	for (const auto& enemy : enemies) {
		// 倒されていない(IsDefeated) かつ 消滅していない(IsExpired) 敵を数える
		if (enemy && !enemy->IsDefeated() && !enemy->IsExpired()) {
			aliveNoobEnemyCount++;
		}
	}

	// 【デバッグ出力】もし湧かないなら、出力ウィンドウにこの数値が出るか確認してください
	// OutputDebugStringA(("Alive Enemies: " + std::to_string(aliveNoobEnemyCount) + "\n").c_str());

	// 2. ラスボスのスポーン
	// 「フェーズ2が開始されている」かつ「生きている雑魚が本当に0」ならスポーン
	if (m_phase2Spawned && aliveNoobEnemyCount == 0 && !m_lastBossSpawned) {
		m_LastBoss = std::make_shared<LastBoss>();
		m_LastBoss->Init();
		//m_LastBoss->SetPos(LASTBOSS_SPAWN_POS);
		AddObject(m_LastBoss);
		SceneManager::Instance().SetLastBoss(m_LastBoss);
		m_lastBossSpawned = true;

		if (m_lanterns.size() > 2 && !m_lanterns[2]->GetPointLight().Radius) {
			m_lanterns[2]->SetLightActive(true);
			UpdatePointLights();
		}
	}

	// 3. 起動トリガー（中央の鳥居をくぐった時の処理）
	// 中央の鳥居（TORII_X_CENTER）を基準に判定。中ボスを倒している(m_midBossDefeated)ことも条件。
	const float TRIGGER_WIDTH = 5.0f;
	const float TRIGGER_MARGIN_Z = 0.0f;

	bool isInsideLastToriiX = std::abs(playerPos.x - SceneParam::Map::ToriiParam::X_CENTER) < TRIGGER_WIDTH;
	bool isPastLastToriiZ = (playerPos.z > SceneParam::Map::ToriiParam::Z_FRONT - TRIGGER_MARGIN_Z);

	if (m_lastBossSpawned && !m_lastBossActivated && m_midBossDefeated) {
		// 雑魚も全滅しており、中央の鳥居をくぐった場合のみ起動
		if (isPastLastToriiZ && isInsideLastToriiX && aliveNoobEnemyCount == 0) {
			// BGM切り替え（SoundManagerを使用）
			SoundManager::Instance().ChangeBGM(SoundManager::BgmState::LastBoss);

			// ラスボスUIの表示
			std::shared_ptr<EnemyUi> lastBossUi = std::make_shared<EnemyUi>();
			lastBossUi->Init();
			lastBossUi->SetEnemy(m_LastBoss);
			lastBossUi->SetPlayer(spPlayer);
			m_LastBoss->SetTarget(spPlayer); // ★ 追加
			m_LastBoss->SetEnemyUi(lastBossUi);
			AddObject(lastBossUi);

			// 結界（壁）とエフェクトの出現
			if (!m_isLastToriiEffectPlayed) {
				Math::Vector3 effectPos = { SceneParam::Map::ToriiParam::X_CENTER, SceneParam::Map::ToriiParam::Y_POS, SceneParam::Map::ToriiParam::Z_FRONT };
				m_wpLastBossToriiEffect = KdEffekseerManager::GetInstance().Play("BattleWall.efk", effectPos, 2.0f);
				if (auto spEff = m_wpLastBossToriiEffect.lock()) { spEff->SetLoop(true); }

				// 物理的な壁（ラスボス用）を有効化
				if (m_lastBossBarrier && m_lastBossBarrier->GetCollider()) {
					m_lastBossBarrier->GetCollider()->SetEnableAll(true);
				}
				m_isLastToriiEffectPlayed = true;
			}
			m_lastBossActivated = true;
		}
	}

	// 4. ラスボス消滅後のリザルト遷移
	// BTAction_Dissolveによって Expired フラグが立った瞬間に反応する
	if (m_LastBoss && m_LastBoss->IsExpired()) {
		// 結界解除
		if (auto spEff = m_wpLastBossToriiEffect.lock()) { spEff->StopEffect(); }
		m_wpLastBossToriiEffect.reset();

		if (m_lastBossBarrier && m_lastBossBarrier->GetCollider()) {
			m_lastBossBarrier->GetCollider()->SetEnableAll(false);
		}

		// 全音停止してリザルトシーンへ
		SoundManager::Instance().ChangeBGM(SoundManager::BgmState::Stop);
		SceneManager::Instance().ChangeScene<ResultScene>();
		return;
	}
}

// GameScene.cpp
void GameScene::ForceStopBattleWall()
{
	// 中ボスの壁エフェクトを停止
	if (auto spEff = m_wpMidBossToriiEffect.lock())
	{
		spEff->StopEffect(); // 物理的な停止
	}
	m_wpMidBossToriiEffect.reset(); // 管理権をリセット

	// ラスボスの壁エフェクトも同様に停止
	if (auto spEff = m_wpLastBossToriiEffect.lock())
	{
		spEff->StopEffect();
	}
	m_wpLastBossToriiEffect.reset();
}