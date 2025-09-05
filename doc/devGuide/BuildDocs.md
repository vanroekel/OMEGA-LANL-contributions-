<!--
© 2025. Triad National Security, LLC. All rights reserved.
This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
-->

(omega-dev-building-docs)=

# Building the Documentation

As long as you have followed the procedure in {ref}`omega-dev-conda-env` for
setting up your conda environment, you will already have the packages available
that you need to build the documentation.

From the root of an Omega branch, run the following script to build the docs:

```bash
cd components/omega/doc
make clean html
```

You can view the documentation by opening `_build/html/index.html` in your
choice of browsers or copy it to a web portal.

If you run into errors or warnings related to changes you have made when you
build the docs, check with the development team for help on fixing them.
