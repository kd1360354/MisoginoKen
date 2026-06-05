# 3Dアクションゲーム『禊の剣』

## 📺 プレイ動画
> [!IMPORTANT]
> 映像をクリックすると、YouTube（紹介動画）へジャンプします。

[![プレイ動画](https://img.youtube.com/vi/noXluIoCWq8/maxresdefault.jpg)](https://youtu.be/noXluIoCWq8)

---

## 🎮 開発概要
- **作品名:** 禊の剣
- **開発言語:** C++
- **グラフィックス:** DirectX 11
- **開発環境:** Microsoft Visual Studio 2022
- **開発人数:** 1名（個人開発）
- **開発期間:** 5ヵ月
- **使用ツール:** GitHub (バージョン管理), Blender ver.4.4.0, Audacity, Effekseer v1.70e

---

## 💡 注力した点
プレイヤーアクションの基盤として**ステートパターンによる状態管理**を構築し、攻撃・回避などの複雑な遷移を疎結合に設計しました。これにより、状態遷移に伴うバグを最小限に抑えつつ、アクションの追加や調整を容易にする高い拡張性を持たせています。
また、DirectX 11を用いた開発において、**ベクトル演算による挙動制御や当たり判定の最適化を自前で実装**し、3Dアクション特有の「手触りの良さ」とレスポンスの向上に注力しました。

---

## 🛠️ 苦労した点と解決策
3DCGモデルのアニメーション制御において、ボーン構造の理解不足から再生タイミングや座標のズレが発生し、仕様再現に苦慮しました。
**【解決策】**
これを解決するため、**デバッグ用の描画機能を自作してボーンや当たり判定を可視化**し、フレーム単位での数値検証を繰り返すことで、意図通りの挙動を実現しました。この経験を通じ、ブラックボックス化させずに内部構造を解析・可視化して問題を切り分ける、エンジニアとしてのデバッグ手法の重要性を学びました。

---

## 📁 主要コードへのリンク（ソースコードの確認）
特に注力して実装した、自作のコアロジック部分のソースコードです。

- [ステートパターンによる状態管理・アクション遷移のコード](https://github.com/kd1360354/MisoginoKen/tree/main/%E3%82%BD%E3%83%BC%E3%82%B9/Src/Framework/StateMachine/Player/PlayerState)
- [ビヘイビアーツリーによる敵の行動制御のコード](https://github.com/kd1360354/MisoginoKen/tree/main/%E3%82%BD%E3%83%BC%E3%82%B9/Src/Framework/AI/BehaviorTree)


---

## 📚 使用ライブラリ・アセット・フレームワークの明示
本プロジェクトでは、学校配布の基本フレームワークを活用しつつ、各種ライブラリを組み込んで開発しています。

### 使用ライブラリ一覧
- **Direct3D11 / DirectX Math** (Windows SDK: グラフィックス描画、ベクトル・行列・レイ判定など)
- **DirectX Tool Kit** (ベクトル、行列、オーディオ)
- **DirectX Tex** (テクスチャ読み込み)
- **ImGui** (デバッグ用GUI表示)
- **Effekseer ver.1.7.0e** (エフェクトの表示制御)

### 使用素材・アセット
- モデル・アニメーション・BGM: [Mixamo](https://www.mixamo.com/#) / [BOOTH](https://booth.pm/) / [Unity Asset Store](https://assetstore.unity.com/) 等からライセンスに準拠して使用

<details>
<summary><b>学校配布ソースコード一覧（Src/Framework 内）</b></summary>

学校より配布された基盤フレームワークの構成は以下の通りです。これら以外のゲームループ、オブジェクト管理、キャラクター制御、ステート遷移、コリジョン応答などのゲームロジックはすべて自作しています。

- ウィンドウ作成 (`Window/KdWindow.h`)
- Direct3D 初期化・バッファ作成 (`Direct3D/KdDirect3D.h`, `KdBuffer.h`)
- 描画機能・カメラ・テクスチャ・メッシュ管理 (`Direct3D/KdCamera.h`, `KdTexture.h`, `KdMesh.h`)
- 3D モデル管理・gltf読み込み (`Direct3D/KdModel.h`, `KdGLTFLoader.h`)
- シェーダー管理・コントローラー (`Shader/KdShaderManager.h`, `KdAmbientController.h`, `KdRenderTargetChange.h`)
- 各種シェーダー (`Shader/StandardShader`, `PostProcessShader`, `SpriteShader`)
- 音声・入力管理 (`Audio/KdAudio.h`, `Input/KdInput.h`)
- アニメーション・当たり判定計算補助 (`Math/KdAnimation.h`, `KdCollider.h`)
- CSV読込・アセット管理 (`Utility/KdCSVData.h`, `KdDataStorage.h`)
- デバッグワイヤー描画補助 (`Utility/KdDebugWireFrame.h`)
- エフェクト制御補助 (`Effekseer/KdEffekseerManager.h`)
</details>

---
神戸電子専門学校 ゲームエンジニア学科
藤原 天翔
更新日: 2026年6月5日