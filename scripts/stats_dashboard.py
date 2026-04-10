import argparse
import pandas as pd
import numpy as np
import plotly.graph_objects as go
from plotly.subplots import make_subplots


def compute_counts(df: pd.DataFrame) -> pd.DataFrame:
    alive = df[df["state"] == "alive"][["t", "id"]].drop_duplicates()
    alive_counts = alive.groupby("t")["id"].nunique().rename("alive")

    born_counts = df[df["state"] == "born"].groupby("t").size().rename("born")
    dead_counts = df[df["state"] == "dead"].groupby("t").size().rename("dead")

    out = pd.concat([alive_counts, born_counts, dead_counts], axis=1).fillna(0).sort_index()
    out["born_cum"] = out["born"].cumsum()
    out["dead_cum"] = out["dead"].cumsum()
    return out.reset_index().rename(columns={"index": "t"})


def compute_spatial_stats(df: pd.DataFrame) -> pd.DataFrame:
    alive = df[df["state"] == "alive"][["t", "x", "y"]].copy()
    g = alive.groupby("t")
    out = pd.DataFrame({
        "t": g.size().index,
        "x_mean": g["x"].mean().values,
        "y_mean": g["y"].mean().values,
        "x_std":  g["x"].std(ddof=0).fillna(0).values,
        "y_std":  g["y"].std(ddof=0).fillna(0).values,
        "x_min":  g["x"].min().values,
        "x_max":  g["x"].max().values,
        "y_min":  g["y"].min().values,
        "y_max":  g["y"].max().values,
    })
    out["bbox_area"] = (out["x_max"] - out["x_min"]) * (out["y_max"] - out["y_min"])
    return out


def birth_heatmap_fixed_cell(df: pd.DataFrame,
                             until_t: int | None,
                             cell: float,
                             domain: tuple[float, float, float, float]) -> tuple[np.ndarray, np.ndarray, np.ndarray]:

    xmin, xmax, ymin, ymax = domain

    born = df[df["state"] == "born"][["t", "x", "y"]].copy()
    if until_t is not None:
        born = born[born["t"] <= until_t]

    # Edges aligned to domain with step=cell
    nx = int(np.round((xmax - xmin) / cell))
    ny = int(np.round((ymax - ymin) / cell))

    # Ensure exact edges (nx*cell matches range)
    xedges = xmin + cell * np.arange(nx + 1)
    yedges = ymin + cell * np.arange(ny + 1)

    H = np.zeros((ny, nx), dtype=float)
    if born.empty:
        return H, xedges, yedges

    # Bin indices
    x = born["x"].to_numpy(dtype=float)
    y = born["y"].to_numpy(dtype=float)

    # Clamp into [xmin, xmax) to avoid edge issues at xmax
    x = np.clip(x, xmin, np.nextafter(xmax, xmin))
    y = np.clip(y, ymin, np.nextafter(ymax, ymin))

    ix = np.floor((x - xmin) / cell).astype(int)
    iy = np.floor((y - ymin) / cell).astype(int)

    np.add.at(H, (iy, ix), 1.0)

    return H, xedges, yedges


def normalize_heatmap(H: np.ndarray, scale: str, clip_q: float) -> tuple[np.ndarray, str]:
    Z = H.astype(float)

    # Ignore zeros
    nz = Z[Z > 0]
    if nz.size > 0:
        vmax = float(np.quantile(nz, clip_q))
        if vmax > 0:
            Z = np.minimum(Z, vmax)

    if scale == "log":
        Z = np.log1p(Z)
        return Z, "log(1 + births)"
    return Z, "births (clipped)"


