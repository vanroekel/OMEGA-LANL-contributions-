/*
 * © 2025. Triad National Security, LLC. All rights reserved.
 * This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
 */

//===-- Test driver for Omega standalone driver  -----------------*- C++ -*-===/
//
/// \file
/// \brief Test driver for Omega standalone driver
///
/// This driver runs the standalone model to confirm that the initialization
/// phase builds all the necessary modules, the run phase successfully
/// integrates the model a few time steps, and the finalization phase
/// successfully cleans up all objects and exits without error.
///
//
//===-----------------------------------------------------------------------===/

#include "OceanDriver.h"
#include "OmegaKokkos.h"
#include "Pacer.h"
#include "TimeMgr.h"
#include "TimeStepper.h"
#include <mpi.h>

//------------------------------------------------------------------------------
// The test driver for the standalone driver
//
int main(int argc, char *argv[]) {

   OMEGA::I4 ErrAll;
   OMEGA::I4 ErrCurr;
   OMEGA::I4 ErrFinalize;

   MPI_Init(&argc, &argv); // initialize MPI
   Kokkos::initialize();   // initialize Kokkos
   Pacer::initialize(MPI_COMM_WORLD);
   Pacer::setPrefix("Omega:");

   Pacer::start("Init");
   ErrCurr = OMEGA::ocnInit(MPI_COMM_WORLD);
   if (ErrCurr == 0) {
      LOG_INFO("DriverTest: Omega initialize PASS");
   } else {
      LOG_INFO("DriverTest: Omega initialize FAIL");
   }
   Pacer::stop("Init");

   // Time management objects
   OMEGA::TimeStepper *DefStepper = OMEGA::TimeStepper::getDefault();
   OMEGA::Clock *ModelClock       = DefStepper->getClock();
   OMEGA::Alarm *EndAlarm         = DefStepper->getEndAlarm();
   OMEGA::TimeInstant CurrTime    = ModelClock->getCurrentTime();

   Pacer::start("RunLoop");
   if (ErrCurr == 0) {
      ErrCurr = OMEGA::ocnRun(CurrTime);
   }
   if (ErrCurr == 0) {
      LOG_INFO("DriverTest: Omega model run PASS");
   } else {
      LOG_INFO("DriverTest: Omega model run FAIL");
   }
   Pacer::stop("RunLoop");

   Pacer::start("Finalize");
   ErrFinalize = OMEGA::ocnFinalize(CurrTime);
   if (ErrFinalize == 0) {
      LOG_INFO("DriverTest: Omega finalize PASS");
   } else {
      LOG_INFO("DriverTest: Omega finalize FAIL");
   }
   Pacer::stop("Finalize");

   ErrAll = abs(ErrCurr) + abs(ErrFinalize);
   if (ErrAll == 0) {
      LOG_INFO("DriverTest: Successful completion");
   }

   Pacer::print("omega_driver_test");
   Pacer::finalize();

   Kokkos::finalize();
   MPI_Finalize();

   if (ErrAll >= 256)
      ErrAll = 255;
   return ErrAll;

} // end of main
//===-----------------------------------------------------------------------===/
