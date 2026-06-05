#include "KdDebugGUI.h"
#include "../../../Application/main.h"
#include"../../../Application//Scene/BaseScene/BaseScene.h"
#include"../../../Application/Scene/GameScene/GameScene.h"
#include"../../../Application/Objects/Camera/CameraBase.h"
#include"../../../Application/Objects/Character/CharactorBase.h"
#include"../../../Application/Objects/Character/Player/Player.h"
#include"../../../Application/Objects/Character/Enemy/EnemyBase.h"
#include"../../../Application/Objects/Character/Enemy/NoobEnemy/NoobEnemy.h"
#include"../../../Application/Objects/Character/Enemy/MidBoss/MidBoss.h"
#include"../../../Application/Objects/Character/Enemy/Boss/LastBoss.h"
#include"../../EditableObject/Factory/PrefabFactory/PrefabFactory.h"
#include"../../../Application/Constants/GameScenenConstsnts/GameScenenConstsnts.h"
#include"../../../Application/Objects/Map/Stage1/Approach/Approach.h"
#include"../../../Application/Objects/Map/Stage1/lantern/lantern.h"
//Jsonのエイリアス
using json = nlohmann::json;

void KdDebugGUI::GuiInit()
{
	// 初期化済みなら動作させない
	if (m_uqLog) return;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();//スタイルをクラシックに変更
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(Application::Instance().GetWindowHandle());
	ImGui_ImplDX11_Init(
		KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext());

#include "imgui/ja_glyph_ranges.h"
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	// 日本語対応
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	m_uqLog = std::make_unique<ImGuiAppLog>();
}

