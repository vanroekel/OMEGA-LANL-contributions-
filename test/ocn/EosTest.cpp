//===-- Test driver for OMEGA GSW-C library -----------------------------*- C++
//-*-===/
//
/// \file
/// \brief Test driver for OMEGA GSW-C external library
///
/// This driver tests that the GSW-C library can be called
/// and returns expected value (as published in Roquet et al 2015)
//
//===-----------------------------------------------------------------------===/

#include "Eos.h"
#include "Config.h"
#include "DataTypes.h"
#include "Decomp.h"
#include "Dimension.h"
#include "IO.h"
#include "Logging.h"
#include "MachEnv.h"
#include "OceanTestCommon.h"
#include "OmegaKokkos.h"
#include "Pacer.h"
#include "mpi.h"

// added for debug
#include "AuxiliaryState.h"
#include "Field.h"
#include "Halo.h"
#include "HorzMesh.h"

#include <gswteos-10.h>

using namespace OMEGA;

/// Test constants and expected values
constexpr int NVertLevels = 60;

/// Published values (TEOS-10 and linear) to test against
const Real TeosExpValue = 0.0009732819628; // Expected value for TEOS-10 eos
const Real LinearExpValue =
    0.0009784735812133072; // Expected value for Linear eos

/// Test input values
double Sa       = 30.0;   // Absolute Salinity in g/kg
double Ct       = 10.0;   // Conservative Temperature in degC
double P        = 1000.0; // Pressure in dbar
const I4 KDisp  = 1;      // Displace parcel to K=1 for TEOS-10 eos
const Real RTol = 1e-10;  // Relative tolerance for isApprox checks

/// The initialization routine for Eos testing. It calls various
/// init routines, including the creation of the default decomposition.
I4 initEosTest(const std::string &mesh) {

   I4 Err = 0;

   /// Initialize the Machine Environment class - this also creates
   /// the default MachEnv. Then retrieve the default environment and
   /// some needed data members.
   MachEnv::init(MPI_COMM_WORLD);
   MachEnv *DefEnv  = MachEnv::getDefault();
   MPI_Comm DefComm = DefEnv->getComm();

   /// Initialize logging
   initLogging(DefEnv);

   /// Open and read config file
   Config("Omega");
   Config::readAll("omega.yml");

   /// Initialize parallel IO
   int IOErr = IO::init(DefComm);
   if (IOErr != 0) {
      Err++;
      LOG_ERROR("EosTest: error initializing parallel IO");
   }

   /// Initialize decomposition
   Decomp::init(mesh);

   /// Initialize mesh
   HorzMesh::init();

   /// Create vertical dimension for test arrays
   const auto &Mesh = HorzMesh::getDefault();
   std::shared_ptr<Dimension> VertDim =
       Dimension::create("NVertLevels", NVertLevels);

   /// Initialize Eos
   Eos::init();

   /// Retrieve Eos
   Eos *DefEos = Eos::getInstance();
   if (DefEos) {
      LOG_INFO("EosTest: Eos retrieval PASS");
   } else {
      Err++;
      LOG_INFO("EosTest: Eos retrieval FAIL");
      return -1;
   }

   return Err;
}

