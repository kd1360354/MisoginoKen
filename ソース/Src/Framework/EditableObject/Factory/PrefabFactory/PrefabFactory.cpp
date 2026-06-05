#include "PrefabFactory.h"

#include "../../../../Application/Objects/Character/Player/Player.h"

#include <fstream>

using json = nlohmann::json;

std::shared_ptr<KdGameObject> PrefabFactory::LoadPrefab(const std::string& prefabName)
{
	std::ifstream ifs("Asset/Prefabs/" + prefabName + ".json");
	if (!ifs) return nullptr;

	json data;

	// JSONパース時の例外処理は、クラッシュを防ぐために推奨されますが、
	// ここでは元のコードの意図に従い、ファイルが開けた後の構造チェックを行います。
	ifs >> data;

	// 読み込みが成功したか、かつデータがJSONオブジェクト構造になっているかチェック
	if (ifs.fail() || !data.is_object()) {
		return nullptr;
	}

	// data内に "type" キーが存在するか確認
	if (!data.contains("type")) {
		return nullptr;
	}

	// typeキーの値が文字列であるか確認してから取得
	if (!data["type"].is_string()) return nullptr;

	std::string type = data["type"].get<std::string>();
	std::shared_ptr<KdGameObject> obj;

	// ------------------------------------
	// 1. Enemy プレハブのロード
	// ------------------------------------
	if (type == "Enemy") {
		// auto enemy = std::make_shared<Enemy>();
		// if (data.contains("hp")) enemy->SetHp(data["hp"]);
		// if (data.contains("attackPower")) enemy->SetAttackPower(data["attackPower"]);
		// // ... 固有のプロパティをすべて読み込む ...
		// obj = enemy;
	}

	// ------------------------------------
	// 2. Player プレハブのロード (全カスタム設定を反映)
	// ------------------------------------
	else if (type == "Player") {
		auto player = std::make_shared<Player>();

		// 【移動・基本設定】
		if (data.contains("moveSpeed")) player->SetMoveSpeed(data["moveSpeed"]);
		if (data.contains("jumpPower")) player->SetJumpPower(data["jumpPower"]);
		if (data.contains("rotationSpeed")) player->SetRotationSpeed(data["rotationSpeed"]);
		if (data.contains("potionCount")) player->SetPotionCount(data["potionCount"]);

		// 【攻撃・回避パラメータ】
		if (data.contains("maxAttackMoveDist")) player->SetMaxAttackMoveDistance(data["maxAttackMoveDist"]);
		if (data.contains("attackMoveSpeed")) player->SetAttackMoveSpeed(data["attackMoveSpeed"]);
		if (data.contains("maxDodgeMoveDist")) player->SetMaxDodgeMoveDistance(data["maxDodgeMoveDist"]);
		if (data.contains("dodgeMoveSpeed")) player->SetDodgeMoveSpeed(data["dodgeMoveSpeed"]);

		// 【スタミナコスト・回復パラメータ】
		if (data.contains("staminaCost_Attack")) player->SetAttackStaminaCost(data["staminaCost_Attack"]);
		if (data.contains("staminaCost_Dodge")) player->SetDodgeStaminaCost(data["staminaCost_Dodge"]);
		if (data.contains("staminaCost_BlockHit")) player->SetBlockHitStaminaCost(data["staminaCost_BlockHit"]);
		if (data.contains("staminaCost_RunPerSec")) player->SetRunStaminaCostPerSec(data["staminaCost_RunPerSec"]);
		if (data.contains("staminaDelay")) player->SetStaminaRecoveryDelay(data["staminaDelay"]);
		if (data.contains("staminaRecoverRate")) player->SetStaminaRecoverRate(data["staminaRecoverRate"]);
		if (data.contains("blockRecoveryRate")) player->SetBlockRecoveryRate(data["blockRecoveryRate"]);

		// 【HP・サウンド・その他】
		if (data.contains("potionHealRate")) player->SetPotionHealRate(data["potionHealRate"]);
		if (data.contains("hpDecreaseRate")) player->SetHpDecreaseRate(data["hpDecreaseRate"]);
		if (data.contains("audioVolume_BGM")) player->SetAudioVolume(data["audioVolume_BGM"]);
		if (data.contains("audioVolume_SE")) player->SetAudioSEVolume(data["audioVolume_SE"]);

		// 【HP/MaxHP/MaxStamina】
		// Max系のSetterが存在しない場合があるため、ここではSetHpのみ残します。
		// Playerクラスで SetMaxHP/SetMaxStamina が定義されていれば追加します。
		// if (data.contains("maxHp")) player->SetMaxHP(data["maxHp"]);
		if (data.contains("hp")) player->SetHp(data["hp"]);

		//obj = player;
	}

	if (!obj) return nullptr;

	// ------------------------------------
	// 3. 共通のトランスフォーム設定
	// ------------------------------------

	// データ存在チェックは冗長になるため、データが確実にあることを前提に読み込みます。
	// (JSONファイル作成時に必須キーとして扱われている場合)

	obj->SetName(data["name"].get<std::string>());
	obj->SetPos({ data["position"][0], data["position"][1], data["position"][2] });
	obj->SetRot({ data["rotation"][0], data["rotation"][1], data["rotation"][2] });
	obj->SetScale({ data["scale"][0], data["scale"][1], data["scale"][2] });

	// ★ プレハブとしてロードした後、初期化関数を呼び出す
	//    (MaxHP/MaxStaminaを現在のHP/Staminaに反映するなど、派生クラスの初期化に必要)
	obj->Init();

	return obj;
}
