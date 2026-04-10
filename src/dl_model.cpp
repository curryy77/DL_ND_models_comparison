#include "models/dl_model.hpp"
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

DLModel::DLModel(DLParams p, std::uint64_t seed)
    : P_(p),
      gen_(make_seed_(seed)),
      uni01_(0.0, 1.0),
      gauss_move_(0.0, (P_.D > 0 ? std::sqrt(2.0 * P_.D * P_.dt) : 0.0)),
      gauss_disp_(0.0, P_.Rd) {

    if (P_.dt <= 0 || P_.steps <= 0) throw std::runtime_error("Invalid dt/steps");
    if (P_.periodic && P_.L <= 0) throw std::runtime_error("Invalid L");
    if (P_.Rc <= 0 || P_.Rd < 0) throw std::runtime_error("Invalid Rc/Rd");
}

double DLModel::wrap_(double v, double L) {
    v -= L * std::floor(v / L);
    return v;
}

double DLModel::periodic_dx_(double dx, double L) {
    dx -= L * std::round(dx / L);
    return dx;
}

double DLModel::dist2_periodic_(double x1, double y1, double x2, double y2, double L) {
    const double dx = periodic_dx_(x1 - x2, L);
    const double dy = periodic_dx_(y1 - y2, L);
    return dx*dx + dy*dy;
}

double DLModel::gaussian_kernel_(double r2, double sigma) const {
    const double s2 = sigma * sigma;
    return std::exp(-0.5 * r2 / s2);
}

double DLModel::competition_sum_(std::size_t i, const std::vector<Bug>& snapshot) const {
    const auto& bi = snapshot[i];
    if (!bi.alive) return 0.0;

    double sum = 0.0;
    for (std::size_t k = 0; k < snapshot.size(); ++k) {
        if (k == i) continue;
        const auto& bk = snapshot[k];
        if (!bk.alive) continue;

        const double r2 = dist2_periodic_(bi.x, bi.y, bk.x, bk.y, P_.L);
        sum += gaussian_kernel_(r2, P_.Rc);
    }
    return sum;
}

void DLModel::initialize() {
    bugs_.clear();
    events_.clear();
    next_id_ = 0;

    bugs_.reserve(std::max<std::size_t>(1024, static_cast<std::size_t>(P_.N0 * 2)));

    for (int i = 0; i < P_.N0; ++i) {
        Bug b;
        b.id = next_id_++;
        b.x = uni01_(gen_) * P_.L;
        b.y = uni01_(gen_) * P_.L;
        b.alive = true;
        bugs_.push_back(b);
    }
}

void DLModel::snapshot_(int t) {
    if (P_.snapshot_every <= 0) return;
    if (t % P_.snapshot_every != 0) return;

    for (const auto& b : bugs_) {
        if (!b.alive) continue;
        events_.push_back(io::Event{t, b.id, b.x, b.y, io::EventState::ALIVE, -1});
    }
}

void DLModel::run() {
    for (int t = 0; t <= P_.steps; ++t) {
        snapshot_(t);
        if (t == P_.steps) break;

        if (t % 10 == 0) std::cerr << "t=" << t << " N=" << bugs_.size() << "\n";
        if (bugs_.size() > P_.max_bugs - 10) std::cerr << "NEAR MAX\n";

        const std::vector<Bug> snapshot = bugs_;

        std::vector<Bug> newborns;
        newborns.reserve(256);

        for (std::size_t i = 0; i < bugs_.size(); ++i) {
            auto& b = bugs_[i];
            if (!b.alive) continue;

            // Optional adult movement (often false for plants)
            if (P_.move_adults && P_.D > 0) {
                b.x += gauss_move_(gen_);
                b.y += gauss_move_(gen_);
                b.x = wrap_(b.x, P_.L);
                b.y = wrap_(b.y, P_.L);
            }

            const double C = competition_sum_(i, snapshot);
            const double death_rate = P_.d0 + P_.comp_strength * C;

            double p_death = std::min(1.0, death_rate * P_.dt);
            double p_birth = std::min(1.0, P_.b0 * P_.dt);

            // Death first
            if (uni01_(gen_) < p_death) {
                b.alive = false;
                events_.push_back(io::Event{t + 1, b.id, b.x, b.y, io::EventState::DEAD, -1});
                continue;
            }

            if (uni01_(gen_) < p_birth) {
                if (bugs_.size() + newborns.size() < P_.max_bugs) {
                    Bug c;
                    c.id = next_id_++;
                    c.x = b.x + gauss_disp_(gen_);
                    c.y = b.y + gauss_disp_(gen_);
                    c.alive = true;

                    c.x = wrap_(c.x, P_.L);
                    c.y = wrap_(c.y, P_.L);

                    newborns.push_back(c);
                    events_.push_back(io::Event{t + 1, c.id, c.x, c.y, io::EventState::BORN, b.id});
                }
            }
        }

        bugs_.insert(bugs_.end(), newborns.begin(), newborns.end());
    }
}

}
