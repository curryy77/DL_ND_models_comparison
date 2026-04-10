#pragma once
#include <vector>
#include <random>
#include <cstdint>
#include "io/csv_writer.hpp"

namespace models {

    struct DLParams {
        // If adults will move or not
        bool move_adults = false;

        double D  = 0; // Diffusion rate
        double dt = 0.1;

        double b0 = 0.20;      // Birth rate
        double d0 = 0.20;      // Death rate

        // Competition presets
        double comp_strength = 0.0015;  // Competition effect
        double Rc = 0.12;            // Competition range

        // Newborn distance from the parent
        double Rd = 0.2;

        // Box
        bool periodic = true;
        double L = 1.0;

        // How many bugs there will be initially
        int N0 = 200;

        int steps = 1000;

        // Limitation for safety
        std::size_t max_bugs = 1000000;

        // Output cadence
        int snapshot_every = 5;
    };

    // Dieckmann-Law model
    class DLModel {
    public:
        explicit DLModel(DLParams p, std::uint64_t seed = 0);

        void initialize();
        void run();

        const std::vector<io::Event>& events() const { return events_; }

    private:
        struct Bug {
            long long id = -1;
            double x = 0.0, y = 0.0;
            bool alive = true;
        };

        DLParams P_;

        // Random generator
        std::mt19937_64 gen_;

        // Randomizing event
        std::uniform_real_distribution<double> uni01_;

        // Randomizing the step of bug
        std::normal_distribution<double> gauss_move_;

        // Randomizing the newborn distance from parent
        std::normal_distribution<double> gauss_disp_;

        std::vector<Bug> bugs_;
        std::vector<io::Event> events_;
        long long next_id_ = 0;

        static double wrap_(double v, double L);
        static double periodic_dx_(double dx, double L);
        static double dist2_periodic_(double x1, double y1, double x2, double y2, double L);

        double gaussian_kernel_(double r2, double sigma) const;
        double competition_sum_(std::size_t i, const std::vector<Bug>& snapshot) const;

        void snapshot_(int t);
};

}
