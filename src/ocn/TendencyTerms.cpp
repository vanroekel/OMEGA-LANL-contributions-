/*
 * © 2025. Triad National Security, LLC. All rights reserved.
 * This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
 */

//===-- ocn/TendencyTerms.cpp - Tendency Terms ------------------*- C++ -*-===//
//
// The tendency terms that update state variables are implemented as functors,
// i.e. as classes that act like functions. This source defines the class
// constructors for these functors, which initialize the functor objects using
// the Mesh objects and info from the Config. The function call operators () are
// defined in the corresponding header file.
//
//===----------------------------------------------------------------------===//

#include "TendencyTerms.h"
#include "AuxiliaryState.h"
#include "DataTypes.h"
#include "HorzMesh.h"
#include "OceanState.h"
#include "Tracers.h"

namespace OMEGA {

ThicknessFluxDivOnCell::ThicknessFluxDivOnCell(const HorzMesh *Mesh)
    : NEdgesOnCell(Mesh->NEdgesOnCell), EdgesOnCell(Mesh->EdgesOnCell),
      DvEdge(Mesh->DvEdge), AreaCell(Mesh->AreaCell),
      EdgeSignOnCell(Mesh->EdgeSignOnCell) {}

PotentialVortHAdvOnEdge::PotentialVortHAdvOnEdge(const HorzMesh *Mesh)
    : NEdgesOnEdge(Mesh->NEdgesOnEdge), EdgesOnEdge(Mesh->EdgesOnEdge),
      WeightsOnEdge(Mesh->WeightsOnEdge), EdgeMask(Mesh->EdgeMask) {}

KEGradOnEdge::KEGradOnEdge(const HorzMesh *Mesh)
    : CellsOnEdge(Mesh->CellsOnEdge), DcEdge(Mesh->DcEdge),
      EdgeMask(Mesh->EdgeMask) {}

SSHGradOnEdge::SSHGradOnEdge(const HorzMesh *Mesh)
    : CellsOnEdge(Mesh->CellsOnEdge), DcEdge(Mesh->DcEdge),
      EdgeMask(Mesh->EdgeMask) {}

VelocityDiffusionOnEdge::VelocityDiffusionOnEdge(const HorzMesh *Mesh)
    : CellsOnEdge(Mesh->CellsOnEdge), VerticesOnEdge(Mesh->VerticesOnEdge),
      DcEdge(Mesh->DcEdge), DvEdge(Mesh->DvEdge),
      MeshScalingDel2(Mesh->MeshScalingDel2), EdgeMask(Mesh->EdgeMask) {}

VelocityHyperDiffOnEdge::VelocityHyperDiffOnEdge(const HorzMesh *Mesh)
    : CellsOnEdge(Mesh->CellsOnEdge), VerticesOnEdge(Mesh->VerticesOnEdge),
      DcEdge(Mesh->DcEdge), DvEdge(Mesh->DvEdge),
      MeshScalingDel4(Mesh->MeshScalingDel4), EdgeMask(Mesh->EdgeMask) {}

WindForcingOnEdge::WindForcingOnEdge(const HorzMesh *Mesh)
    : Enabled(false), EdgeMask(Mesh->EdgeMask) {}

BottomDragOnEdge::BottomDragOnEdge(const HorzMesh *Mesh)
    : Enabled(false), Coeff(0), CellsOnEdge(Mesh->CellsOnEdge),
      NVertLevels(Mesh->NVertLevels), EdgeMask(Mesh->EdgeMask) {}

TracerHorzAdvOnCell::TracerHorzAdvOnCell(const HorzMesh *Mesh)
    : NEdgesOnCell(Mesh->NEdgesOnCell), EdgesOnCell(Mesh->EdgesOnCell),
      CellsOnEdge(Mesh->CellsOnEdge), EdgeSignOnCell(Mesh->EdgeSignOnCell),
      DvEdge(Mesh->DvEdge), AreaCell(Mesh->AreaCell), EdgeMask(Mesh->EdgeMask) {
}

TracerDiffOnCell::TracerDiffOnCell(const HorzMesh *Mesh)
    : NEdgesOnCell(Mesh->NEdgesOnCell), EdgesOnCell(Mesh->EdgesOnCell),
      CellsOnEdge(Mesh->CellsOnEdge), EdgeSignOnCell(Mesh->EdgeSignOnCell),
      DvEdge(Mesh->DvEdge), DcEdge(Mesh->DcEdge), AreaCell(Mesh->AreaCell),
      MeshScalingDel2(Mesh->MeshScalingDel2), EdgeMask(Mesh->EdgeMask) {}

TracerHyperDiffOnCell::TracerHyperDiffOnCell(const HorzMesh *Mesh)
    : NEdgesOnCell(Mesh->NEdgesOnCell), EdgesOnCell(Mesh->EdgesOnCell),
      CellsOnEdge(Mesh->CellsOnEdge), EdgeSignOnCell(Mesh->EdgeSignOnCell),
      DvEdge(Mesh->DvEdge), DcEdge(Mesh->DcEdge), AreaCell(Mesh->AreaCell),
      MeshScalingDel4(Mesh->MeshScalingDel4), EdgeMask(Mesh->EdgeMask) {}

} // end namespace OMEGA

//===----------------------------------------------------------------------===//
