# Memo

## 振り返り

### 最初にやること

- Create New Projectする
- platformio.iniに`monitor_speed = 115200`を書き込む

### ライブラリの入れ方

1. ホーム
2. Libraries
3. 検索
4. 選択
5. Add to Project
6. プロジェクトを選択
7. Add

### 3.x系をつかいたい場合

[リリースページ](https://github.com/pioarduino/platform-espressif32/releases)にアクセスし、使いたいバージョンのzipのリンクを取得。（`https://github.com/pioarduino/platform-espressif32/releases/download/x.y.z/platform-espressif32.zip`形式）

### 環境切り替え方法

1. 下のバーから環境を選択
2. ビルド
3. `>PlatformIO: Rebuild IntelliSense Index`

うまく切り替わらない場合は1の環境を選択した後に

```bash
pio platform uninstall espressif32
pio platform install espressif32
```

を実行

### なんかおかしい場合は、

```bash
rm -rf ~/.platformio
```

### その他操作

- チェック: ビルド
- 右矢印: 書き込み
- ゴミ箱: ビルド成果物削除
- 三角フラスコ: テスト
- ケーブル: モニター

### 宣言とコンパイラ制約、および副次的な型安全と契約の分離について

Cppは上から下に読む。
下のものを上で使う場合、あらかじめ関数宣言（シグネチャ）を定義する必要がある。
または、別のヘッダーファイル（.h）で定義したファイルを読み込む。

実際に、宣言を行うことによってコンパイル時の型安全性が担保できる。
また、静的束縛としての契約によって、責務の分離が可能となっている。
ただし、インターフェースや純粋仮装関数（抽象メソッド）のような動的束縛とは異なる。

## 方針

## todo
