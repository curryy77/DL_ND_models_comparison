#include <iostream>
#include <string>
#include <filesystem>

#include "io/csv_writer.hpp"
#include "models/bb_model.hpp"
#include "models/nd_model.hpp"
#include "models/dl_model.hpp"

static void ensure_output_dir() {
    std::filesystem::create_directories("data/output");
}

// -------- Presets --------

static models::BBParams preset_bb_crit() {
    models::BBParams p;

    p.reflecting = true;
    p.xmin = 0.0; p.xmax = 1.0;
    p.ymin = 0.0; p.ymax = 1.0;

    // Critical: lambda == mu
    p.D = 1e-4;
    p.dt = 0.5;
    p.lambda = 2.1;
    p.mu     = 1.0;

    p.N0 = 200;
    p.steps = 1000;

    p.newborn_sigma = 0.0;
    p.snapshot_every = 5;
    p.max_bugs = 1000000;

    return p;
}

static models::NDParams preset_nd_lattice() {
    models::NDParams p;

    p.periodic = true;
    p.L = 1.0;

    p.R  = 0.1;
    p.Ns = 50.0;
    p.alpha = 0.0;

    p.D  = 1e-5;
    p.dt = 1.0;

    // mu = lambda0 - beta0 = 0.7
    p.lambda0 = 1.0;
    p.beta0   = 0.3;

    p.N0 = 200;
    p.steps = 1000;

    p.newborn_sigma = 0.0;
    p.snapshot_every = 5;
    p.max_bugs = 1000000;

    return p;
}

static models::DLParams preset_dl_random_like() {
    models::DLParams p;

    p.periodic = true;
    p.L = 1.0;
    p.move_adults = false;

    p.b0 = 0.4;
    p.d0 = 0.2;

    p.comp_strength = 0.001;

    p.Rd = 0.12;
    p.Rc = 0.12;

    p.N0 = 200;
    p.dt = 0.1;
    p.steps = 1000;

    p.snapshot_every = 5;
    p.max_bugs = 1000000;

    return p;
}

static models::DLParams preset_dl_cluster() {
    auto p = preset_dl_random_like();
    // Short dispersal, wider competition => clustered
    p.Rd = 0.02;
    p.Rc = 0.12;
    return p;
}

// -------- Main --------

static void print_usage() {
    std::cerr
        << "Usage: ./spatial_models <mode> [seed]\n"
        << "Modes:\n"
        << "  bb           (default BB params)\n"
        << "  nd           (default ND params)\n"
        << "  dl           (default DL params)\n"
        << "  bb_crit      (BB: lambda==mu critical)\n"
        << "  nd_lattice   (ND: clear clustering / lattice)\n"
        << "  dl_random    (DL: wide dispersal & competition)\n"
        << "  dl_cluster   (DL: short dispersal, wide competition)\n"
        << "  dl_regular   (DL: wide dispersal, short competition)\n";
}

int main(int argc, char** argv) {
    ensure_output_dir();

    const std::string mode = (argc >= 2) ? argv[1] : "bb";
    std::uint64_t seed = 0;
    if (argc >= 3) seed = static_cast<std::uint64_t>(std::stoull(argv[2]));

    try {
        if (mode == "bb") {
            models::BBParams p;
            models::BBModel m(p, seed);
            m.initialize();
            m.run();
            io::write_csv("data/output/bb_output.csv", m.events());
            std::cerr << "Wrote data/output/bb_output.csv\n";
        } else if (mode == "nd") {
            models::NDParams p;
            models::NDModel m(p, seed);
            m.initialize();
            m.run();
            io::write_csv("data/output/nd_output.csv", m.events());
            std::cerr << "Wrote data/output/nd_output.csv\n";
        } else if (mode == "dl") {
            models::DLParams p;
            models::DLModel m(p, seed);
            m.initialize();
            m.run();
            io::write_csv("data/output/dl_output.csv", m.events());
            std::cerr << "Wrote data/output/dl_output.csv\n";
        } else if (mode == "bb_crit") {
            auto p = preset_bb_crit();
            models::BBModel m(p, seed);
            m.initialize();
            m.run();
            io::write_csv("data/output/bb_crit.csv", m.events());
            std::cerr << "Wrote data/output/bb_crit.csv\n";
        } else if (mode == "nd_lattice") {
            auto p = preset_nd_lattice();
            models::NDModel m(p, seed);
            m.initialize();
            m.run();
            io::write_csv("data/output/nd_lattice.csv", m.events());
            std::cerr << "Wrote data/output/nd_lattice.csv\n";
        } else if (mode == "dl_random") {
            auto p = preset_dl_random_like();
            models::DLModel m(p, seed);
            m.initialize();
            m.run();
            io::write_csv("data/output/dl_random.csv", m.events());
            std::cerr << "Wrote data/output/dl_random.csv\n";
        } else if (mode == "dl_cluster") {
            auto p = preset_dl_cluster();
            models::DLModel m(p, seed);
            m.initialize();
            m.run();
            io::write_csv("data/output/dl_cluster.csv", m.events());
            std::cerr << "Wrote data/output/dl_cluster.csv\n";
        } else {
            print_usage();
            return 2;
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

