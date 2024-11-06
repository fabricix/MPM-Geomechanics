///
/// \mainpage MPM-Geomechanics
///
/// \section program_intro Introduction
///
/// The MPM-Geomechanics is a implementation of the Material Point Method (MPM) to simulate large strain geomechanical problems in 3D. The main objective of this program is to provide an open source platform suitable for the study and application of the MPM in various geomechanical problems were materials are subjected to extreme deformation conditions.
///
/// MPM-Geomechanics is a program that allows us to model the behavior of geo-materials, like soil and rock, when these materials are subjected to different initial and boundary conditions. Currently, the geo-materials are present in several areas of the society, like for example, in the slopes and excavation process in mining industry activities, or in the study of risk associated to naturals disasters.
///
/// This program uses the Material Point Method (MPM) to integrate the motion equation of continuum mechanics:
///
/// 
/// \f$ \frac{\partial \sigma_{i j}}{\partial x_j}+\rho b_i=\rho \ddot{u}_i \f$
///
/// that can be expressed discretely, using the nodes of an Eulerian mesh:
/// 
/// \f$ \dot{p}_{i I}=f_{i I}^{i n t}+f_{i I}^{e x t} \f$
/// 
/// where \f$ p_{i I}=\sum_p S_{I p} p_{i p}\f$ is the momentum, \f$ f_{i I}^{int}=-\sum_p \sigma_{i j p} S_{I p, j} V_p\f$ is the internal force, and \f$ f_{i I}^{e x t}=\sum_p m_p S_{I p} b_{i p}+\int_{\Gamma} \mathrm{t}_i N_I\left(x_i\right) d A\f$ is the external force at node \f$ I \f$.
///
/// The function \f$ S_{I p} \f$ and its gradient \f$ S_{I p, j} \f$ are the weighting functions of node \f$ I \f$ evaluated at the position of particle \f$ p \f$, defined by \f$ S_{I p}=\frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p\left(x_{ip}\right) N_I\left(x_{ip}\right) dV \f$ and \f$ S_{I p, j}=\frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p\left(x_{ip}\right) N_{I, j}\left(x_{ip}\right) dV \f$. 
/// 
/// The integration of the weight functions is performed analytically over the particle domain using linear functions for \f$ N_I\left(x_{ip}\right) \f$ and unit step functions for \f$ \chi_p\left(x_{ip}\right) \f$. 
///
/// For more details on the formulation and integration process, refer to: \ref program_theory_manual
///
/// \section program_features Program Features
///
/// The main features of the program are:
///
/// - Three-dimensional 3D formulation
/// - Dynamic formulation
/// - Shared memory parallelization using OpenMP
/// - Many ways to crate bodies (polygons, particle list, pre-defined bodies)
/// - Several constitutive models for soil and rock materials
/// - Softening/hardening models to represent weakness during large deformations 
/// - Coupled fluid-mechanical formulation (*under development*)
///
/// \section program_binaries Program compiled binaries
/// For downloading the compiled binaries
/// 1. Go to the [Actions page](https://github.com/fabricix/MPM-Geomechanics/actions).
/// 2. Select the latest run of the **MSBuild** workflow for Window, or **CI** for Linux.
/// 3. At the bottom, you will find the available artifacts under the **Artifacts** section.
/// 4. Download the `compiled-binaries` artifact to get the compiled code.
///
/// \section program_compilation Compilation
///
/// The best way to use the program is to compile it into your system.
///
/// ## Compilation in windows
/// For compiling the code in windows you can use the Visual Studio solution file `/build/MPM-Geomechanics.sln`, and build it by pressing `Ctr+B`.
///
/// Alternatively you can compile it by using command in a *Developer Command Prompt*: 
/// 
/// ```
/// msbuild MPM-Geomechanics.sln -p:Configuration=Release
/// ```
///
/// ## Compilation in Linux
/// 
/// For compile the code in a linux environment, execute the [make](https://www.gnu.org/software/make/) command into the make folder, in with is located de makefile `MPM-Geomechanics\build\make\makefile`:
///  
/// ```
/// make
/// ```
///
/// \section program_documentation Documentation
/// 
/// The program documentation is generated using [Doxygen](https://www.doxygen.nl/index.html):
///
/// ```
/// doxygen Doxyfile
/// ```
///
/// The HTML generated documentation is located in `/docs/index.html`.
///
/// \section program_execution Execution
///
/// In order to run simulations in several terminal, you can add the compiled code in the system `PATH`. After that, the common use is to call the program with the input file as argument:
///
/// ```
/// MPM-Geomechanics my_model.json
/// ```
///

#ifndef MPM_H_
#define MPM_H_

#include "Mesh/Mesh.h"
#include "Body/Body.h"
#include "Model.h"
#include "Output.h"
#include "Input.h"
#include "States.h"
#include "Solver/Solver.h"

/// \class MPM
/// \brief Components and algorithms of the MPM
/// This class contain the basic objects of a MPM model,
/// and the operations between them.
class MPM {

public:

	/// \brief Default constructor
	///
	MPM(); 
	
	/// \brief Default destructor
	///
	virtual ~MPM();

	/// \brief Read the input file of the model
	/// \param[in] argument_counter Argument counter
	/// \param[in] argument_vector Argument vector
	void readInputFile(int argument_counter, char ** argument_vector);
	
	/// \brief Create the numerical MPM model
	///
	void createModel();
	
	/// \brief Solve the problem in time
	///
	void solve();
	
	/// \brief Finish all program structure
	///
	void end();

	/// \brief Load model state
	///
	void loadState();

	/// \brief Save model state
	///
	void saveState();
private:
	
	Mesh mesh; //!< grid mesh

	vector<Body*> bodies; //!< bodies discretized by material points

	vector<Particle*> particles; //!< material points list

	vector<Material*> materials; //!< bodies discretized by material points
	
	Solver* solver; //!< operation for solve the equations in time

	/// \brief Configure the simulation time
	///
	void setSimulationTime();
	
	/// \brief Configure the solver of the model
	///
	void setSolver();
	
	/// \brief Configure the mesh
	///
	void setupMesh();
	
	/// \brief Configures the material list
	///
	void setupMaterialList();
	
	/// \brief Configure the body list
	///
	void setupBodyList();
	
	/// \brief Create bodies
	///
	void createBodies();
	
	/// \brief Configure the time step 
	///
	void setTimeStep();

	/// \brief Configure the interpolation functions
	///
	void setInterpolationFunctions();
	
	/// \brief Configure particles
	///
	void setupParticles();
	
	/// \brief Configure loads in the model
	///
	void setupLoads();
	
	/// \brief Configure results to be written
	///
	void setupResults();
	
	/// \brief Configure damping forces
	/// 
	void setupDamping();

	/// \brief Configure number of phases in the simulation
	///
	void setNumberPhasesInSimulation();


	/// \brief Configure number of threads
	///
	void setThreads();

	bool getSeismicAnalysis();
	void setSeismicAnalysis(bool);
};

#endif /* MPM_H_ */