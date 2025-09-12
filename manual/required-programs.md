\page required_programs Required Programs

The following instructions outline the steps required to install all necessary programs to build the MPM-Geomechanics project and execute its test suite.

\section required_programs_windows Required Programs - Windows

| Program        | Installation                                             | Command                                              |
| -------------- | -------------------------------------------------------- | ---------------------------------------------------- |
| **Winget**     | [Microsoft's official website][winget_installation_link] | Native of Windows                                    |
| **Git**        | via Winget                                               | `winget install -e --id Git.Git -e --source winget`  |
| **MSYS2**      | via Winget                                               | `winget install -e --id MSYS2.MSYS2 --source winget` |
| **GitHub cli** | via MSYS2 MINGW64                                        | `pacman -S mingw-w64-x86_64-github-cli`              |
| **CMake**      | via MSYS2 MINGW64                                        | `pacman -S mingw-w64-x86_64-cmake`                   |
| **GCC**        | via MSYS2 MINGW64                                        | `pacman -S mingw-w64-x86_64-gcc`                     |
| **G++**        | via MSYS2 MINGW64                                        | `pacman -S mingw-w64-x86_64-gcc`                     |
| **Make**       | via MSYS2 MINGW64                                        | `pacman -S make`                                     |

**Make sure you have Winget installed**, you can verify this by running `winget --version`. If you don't have installed **Winget**, you can get it from [Microsoft's official website][winget_installation_link]

[winget_installation_link]: https://learn.microsoft.com/en-us/windows/package-manager/winget/

**Step 1**: Install **Git**, **GitHub** and **MSYS2** using `winget`, by running the following commands:

```
winget install -e --id Git.Git -e --source winget
```

```
winget install -e --id MSYS2.MSYS2 --source winget
```

**Step 2**: Open the `MSYS2 MINGW64` console and install **GitHub cli**, **CMake**, **GCC**, **G++** and **Make**:

```
pacman -S mingw-w64-x86_64-github-cli
```

```
pacman -S mingw-w64-x86_64-cmake
```

```
pacman -S mingw-w64-x86_64-gcc
```

```
pacman -S make
```

**Step 3**: Verify the versions of each installation :

```
git --version
gh --version
cmake --version
make --version 
gcc --version   
g++ --version   
```
---

\section required_programs_linux Required Programs - Linux

| Program        | Installation | Command                            |
| -------------- | ------------ | ---------------------------------- |
| **Git**        | via apt      | `sudo apt install git`             |
| **GitHub cli** | via apt      | `sudo apt install gh`              |
| **CMake**      | via apt      | `sudo apt install cmake`           |
| **GCC**        | via apt      | `sudo apt install build-essential` |
| **G++**        | via apt      | `sudo apt install build-essential` |
| **Make**       | via apt      | `sudo apt install build-essential` |

**Step 1**:  Install all the necessary packages referenced above (**Git**, **GitHub cli**, **CMake**, **GCC**, **G++** and **Make**) via `apt`:

```
sudo apt install git
```

```
sudo apt install gh
```

```
sudo apt install cmake
```

```
sudo apt install build-essential
```

**Step 2**: Verify the versions of each installation:

```
git --version
gh --version
cmake --version
make --version 
gcc --version   
g++ --version   
```