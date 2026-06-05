#pragma once
#include "../EnemyBase.h"
#include"../../../../Constants/CharactorConstants/Enemy/MidBoss/MidBossConstants.h"
class WeaponBase;

/**
 * @brief 中ボス（MidBoss）クラス
 * 理由：HP低下による二刀流モードへの移行、武器の動的なアタッチ位置変更、および複雑な武器行列計算を管理するため。
 */
class MidBoss : public EnemyBase
{
public:
	MidBoss() {}
	~MidBoss() override {}

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void LoadParameters(const nlohmann::json& _j) override;

	// 自分の型名を返す
	const char* GetTypeName() const override { return MidBossParam::NAME; }

	// ImGuiでのデバッグ情報描画
	void OnImGui() override;

private:

	void BuildBehaviorTree();


	void UpdateWeaponAttachment(bool isDualWield);


	void UpdateWeaponLocalMatrices(bool isDualWield);

	void UpdateAttackCollision(bool isDualWield);

private:
	std::shared_ptr<WeaponBase> m_spRightWeapon = nullptr;
	std::shared_ptr<WeaponBase> m_spLeftWeapon = nullptr;
	float m_Angle = 0;
};