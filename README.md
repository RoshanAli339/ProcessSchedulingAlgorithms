# Process Scheduling Algorithms

The operating systems we use implement various process scheduling algorithms for performing the operations we are experiencing through the OS.

This project is made to implement and understand those process scheduling algorithms.

The following algorithms have been implmeneted:

- First Come First Servce (FCS)
- Round Robin (RR)
- Shortest Processes Next (SPN)
- Shortest Remaining Time Next (SRT)
- Highest Response Ratio Next (HRRN)
- Feedback-Multiqueue (FB1)
- Feedback with varying time quantum (FB2)
- Aging(AGING)

## Installatoin

---

1 - Clone this repository

2 - Install g++ compiler and make
For Linux:

```bash
sudo apt install g++ make
```

For MacOS:

```zsh
brew install g++ make
```

3 - Compile the code using the `make` command

4 - Run the executable file `my_program`

## Input format

---

- Line 1 : `trace` or `stats`
  trace produces a Gantt Chart of the scheduling process
  stats produces the necessary statistics such as Arrival Time, Turn Around Time etc.

- Line 2 : a comma separated list telling which CPU Scheduling algorithm to be analyzed or visualized along with
  paraemeters, if applicable. Each algorithm is represented by a short ID listed in paranthesis at the
  introduction.
  Round robin and Agin algorithms have a time quantum parameter to be specified. Therefore, while using
  those algorithms specify the time quantum as follows: RR-q, AGING-q where q is the time quantum.
- Line 3 : An integer specifying the last instant to be used in your simulation and to be shown on the timeline.
- Line 4 : An integer specifying the number of processes to be simulated.
- Line 5 : Start of description of processes. Each process is to be described on a separate line. For algorithms 1 through 7, each process is described using a comma-separated list specifying:

  1- String specifying a process name\
   2- Arrival Time\
   3- Service Time

- **Note:** For Aging algorithm (algorithm 8), each process is described using a comma-separated list specifying:

  1- String specifying a process name\
   2- Arrival Time\
   3- Priority

- Processes are assumed to be sorted based on the arrival time. If two processes have the same arrival time, then the one with the lower priority is assumed to arrive first.

### Example

---

- Input 1:

```
Operation : trace
Algorithms : FCS,RR-3
Last instant : 20
Process count : 5
Processes :
Process 1 : A,0,3
Process 2 : B,2,6
Process 3 : C,4,4
Process 4 : D,6,5
Process 5 : E,8,2
```

- Output 1:

```
FCFS  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
------------------------------------------------
A     |*|*|*| | | | | | | | | | | | | | | | | |
B     | | |.|*|*|*|*|*|*| | | | | | | | | | | |
C     | | | | |.|.|.|.|.|*|*|*|*| | | | | | | |
D     | | | | | | |.|.|.|.|.|.|.|*|*|*|*|*| | |
E     | | | | | | | | |.|.|.|.|.|.|.|.|.|.|*|*|
------------------------------------------------

RR-3  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
------------------------------------------------
A     |*|*|*| | | | | | | | | | | | | | | | | |
B     | | |.|*|*|*|.|.|.|.|.|.|*|*|*| | | | | |
C     | | | | |.|.|*|*|*|.|.|.|.|.|.|.|.|*| | |
D     | | | | | | |.|.|.|*|*|*|.|.|.|.|.|.|*|*|
E     | | | | | | | | |.|.|.|.|.|.|.|*|*| | | |
------------------------------------------------
```

- Input 2:

```
Operation : stats
Algorithms : SRT,FB2
Last instant : 20
Process count : 5
Processes :
Process 1 : A,0,3
Process 2 : B,2,6
Process 3 : C,4,4
Process 4 : D,6,5
Process 5 : E,8,2
```

- Output 2:

```
SRT, Time Quantum : 1
Process    |  A  |  B  |  C  |  D  |  E  |
Arrival    |  0  |  2  |  4  |  6  |  8  |
Service    |  3  |  6  |  4  |  5  |  2  | Mean|
Finish     |  3  | 15  |  8  | 20  | 10  |-----|
Turnaround |  3  | 13  |  4  | 14  |  2  | 7.20|
NormTurn   | 1.00| 2.17| 1.00| 2.80| 1.00| 1.59|

FB2, Time Quantum : 1
Process    |  A  |  B  |  C  |  D  |  E  |
Arrival    |  0  |  2  |  4  |  6  |  8  |
Service    |  3  |  6  |  4  |  5  |  2  | Mean|
Finish     |  4  | 17  | 18  | 20  | 14  |-----|
Turnaround |  4  | 15  | 14  | 14  |  6  |10.60|
NormTurn   | 1.33| 2.50| 3.50| 2.80| 3.00| 2.63|
```
