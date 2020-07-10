## Table of Contents

* [Benchmarking](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/benchmark#benchmarking)
  * [Mpi measurements](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/benchmark#mpi-measurements)
  * [Mpi OpenMp measurements](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/benchmark#mpi-openmp-measurements--8-threads-)


## Benchmarking

A benchmark is a test that measures the performance of hardware, software, or computer. These tests can be used to help 
compare how well a product may do against other products. When comparing benchmarks, the higher the value of the results, 
the faster the component, software, or overall computer is.

### Mpi measurements

#### Execution time

 Dmnsn\Pro| 1         | 4         | 9         | 16        | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|-----------|----------|----------|----------|----------|
 840      | 0.752412  | 0.193130  | 0.138504  | 0.082561  | 0.078703 | 0.091018 | 0.091000 | 0.102858 |
 1680     | 3.002919  | 0.755565  | 0.374157  | 0.229508  | 0.168460 | 0.148978 | 0.140478 | 0.140119 |
 3360     | 12.088203 | 3.021306  | 1.380812  | 0.796646  | 0.534765 | 0.405058 | 0.325011 | 0.285601 |
 6720     | 48.171624 | 12.172924 | 5.480808  | 3.134355  | 2.008271 | 1.423471 | 1.063272 | 0.852721 |

#### Speed up

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 3.895884  | 5.432421  | 9.113407 | 9.560144 | 8.266629 | 8.268264 | 7.315056 |
 1680     | 1         | 3.974402  | 8.025826  |13.084158 |17.825710 |20.156796 |21.376436 |21.431204 |
 3360     | 1         | 4.000986  | 8.754416  |15.173871 |22.604702 |29.843142 |37.193214 |42.325497 |
 6720     | 1         | 3.957276  | 8.789146  |15.368911 |23.986616 |33.840958 |45.305080 |56.491661 |

#### Efficiency

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 0.973971  | 0.603602  | 0.569588 | 0.382406 | 0.229629 | 0.168740 | 0.114298 |
 1680     | 1         | 0.993600  | 0.891758  | 0.817760 | 0.713028 | 0.559911 | 0.436254 | 0.334863 |
 3360     | 1         | 1.000247  | 0.972713  | 0.948367 | 0.904188 | 0.828976 | 0.759045 | 0.661336 |
 6720     | 1         | 0.989319  | 0.976572  | 0.960557 | 0.959465 | 0.940027 | 0.924593 | 0.882682 |

![mpi_execution_time](https://user-images.githubusercontent.com/48658768/74972536-94cc5480-542a-11ea-8039-fedb03cd7343.png)
![mpi_speed_up](https://user-images.githubusercontent.com/48658768/74972132-e2948d00-5429-11ea-805f-dd71f4800f39.png)
![mpi_efficiency](https://user-images.githubusercontent.com/48658768/74972139-e3c5ba00-5429-11ea-91e6-e7107bf8bd5d.png)

The ideal value for Speed-up/S(n,p) is p. If S(n,p) = p, then the parallel program with comm_sz = p processes is executed 
p times faster than the serial program. So we can see from our Speed-up table and graph that for small number of Processes 
and big number of Dimension our program has almost serial Speed-up. But, for big number of Processes and small number of 
Dimension our Speed-up is too small. Also, we have very good and decent Speed-up for big number of Processes 
and big number of Dimension.
Εfficiency is the Speed-up / Processes, so if we have a serial Speed-up our parallel Εfficiency will be p/p = 1. 
So we can see from our Εfficiency table and graph that for small number of Processes and big number of Dimension our 
Εfficiency is almost serial Speed-up. But, for big number of Processes and small number of Dimension our Εfficiency 
is too small.

### Mpi OpenMp measurements ( 8-threads )

#### Execution time

 Dmnsn\Pro| 1         | 4         | 9         | 16        | 25        | 36       | 49       | 64       |
----------|-----------|-----------|-----------|-----------|-----------|----------|----------|----------|
 840      | 0.803773  | 0.225603  | 0.140552  | 0.203057  | 0.151474  | 0.650479 | 0.511546 | 0.659165 |
 1680     | 3.201418  | 0.640185  | 0.401929  | 0.268558  | 0.563328  | 0.698334 | 1.330187 | 0.993040 |
 3360     | 12.733003 | 2.032802  | 1.475748  | 0.876268  | 0.763466  | 0.664128 | 0.880283 | 1.209686 |
 6720     | 50.990898 | 7.043701  | 6.439718  | 3.924058  | 2.743390  | 1.910961 | 1.500764 | 1.401790 |

#### Speed up

 Dmnsn\Pro| 1         | 4         | 9         | 16        | 25       | 36        | 49        | 64        |
----------|-----------|-----------|-----------|-----------|----------|-----------|-----------|-----------|
 840      | 1         | 3.562776  | 5.718688  | 3.958361  | 5.306343 | 1.235663  | 1.571262  | 1.219380  |
 1680     | 1         | 5.000770  | 7.965133  | 11.920770 | 5.683044 | 4.584365  | 2.406743  | 3.223856  |
 3360     | 1         | 6.263769  | 8.628169  | 14.530945 |16.677891 | 19.172512 | 14.464669 | 10.525874 |
 6720     | 1         | 7.239219  | 7.918189  | 12.994431 |18.586821 | 26.683380 | 33.976627 | 36.375561 |

#### Efficiency

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 0.890694  | 0.635410  | 0.247398 | 0.212254 | 0.034324 | 0.032067 | 0.019053 |
 1680     | 1         | 1.250193  | 0.885015  | 0.745048 | 0.227322 | 0.127343 | 0.049117 | 0.050373 |
 3360     | 1         | 1.565942  | 0.958685  | 0.908184 | 0.667116 | 0.532570 | 0.295197 | 0.164467 |
 6720     | 1         | 1.809805  | 0.879799  | 0.812152 | 0.743473 | 0.741205 | 0.693401 | 0.568368 |

![mpi_openmp_execution_time](https://user-images.githubusercontent.com/48658768/74972148-e6c0aa80-5429-11ea-8b40-f8f2891ba14d.png)
![mpi_openmp_speed_up](https://user-images.githubusercontent.com/48658768/74972155-e9bb9b00-5429-11ea-901b-67adb6750192.png)
![mpi_openmp_efficiency](https://user-images.githubusercontent.com/48658768/74972161-eb855e80-5429-11ea-9d06-d4ecb903273e.png)

So we can see from our Speed-up table and graph that for four processes we get double Speed-up. Μore generally, 
we can conclude that only for small Processes we have good Speed-up. But, for big number of Processes and big number of 
Dimension our Speed-up is too small.
From our Εfficiency table and graph, we can see that for small number of Processes and big number of Dimension our 
Εfficiency is almost one or even greater than one for four Processes. But, for big number of Processes our Εfficiency 
is too small.