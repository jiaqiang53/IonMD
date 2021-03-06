#include <iostream>
#include <cmath>
#include <vector>
#include <array>
#include <thread>
#include <fstream>

#include <ionmd/simulation.hpp>
#include <ionmd/data.hpp>
#include <ionmd/util.hpp>

namespace ionmd {

using arma::vec;
using arma::mat;


Simulation::Simulation()
{
    auto default_params = SimParams();
    auto default_trap = Trap();
    this->set_params(default_params);
    this->set_trap(default_trap);
    status = SimStatus::IDLE;
}


Simulation::Simulation(SimParams p, Trap trap)
    : Simulation()
{
    this->p = std::make_shared<SimParams>(p);
    this->trap = std::make_shared<Trap>(trap);
}


Simulation::Simulation(SimParams p, Trap trap, std::vector<Ion> ions)
    : Simulation(p, trap)
{
    for (auto &ion: ions) {
        this->ions.push_back(ion);
    }
    // BOOST_LOG_TRIVIAL(debug) << "Number of ions: " << this->ions.size();
}


const mat Simulation::precompute_coulomb()
{
    mat Flist(3, ions.size());

    #pragma omp parallel for
    for (unsigned int i = 0; i < ions.size(); i++)
    {
        auto F = ions[i].coulomb(ions);
        Flist.col(i) = F;
    }
    return Flist;
}


auto Simulation::get_params() -> SimParams
{
    return *p.get();
}


void Simulation::set_params(SimParams new_params)
{
    if (status != SimStatus::RUNNING) {
        p = std::make_shared<SimParams>(new_params);
    }
    else {
        // BOOST_LOG_TRIVIAL(error) << "Can't change parameters while simulation is running!";
    }
}


auto Simulation::get_trap() -> Trap
{
    return *trap.get();
}


void Simulation::set_trap(Trap new_trap)
{
    if (status != SimStatus::RUNNING) {
        trap = std::make_shared<Trap>(new_trap);
    }
}


Ion Simulation::make_ion(const double &m, const double &Z,
                         const std::vector<double> &x0)
{
    return Ion(p, trap, m, Z, x0);
}


void Simulation::add_ion(const double &m, const double &z,
                         const std::vector<double> &x0)
{
    if (status != SimStatus::RUNNING) {
        ions.push_back(make_ion(m, z, x0));
    }
}


void Simulation::set_ions(std::vector<Ion> ions)
{
    if (status != SimStatus::RUNNING) {
        this->ions.clear();

        for (auto &ion: ions) {
            this->ions.push_back(ion);
        }
    }
}


void Simulation::run()
{
    if (p == nullptr)
    {
        std::cerr << "No parameters set!" << std::endl;
        status = SimStatus::ERRORED;
        return;
    }
    else if (trap == nullptr)
    {
        std::cerr << "No trap set!" << std::endl;
        status = SimStatus::ERRORED;
        return;
    }
    else if (ions.size() == 0)
    {
        std::cerr << "No ions set!" << std::endl;
        status = SimStatus::ERRORED;
        return;
    }

    // Initialize RNG
    // std::mersenne_twister_engine<double> rng;

    // Storage of pre-computed Coulomb force data
    mat coulomb_forces = arma::zeros<mat>(3, ions.size());

    // Stores every ion's position in one iteration
    vec current_positions(ions.size() * 3);

    // Create output directory and files
    // FIXME: don't always overwrite
    // DataWriter writer(p, trap, ions, true);

    std::ofstream traj_stream("data.out", std::ios::out | std::ios::binary);
    traj_stream << ions.size() << "\n" << p->num_steps << "\n";

    // Run simulation
    // BOOST_LOG_TRIVIAL(info) << "Start simulation: " << timestamp_str() << "\n";
    auto t = double(0);
    status = SimStatus::RUNNING;

    for (unsigned int step = 0; step < p->num_steps; step++)
    {
        // Calculate Coulomb forces
        if (p->coulomb_enabled) {
            coulomb_forces = precompute_coulomb();
        }

        // Update each ion
        // TODO: update to use Boost.compute
        #pragma omp parallel for
        for (unsigned int i = 0; i < ions.size(); i++)
        {
            const auto x = ions[i].update(t, coulomb_forces, i);
            // writer.update_buffer(i, x);
            for (unsigned int j = 0; j < 3; j++) {
                current_positions[3*i + j] = x[j];
            }

            // TODO: Check bounds
        }

        current_positions.save(traj_stream, arma::raw_binary);
        t += p->dt;
    }

    traj_stream.close();

    // trajectories.save(p->filename, arma::raw_binary);
    // trajectories.save(p->filename, arma::csv_ascii);
    status = SimStatus::FINISHED;
}


void Simulation::start()
{
    std::thread([this]() { this->run(); }).detach();
}

}  // namespace ionmd
