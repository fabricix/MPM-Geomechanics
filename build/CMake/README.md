## Compilation with Cmake

### 1. Windows

#### 1.1. Required Programs

| Program          | Installation |
| ---------------- | ------------ |
| **CMake**        | via Winget   |
| **MinGW32-make** | via MSYS2    |
| **GCC**          | via MSYS2    |
| **G++**          | via MSYS2    |

#### 1.2. Compilation with Windows

```
cd build
cd cmake
cmake -G "MinGW Makefiles" .
cmake --build .
```

### 2. linux

#### 2.1. Required Programs

| Program   | Installation |
| --------- | ------------ |
| **CMake** | via apt      |
| **Make**  | via apt      |
| **GCC**   | via apt      |
| **G++**   | via apt      |

#### 1.2. Compilation with Linux

```
cd build
cd cmake
cmake -G "Unix Makefiles" .
cmake --build .
```
