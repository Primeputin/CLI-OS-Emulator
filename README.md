# CLI OS Emulator

CLI OS Emulator has schedulers to simulate running processes. The main entry class file, where the main function is located, is **CLI OS Emulator.cpp**

## Developers

- Laolao, Gian Chill [@Nidokohi](https://github.com/Nidokohi)
- Lim, Andrei Zachary [@LarsBasura](https://github.com/https://github.com/LarsBasura)
- Hokia, Pierce Zachary [@nimbus7462](https://github.com/nimbus7462)
- Tang, Vladimir Casey [@Primeputin](https://github.com/Primeputin)

## [Configuration](#configuration)

To run this project, you will need to add a config.txt file.
Go to the project directory and the configurations like in the following:

```
  num-cpu 4
  scheduler "fcfs"
  quantum-cycles 5
  batch-process-freq 1
  min-ins 1000
  max-ins 2000
  delay-per-exec 33
```

## Configuration Parameters

| Parameter              | Description                                                                                                                                                                                                 |
| ---------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **num-cpu**            | Number of CPUs available. **Range:** [1, 128]                                                                                                                                                               |
| **scheduler**          | The scheduler algorithm to use: - `"fcfs"` (First-Come-First-Served) - `"rr"` (Round Robin)                                                                                                                 |
| **quantum-cycles**     | Time slice given to each process when using Round Robin scheduler. **Range:** [1, 2<sup>22</sup>] **Note:** Has no effect on other schedulers.                                                              |
| **batch-process-freq** | Frequency of process generation in "scheduler-test" command (in CPU cycles). **Range:** [1, 2<sup>32</sup>] **Note:** If set to 1, a new process is generated at the end of each CPU cycle.                 |
| **min-ins**            | Minimum number of instructions/commands per process. **Range:** [1, 2<sup>32</sup>]                                                                                                                         |
| **max-ins**            | Maximum number of instructions/commands per process. **Range:** [1, 2<sup>32</sup>]                                                                                                                         |
| **delays-per-exec**    | Delay before executing next instruction (in CPU cycles). Uses busy-waiting (process remains in CPU during delay). **Range:** [0, 2<sup>32</sup> **Note:** If 0, each instruction executes in one CPU cycle. |

## Steps to run

1. clone the respository

```bash
git clone https://github.com/Primeputin/CLI-OS-Emulator.git
```

2. Setup the config.txt as stated in the [configuration section](#configuration).
3. In VS 2022 IDE, click "Local Windows Debugger" at the top part section to compile and run.