void KdDebugGUI::GuiProcess(BaseScene& scene)
{
	if (!m_uqLog) return;

	// 1. メインウィンドウの設定
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(450, 800), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Debug Editor", nullptr, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	// 2. タブバー開始
	if (ImGui::BeginTabBar("MainEditorTabs"))
	{
		// --- 【タブ1】Hierarchy（シーン内のオブジェクト管理） ---
		if (ImGui::BeginTabItem("Hierarchy"))
		{
			ImGui::SeparatorText(U8("シーン操作"));
			if (ImGui::Button(U8("保存 (Json)"))) { SaveSceneToFile(scene); AddLog("Saved Scene.\n"); }
			ImGui::SameLine();
			if (ImGui::Button(U8("読込 (Json)"))) { LoadSceneFromFile(scene); AddLog("Loaded Scene.\n"); }

			ImGui::SeparatorText(U8("シーンオブジェクト一覧"));
			std::shared_ptr<KdGameObject> objToDelete = nullptr;
			if (ImGui::BeginChild("ObjList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true))
			{
				for (auto& obj : scene.GetObjList())
				{
					if (dynamic_cast<CameraBase*>(obj.get())) continue;
					ImGui::PushID(obj.get());
					if (ImGui::TreeNode(obj->GetName().c_str()))
					{
						if (ImGui::Button(U8("削除"))) { objToDelete = obj; }
						ImGui::SameLine();
						if (ImGui::Button(U8("Prefab保存"))) { SaveObjectAsPrefab(obj); }
						obj->OnImGui();
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
			}
			ImGui::EndChild();
			if (objToDelete) objToDelete->Expire();
			ImGui::EndTabItem();
		}

		// --- 【タブ2】Inspector（環境・表示設定） ---
		if (ImGui::BeginTabItem("Inspector"))
		{
			ImGui::SeparatorText(U8("ライティング設定"));
			if (ImGui::Checkbox(U8("夜の設定を適用"), &m_enableLighting)) { ApplyEnvironment(); }
			ImGui::BeginDisabled(!m_enableLighting);
			bool changed = false;
			if (ImGui::ColorEdit3(U8("環境光"), &m_ambientColor.x)) changed = true;
			if (ImGui::ColorEdit3(U8("平行光"), &m_dirLightColor.x)) changed = true;
			if (ImGui::DragFloat3(U8("方向"), &m_dirLightDir.x, 0.01f, -1.0f, 1.0f)) changed = true;
			if (changed) ApplyEnvironment();
			ImGui::EndDisabled();

			ImGui::SeparatorText(U8("デバッグ表示"));
			static bool isAllDebugMode = false;
			if (ImGui::Checkbox(U8("全デバッグ描画一括切替"), &isAllDebugMode)) {
				for (auto& obj : scene.GetObjList()) {
					if (auto chara = std::dynamic_pointer_cast<CharacterBase>(obj)) { chara->SetDebugDisplay(isAllDebugMode); }
				}
			}
			ImGui::EndTabItem();
		}

		// --- 【タブ3】Console（ログ出力） ---
		if (ImGui::BeginTabItem("Console"))
		{
			if (ImGui::Button(U8("Clear"))) { m_uqLog->Clear(); }
			ImGui::Separator();

			// ログ描画エリア
			if (ImGui::BeginChild("LogScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
			{
				// 既存のImGuiAppLogのDraw関数の中身をここで行うか、
				// Draw関数に「ウィンドウを作らず中身だけ描画するモード」を追加すると綺麗です。
				// 今回は既存の Draw をそのまま呼ぶか、中身を直接書きます。
				m_uqLog->DrawContents(); // ※もしDrawContentsという中身だけの関数があればそれを呼ぶ
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}
void KdDebugGUI::AddLog(const char* fmt, ...)
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	char buf[1024]; // 一時的な文字バッファ
	va_list args;
	va_start(args, fmt);

	// ここでフォーマット（%sや%dなど）を実際の文字に変換する
	vsnprintf(buf, sizeof(buf), fmt, args);

	va_end(args);

	// 完成した文字列をログに渡す。第1引数に "%s" を指定するのが安全
	m_uqLog->AddLog("%s", buf);
}

void KdDebugGUI::GuiRelease()
{
	if (!m_uqLog)return;

	m_uqLog.reset();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void KdDebugGUI::ApplyEnvironment()
{
	if (m_enableLighting)
	{
		// ★コメントアウトを解除している状態（夜の設定を適用）
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambientColor);
		KdShaderManager::Instance().WorkAmbientController().SetDirLight(m_dirLightDir, m_dirLightColor);
		AddLog("Lighting: NIGHT_MODE Applied.\n");
	}
	else
	{
		// ★コメントアウトしている状態（＝フレームワークの初期値に戻す）
		// 通常、初期値は Ambient(1.0, 1.0, 1.0) などの明るい状態です
		Math::Vector4 defaultAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };
		Math::Vector3 defaultDirCol = { 1.0f, 1.0f, 1.0f };
		Math::Vector3 defaultDir = { 0.0f, -1.0f, 0.0f }; // 真上から

		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(defaultAmbient);
		KdShaderManager::Instance().WorkAmbientController().SetDirLight(defaultDir, defaultDirCol);

		AddLog("Lighting: BYPASS (Comment-out style).\n");
	}
}



void KdDebugGUI::SaveSceneToFile(const BaseScene& scene)
{
	json data;
	// --- 🌌 環境設定をJSONに逃がす ---
	data["environment"]["ambientColor"] = { m_ambientColor.x, m_ambientColor.y, m_ambientColor.z, m_ambientColor.w };
	data["environment"]["dirLightColor"] = { m_dirLightColor.x, m_dirLightColor.y, m_dirLightColor.z };
	data["environment"]["dirLightDir"] = { m_dirLightDir.x, m_dirLightDir.y, m_dirLightDir.z };
	json objectsArray = json::array();
	std::map<std::string, int> typeCounter;
	for (const auto& obj : scene.GetObjList())
	{
		if (std::dynamic_pointer_cast<Player>(obj)) continue;
		if (std::dynamic_pointer_cast<CameraBase>(obj)) continue;

		json objJson;
		std::string typeName = obj->GetTypeName();

		// --- 名前を生成順（連番）にする ---
		char nameBuf[64];
		sprintf_s(nameBuf, "%s_%02d", typeName.c_str(), typeCounter[typeName]++);
		objJson["name"] = nameBuf;
		obj->SetName(nameBuf); // オブジェクト自身の名前も更新

		objJson["type"] = typeName;

		// 座標・回転・拡縮（今のコードのままでOK）
		Math::Vector3 pos = obj->GetPos();
		objJson["position"] = { pos.x, pos.y, pos.z };
		Math::Vector3 rot = obj->GetRot();
		objJson["rotation"] = { rot.x, rot.y, rot.z };
		Math::Vector3 scale = obj->GetScale();
		objJson["scale"] = { scale.x, scale.y, scale.z };

		objectsArray.push_back(objJson);
	}
	data["objects"] = objectsArray;
	std::ofstream ofs("Asset/Data/GameObjectLayout.json");
	ofs << std::setw(4) << data << std::endl;
}

void KdDebugGUI::LoadSceneFromFile( BaseScene& filePath)
{
	std::ifstream ifs("Asset/Data/GameObjectLayout.json");
	if (!ifs) return;

	json data;
	ifs >> data;
	// --- 1. まずは環境（シェーダー）の設定を最優先で反映 ---
	if (data.contains("environment"))
	{
		auto& env = data["environment"];

		// JSONから変数へロード
		m_ambientColor = { env["ambientColor"][0], env["ambientColor"][1], env["ambientColor"][2], env["ambientColor"][3] };
		m_dirLightColor = { env["dirLightColor"][0], env["dirLightColor"][1], env["dirLightColor"][2] };
		m_dirLightDir = { env["dirLightDir"][0], env["dirLightDir"][1], env["dirLightDir"][2] };

		// ★ここでシェーダー（ShaderManager）に即座に流し込む！
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambientColor);
		KdShaderManager::Instance().WorkAmbientController().SetDirLight(m_dirLightDir, m_dirLightColor);

		AddLog("Shader environment updated from JSON.\n");
	}
	if (!data.contains("objects")) return;

	// 既存の一時オブジェクトを消す（Player・Cameraは残す）
	for (auto& obj : filePath.GetObjList())
	{
		if (std::dynamic_pointer_cast<Player>(obj)) continue;
		if (std::dynamic_pointer_cast<CameraBase>(obj)) continue;
	}

	auto& factory = filePath.GetFactory();

	for (const auto& objData : data["objects"])
	{
		std::string type = objData["type"];
		std::string name = objData["name"];
		auto posArray = objData["position"];
		auto rotArray = objData["rotation"];
		auto scaleArray = objData["scale"];

		auto newObj = factory.CreateGameObject(type);
		if (!newObj) continue;

		newObj->SetName(name);
		newObj->SetPos(Math::Vector3(posArray[0], posArray[1], posArray[2]));
		newObj->SetRot(Math::Vector3(rotArray[0], rotArray[1], rotArray[2]));
		newObj->SetScale(Math::Vector3(scaleArray[0], scaleArray[1], scaleArray[2]));
		//if (auto enemy = std::dynamic_pointer_cast<Enemy>(newObj)) {
		////	if (objData.contains("hp")) enemy->SetHP(objData["hp"]);
		////	if (objData.contains("attackPower")) enemy->SetAttackPower(objData["attackPower"]);
		//}
		//newObj->SetHP(objData.value("hp", 100)); // HPのデフォルト値は100
		filePath.AddObject(newObj);
	}
}

void KdDebugGUI::SaveObjectAsPrefab(const std::shared_ptr<KdGameObject>& obj)
{
	json objJson;

	objJson["type"] = obj->GetTypeName();
	objJson["name"] = obj->GetName();
	objJson["modelPath"] = obj->GetModelPath();


	auto pos = obj->GetPos();
	objJson["position"] = { pos.x, pos.y, pos.z };

	auto rot = obj->GetRot();
	objJson["rotation"] = { rot.x, rot.y, rot.z };

	auto scale = obj->GetScale();
	objJson["scale"] = { scale.x, scale.y, scale.z };

	// Player固有の設定（完全に修正済み）
	if (auto _Player = std::dynamic_pointer_cast<Player>(obj)) {
		// Base ClassのGetMaxHP/GetMaxStaminaがCharacterBaseにあると仮定
		objJson["maxHp"] = _Player->GetMaxHP();
		objJson["maxStamina"] = _Player->GetMaxStamina();

		// ★★★ Player固有の設定値 ★★★
		objJson["moveSpeed"] = _Player->GetMoveSpeed();
		objJson["jumpPower"] = _Player->GetJumpPower();
		objJson["rotationSpeed"] = _Player->GetRotationSpeed();
		objJson["potionCount"] = _Player->GetPotionCount();

		objJson["attackPower"] = _Player->GetAttackPower();

		objJson["maxAttackMoveDist"] = _Player->GetMaxAttackMoveDistance();
		objJson["attackMoveSpeed"] = _Player->GetAttackMoveSpeed();
		objJson["maxDodgeMoveDist"] = _Player->GetMaxDodgeMoveDistance();
		objJson["dodgeMoveSpeed"] = _Player->GetDodgeMoveSpeed();

		objJson["staminaCost_Attack"] = _Player->GetAttackStaminaCost();
		objJson["staminaCost_Dodge"] = _Player->GetDodgeStaminaCost();
		objJson["staminaCost_BlockHit"] = _Player->GetBlockHitStaminaCost();
		objJson["staminaCost_RunPerSec"] = _Player->GetRunStaminaCostPerSec();
		objJson["staminaDelay"] = _Player->GetStaminaRecoveryDelay();
		objJson["staminaRecoverRate"] = _Player->GetStaminaRecoverRate();
		objJson["blockRecoveryRate"] = _Player->GetBlockRecoveryRate();

		objJson["potionHealRate"] = _Player->GetPotionHealRate();
		objJson["hpDecreaseRate"] = _Player->GetHpDecreaseRate();

		objJson["audioVolume_BGM"] = _Player->GetAudioVolume();
		objJson["audioVolume_SE"] = _Player->GetAudioSEVolume();
	}
	if (auto _Enemy = std::dynamic_pointer_cast<EnemyBase>(obj)) {
		objJson["maxHp"] = _Enemy->GetMaxHP();
		objJson["moveSpeed"] = _Enemy->GetMoveSpeed();
		objJson["rotationSpeed"] = _Enemy->GetRotationSpeed();
		objJson["searchRadius"] = _Enemy->GetSearchRadius();
		objJson["attackRadius"] = _Enemy->GetAttackRadius();
		objJson["rimPower"] = _Enemy->GetRimPower();
		objJson["baseattackPower"] = _Enemy->GetBaseAttackPower();
		Math::Vector3 rimCol = _Enemy->GetRimLightColor();
		objJson["rimLightColor"] = { rimCol.x, rimCol.y, rimCol.z };

		objJson["deathEffect"] = _Enemy->GetDeathEffectHandle(); // 構文エラー箇所を修正
	}
	if (auto _MidBoss = std::dynamic_pointer_cast<MidBoss>(obj)) {
		objJson["baseattackPower"] = _MidBoss->GetBaseAttackPower();
		objJson["CurrentattackPower"] = _MidBoss->GetCurrentAttackPower();
		objJson["hpRatio"] = _MidBoss->GetHpRatio();
	
	}
	if (auto _LastBoss = std::dynamic_pointer_cast<LastBoss>(obj)) {
		objJson["baseattackPower"] = _LastBoss->GetBaseAttackPower();
		objJson["CurrentattackPower"] = _LastBoss->GetCurrentAttackPower();
		objJson["hpRatio"] = _LastBoss->GetHpRatio();

	}
	if (auto _Apprach = std::dynamic_pointer_cast<Approach>(obj))
	{
		auto uvpos	=_Apprach->GetUVTile();
		objJson["uvTile"] = { uvpos.x,uvpos.y };
	}
	if (auto _Lantern = std::dynamic_pointer_cast<Lantern>(obj)) {
		// ライトの色（Vector3）
		Math::Vector3 activeCol = _Lantern->GetActiveColor();
		objJson["activeColor"] = { activeCol.x, activeCol.y, activeCol.z };

		// 半径と明るさ設定
		objJson["activeRadius"] = _Lantern->GetActiveRadius();
		objJson["activeIsBright"] = _Lantern->GetActiveIsBright();
	}
	// ファイル名をそのまま使う
	std::string fileName = "Asset/Prefabs/" + obj->GetName() + ".json";
	std::ofstream ofs(fileName);

	// ★★★ 堅牢な書き込み処理に修正 ★★★
	if (ofs.is_open())
	{
		ofs << std::setw(4) << objJson << '\n'; // std::endl を '\n' に変更
		ofs.flush(); // 書き込みバッファを強制的にフラッシュ
		ofs.close(); // ファイルを明示的に閉じる

		AddLog("Saved Prefab to %s\n", fileName.c_str());
	}
	else
	{
		 AddLog("Error: Failed to open file for saving: %s\n", fileName.c_str());
		// ファイルが開けなかった場合の処理
	}
}

