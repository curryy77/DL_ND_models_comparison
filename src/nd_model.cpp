#include "models/nd_model.hpp"
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

NDModel::NDModel(NDParams p, std::uint64_t seed)
    : P_(p),
      gen_(make_seed_(seed)),
      uni01_(0.0, 1.0),
      gauss_step_(0.0, std::sqrt(2.0 * P_.D * P_.dt)),
      gauss_newborn_(0.0, P_.newborn_sigma) {

    if (P_.dt <= 0 || P_.steps <= 0) throw std::runtime_error("Invalid dt/steps");
    if (P_.R <= 0 || P_.Ns <= 0) throw std::runtime_error("Invalid R/Ns");
    if (P_.periodic && P_.L <= 0) throw std::runtime_error("Invalid L");
}

double NDModel::reflect_(double v, double vmin, double vmax) {
    if (v < vmin) return vmin + (vmin - v);
    if (v > vmax) return vmax - (v - vmax);
    return v;
}

double NDModel::wrap_(double v, double L) {
    v -= L * std::floor(v / L);
    return v;
}

double NDModel::periodic_dx_(double dx, double L) {
    dx -= L * std::round(dx / L);
    return dx;
}

double NDModel::dist2_periodic_(double x1, double y1, double x2, double y2, double L) {
    const double dx = periodic_dx_(x1 - x2, L);
    const double dy = periodic_dx_(y1 - y2, L);
    return dx*dx + dy*dy;
}

void NDModel::initialize() {
    bugs_.clear();
    events_.clear();
    next_id_ = 0;

    bugs_.reserve(std::max<std::size_t>(1024, static_cast<std::size_t>(P_.N0 * 2)));

    if (P_.periodic) {
        for (int i = 0; i < P_.N0; ++i) {
            Bug b;
            b.id = next_id_++;
            b.x = uni01_(gen_) * P_.L;
            b.y = uni01_(gen_) * P_.L;
            b.alive = true;
            bugs_.push_back(b);
        }
    } else {
        std::uniform_real_distribution<double> initx(P_.xmin, P_.xmax);
        std::uniform_real_distribution<double> inity(P_.ymin, P_.ymax);
        for (int i = 0; i < P_.N0; ++i) {
            Bug b;
            b.id = next_id_++;
            b.x = initx(gen_);
            b.y = inity(gen_);
            b.alive = true;
            bugs_.push_back(b);
        }
    }
}

int NDModel::count_neighbors_R_(std::size_t i, const std::vector<Bug>& snapshot) const {
    const double R2 = P_.R * P_.R;
    const auto& bi = snapshot[i];
    if (!bi.alive) return 0;

    int cnt = 0;
    for (std::size_t k = 0; k < snapshot.size(); ++k) {
        if (k == i) continue;
        const auto& bk = snapshot[k];
        if (!bk.alive) continue;

        double d2 = 0.0;
        if (P_.periodic) d2 = dist2_periodic_(bi.x, bi.y, bk.x, bk.y, P_.L);
        else {
            const double dx = bi.x - bk.x;
            const double dy = bi.y - bk.y;
            d2 = dx*dx + dy*dy;
        }

        if (d2 < R2) cnt++;
    }
    return cnt;
}

void NDModel::snapshot_(int t) {
    if (P_.snapshot_every <= 0) return;
    if (t % P_.snapshot_every != 0) return;

    for (const auto& b : bugs_) {
        if (!b.alive) continue;
        events_.push_back(io::Event{t, b.id, b.x, b.y, io::EventState::ALIVE, -1});
    }
}

void NDModel::run() {
    for (int t = 0; t <= P_.steps; ++t) {
        snapshot_(t);
        if (t == P_.steps) break;

        if (t % 10 == 0) std::cerr << "t=" << t << " N=" << bugs_.size() << "\n";
        if (bugs_.size() > P_.max_bugs - 10) std::cerr << "NEAR MAX\n";


        // Freeze positions to compute neighbors consistently for this step
        const std::vector<Bug> snapshot = bugs_;

        std::vector<int> neigh(bugs_.size(), 0);
        for (std::size_t i = 0; i < bugs_.size(); ++i) {
            if (bugs_[i].alive) neigh[i] = count_neighbors_R_(i, snapshot);
        }

        std::vector<Bug> newborns;
        newborns.reserve(256);

        for (std::size_t i = 0; i < bugs_.size(); ++i) {
            auto& b = bugs_[i];
            if (!b.alive) continue;

            // Brownian move
            b.x += gauss_step_(gen_);
            b.y += gauss_step_(gen_);

            if (P_.periodic) {
                b.x = wrap_(b.x, P_.L);
                b.y = wrap_(b.y, P_.L);
            } else if (P_.reflecting) {
                b.x = reflect_(b.x, P_.xmin, P_.xmax);
                b.y = reflect_(b.y, P_.ymin, P_.ymax);
            }

            const double NjR = static_cast<double>(neigh[i]);
            const double lambda_j = std::max(0.0, P_.lambda0 - (1.0 / P_.Ns) * NjR);
            const double beta_j   = std::max(0.0, P_.beta0   - (P_.alpha / P_.Ns) * NjR);

            double p_birth = std::min(1.0, lambda_j * P_.dt);
            double p_death = std::min(1.0, beta_j   * P_.dt);

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

                    if (P_.periodic) {
                        c.x = wrap_(c.x, P_.L);
                        c.y = wrap_(c.y, P_.L);
                    } else if (P_.reflecting) {
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
