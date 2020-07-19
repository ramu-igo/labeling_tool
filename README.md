# labeling_tool
Qt-based GUI labeling tool.

![tool_example](https://user-images.githubusercontent.com/68230762/87870056-8c7b4400-c9df-11ea-9f3d-0f48a0785905.png)

![result](https://user-images.githubusercontent.com/68230762/87870067-99983300-c9df-11ea-834a-fcc3d2f1f498.png)

・(主に)人物の全身画像に対するラベリングツール.
　想定用途は、画像変換タスク(pix2pixやその派生等)の学習データセット作成.

・配置する点の種類やつなぎ方は設定ファイル(config.json)で指定可能.

・点のサイズはマウスホイールで調整可能.

・シンプルなプリセット機能(Set/Put).

・編集結果はjsonおよびラベル画像としてエクスポート可能.

※推奨環境: Qt Creator 4.1.0 / Qt 5.7.0
