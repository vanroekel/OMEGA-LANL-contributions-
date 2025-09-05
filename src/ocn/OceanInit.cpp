//===-- ocn/OceanInit.cpp - Ocean Initialization ----------------*- C++ -*-===//
//
// This file contians ocnInit and associated methods which initialize Omega.
// The ocnInit process reads the config file and uses the config options to
// initialize time management and call all the individual initialization
// routines for each module in Omega.
//
//===----------------------------------------------------------------------===//

#include "AuxiliaryState.h"
#include "Config.h"
#include "DataTypes.h"
#include "Decomp.h"
#include "Error.h"
#include "Field.h"
#include "Halo.h"
#include "HorzMesh.h"
#include "IO.h"
#include "IOStream.h"
#include "Logging.h"
#include "MachEnv.h"
#include "OceanDriver.h"
#include "OceanState.h"
#include "Tendencies.h"
#include "TimeMgr.h"
#include "TimeStepper.h"
#include "Tracers.h"

#include "mpi.h"

namespace OMEGA {

int ocnInit(MPI_Comm Comm ///< [in] ocean MPI communicator
) {

   I4 Err = 0; // return error code

   // Init the default machine environment based on input MPI communicator
   MachEnv::init(Comm);
   MachEnv *DefEnv = MachEnv::getDefault();

   // Initialize Omega logging
   initLogging(DefEnv);

   // Read config file into Config object
   Config("Omega");
   Config::readAll("omega.yml");
   Config *OmegaConfig = Config::getOmegaConfig();

   // initialize remaining Omega modules
   Err = initOmegaModules(Comm);
   if (Err != 0)
      ABORT_ERROR("ocnInit: Error initializing Omega modules");

   return Err;

} // end ocnInit

// Call init routines for remaining Omega modules
int initOmegaModules(MPI_Comm Comm) {

   // error and return codes
   int Err = 0;

   // Initialize the default time stepper (phase 1) that includes the
   // calendar, model clock and start/stop times and alarms
   TimeStepper::init1();
   TimeStepper *DefStepper = TimeStepper::getDefault();
   Clock *ModelClock       = DefStepper->getClock();

   // Initialize IOStreams - this does not yet validate the contents
   // of each file, only creates streams from Config
   IOStream::init(ModelClock);

   Err = IO::init(Comm);
   if (Err != 0) {
      ABORT_ERROR("ocnInit: Error initializing parallel IO");
   }

   Err = Field::init(ModelClock);
   if (Err != 0) {
      ABORT_ERROR("ocnInit: Error initializing Fields");
   }

   Decomp::init();

   Err = Halo::init();
   if (Err != 0) {
      ABORT_ERROR("ocnInit: Error initializing default halo");
   }

   HorzMesh::init();

   // Create the vertical dimension - this will eventually move to
   // a vertical mesh later
   Config *OmegaConfig = Config::getOmegaConfig();
   Config DimConfig("Dimension");
   Error ConfigErr = OmegaConfig->get(DimConfig);
   CHECK_ERROR_ABORT(ConfigErr, "ocnInit: Dimension group not found in Config");

   I4 NVertLevels;
   ConfigErr += DimConfig.get("NVertLevels", NVertLevels);
   CHECK_ERROR_ABORT(ConfigErr,
                     "ocnInit: NVertLevels not found in Dimension Config");

   auto VertDim = OMEGA::Dimension::create("NVertLevels", NVertLevels);

   Tracers::init();
   AuxiliaryState::init();
   Tendencies::init();
   TimeStepper::init2();

   Err = OceanState::init();
   if (Err != 0) {
      ABORT_ERROR("ocnInit: Error initializing default state");
   }

   // Now that all fields have been defined, validate all the streams
   // contents
   bool StreamsValid = IOStream::validateAll();
   if (!StreamsValid) {
      ABORT_ERROR("ocnInit: Error validating IO Streams");
   }

   // Initialize data from Restart or InitialState files
   std::string SimTimeStr          = " "; // create SimulationTime metadata
   std::shared_ptr<Field> SimField = Field::get(SimMeta);
   SimField->addMetadata("SimulationTime", SimTimeStr);
   int Err1 = IOStream::Success;
   int Err2 = IOStream::Success;

   // read from initial state if this is starting a new simulation
   Metadata ReqMeta; // no requested metadata for initial state
   Err1 = IOStream::read("InitialState", ModelClock, ReqMeta);

   // read restart if starting from restart
   SimTimeStr                = " ";
   ReqMeta["SimulationTime"] = SimTimeStr;
   Err2 = IOStream::read("RestartRead", ModelClock, ReqMeta);

   // One of the above two streams must be successful to initialize the
   // state and other fields used in the model
   if (Err1 != IOStream::Success and Err2 != IOStream::Success) {
      ABORT_ERROR("Error initializing ocean variables from input streams");
   }

   // If reading from restart, reset the current time to the input time
   if (SimTimeStr != " ") {
      TimeInstant NewCurrentTime(SimTimeStr);
      ModelClock->setCurrentTime(NewCurrentTime);
   }

   // Update Halos and Host arrays with new state, auxiliary state, and tracer
   // fields

   OceanState *DefState = OceanState::getDefault();
   I4 CurTimeLevel      = 0;
   DefState->exchangeHalo(CurTimeLevel);
   DefState->copyToHost(CurTimeLevel);

   AuxiliaryState *DefAuxState = AuxiliaryState::getDefault();
   DefAuxState->exchangeHalo();

   // Now update tracers - assume using same time level index
   Err = Tracers::exchangeHalo(CurTimeLevel);
   if (Err != 0) {
      ABORT_ERROR("Error updating tracer halo after restart");
   }
   Err = Tracers::copyToHost(CurTimeLevel);
   if (Err != 0) {
      ABORT_ERROR("Error updating tracer device arrays after restart");
   }

   return Err;

} // end initOmegaModules

} // end namespace OMEGA
//===----------------------------------------------------------------------===//
