<p align="center">
  <a href="README_en.md">🇬🇧 English</a> |
  <a href="README_ru.md">🇷🇺 Русский</a> |
  <a href="README_de.md">🇩🇪 Deutsch</a> |
  <a href="README_ja.md">🇯🇵 日本語</a> 
</p>

# 空間個体群動態モデル
### ブラウンバグモデル、近傍依存モデル、およびディックマン・ローモデルの実装と比較分析
![License](https://img.shields.io/badge/license-Academic%20Use%20Only-red)
![C++](https://img.shields.io/badge/language-C++-yellow)
![Python](https://img.shields.io/badge/visualization-Python-blue)
![Field](https://img.shields.io/badge/field-computational%20biology-purple)

---
### 概要

このレポジトリには、**空間的確率的個体群モデル**: **ブラウンバグ(Brownian Bug, BB)** モデル、**近傍依存(Neighborhood-Dependent, ND)** モデル、および**ディックマン・ロー（Dieckmann-Law, DL)** モデルの実装と比較研究が含まれています。

本プロジェクトは、**数値シミュレーション**、**空間的パターン形成**、および**個体群動体の解析**に焦点を当ててあり、特にハードカットオフとカーネルベースの相互作用メカニズムの違いに重点を置いています。

---
### 研究目標

その目的は、個体群動態の空間的確率モデルを**実装**し、NDのハードな近傍相互作用メカニズムとDLの『滑らかな競合カーネル』を**比較**し、クラスタリング、絶滅メカニズム、臨界領域を**分析**することであります。

---
### モデルの説明

#### ブラウンバグ (BB) モデル 
- 個体はブラウン運動によって移動する、
- 一定の出生率と死亡率: $\lambda = \text{const} \quad \mu = \text{const}$、
- 個体間の相互作用はない。

#### 近傍依存 (ND) モデル
- 局所的な相互作用を伴うブラウン運動、
- 出生率は近傍個体の数に依存する: $\lambda_i = \lambda_0 - \frac{1}{N_s} N_R^{(i)}$,
- ハードな相互作用の半径 $R$。

#### ディックマン・ロー (DL) モデル　
- 個体は**空間的に固定されており (移動しない)**、
- カーネルの相互作用: $d_i = d_0 + \alpha C_i$, ここで $C_i = \sum_j \exp\left(-\frac{r_{ij}^2}{2R_c^2}\right)$、
- 子孫が地域内に分散している: $x_{\text{child}} = x_i + \xi, \quad \xi \sim \mathcal{N}(0, \sigma^2)$。

---
### 可視化

インタラクティブな可視化には、Pythonライブラリである**Plotly**が使用されています。また、Plotlyは、経時的な個体数、出生および死亡の動態、累積統計、および出生イベントの累積ヒートマップの描画にも使用されています。

`data/example` ディレクトリにあるシミュレーション用のサンプルダッシュボード：

![Brownian Bug Dash](../assets/bb_example_dash.png)
![Neighborhood-Dependent Dash](../assets/nd_example_dash.png)
![Dieckmann-Law Dash](../assets/dl_example_dash.png)

---

### 組み立て手順

**プロジェクトを組み立てます：**
```bash
mkdir build
cd build
cmake ..
make
```

**シムレーションを実行します：**
```bash
./spatial_models bb
./spatial_models nd
./spatial_models dl
```

**可視化します：**
```bash
python scripts/plot_dashboard.py data/output/dl_output.csv
```
---
### 参考文献

- Dieckmann, U. & Law, R. (1996) *The dynamical theory of coevolution*.
- Hernández-García, E., & López, C. (2004) *Clustering, advection, and patterns in population dynamics*.
- Law, R., & Dieckmann, U. (2000) *Plant community spatial models*.

---
### ライセンス

本プロジェクトは、学術および教育目的でのみ利用することを意図しています。

詳細については、`LICENSE`ファイルをご覧ください。

---
### プロジェクトの進捗状況: 🟨 🟨 🟨

**本プロジェクトは現在、活発に開発が進められています！** 

---
© 2026, Mikhail Kolesnikov (ミハイル・コレスニコフ） \
Moscow, Higher School of Economics, Faculty of Computer Science, BSc 

All rights reserved.