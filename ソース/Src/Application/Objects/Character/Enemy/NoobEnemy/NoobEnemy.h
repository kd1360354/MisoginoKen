#pragma once
#include "../EnemyBase.h"



class NoobEnemy : public EnemyBase
{
public:
	NoobEnemy() {}
	~NoobEnemy() override {}

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;

	// 自分の型名を返す
	const char* GetTypeName() const override { return "NoobEnemy"; }

	// ImGuiでのデバッグ情報描画
	void OnImGui() override;

private:
	
	void BuildBehaviorTree();

	
	void FinalizeTransform();

private:
	
};