<!--
© 2025. Triad National Security, LLC. All rights reserved.
This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
-->

(omega-user-eos)=

# Equation of State (EOS)

The equation of state (EOS) for the ocean describes the relationship between specific volume of seawater (in $\textrm{m}^3/\textrm{kg}$; the reciprocal of density) and temperature (in $^{\circ}\textrm{C}$), salinity (in $\textrm{g/kg}$), and pressure (in $\textrm{dbar}$). Through the hydrostatic balance (which relates density/specific volume gradients to pressure gradients), the equation of state provides a connection between active tracers (temperature and salinity) and the fluid dynamics.

Two choices of EOS are provided by Omega: a linear EOS and a TEOS-10 EOS. The linear EOS simplifies the relationship by excluding the influence of pressure and using constant expansion/contraction coefficients, making the specific volume a simple linear function of temperature and salinity. However, this option is only recommended for simpler idealized test cases as its accuracy is not sufficient for real ocean simulations. The TEOS-10 EOS is a 75-term polynomial expression from [Roquet et al. 2015](https://www.sciencedirect.com/science/article/pii/S1463500315000566) that approximates the [Thermodynamic Equation of Seawater 2010](https://www.teos-10.org/pubs/TEOS-10_Manual.pdf) , but in a less complex and more computationally efficient manner, and is the preferred EOS for real ocean simulations in Omega.

The user-configurable options are: `EosType` (choose either `Linear` or `Teos-10`), as well as the parameters needed for the linear EOS.

```yaml
Eos:
   EosType : teos10
   Linear:
      DRhoDT: -0.2
      DRhoDS: 0.8
      RhoT0S0: 1000.0
```

where `DRhoDT` is the thermal expansion coefficient ($\textrm{kg}/(\textrm{m}^3 \cdot ^{\circ}\textrm{C})$), `DRhoDS` is the saline contraction coefficient ($\textrm{kg}/\textrm{m}^3$), and `RhoT0S0` is the reference density at (T,S)=(0,0) (in $\textrm{kg}/\textrm{m}^3$).

In addition to `SpecVol`, the displaced specific volume `SpecVolDisplaced` is also calculated by the EOS. This calculates the density of a parcel of fluid that is adiabatically displaced by a relative `k` levels, capturing the effects of pressure/depth changes. This is primarily used to calculate quantities for determining the water column stability (i.e. the stratification) and the vertical mixing coefficients (viscosity and diffusivity). Note: when using the linear EOS, `SpecVolDisplaced` will be the same as `SpecVol` since the specific volume calculation is independent of pressure/depth.
