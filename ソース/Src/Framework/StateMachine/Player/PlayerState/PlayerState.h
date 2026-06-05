#pragma once

class Player;

/**
 * @brief プレイヤーの状態管理基底クラス
 * 各ステートの共通インターフェースと、ステートの性質（性質）を定義する。
 */
class PlayerState
{
public:
	virtual ~PlayerState() {}

	virtual void Enter(Player& player) {}
	virtual void Update(Player& player, float deltaTime) = 0;
	virtual void Exit(Player& player) {}

	// =========================================================
	// ステートの性質を問い合わせるインターフェース
	// これにより Player.cpp での typeid 判定を撤廃する
	// =========================================================

	// 攻撃系のステートか？（Attack, PowerAttack, RunAttackなど）
	virtual bool IsAttacking() const { return false; }

	// 回避系のステートか？（Dodge, JustDodgeなど）
	virtual bool IsDodge() const { return false; }

	// 移動入力による座標更新を許可するか？（Idle, Runなどはtrue, AttackやHitはfalse）
	virtual bool CanMove() const { return true; }

	// 被弾時にのけぞる（Hitステートへ遷移する）のを許可するか？（基本true, 回避中はfalse）
	virtual bool CanTakeDamage() const { return true; }

	// ガード（Block）中か？
	virtual bool IsBlocking() const { return false; }
};