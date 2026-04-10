<p align="center">
  <a href="README_en.md">🇬🇧 English</a> |
  <a href="README_ru.md">🇷🇺 Русский</a> |
  <a href="README_de.md">🇩🇪 Deutsch</a> |
  <a href="README_ja.md">🇯🇵 日本語</a> 
</p>

# Räumliche Modelle der Populationsdynamik
### Implementierung und vergleichende Analyse von Brownian-Bug-, Neighborhood-Dependent- und Dieckmann-Law-Modellen

![License](https://img.shields.io/badge/license-Academic%20Use%20Only-red)
![C++](https://img.shields.io/badge/language-C++-yellow)
![Python](https://img.shields.io/badge/visualization-Python-blue)
![Field](https://img.shields.io/badge/field-computational%20biology-purple)

---
### Über
Dieses Repository enthält eine Implementierung und eine vergleichende Untersuchung von drei **räumlichen stochastischen Populationsmodellen**: dem **Brownian-Bug-Modell (BB)**, dem **Neighborhood-Dependent-Modell (ND)** und dem **Dieckmann-Law-Modell (DL)**. 

Im Mittelpunkt dieses Projekts stehen **numerische Simulationen**, **die Entstehung räumlicher Muster** und die **Analyse der Populationsdynamik**, wobei ein besonderer Schwerpunkt auf den Unterschieden zwischen Interaktionsmechanismen mit harter Abgrenzung und kernelbasierten Interaktionsmechanismen liegt.

---
### Forschungsziele

Die Ziele bestehen darin, räumliche stochastische Modelle der Populationsdynamik **zu implementieren**, den Mechanismus der „Hard Neighborhood“-Interaktion in ND mit dem „Smooth Competition“-Kernel in DL **zu vergleichen** sowie Clusterbildung, Aussterbemechanismen und kritische Regime **zu analysieren**.

---
### Modellbeschreibung

#### Brownian Bug (BB) Modell
- Einzelne Teilchen bewegen sich nach dem Brownschen Bewegungsgesetz,
- Konstante Geburten- und Sterberaten: $\lambda = \text{const} \quad \mu = \text{const}$,
- Keine Interaktion zwischen den Teilnehmern.

#### Neighborhood-Dependent (ND) Modell
- Brownsche Bewegung mit lokaler Wechselwirkung,
- Die Geburtenrate hängt von der Anzahl der Nachbarn ab: $\lambda_i = \lambda_0 - \frac{1}{N_s} N_R^{(i)}$,
- Harter Wechselwirkungsradius $R$.

#### Dieckmann-Law (DL) Modell
- Teilchen sind **ortsfest (keine Bewegung)**,
- Interaktion über einen kontinuierlichen Kernel: $d_i = d_0 + \alpha C_i$, wo $C_i = \sum_j \exp\left(-\frac{r_{ij}^2}{2R_c^2}\right)$,
- Nachkommen vor Ort verteilt: $x_{\text{child}} = x_i + \xi, \quad \xi \sim \mathcal{N}(0, \sigma^2)$.

---
### Visualisierung
Die interaktive Visualisierung wird mithilfe von **Plotly**, einer Python-Bibliothek, umgesetzt. Außerdem wird Plotly verwendet, um die Populationsgröße im Zeitverlauf, die Geburten- und Sterbedynamik, kumulative Statistiken sowie eine kumulative Heatmap der Geburtenereignisse darzustellen.

Beispiel-Dashboards für Simulationen im Verzeichnis `data/example`:

![Brownian Bug Dash](../assets/bb_example_dash.png)
![Neighborhood-Dependent Dash](../assets/nd_example_dash.png)
![Dieckmann-Law Dash](../assets/dl_example_dash.png)

---

### Bauanleitung

**Das Projekt aufbauen:**
```bash
mkdir build
cd build
cmake ..
make
```

**Durchführen der Simulationen:**
```bash
./spatial_models bb
./spatial_models nd
./spatial_models dl
```

**Visualisierung:**
```bash
python scripts/plot_dashboard.py data/output/dl_output.csv
```
---
### Literaturverzeichnis

- Dieckmann, U. & Law, R. (1996) *The dynamical theory of coevolution*.
- Hernández-García, E., & López, C. (2004) *Clustering, advection, and patterns in population dynamics*.
- Law, R., & Dieckmann, U. (2000) *Plant community spatial models*.

---
### Lizenz

Dieses Projekt ist ausschließlich für akademische und pädagogische Zwecke bestimmt.

Weitere Informationen finden Sie in der Datei `LICENSE`.

---
### Projektstatus: 🟨 🟨 🟨

**Das Projekt befindet sich derzeit in der aktiven Entwicklungsphase!** 

---
© 2026, Mikhail Kolesnikov (Michail Kolesnikow) \
Moscow, Higher School of Economics, Faculty of Computer Science, BSc 

All rights reserved.