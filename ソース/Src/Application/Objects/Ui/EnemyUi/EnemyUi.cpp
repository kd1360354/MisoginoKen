#include "EnemyUi.h"
#include "../../Character/Enemy/EnemyBase.h"
#include "../../Character/Enemy/Boss/LastBoss.h"
#include "../../Character/Enemy/MidBoss/MidBoss.h"
#include "../../../Scene/SceneManager.h"
#include "../../Character/Player/Player.h"
// 定数ファイルのインクルード
#include "../../../Constants/CharactorConstants/Enemy/EnemyUiConstants/EnemyUiConstants.h"

void EnemyUi::Init()
{
	// --- 🖼️ テクスチャパスの適用 ---
	m_SquareTex[0] = std::make_shared<KdSquarePolygon>();
	m_SquareTex[0]->SetMaterial(EnemyUIParam::Path::HP_BACKGROUND);
	m_SquareTex[0]->SetPivot(KdSquarePolygon::PivotType::Right_Middle);

	m_SquareTex[1] = std::make_shared<KdSquarePolygon>();
	m_SquareTex[1]->SetMaterial(EnemyUIParam::Path::HP_FRAME);
	m_SquareTex[1]->SetPivot(KdSquarePolygon::PivotType::Right_Middle);

	m_SquareTex[2] = std::make_shared<KdSquarePolygon>();
	m_SquareTex[2]->SetMaterial(EnemyUIParam::Path::HP_BAR);
	m_SquareTex[2]->SetPivot(KdSquarePolygon::PivotType::Right_Middle);

	m_Tex[0] = std::make_shared<KdTexture>(); m_Tex[0]->Load(EnemyUIParam::Path::HP_BACKGROUND);
	m_Tex[1] = std::make_shared<KdTexture>(); m_Tex[1]->Load(EnemyUIParam::Path::HP_FRAME);
	m_Tex[2] = std::make_shared<KdTexture>(); m_Tex[2]->Load(EnemyUIParam::Path::HP_BAR);
	m_Tex[3] = std::make_shared<KdTexture>(); m_Tex[3]->Load(EnemyUIParam::Path::NAME_MID_BOSS);
	m_Tex[4] = std::make_shared<KdTexture>(); m_Tex[4]->Load(EnemyUIParam::Path::NAME_LAST_BOSS);
}

void EnemyUi::DrawSprite()
{
	std::shared_ptr<EnemyBase> spEnemy = m_wpEnemy.lock();
	if (!spEnemy) return;

	// ボス判定
	std::shared_ptr<LastBoss> spLastBoss = std::dynamic_pointer_cast<LastBoss>(spEnemy);
	std::shared_ptr<MidBoss>  spMidBoss = std::dynamic_pointer_cast<MidBoss>(spEnemy);

	if (spLastBoss || spMidBoss) {

		// --- 👑 ボスUI (スプライト) パラメータの適用 ---
		const float xPos = 0.0f;
		const float yPos = EnemyUIParam::Boss::POS_Y;
		const float nameUiHeight = EnemyUIParam::Boss::NAME_HEIGHT;
		const float hpBarYOffset = EnemyUIParam::Boss::HP_BAR_OFFSET_Y;
		const float baseWidth = EnemyUIParam::Boss::HP_BAR_WIDTH;
		const float height = EnemyUIParam::Boss::HP_BAR_HEIGHT;

		// 共通描画設定
		Math::Rectangle rc = { 0, 0, (int)EnemyUIParam::Boss::HP_BAR_WIDTH, (int)EnemyUIParam::Boss::HP_BAR_HEIGHT };
		Math::Rectangle Namerc = { 0, 0, (int)EnemyUIParam::Boss::NAME_RC_WIDTH, (int)EnemyUIParam::Boss::NAME_RC_HEIGHT };
		Math::Color color = { 1, 1, 1, 1 };

		// -------------------------------------------------------------
		// 1. 名前UIの描画 (中央基準)
		std::shared_ptr<KdTexture> nameTex;
		if (spLastBoss) { nameTex = m_Tex[4]; }
		else if (spMidBoss) { nameTex = m_Tex[3]; }

		Math::Vector2 pivotCenter = { 0.5f, 0.5f };
		if (nameTex) {
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				nameTex, xPos, yPos,
				EnemyUIParam::Boss::NAME_WIDTH, nameUiHeight, &Namerc, &color, pivotCenter
			);
		}

		// 2. HPバーの描画 (左端基準)
		Math::Vector2 pivotLeft = { 0.0f, 0.5f };

		const float hpBarStartX = xPos - baseWidth / 2.0f;
		const float hpBarY = yPos - hpBarYOffset;

		if (m_Tex[0])
		{
			// HPバーの背景
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[0], hpBarStartX, hpBarY, baseWidth, height, &rc, &color, pivotLeft);

			// HPバー本体
			float hpBarWidth = baseWidth * m_hp_rate;
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[2], hpBarStartX, hpBarY, hpBarWidth, height, &rc, &color, pivotLeft);

			// HPバーのフレーム
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[1], hpBarStartX, hpBarY, baseWidth, height, &rc, &color, pivotLeft);
		}
	}
}

