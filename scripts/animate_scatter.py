import argparse
import pandas as pd
import plotly.express as px

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True, help="Path to csv, e.g. data/output/nd_output.csv")
    ap.add_argument("--every", type=int, default=1, help="Use every k-th time step (downsample frames)")
    ap.add_argument("--only-alive", action="store_true", help="Animate only state==alive rows")
    ap.add_argument("--range", nargs=4, type=float, default=None,
                    metavar=("XMIN","XMAX","YMIN","YMAX"),
                    help="Fix axes range")
    ap.add_argument("--html", default=None, help="Save interactive HTML (optional)")
    args = ap.parse_args()

    df = pd.read_csv(args.csv, comment="#")

    if args.only_alive:
        df = df[df["state"] == "alive"].copy()

    if args.every > 1:
        df = df[df["t"] % args.every == 0].copy()

    fig = px.scatter(
        df,
        x="x",
        y="y",
        animation_frame="t",
        animation_group="id",
        hover_name="id",
        color="state" if not args.only_alive else None,
        title=f"Animation: {args.csv}"
    )

    # Fix axes so they don't jump
    if args.range is not None:
        xmin, xmax, ymin, ymax = args.range
    else:
        xmin, xmax = float(df["x"].min()), float(df["x"].max())
        ymin, ymax = float(df["y"].min()), float(df["y"].max())

    fig.update_xaxes(range=[xmin, xmax])
    fig.update_yaxes(range=[ymin, ymax], scaleanchor="x", scaleratio=1)

    # Animation speed tweaks
    fig.layout.updatemenus[0].buttons[0].args[1]["frame"]["duration"] = 40
    fig.layout.updatemenus[0].buttons[0].args[1]["transition"]["duration"] = 0

    if args.html:
        fig.write_html(args.html, include_plotlyjs="cdn")
        print(f"Saved: {args.html}")

    fig.show()

if __name__ == "__main__":
    main()
