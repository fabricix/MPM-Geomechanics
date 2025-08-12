\page testing_manual Testing Compilation Manual

\section testing_manual_required_programs 1. Required Programs

\note Prior to proceeding with these instructions, please consult the [Required Programs](#required_programs) section.

The tests use **GoogleTest**. It is necessary to import this library by cloning the official repository into the folder `/external`. Each developer must clone this repository independently.

```
cd external
git clone https://github.com/google/googletest.git
```

\section testing_manual_howto 2. How to Compile the Tests

Your directory must have the following structure:

```
mpm-geomechanics/
├─ build/
│  ├─ CMakeFiles/
│  ├─ make/
│  ├─ msbuild/
│  ├─ testing/
│  │  ├─ CMakeLists.txt
│  │  ├─ build/
```

\subsection testing_manual_windows Windows

The simplest way to compile on windows is by using the **`.bash`** file at **`/build/Testing`** with **`MSYS2 MINGW64`** console line, just execute:

```
cd project-directory/build/Testing
./cmake-build-win.bash
```

Alternatively, you can use the following commands:

```
cd project-directory/build/Testing
cmake -G "Unix Makefiles" -B build
cmake --build build
```

These commands will generate a .exe file named `MPM-Test.exe`.

\subsection testing_manual_linux Linux (Ubuntu/Debian)

The simplest way to compile on Linux is by using the `.bash` file at `/build/CMake`, just execute:

```
cd project-directory/build/Testing
./cmake-build-linux.bash
```

Alternatively, you can use the following commands:

```
cd project-directory/build/Testing
cmake -G "Unix Makefiles" -B build
cmake --build build
```

These commands will generate a executable file named `MPM-Test`.

