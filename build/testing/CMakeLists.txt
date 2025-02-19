# Especifica la versión mínima de CMake requerida para este proyecto
cmake_minimum_required(VERSION 3.10)

# Define el nombre del proyecto
project(Testing)

include_directories(../../inc)

# Agrega el subdirectorio donde está GoogleTest (embebido en "external/googletest")
# Esto permitirá que CMake lo compile junto con el proyecto
add_subdirectory(../../external/googletest ${CMAKE_BINARY_DIR}/googletest)

# Agrega el directorio de inclusión de GoogleTest para poder usar #include <gtest/gtest.h>
include_directories(../../external/googletest/googletest/include)

# Busca todos los archivos fuente que terminen en ".test.cpp" en el directorio "tests"
file(GLOB_RECURSE TEST_SOURCES "../../tests/*.test.cpp")
# Define un ejecutable llamado "MPM-Test". Añade todos los arrchivos fuente de las pruebas
add_executable(MPM-Test ../../tests/main.test.cpp ${TEST_SOURCES})

# Enlaza el ejecutable con las bibliotecas de GoogleTest (gtest y gtest_main)
# gtest: Biblioteca principal de GoogleTest
# gtest_main: Proporciona una función main() para ejecutar las pruebas
target_link_libraries(MPM-Test gtest gtest_main)