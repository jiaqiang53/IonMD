#ifndef IONMD_HPP
#define IONMD_HPP

#include <armadillo>
#include "ion.hpp"
#include "trap.hpp"
#include "params.hpp"


namespace ionmd {

using arma::mat;

/**
 * Simulation status enum
 *
 */
enum class SimStatus { IDLE, RUNNING, FINISHED, ERRORED };


/**
 * Class that controls the overall simulation.
 */
class Simulation {
private:
    /// General simulation parameters.
    params_ptr p;

    /// Trap parameters.
    trap_ptr trap;

    /// All ions to simulate.
    std::vector<Ion> ions;

    /**
     * Precomputes all Coulomb interactions between ions that way they can be
     * applied all at once when advancing a time step.
     *
     * TODO: use Boost.compute
     */
    const mat precompute_coulomb();

public:
    /// Simulation status
    SimStatus status;

    Simulation();
    Simulation(SimParams p, Trap trap);
    Simulation(SimParams p, Trap trap, std::vector<Ion> ions);

    /**
     * Return a copy of the parameters. This is useful for creating a modified
     * set of parameters for a next run of the simulation.
     */
    auto get_params() -> SimParams;

    /**
     * Set new simulation parameters. This method will only set parameters when
     * the simulation is not in progress.
     * @param new_params
     */
    void set_params(SimParams new_params);

    /**
     * Return a copy of the trap.
     */
    auto get_trap() -> Trap;

    /**
     * Set new trap parameters. This method will only set parameters when the
     * simulation is not in progress.
     * @param new_trap
     */
    void set_trap(Trap new_trap);

    /**
     * Make an ion with given intial position and zero velocity.
     * @param m Ion mass in amu
     * @param Z Ion charge in units of e
     * @param x0 Initial position
     */
    Ion make_ion(const double &m, const double &Z,
                 const std::vector<double> &x0);

    /**
     * Create an ion with the given initial position and zero velocity and add
     * to the inventory of ions in the simulation.
     * @param m Ion mass in amu
     * @param Z Ion charge in units of e
     * @param x0 Initial position
     */
    void add_ion(const double &m, const double &z, const std::vector<double> &x0);

    /**
     * Set ions. This method will only set parameters when the simulation is not
     * in progress.
     */
    void set_ions(std::vector<Ion> ions);

    /** Run the simulation. This is a blocking function. */
    void run();

    /** Starts the simulation in the background. */
    void start();
};

}

#endif
