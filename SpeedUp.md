## Measurements
### SpeedUp
- Throughput = (#tasks)/time
- Latency = time/task
- SpeedUp = (sqeuntial execution time) / (parallel execution time with N workers)

### Amdahl's Law
It is a way to estimate how much bang for your buck you will actually get by parallelizing a program.
- Overall SpeedUp = 1 / ((1-P) + P/S)
    - P = Portion of program that is parallelizable
    - S = SpeedUp of the parallelized portion (number of processors)
    - Example:
        - P = 0.95
        - S = 2
        - Overall SpeedUp = 1.9
        - P = 0.95
        - S = 100000
        - Overall SpeedUp = 20


### Efficiency
How weel additional respources are utilized
- Efficiency = SpeedUp / (Number of processors)


## Parallel Design Process Stages

### 1) Decomposition

- Block Decomposition
- Cyclic Decomposition

We have to decompose the problem into several tasks

After doing above we need to: *Stablish Decompision*

## 2) Communication

- Point to Point Communication
- Collective Communication

## 3) Agglomeration

## 4) Mapping