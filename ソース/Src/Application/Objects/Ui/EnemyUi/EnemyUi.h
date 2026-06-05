#pragma once
#include"../UiBase.h"

class EnemyBase;
class Player;



// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class EnemyUi :public UiBase
{
public:
	void Init() override;
	void DrawSprite() override;
	void DrawUnLit()override;
	void Update()override;
	const char* GetTypeName()const override { return "EnemyUi"; }

	void SetEnemy(std::weak_ptr<EnemyBase> enemy) { m_wpEnemy = enemy; }
	void SetPlayer(std::weak_ptr<Player> player) { m_wpPlayer = player; }

	float GetDisplayHp() const { return m_display_hp; }
	float GetHpRate() const { return m_hp_rate; }

private:
	std::weak_ptr<EnemyBase> m_wpEnemy;
	std::weak_ptr<Player> m_wpPlayer;



	// UI値が実HPに追いつく速度
	// ★ m_hpDecreaseRate が Update で 200.0f としてハードコードされているため、
	//    ここではコメントアウトまたは初期値調整推奨
	//const float m_hp_decrease_rate = 10.0f;

	// HPバーのスケーリング（サイズ調整用）
	static constexpr float DEFAULT_SCALE_X = 2.5f;
	static constexpr float DEFAULT_SCALE_Y = 0.5f;
	Math::Vector3 m_scale = {DEFAULT_SCALE_X, DEFAULT_SCALE_Y, 0.0f };
};