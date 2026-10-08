# Chat

A terminal chat application written in C++17. It consists of two separate programs: a **server** and a **client**.

## Requirements

- A C++17 compiler (GCC, Clang)
- CMake 3.16 or newer
- Make (or any other generator CMake supports)
- A POSIX system with pthreads (Linux, macOS)

## Project structure

```
.
├── CMakeLists.txt
├── h/                  Headers
│   ├── ChatInput.hpp
│   ├── ChatUI.hpp
│   ├── cpptui.hpp
│   ├── Message.hpp
│   ├── Socket.hpp
│   └── Time.hpp
├── src/                Sources
│   ├── ChatInput.cpp
│   ├── ChatUI.cpp
│   ├── Socket.cpp
│   ├── Time.cpp
│   ├── server.cpp      Entry point: server
│   └── client.cpp      Entry point: client
└── build/              Build output (generated)
```

## Building

From the project root:

```sh
mkdir -p build
cd build
cmake ..
```

Then build whichever program you need. Each one is built by its own command:

```sh
make server
make client
```

Add `-j` for a parallel build, e.g. `make -j server`.

A plain `make` with no target only builds the shared library (`chat_common`), not the programs.

### Build types

The default build type is `Debug`. For an optimized build:

```sh
cmake -DCMAKE_BUILD_TYPE=Release ..
```

You only need to re-run `cmake ..` when you change `CMakeLists.txt` or the build type.

### Alternative: without `cd`

```sh
cmake -S . -B build
cmake --build build --target server -j
cmake --build build --target client -j
```

## Running

The binaries are placed in `build/`.

Start the server first:

```sh
./build/server
```

Then, in another terminal (one per user), start a client:

```sh
./build/client
```

<!-- TODO: document command-line arguments (host, port, username) if the programs take any. -->

## Adding code

- **New source files:** drop a `.cpp` file in `src/` and a header in `h/`. It is added to the shared library automatically on the next `make <target>`; no changes to `CMakeLists.txt` are needed.
- **New program (a file with its own `main()`):** add its path to `APP_SOURCES` in `CMakeLists.txt` and add an `add_executable` / `target_link_libraries` pair next to the existing ones.

## Cleaning

```sh
rm -rf build
```

Or, to keep the directory, delete its contents only:

```sh
cd build && rm -rf ./*
```