/// Test Linear EOS calculation for all cells/levels
int testEosLinear() {
   int Err          = 0;
   const auto *Mesh = HorzMesh::getDefault();
   /// Get Eos instance to test
   Eos *TestEos       = Eos::getInstance();
   TestEos->EosChoice = EosType::LinearEos;

   /// Create and fill ocean state arrays
   Array2DReal SArray = Array2DReal("SArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal TArray = Array2DReal("TArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal PArray = Array2DReal("PArray", Mesh->NCellsAll, NVertLevels);
   /// Use Kokkos::deep_copy to fill the entire view with the ref value
   deepCopy(SArray, Sa);
   deepCopy(TArray, Ct);
   deepCopy(PArray, P);
   deepCopy(TestEos->SpecVol, 0.0);

   /// Compute specific volume
   TestEos->computeSpecVol(TArray, SArray, PArray);

   /// Check all array values against expected value
   int numMismatches   = 0;
   Array2DReal SpecVol = TestEos->SpecVol;
   parallelReduce(
       "CheckSpecVolMatrix-linear", {Mesh->NCellsAll, NVertLevels},
       KOKKOS_LAMBDA(int i, int j, int &localCount) {
          if (!isApprox(SpecVol(i, j), LinearExpValue, RTol)) {
             localCount++;
          }
       },
       numMismatches);

   auto SpecVolH = createHostMirrorCopy(SpecVol);
   if (numMismatches != 0) {
      Err++;
      LOG_ERROR("EosTest: SpecVol Linear isApprox FAIL, "
                "expected {}, got {} with {} mismatches",
                LinearExpValue, SpecVolH(1, 1), numMismatches);
   }
   if (Err == 0) {
      LOG_INFO("EosTest SpecVolCalc Linear: PASS");
   }

   return Err;
}

/// Test Linear EOS calculation with vertical displacement
int testEosLinearDisplaced() {
   int Err          = 0;
   const auto *Mesh = HorzMesh::getDefault();
   /// Get Eos instance to test
   Eos *TestEos       = Eos::getInstance();
   TestEos->EosChoice = EosType::LinearEos;

   /// Create and fill ocean state arrays
   Array2DReal SArray = Array2DReal("SArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal TArray = Array2DReal("TArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal PArray = Array2DReal("PArray", Mesh->NCellsAll, NVertLevels);
   /// Use Kokkos::deep_copy to fill the entire view with the ref value
   deepCopy(SArray, Sa);
   deepCopy(TArray, Ct);
   deepCopy(PArray, P);
   deepCopy(TestEos->SpecVolDisplaced, 0.0);

   /// Compute displaced specific volume
   TestEos->computeSpecVolDisp(TArray, SArray, PArray, KDisp);

   /// Check all array values against expected value
   int numMismatches            = 0;
   Array2DReal SpecVolDisplaced = TestEos->SpecVolDisplaced;
   parallelReduce(
       "CheckSpecVolDispMatrix-linear", {Mesh->NCellsAll, NVertLevels},
       KOKKOS_LAMBDA(int i, int j, int &localCount) {
          if (!isApprox(SpecVolDisplaced(i, j), LinearExpValue, RTol)) {
             localCount++;
          }
       },
       numMismatches);

   auto SpecVolDisplacedH = createHostMirrorCopy(SpecVolDisplaced);
   if (numMismatches != 0) {
      Err++;
      LOG_ERROR("EosTest: Linear SpecVolDisp isApprox FAIL, "
                "expected {}, got {} with {} mismatches",
                LinearExpValue, SpecVolDisplacedH(1, 1), numMismatches);
   }
   if (Err == 0) {
      LOG_INFO("EosTest SpecVolCalcDisp Linear: PASS");
   }

   return Err;
}

/// Test TEOS-10 EOS calculation for all cells/levels
int testEosTeos10() {
   int Err          = 0;
   const auto *Mesh = HorzMesh::getDefault();
   /// Get Eos instance to test
   Eos *TestEos       = Eos::getInstance();
   TestEos->EosChoice = EosType::Teos10Eos;

   /// Create and fill ocean state arrays
   Array2DReal SArray = Array2DReal("SArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal TArray = Array2DReal("TArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal PArray = Array2DReal("PArray", Mesh->NCellsAll, NVertLevels);
   /// Use Kokkos::deep_copy to fill the entire view with the ref value
   deepCopy(SArray, Sa);
   deepCopy(TArray, Ct);
   deepCopy(PArray, P);
   deepCopy(TestEos->SpecVol, 0.0);

   /// Compute specific volume
   TestEos->computeSpecVol(TArray, SArray, PArray);

   /// Check all array values against expected value
   int numMismatches   = 0;
   Array2DReal SpecVol = TestEos->SpecVol;
   parallelReduce(
       "CheckSpecVolMatrix-Teos", {Mesh->NCellsAll, NVertLevels},
       KOKKOS_LAMBDA(int i, int j, int &localCount) {
          if (!isApprox(SpecVol(i, j), TeosExpValue, RTol)) {
             localCount++;
          }
       },
       numMismatches);

   auto SpecVolH = createHostMirrorCopy(SpecVol);
   if (numMismatches != 0) {
      Err++;
      LOG_ERROR("EosTest: TEOS SpecVol isApprox FAIL, "
                "expected {}, got {} with {} mismatches",
                TeosExpValue, SpecVolH(1, 1), numMismatches);
   }
   if (Err == 0) {
      LOG_INFO("EosTest SpecVolCalc TEOS-10: PASS");
   }

   return Err;
}

/// Test TEOS-10 EOS calculation with vertical displacement
int testEosTeos10Displaced() {
   int Err          = 0;
   const auto *Mesh = HorzMesh::getDefault();
   /// Get Eos instance to test
   Eos *TestEos       = Eos::getInstance();
   TestEos->EosChoice = EosType::Teos10Eos;

   /// Create and fill ocean state arrays
   Array2DReal SArray = Array2DReal("SArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal TArray = Array2DReal("TArray", Mesh->NCellsAll, NVertLevels);
   Array2DReal PArray = Array2DReal("PArray", Mesh->NCellsAll, NVertLevels);
   /// Use Kokkos::deep_copy to fill the entire view with the ref value
   deepCopy(SArray, Sa);
   deepCopy(TArray, Ct);
   deepCopy(PArray, P);
   deepCopy(TestEos->SpecVolDisplaced, 0.0);

   /// Compute displaced specific volume
   TestEos->computeSpecVolDisp(TArray, SArray, PArray, KDisp);

   /// Check all array values against expected value
   int numMismatches            = 0;
   Array2DReal SpecVolDisplaced = TestEos->SpecVolDisplaced;
   parallelReduce(
       "CheckSpecVolDispMatrix-Teos", {Mesh->NCellsAll, NVertLevels},
       KOKKOS_LAMBDA(int i, int j, int &localCount) {
          if (!isApprox(SpecVolDisplaced(i, j), TeosExpValue, RTol)) {
             localCount++;
          }
       },
       numMismatches);

   auto SpecVolDisplacedH = createHostMirrorCopy(SpecVolDisplaced);
   if (numMismatches != 0) {
      Err++;
      LOG_ERROR("EosTest: TEOS SpecVolDisp isApprox FAIL, "
                "expected {}, got {} with {} mismatches",
                TeosExpValue, SpecVolDisplacedH(1, 1), numMismatches);
   }
   if (Err == 0) {
      LOG_INFO("EosTest SpecVolCalcDisp TEOS-10: PASS");
   }

   return Err;
}

/// Finalize and clean up all test infrastructure
void finalizeEosTest() {
   HorzMesh::clear();
   Decomp::clear();
   Field::clear();
   Dimension::clear();
   MachEnv::removeAll();
}

/// Test that the external GSW-C library returns the expected specific volume
int checkValueGswcSpecVol() {
   int Err         = 0;
   const Real RTol = 1e-10;

   /// Get specific volume from GSW-C library
   double SpecVol = gsw_specvol(Sa, Ct, P);
   /// Check the value against the expected TEOS-10 value
   bool Check = isApprox(SpecVol, TeosExpValue, RTol);
   if (!Check) {
      Err++;
      LOG_ERROR(
          "checkValueGswcSpecVol: SpecVol isApprox FAIL, expected {}, got {}",
          TeosExpValue, SpecVol);
   }
   if (Err == 0) {
      LOG_INFO("checkValueGswcSpecVol: PASS");
   }
   return Err;
}

// the main test (all in one to have the same log)
// Single value test:
// --> test calls the external GSW-C library
// and compares the specific volume to the published value
// Full array tests:
// --> one tests the value on a Eos with linear option
// --> next checks the value on a Eos with linear displaced option
// --> next checks the value on a Eos with TEOS-10 option
// --> next checks the value on a Eos with TEOS-10 displaced option
int eosTest(const std::string &MeshFile = "OmegaMesh.nc") {
   int Err = initEosTest(MeshFile);
   if (Err != 0) {
      LOG_CRITICAL("EosTest: Error initializing");
   }
   const auto &Mesh = HorzMesh::getDefault();

   LOG_INFO("Single value checks:");
   Err += checkValueGswcSpecVol();

   LOG_INFO("Full array checks:");
   Err += testEosLinear();
   Err += testEosLinearDisplaced();
   Err += testEosTeos10();
   Err += testEosTeos10Displaced();

   if (Err == 0) {
      LOG_INFO("EosTest: Successful completion");
   }
   finalizeEosTest();

   return Err;
}

// The test driver for Eos testing
int main(int argc, char *argv[]) {

   int RetVal = 0;

   MPI_Init(&argc, &argv);
   Kokkos::initialize(argc, argv);
   Pacer::initialize(MPI_COMM_WORLD);
   Pacer::setPrefix("Omega:");

   RetVal += eosTest();

   Eos::destroyInstance();
   Kokkos::finalize();
   MPI_Finalize();

   if (RetVal >= 256)
      RetVal = 255;

   return RetVal;

} // end of main
//===-----------------------------------------------------------------------===/
