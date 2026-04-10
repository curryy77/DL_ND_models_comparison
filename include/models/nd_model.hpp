#pragma once
#include <vector>
#include <random>
#include <cstdint>
#include "io/csv_writer.hpp"

namespace models {

    struct NDParams {
        double D  = 1e-5; // Diffusion rate
        double dt = 0.2;

        double lambda0 = 1.00; // Birth rate
        double beta0   = 0.625; // Death rate

        double R  = 0.10;
        double Ns = 50.0;
        double alpha = 0.0;

        // Box
        bool periodic = true;
        double L = 1.0;

        // Another option: reflecting box
        bool reflecting = false;
        double xmin = 0.0, xmax = 1.0;
        double ymin = 0.0, ymax = 1.0;

        // Newborn distance from the parent
        double newborn_sigma = 0.0;


        int steps = 1000;

        // How many bugs there will be initially
        int N0 = 100;

        // Limitation for safety
        std::size_t max_bugs = 1000000;

        // Output cadence
        int snapshot_every = 5;
    };

    // Neighborhood-Dependent model
    class NDModel {
    public:
        explicit NDModel(NDParams p, std::uint64_t seed = 0);

        void initialize();
        void run();

        const std::vector<io::Event>& events() const { return events_; }

    private:
        struct Bug {
            long long id = -1;
            double x = 0.0, y = 0.0;
            bool alive = true;
        };

        NDParams P_;
        std::mt19937_64 gen_;
        std::uniform_real_distribution<double> uni01_;
        std::normal_distribution<double> gauss_step_;
        std::normal_distribution<double> gauss_newborn_;

        std::vector<Bug> bugs_;
        std::vector<io::Event> events_;
        long long next_id_ = 0;

        static double reflect_(double v, double vmin, double vmax);
        static double wrap_(double v, double L);
        static double periodic_dx_(double dx, double L);
        static double dist2_periodic_(double x1, double y1, double x2, double y2, double L);

        int count_neighbors_R_(std::size_t i, const std::vector<Bug>& snapshot) const;

        void snapshot_(int t);
    };

}

