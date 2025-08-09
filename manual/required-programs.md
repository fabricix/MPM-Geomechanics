# Required Programs {#required_programs}

The following instructions outline the steps required to install all necessary programs to build the MPM-Geomechanics project and execute its test suite.

## Required Programs - Windows

| Program    | Installation                                             | Command                                            |
| ---------- | -------------------------------------------------------- | -------------------------------------------------- |
| **Winget** | [Microsoft's official website][winget_installation_link] | Native of Windows                                  |
| **Git**    | via Winget                                               | winget install -e --id Git.Git -e --source winget  |
| **MSYS2**  | via Winget                                               | winget install -e --id MSYS2.MSYS2 --source winget |
| **CMake**  | via MSYS2 MINGW64                                        | pacman -S mingw-w64-x86_64-cmake                   |
| **GCC**    | via MSYS2 MINGW64                                        | pacman -S mingw-w64-x86_64-gcc                     |
| **G++**    | via MSYS2 MINGW64                                        | pacman -S mingw-w64-x86_64-gcc                     |
| **Make**   | via MSYS2 MINGW64                                        | pacman -S make                                     |

**Make sure you have Winget installed**, you can verify this by running `winget --version`. If you don't have installed **Winget**, you can get it from [Microsoft's official website][winget_installation_link]

1. Install **Git** and **MSYS2** using `winget`, by running the following commands:

```
# Git
winget install -e --id Git.Git -e --source winget

# MSYS2
winget install -e --id MSYS2.MSYS2 --source winget
```

2. Open the `MSYS2 MINGW64` console and execute the following commands:

```
# CMake
pacman -S mingw-w64-x86_64-cmake

# GCC and G++
pacman -S mingw-w64-x86_64-gcc

# Make
pacman -S make
```

Verify the **environment variables** for the directory `C:/msys64/mingw64/bin`. If it does not exist, add it.
Then, in the **MSYS2 MINGW64 console line**, you must verify the versions of each installation via:

```
git --version
cmake --version
make --version 
gcc --version   
g++ --version   
```

## Required Programs - Linux (Ubuntu/Debian)

| Program   | Installation | Command                          |
| --------- | ------------ | -------------------------------- |
| **Git**   | via apt      | sudo apt install git             |
| **CMake** | via apt      | sudo apt install cmake           |
| **GCC**   | via apt      | sudo apt install build-essential |
| **G++**   | via apt      | sudo apt install build-essential |
| **Make**  | via apt      | sudo apt install build-essential |

1. Install all the necessary progrmas using `apt` by running the following commands:

```
# Git
sudo apt install git

# CMake
sudo apt install cmake

# Make, GCC, G++
sudo apt install build-essential
```

2. You must verify the versions of each installation via:

```
git --version
cmake --version
make --version 
gcc --version   
g++ --version   
```