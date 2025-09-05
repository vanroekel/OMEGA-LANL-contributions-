<!--
© 2025. Triad National Security, LLC. All rights reserved.
This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
-->

(omega-user-error)=

# Error handling

Omega includes an error handling facility that checks for potential errors
and responds to those errors when encountered.
It uses the [Logging](#omega-dev-logging) facility for writing the error
messages so its behavior follows that of Logging. In particular, the error
messages will be output to a single log file (omega.log) unless the option
to write a log file for each MPI task is enabled at build time. Each message
will have the format
```
[*level] [file:line number] error message
```
where level is the severity (warn, error, critical) and file and line number
refer to the source code line where the error occurs. The error facility
sometimes accumulates multiple error messages before aborting. In
these cases, the most recent error in these accumulated errors will be printed
first and the other error messages in the chain will be printed in reverse
order and indented.  For critical errors, a full stack trace is generated
if debug information is available, either with the `-g` compiler
option or a DEBUG build using `-DOMEGA_BUILD_TYPE=Debug` during the Cmake
configuration.
