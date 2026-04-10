#pragma once
#include <vector>
#include <random>
#include <cstdint>
#include "io/csv_writer.hpp"

namespace models {

    struct BBParams {
        double D = 1.0; // Diffusion rate
        double dt = 0.01;
        double lambda = 1.0;   // Birth rate
        double mu = 1.0;       // Death rate
        int steps = 2000;

        // Box
        bool reflecting = true;
        double xmin = 0.0, xmax = 1.0;
        double ymin = 0.0, ymax = 1.0;

        // Newborn distance from the parent
        double newborn_sigma = 0.02;

        // How many bugs there will be initially
        int N0 = 100;

        // Limitation for safety
        std::size_t max_bugs = 300000;

        // Output cadence
        int snapshot_every = 5;
    };

    // Brownian Bug Model
    class BBModel {
    private:
        struct Bug {
            long long id = -1;
            double x = 0.0, y = 0.0;
            bool alive = true;
        };

        BBParams P_;

        // Random generator
        std::mt19937_64 gen_;

        // Randomizing event
        std::uniform_real_distribution<double> uni01_;

        // Randomizing the step of bug
        std::normal_distribution<double> gauss_step_;

        // Randomizing the newborn distance from parent
        std::normal_distribution<double> gauss_newborn_;

        std::vector<Bug> bugs_;
        std::vector<io::Event> events_;
        long long next_id_ = 0;

        static double reflect_(double v, double vmin, double vmax);
        void snapshot_(int t);

    public:
        explicit BBModel(BBParams p, std::uint64_t seed = 0);

        void initialize();
        void run();

        const std::vector<io::Event>& events() const { return events_; }
    };

}

