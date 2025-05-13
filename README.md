
# My C Thread Library

This project implements a basic user-level thread library in C, featuring cooperative multitasking and synchronization primitives. It provides an educational comparison between custom threads and POSIX `pthreads` using workloads like matrix multiplication and producer-consumer problems.

---

## 🚀 Features

- Custom thread creation, scheduling, and joining
- Cooperative multitasking via `mythread_yield`
- Spinlock implementation for thread synchronization
- Matrix multiplication with performance benchmarking
- Producer-consumer problem with thread safety
- Performance comparison with `pthread` library using Gnuplot

---

## 📄 Files

- `userthread.c`: Core thread library implementation
- `userthread.h`: Thread library header
- `main.c`: Entrypoint for testing APIs
- `matrix.c`: Matrix multiplication using custom threads
- `pthreadmatrix.c`: Matrix multiplication using pthreads
- `producer_consumer.c`: Producer-consumer implementation
- `plotmatrix.c`: Plotting code for benchmarking
- `plot.gnu`: Gnuplot script to visualize results
- `mythreaddata.txt`: Custom thread timing data
- `pthreaddata.txt`: Pthread timing data
- `test1.c` to `test5.c`: Unit and concurrency tests
- `Makefile`: Build script
- `README.md`: Project documentation

---

## ⚙️ Build Instructions

To build everything:

```bash
make all
```

This compiles the thread library and all binaries. If directories for binaries, libraries, or logs are needed, create them:

```bash
mkdir -p bin lib log
```

---

## 🧪 Running Examples

### Matrix Multiplication

Using custom threads:

```bash
bin/matrix <threads> <matrix_size>
```

Using pthreads:

```bash
bin/pthreadmatrix <threads> <matrix_size>
```

### Producer-Consumer Problem

```bash
bin/producer_consumer
```

### Run All Tests

```bash
make run
```

---

## 📊 Performance Visualization

To compare custom thread vs. pthread performance:

```bash
gnuplot plot.gnu
```

This uses `mythreaddata.txt` and `pthreaddata.txt` for plotting.

---

## 🛠 Makefile Targets

| Target               | Description                           |
|----------------------|---------------------------------------|
| `make all`           | Build everything                      |
| `make run`           | Run all test programs                 |
| `make clean`         | Clean compiled files                  |
| `matrix`             | Run matrix multiplication             |
| `producer_consumer`  | Run producer-consumer problem         |

---

## ⚠️ Notes

- Ensure `mythread_gettid` is correctly defined in `userthread.c` and declared in `userthread.h`.
- Use `-std=gnu99` or later if inline assembly issues occur.
- For errors like `MAP_STACK` or `__jmpbuf`, comment out system-dependent code for portability.

---

## 📜 License

MIT License — free to use, modify, and share.

---

## 👨‍💻 Author

Developed by Gaurav Chaudhari  
M.Tech, Computer Science — IIT Madras

For questions or suggestions, raise an issue or submit a PR!
