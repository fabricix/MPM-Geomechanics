# compiler
CC = g++

# compiler flags
#  -g : adds debugging information to the executable file
#  -Wall : turn on compiler warnings
# -fopenmp: openmp for share parallel computing

# Debug flags
CFLAGS = -Wall

# Release flags
# CFLAGS = -fopenmp

# include directory
INC=-I C:\Users\fabri\MPM\MPM-Geomechanics\inc

# dependencies
objects = testCreateCuboid.o

# make the program
Test: $(objects)
	$(CC) $(CFLAGS) $(INC) -o Test $(objects)

# compile dependencies
testCreateCuboid.o : C:\Users\fabri\MPM\MPM-Geomechanics\tests\bodies\createBody\testCreateCuboid.cpp
	$(CC) $(CFLAGS) $(INC) -c C:\Users\fabri\MPM\MPM-Geomechanics\tests\bodies\createBody\testCreateCuboid.cpp	

# compile dependencies
clean :
	rm Test $(objects)