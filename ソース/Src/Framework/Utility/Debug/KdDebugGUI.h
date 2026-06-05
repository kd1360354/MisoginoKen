#pragma once
#include"../../../Application/Constants/GameScenenConstsnts/GameScenenConstsnts.h"
class BaseScene;
class KdGameObject;
struct ImGuiAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
	bool                AutoScroll;
	bool                ScrollToBottom;

	ImGuiAppLog()
	{
		AutoScroll = true;
		ScrollToBottom = false;
		Clear();
	}

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
		if (AutoScroll)
			ScrollToBottom = true;
	}

	// ★これを追加：ウィンドウを作らずに中身（ログテキスト）だけを描画する
	void DrawContents()
	{
		// フィルターやコピーなどのコントロール用ボタンが必要ならここに追加
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		if (ImGui::Button("Copy")) ImGui::LogToClipboard();
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);
		ImGui::Separator();

		// 子ウィンドウ領域を作成してスクロール可能にする
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = Buf.begin();
		const char* buf_end = Buf.end();

		if (Filter.IsActive())
		{
			for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
			{
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				if (Filter.PassFilter(line_start, line_end))
					ImGui::TextUnformatted(line_start, line_end);
			}
		}
		else
		{
			// 大量のログを高速に描画するためのクリッパー
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					ImGui::TextUnformatted(line_start, line_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		// 自動スクロール処理
		if (ScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;

		ImGui::EndChild();
	}
};

class KdDebugGUI
{
public:
	KdDebugGUI()	{}
	~KdDebugGUI()	{}

	//ImGuiの初期化
	void GuiInit();
	
	//毎フレームのUI描画処理
	void GuiProcess(BaseScene& scene);

	//外部からログを追加するための関数
	void AddLog(const char* fmt, ...);

	//ImGuiの解放
	void GuiRelease();
	
	void ApplyEnvironment();

private:
	//現在のシーンの状態をJsonファイルに書き出す
	void SaveSceneToFile(const BaseScene& scene);

	void LoadSceneFromFile( BaseScene& filePath);

	void SaveObjectAsPrefab(const std::shared_ptr<KdGameObject>& obj);

	// 新しいオブジェクトにユニークな名前を付けるためのカウンター
	int	m_nextObjectId = 0;

	// ImGui
	std::unique_ptr<ImGuiAppLog> m_uqLog = nullptr;
	bool m_enableLighting = true; // ライティングを計算するかどうか

	// 定数ファイルから初期値を持ってくる
	Math::Vector4 m_ambientColor = SceneParam::Environment::Night::AMBIENT_COLOR;
	Math::Vector3 m_dirLightColor = SceneParam::Environment::Night::DIR_LIGHT_COLOR;
	Math::Vector3 m_dirLightDir = SceneParam::Environment::Night::DIR_LIGHT_DIR;
public:
	static KdDebugGUI& Instance() {
		static KdDebugGUI Instance;
		return Instance;
	}

};
 