def main():
    ap = argparse.ArgumentParser(description="Plotly dashboard for simulation CSV")
    ap.add_argument("--csv", required=True, help="Path to CSV, for example data/output/nd_lattice.csv")
    ap.add_argument("--every", type=int, default=1, help="Downsample by keeping every k-th t (for speed)")
    ap.add_argument("--html", default=None, help="Save dashboard to HTML (optional)")

    # Birth heatmap controls
    ap.add_argument("--birth-heatmap-until", type=int, default=None,
                    help="Accumulate births up to this t (default: up to max t in CSV)")
    ap.add_argument("--cell", type=float, default=0.02, help="Heatmap cell size (default 0.02)")
    ap.add_argument("--domain", nargs=4, type=float, default=[0, 1, 0, 1],
                    metavar=("XMIN", "XMAX", "YMIN", "YMAX"),
                    help="Heatmap domain. Default: 0 1 0 1")

    # Normalization
    ap.add_argument("--hm-scale", choices=["log", "linear"], default="log",
                    help="Heatmap scaling: log (recommended) or linear")
    ap.add_argument("--hm-clip-q", type=float, default=0.995,
                    help="Clip heatmap at this quantile (0..1), e.g. 0.99/0.995")

    args = ap.parse_args()

    df = pd.read_csv(args.csv, comment="#")

    need_cols = {"t", "id", "x", "y", "state", "parent_id"}
    missing = need_cols - set(df.columns)
    if missing:
        raise SystemExit(f"CSV missing columns: {sorted(missing)}")

    if args.every > 1:
        df = df[df["t"] % args.every == 0].copy()

    counts = compute_counts(df)
    spatial = compute_spatial_stats(df)
    merged = pd.merge(counts, spatial, on="t", how="outer").fillna(0).sort_values("t")

    until_t = args.birth_heatmap_until
    if until_t is None and not df.empty:
        until_t = int(df["t"].max())

    domain = tuple(float(v) for v in args.domain)
    H, xedges, yedges = birth_heatmap_fixed_cell(df, until_t=until_t, cell=args.cell, domain=domain)
    Z, cb_title = normalize_heatmap(H, args.hm_scale, args.hm_clip_q)

    # Centers for plotting
    xcenters = (xedges[:-1] + xedges[1:]) / 2
    ycenters = (yedges[:-1] + yedges[1:]) / 2

    fig = make_subplots(
        rows=2, cols=3,
        subplot_titles=(
            "Alive agents over time",
            "Born/Dead per step",
            "Cumulative born/dead",
            "Position spread (std)",
            "Bounding-box occupied area",
            f"Cumulative birth heatmap"
        )
    )

    # (1) Alive
    fig.add_trace(go.Scatter(x=merged["t"], y=merged["alive"], mode="lines", name="alive"), row=1, col=1)

    # (2) Born/dead per step
    fig.add_trace(go.Bar(x=merged["t"], y=merged["born"], name="born/step"), row=1, col=2)
    fig.add_trace(go.Bar(x=merged["t"], y=merged["dead"], name="dead/step"), row=1, col=2)

    # (3) Cumulative
    fig.add_trace(go.Scatter(x=merged["t"], y=merged["born_cum"], mode="lines", name="born_cum"), row=1, col=3)
    fig.add_trace(go.Scatter(x=merged["t"], y=merged["dead_cum"], mode="lines", name="dead_cum"), row=1, col=3)

    # (4) std of x/y
    fig.add_trace(go.Scatter(x=merged["t"], y=merged["x_std"], mode="lines", name="x_std"), row=2, col=1)
    fig.add_trace(go.Scatter(x=merged["t"], y=merged["y_std"], mode="lines", name="y_std"), row=2, col=1)

    # (5) Bounding box area
    fig.add_trace(go.Scatter(x=merged["t"], y=merged["bbox_area"], mode="lines", name="bbox_area"), row=2, col=2)

    # (6) Heatmap
    fig.add_trace(
        go.Heatmap(
            x=xcenters,
            y=ycenters,
            z=Z,
            colorbar=dict(title=cb_title)
        ),
        row=2, col=3
    )

    xmin, xmax, ymin, ymax = domain
    fig.update_xaxes(range=[xmin, xmax], row=2, col=3)
    fig.update_yaxes(range=[ymin, ymax], row=2, col=3)

    # Map-like aspect ratio
    fig.update_yaxes(scaleanchor="x6", scaleratio=1, row=2, col=3)

    fig.update_layout(
        title=f"Simulation stats dashboard: {args.csv}",
        barmode="group",
        legend=dict(orientation="h", yanchor="bottom", y=-0.15, xanchor="left", x=0),
        height=900
    )

    # Labels
    fig.update_xaxes(title_text="t", row=1, col=1)
    fig.update_xaxes(title_text="t", row=1, col=2)
    fig.update_xaxes(title_text="t", row=1, col=3)
    fig.update_xaxes(title_text="t", row=2, col=1)
    fig.update_xaxes(title_text="t", row=2, col=2)
    fig.update_xaxes(title_text="x", row=2, col=3)
    fig.update_yaxes(title_text="y", row=2, col=3)

    if args.html:
        fig.write_html(args.html, include_plotlyjs="cdn")
        print(f"Saved: {args.html}")

    fig.show()


if __name__ == "__main__":
    main()
