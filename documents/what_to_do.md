### Update models:

```
cmake --build build -j
```

### Build models (example):

```
./build/spatial_models bb_crit 91
./build/spatial_models nd 91
./build/spatial_models dl_cluster 91
./build/spatial_models dl 91
```



### Show animation script (example):

```
python scripts/animate_scatter.py --csv data/output/dl_C3.csv --every 5
python scripts/animate_scatter.py --csv data/output/nd_output.csv --every 5
python scripts/animate_scatter.py --csv data/output/dl_cluster.csv --every 5
```



### Show dashboard script (example):

```
python scripts/stats_dashboard.py \
  --csv data/output/dl_comp1.csv \
  --every 5 \
  --domain 0 1 0 1 \
  --cell 0.02 \
  --birth-heatmap-until 5000 \
  --hm-scale log \
  --hm-clip-q 0.995
python scripts/stats_dashboard.py \
    --csv data/output/nd_lattice.csv \
    --every 5 \
    --domain 0 1 0 1 \
    --cell 0.02 \
    --birth-heatmap-until 5000 \
    --hm-scale log \
    --hm-clip-q 0.995
python scripts/stats_dashboard.py \
  --csv data/output/dl_cluster.csv \
  --every 5 \
  --domain 0 1 0 1 \
  --cell 0.02 \
  --birth-heatmap-until 5000 \
  --hm-scale log \
  --hm-clip-q 0.995
```
