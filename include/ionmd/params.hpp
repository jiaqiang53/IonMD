#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <sstream>
#include <memory>
#include <json.hpp>
#include "util.hpp"


namespace ionmd {

/**
 * Container structure for all parameters of a simulation.
 */
struct SimParams {
    /// Time step
    double dt = 10e-6;

    /// Total number of time steps.
    unsigned int num_steps = 20000;

    /// Verbosity of output. Higher numbers increases verbosity.
    unsigned int verbosity = 0;

    /// Enable secular force (only disable for debugging!)
    bool secular_enabled = true;

    /// Enable micromotion calculations (requires smaller time steps)
    bool micromotion_enabled = false;

    /// Enable Coulomb repulsion calculation
    bool coulomb_enabled = true;

    /// Enable stochastic force calculation
    bool stochastic_enabled = false;

    /// Enable Doppler cooling simulation
    bool doppler_enabled = false;

    /// Directory to write data to
    std::string path = "output";

    /// How many points in time to store before writing to disk.
    // FIXME: enforce being a divisor of num_steps
    size_t buffer_size = 5000;

    auto to_string() const -> std::string
    {
        std::stringstream stream;
        stream << "Simulation parameters:\n"
               << "  dt = " << dt << "\n"
               << "  num_steps = " << num_steps << "\n"
               << "  secular: " << secular_enabled << "\n"
               << "  micromotion: " << micromotion_enabled << "\n"
               << "  coulomb: " << coulomb_enabled << "\n"
               << "  stochastic: " << stochastic_enabled << "\n"
               << "  doppler: " << doppler_enabled << "\n"
               << "  path: " << path << "\n"
               << "  buffer_size: " << buffer_size << "\n";
        return stream.str();
    }

    auto to_json() const -> std::string
    {
        using nlohmann::json;
        json j = {
            {"dt", dt},
            {"num_steps", num_steps},
            {"verbosity", verbosity},
            {"secular_enabled", secular_enabled},
            {"micromotion_enabled", micromotion_enabled},
            {"coulomb_enabled", coulomb_enabled},
            {"stochastic_enabled", stochastic_enabled},
            {"doppler_enabled", doppler_enabled},
            {"buffer_size", buffer_size}
        };

        return j.dump(2);
    }
};

typedef std::shared_ptr<SimParams> params_ptr;

}

#endif
