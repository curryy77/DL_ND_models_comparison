<p align="center">
  <a href="docs/README_en.md">🇬🇧 English</a> |
  <a href="docs/README_ja.md">🇯🇵 日本語</a> |
  <a href="docs/README_fr.md">🇫🇷 Français</a> |
  <a href="docs/README_ru.md">🇷🇺 Русский</a> 
</p>

# Spatial Population Dynamics Models
### Implementation and Comparative Analysis of Brownian Bug, Neighborhood-Dependent and Dieckmann–Law Models

![License](https://img.shields.io/badge/license-Academic%20Use%20Only-red)
![C++](https://img.shields.io/badge/language-C++-yellow)
![Python](https://img.shields.io/badge/visualization-Python-blue)
![Field](https://img.shields.io/badge/field-computational%20biology-purple)

---
### About
This repository contains an implementation and comparative study of three **spatial stochastic population models**: **Brownian Bug (BB)** model, **neighborhood-dependent (ND)** model and **Dieckmann-Law (DL)** model. 

This project focuses on **numerical simulation**, **spatial pattern formation**, and **analysis of population dynamics**, with particular emphasis on the differences between hard-cutoff and kernel-based interaction mechanisms.

---
### Research Goals

The goals are to **implement** spatial stochastic models of population dynamics, **compare** ND's hard neighborhood interaction mechanism with the DL's smooth competition kernel, **analyze** clustering, extinction mechanisms, critical regimes.

---
### Models Description

#### Brownian Bug (BB) Model 
- Individuals move via Brownian motion,
- Constant birth and death rates:
$$
  \lambda = \text{const}, \quad \mu = \text{const},
$$

- No interaction between individuals.

#### Neighborhood-Dependent (ND) Model
- Brownian motion with local interaction,
- Birth rate depends on number of neighbors:
  $$
  \lambda_i = \lambda_0 - \frac{1}{N_s} N_R^{(i)},
  $$
- Hard interaction radius $R$.

#### Dieckmann-Law (DL) Model
- Individuals are **spatially fixed (no movement)**,
- Interaction via continuous kernel:
  $$
  d_i = d_0 + \alpha C_i,
  $$
  $$
  C_i = \sum_j \exp\left(-\frac{r_{ij}^2}{2R_c^2}\right),
  $$
- Offspring distributed locally:
  $$
  x_{\text{child}} = x_i + \xi, \quad \xi \sim \mathcal{N}(0, \sigma^2).
  $$

---
### Visualization
Interactive visualization is implemented using **Plotly**, a Python library. Also, Plotly is used for plotting population size over time, birth/death dynamics, cumulative statistics, and cumulative heatmap of birth events.

Examples for simulations in `data/example`:

**Animate scatter**:

![](assets/dl_animation.gif)

---
