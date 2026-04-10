#include "models/bb_model.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace models {

static std::uint64_t make_seed_(std::uint64_t seed) {
    if (seed != 0) return seed;
    std::random_device rd;
    return (static_cast<std::uint64_t>(rd()) << 32) ^ static_cast<std::uint64_t>(rd());
}

BBModel::BBModel(BBParams p, std::uint64_t seed)
    : P_(p),
      gen_(make_seed_(seed)),
      uni01_(0.0, 1.0),
      gauss_step_(0.0, std::sqrt(2.0 * P_.D * P_.dt)),
      gauss_newborn_(0.0, P_.newborn_sigma) {

    if (P_.dt <= 0 || P_.steps <= 0) throw std::runtime_error("Invalid dt/steps");
}

double BBModel::reflect_(double v, double vmin, double vmax) {
    if (v < vmin) return vmin + (vmin - v);
    if (v > vmax) return vmax - (v - vmax);
    return v;
}

void BBModel::initialize() {
    bugs_.clear();
    events_.clear();
    next_id_ = 0;

    // Init uniformly in the box
    std::uniform_real_distribution<double> initx(P_.xmin, P_.xmax);
    std::uniform_real_distribution<double> inity(P_.ymin, P_.ymax);

    bugs_.reserve(std::max<std::size_t>(1024, static_cast<std::size_t>(P_.N0 * 2)));
    for (int i = 0; i < P_.N0; ++i) {
        Bug b;
        b.id = next_id_++;
        b.x = initx(gen_);
        b.y = inity(gen_);
        b.alive = true;
        bugs_.push_back(b);
    }
}

void BBModel::snapshot_(int t) {
    if (P_.snapshot_every <= 0) return;
    if (t % P_.snapshot_every != 0) return;

    for (const auto& b : bugs_) {
        if (!b.alive) continue;
        events_.push_back(io::Event{t, b.id, b.x, b.y, io::EventState::ALIVE, -1});
    }
}

void BBModel::run() {
    const double p_birth = std::min(1.0, P_.lambda * P_.dt);
    const double p_death = std::min(1.0, P_.mu * P_.dt);

    for (int t = 0; t <= P_.steps; ++t) {
        snapshot_(t);
        if (t == P_.steps) break;

        if (t % 10 == 0) std::cerr << "t=" << t << " N=" << bugs_.size() << "\n";
        if (bugs_.size() > P_.max_bugs - 10) std::cerr << "NEAR MAX\n";

        std::vector<Bug> newborns;
        newborns.reserve(256);

        for (auto& b : bugs_) {
            if (!b.alive) continue;

            // Brownian step
            b.x += gauss_step_(gen_);
            b.y += gauss_step_(gen_);

            if (P_.reflecting) {
                b.x = reflect_(b.x, P_.xmin, P_.xmax);
                b.y = reflect_(b.y, P_.ymin, P_.ymax);
            }

            // Death
            if (uni01_(gen_) < p_death) {
                b.alive = false;
                events_.push_back(io::Event{t + 1, b.id, b.x, b.y, io::EventState::DEAD, -1});
                continue;
            }

            // Birth
            if (uni01_(gen_) < p_birth) {
                if (bugs_.size() + newborns.size() < P_.max_bugs) {
                    Bug c;
                    c.id = next_id_++;
                    c.x = b.x + gauss_newborn_(gen_);
                    c.y = b.y + gauss_newborn_(gen_);
                    c.alive = true;

                    if (P_.reflecting) {
                        c.x = reflect_(c.x, P_.xmin, P_.xmax);
                        c.y = reflect_(c.y, P_.ymin, P_.ymax);
                    }

                    newborns.push_back(c);
                    events_.push_back(io::Event{t + 1, c.id, c.x, c.y, io::EventState::BORN, b.id});
                }
            }
        }

        bugs_.insert(bugs_.end(), newborns.begin(), newborns.end());
    }
}

}
