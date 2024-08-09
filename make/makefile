# compiler
CC = g++

# compiler flags
#  -g : adds debugging information to the executable file
#  -Wall : turn on compiler warnings
# -fopenmp: openmp for share parallel computing

# Debug flags
# CFLAGS = -Wall -g

# Release flags
CFLAGS = -fopenmp

# include directory
INC=-I../inc

# dependencies
objects = BodyCuboid.o \
	BodyParticle.o \
	BodyPolygon.o \
	Contact.o \
	DynamicRelaxation.o \
	Elastic.o \
	ElasticJaumann.o \
	Geometry.o \
	Input.o \
	Integration.o \
	Interpolation.o \
	Loads.o \
	Main.o \
	Mesh.o \
	Model.o \
	MohrCoulomb.o \
	MPM.o \
	Node.o \
	NodeMixture.o \
	Output.o \
	Particle.o \
	ParticleMixture.o \
	ShapeGimp.o \
	Softening.o \
	ShapeLinear.o \
	SolverExplicitTwoPhaseUSL.o \
	SolverExplicitUSL.o \
	States.o \
	Update.o \
	Warning.o

# make the program
MPM-Geomechanics: $(objects)
	$(CC) $(CFLAGS) $(INC) -o MPM-Geomechanics $(objects)

# compile dependencies
BodyCuboid.o : ../src/BodyCuboid.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/BodyCuboid.cpp	
BodyParticle.o : ../src/BodyParticle.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/BodyParticle.cpp	
BodyPolygon.o : ../src/BodyPolygon.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/BodyPolygon.cpp	
Contact.o : ../src/Contact.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Contact.cpp	
DynamicRelaxation.o : ../src/DynamicRelaxation.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/DynamicRelaxation.cpp	
Elastic.o : ../src/Elastic.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Elastic.cpp	
ElasticJaumann.o : ../src/ElasticJaumann.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/ElasticJaumann.cpp	
Geometry.o : ../src/Geometry.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Geometry.cpp	
Input.o : ../src/Input.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Input.cpp	
Integration.o : ../src/Integration.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Integration.cpp	
Interpolation.o : ../src/Interpolation.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Interpolation.cpp	
Loads.o : ../src/Loads.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Loads.cpp	
Main.o : ../src/Main.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Main.cpp	
Mesh.o : ../src/Mesh.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Mesh.cpp	
Model.o : ../src/Model.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Model.cpp	
MohrCoulomb.o : ../src/MohrCoulomb.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/MohrCoulomb.cpp	
MPM.o : ../src/MPM.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/MPM.cpp	
Node.o : ../src/Node.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Node.cpp	
NodeMixture.o : ../src/NodeMixture.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/NodeMixture.cpp	
Output.o : ../src/Output.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Output.cpp	
Particle.o : ../src/Particle.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Particle.cpp	
ParticleMixture.o : ../src/ParticleMixture.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/ParticleMixture.cpp	
ShapeGimp.o : ../src/ShapeGimp.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/ShapeGimp.cpp	
Softening.o : ../src/Softening.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Softening.cpp	
ShapeLinear.o : ../src/ShapeLinear.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/ShapeLinear.cpp	
SolverExplicitTwoPhaseUSL.o : ../src/SolverExplicitTwoPhaseUSL.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/SolverExplicitTwoPhaseUSL.cpp	
SolverExplicitUSL.o : ../src/SolverExplicitUSL.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/SolverExplicitUSL.cpp	
States.o : ../src/States.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/States.cpp	
Update.o : ../src/Update.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Update.cpp	
Warning.o : ../src/Warning.cpp
	$(CC) $(CFLAGS) $(INC) -c ../src/Warning.cpp	

# compile dependencies
clean :
	rm MPM-Geomechanics $(objects)