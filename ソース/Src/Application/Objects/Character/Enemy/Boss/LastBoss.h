#pragma once
#include "../EnemyBase.h"
#include "../../../../Constants/CharactorConstants/Enemy/Boss/LastBossConstants.h"

/**
 * @brief 最終ボス（LastBoss）クラス
 * 理由：広範囲攻撃、カメラ演出を伴う咆哮、およびHPに連動したフェーズ移行演出を一括管理するため。
 */
class LastBoss : public EnemyBase
{
public:
	LastBoss() {}
	~LastBoss() override {}

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void OnHit(float Damage) override;
	void LoadParameters(const nlohmann::json& _j) override;

	// 自分の型名を返す
	const char* GetTypeName() const override { return LastBossParam::NAME; }

	void OnImGui() override;

	// 特殊状態制御
	void SetSuperArmor(bool isSuperArmor) { m_isSuperArmor = isSuperArmor; }
	void ExecuteCameraShake(float intensity, int frame);

private:
	/**
	 * @brief ビヘイビアツリーの構築
	 * 理由：開幕演出、HPによる発狂演出、およびランダムな特殊攻撃を含む複雑な思考ルーチンを定義するため。
	 */
	void BuildBehaviorTree();

	/**
	 * @brief 攻撃判定の生成
	 * 理由：技ごとのアニメーションに合わせて、左手、右手、または自身の中心から広範囲な判定を発生させるため。
	 */
	void UpdateAttackCollision();

	/**
	 * @brief 行列の最終確定
	 * 理由：ボスの巨体やステートに合わせたワールド行列の構築を行うため。
	 */
	void FinalizeTransform();


	float m_angle = 0;
	bool m_hasWarpedForJumpAttack = false;
	std::weak_ptr<KdEffekseerObject> m_wpJumpAttackWaveEffect;
	bool m_hasPlayedJumpAttackEffect = false;
	float m_roarTimer = 0.0f;
	bool m_isRoarShaked = false;
protected:
	void ProcessDeath() override;
};