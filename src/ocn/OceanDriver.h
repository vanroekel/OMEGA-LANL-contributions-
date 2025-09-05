/*
 * © 2025. Triad National Security, LLC. All rights reserved.
 * This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
 */

#ifndef OMEGA_DRIVER_H
#define OMEGA_DRIVER_H
//===-- ocn/OceanDriver.h ---------------------------------------*- C++ -*-===//
//
/// \file
/// \brief Defines ocean driver methods
///
/// This Header defines methods to drive Omega. These methods are designed to
/// run Omega as either a standalone ocean model or as a component of E3SM.
/// This process is divided into three phases: init, run, and finalize.
//
//===----------------------------------------------------------------------===//

#include "Config.h"
#include "TimeMgr.h"

#include "mpi.h"

namespace OMEGA {

/// Read the config file and call all the inidividual initialization routines
/// for each Omega module
int ocnInit(MPI_Comm Comm);

/// Advance the model from starting from CurrTime until EndAlarm rings
int ocnRun(TimeInstant &CurrTime);

/// Clean up all Omega objects
int ocnFinalize(const TimeInstant &CurrTime);

/// Initialize Omega modules needed to run ocean model
int initOmegaModules(MPI_Comm Comm);

} // end namespace OMEGA

//===----------------------------------------------------------------------===//
#endif