void EnemyUi::DrawUnLit()
{
	std::shared_ptr<EnemyBase> spEnemy = m_wpEnemy.lock();
	if (!spEnemy) return;

	if (std::dynamic_pointer_cast<LastBoss>(spEnemy) || std::dynamic_pointer_cast<MidBoss>(spEnemy)) {
		return;
	}


	// --- ⚔️ ノーマルエネミー (ビルボード) パラメータの適用 ---
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

	const float zOffsetBase = EnemyUIParam::Normal::BILLBOARD_Z_OFFSET;

	Math::Matrix baseWorld = m_mWorld;
	Math::Vector3 billBoardForward = baseWorld.Forward();

	// (1) 背景
	Math::Matrix backgroundWorld = baseWorld;
	backgroundWorld.Translation(backgroundWorld.Translation() - billBoardForward * zOffsetBase * 2.0f);
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_SquareTex[0], backgroundWorld);

	// Zファイティング対策
	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);

	// (2) フレーム
	Math::Matrix frameWorld = backgroundWorld;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_SquareTex[1], frameWorld);

	// (3) HPバー
	Math::Matrix hpBarWorld = backgroundWorld;
	hpBarWorld._11 *= m_hp_rate;
	hpBarWorld._12 *= m_hp_rate;
	hpBarWorld._13 *= m_hp_rate;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_SquareTex[2], hpBarWorld);

	KdShaderManager::Instance().UndoDepthStencilState();
	KdShaderManager::Instance().UndoRasterizerState();
}

void EnemyUi::Update()

{
	std::shared_ptr<EnemyBase> spEnemy = m_wpEnemy.lock();

	
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

	// --- 破棄判定ロジック ---
	const float breakMargin = EnemyUIParam::Normal::DEATH_ANIM_MARGIN;

	bool isEnemyDead = (spEnemy && spEnemy->GetHp() <= 0);
	bool isAnimationFinished = (spEnemy && m_display_hp / (float)spEnemy->GetMaxHP() <= breakMargin);
	if (!spEnemy) { isAnimationFinished = true; }

	if (!spEnemy || !spPlayer || (isEnemyDead && isAnimationFinished))
	{
		if (!spEnemy || (isEnemyDead && isAnimationFinished)) { Expire(); }
		return;
	}

	// --- 1. HPの滑らかな減少処理 (Anim名前空間を使用) ---
	float currentActualHp = (float)spEnemy->GetHp();
	float maxHp = (float)spEnemy->GetMaxHP();

	if (m_display_hp == 0.0f) { m_display_hp = currentActualHp; }

	if (m_display_hp > currentActualHp)
	{
		m_display_hp -= EnemyUIParam::Anim::HP_DECREASE_SPEED * (float)DeltaTimer::Instance().GetDeltaTime();
		if (m_display_hp < currentActualHp) { m_display_hp = currentActualHp; }
	}
	else
	{
		m_display_hp = currentActualHp;
	}

	m_hp_rate = m_display_hp / maxHp;

	if (std::dynamic_pointer_cast<LastBoss>(spEnemy) || std::dynamic_pointer_cast<MidBoss>(spEnemy)) {return;}
	// --- 2. ノーマルエネミー (ビルボード) 更新 ---
	Math::Vector3 enemyPos = spEnemy->GetPos();
	enemyPos.y += EnemyUIParam::Normal::HP_BAR_OFFSET_Y;

	Math::Matrix cameraWorldMat = spPlayer->GetCameraWorldMatrix();
	Math::Vector3 flatLook = cameraWorldMat.Backward();
	flatLook.y = 0.0f;
	flatLook.Normalize();

	Math::Vector3 uiForward = flatLook * -1.0f;
	Math::Vector3 uiUp = Math::Vector3(0.0f, 1.0f, 0.0f);
	Math::Vector3 uiRight = uiUp.Cross(uiForward);
	uiRight.Normalize();
	uiUp = uiForward.Cross(uiRight);
	uiUp.Normalize();

	Math::Matrix billBoardRot;
	billBoardRot.Right(uiRight);
	billBoardRot.Up(uiUp);
	billBoardRot.Forward(uiForward);

	// 定数 DEFAULT_SCALE_X を使用
	const float halfWidth = EnemyUIParam::Normal::DEFAULT_SCALE_X / 2.0f;
	Math::Matrix baseScaleMat = Math::Matrix::CreateScale(m_scale);

	Math::Vector3 offset = uiRight * halfWidth;
	m_mWorld = baseScaleMat * billBoardRot * Math::Matrix::CreateTranslation(enemyPos + offset);
}