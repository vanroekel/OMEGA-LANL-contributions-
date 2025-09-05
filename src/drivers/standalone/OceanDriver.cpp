/*
 * © 2025. Triad National Security, LLC. All rights reserved.
 * This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
 */

//===-- drivers/standalone/OceanDriver.cpp - Standalone driver --*- C++ -*-===//
//
//
//===----------------------------------------------------------------------===//

#include "OceanDriver.h"
#include "DataTypes.h"
#include "OceanState.h"
#include "OmegaKokkos.h"
#include "Pacer.h"
#include "TimeMgr.h"
#include "TimeStepper.h"
#include <mpi.h>

#include <iostream>

int main(int argc, char **argv) {

   int ErrAll;
   int ErrCurr;
   int ErrFinalize;

   MPI_Init(&argc, &argv); // initialize MPI
   Kokkos::initialize();   // initialize Kokkos
   Pacer::initialize(MPI_COMM_WORLD);
   Pacer::setPrefix("Omega:");

   Pacer::start("Init");
   ErrCurr = OMEGA::ocnInit(MPI_COMM_WORLD);
   if (ErrCurr != 0)
      LOG_ERROR("Error initializing OMEGA");
   Pacer::stop("Init");

   // Get time information
   OMEGA::TimeStepper *DefStepper = OMEGA::TimeStepper::getDefault();
   OMEGA::Alarm *EndAlarm         = DefStepper->getEndAlarm();
   OMEGA::Clock *ModelClock       = DefStepper->getClock();
   OMEGA::TimeInstant CurrTime    = ModelClock->getCurrentTime();

   Pacer::start("RunLoop");
   while (ErrCurr == 0 && !(EndAlarm->isRinging())) {

      ErrCurr = OMEGA::ocnRun(CurrTime);

      if (ErrCurr != 0)
         LOG_ERROR("Error advancing Omega run interval");
   }
   Pacer::stop("RunLoop");

   Pacer::start("Finalize");
   ErrFinalize = OMEGA::ocnFinalize(CurrTime);
   if (ErrFinalize != 0)
      LOG_ERROR("Error finalizing OMEGA");
   Pacer::stop("Finalize");

   ErrAll = abs(ErrCurr) + abs(ErrFinalize);
   if (ErrAll == 0) {
      LOG_INFO("OMEGA successfully completed");
   } else {
      LOG_ERROR("OMEGA terminating due to error");
   }

   Pacer::print("omega");
   Pacer::finalize();

   Kokkos::finalize();
   MPI_Finalize();

   if (ErrAll >= 256)
      ErrAll = 255;
   return ErrAll;
}

//===----------------------------------------------------------------------===//
