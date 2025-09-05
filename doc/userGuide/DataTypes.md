<!--
© 2025. Triad National Security, LLC. All rights reserved.
This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
-->

(omega-user-data-types)=

## Data Types and Precision

Omega supports all standard data types and uses some specific defined
types to guarantee a specific level of precision. There is only one
user-configurable option for precision. When a specific floating point
precision is not required, we use a Real data type that is, by default,
double precision (8 bytes/64-bit) but if the code is built with a
`-DOMEGA_SINGLE_PRECISION` (see insert link to build system) preprocessor flag,
the default Real becomes single precision (4-byte/32-bit). Users are
encouraged to use the default double precision unless exploring the
performance or accuracy characteristics of single precision.
