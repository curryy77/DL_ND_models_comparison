// Helper script for generating CSV-files that "code" the simulation

#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>

namespace io {

    enum class EventState { ALIVE, DEAD, BORN };

    // Converting EventState to std::string
    inline const char* to_string(EventState s) {
        switch (s) {
            case EventState::ALIVE: return "alive";
            case EventState::DEAD:  return "dead";
            case EventState::BORN:  return "born";
        }
        return "unknown";
    }

    struct Event {
        int t = 0;
        long long id = -1;
        double x = 0.0, y = 0.0;
        EventState state = EventState::ALIVE;
        long long parent_id = -1;   // -1 if none
    };

    // Converting data from Event structures to CSV-format
    inline void write_csv(const std::string& filename, const std::vector<Event>& events) {
        std::ofstream out(filename);
        if (!out) throw std::runtime_error("Failed to open: " + filename);

        out << "t,id,x,y,state,parent_id\n";

        // Leave 6 decimal pieces for coordinates x, y
        out << std::fixed << std::setprecision(6);

        for (const auto& e : events) {
            out << e.t << "," << e.id << "," << e.x << "," << e.y << "," << to_string(e.state) << ",";
            if (e.parent_id >= 0) out << e.parent_id;
            out << "\n";
        }
    }

}
