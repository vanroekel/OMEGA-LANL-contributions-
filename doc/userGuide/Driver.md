<!--
© 2025. Triad National Security, LLC. All rights reserved.
This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
-->

(omega-user-driver)=

## Driver

The Omega Driver manages the execution of the Omega ocean model. Currently,
the only user-configurable parameters unique to the Driver relate to the
time management of the simulation.
```yaml
   TimeManagement:
      StartTime: 0001-01-01_00:00:00
      StopTime: none
      RunDuration: 0000_02:00:00.000
      CalendarType: No Leap
```
The `StartTime` and `StopTime` should be formatted strings in the form
`YYYY-MM-DD_HH:MM:SS.SSSS` and the `RunDuration` should be a formatted string in
the form `DDDD_HH:MM:SS.SSSS` (the actual width of each unit can be arbitrary
and the separators can be any single non-numeric character). Either the
`StopTime` or `RunDuration` can be set to `none` in order to use the other to
determine the duration of the run. If both are set and `StopTime - StartTime`
is incosistent with `RunDuration`, then `RunDuration` is used for the
simulation. The supported calendar types are:
| Config option name | Calendar type |
| ------------------ | ------------- |
| Gregorian | Gregorian calendar |
| No Leap | Gregorian without leap years |
| Julian | Julian calendar |
| Julian Day | Julian day |
| Modified Julian Day | modified Julian day |
| 360 Day | 12 months, 30 days each |
| Custom | user-defined calendar |
| No Calendar | tracks elapsed time only |
