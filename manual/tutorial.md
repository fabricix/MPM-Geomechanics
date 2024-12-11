\page program_tutorial_manual Tutorial manual

## Base acceleration example

In this example we will show how to model the movement of an elastic body subjected to a dynamic boundary condition. The geometry of the body will be a regular cuboid with edge dimension \f$ l_x = 0.3\f$ m, \f$ l_y = 0.3 \f$ m, and \f$ l_z = 0.8 \f$ m in the \f$ xyz \f$ coordinate system. The lower coordinate point is located at \f$ p_{min} = (0.4, 0.4, 0.0) \f$ m. The dynamic boundary condition considered is a base acceleration defined as a function of time as \f$ \ddot{u} = A 2 \pi f cos ( 2 \pi f t + \alpha ) \f$. The total time of the simulation is 2 seconds, and the time step is \f$ \Delta t = 1e-4\f$ seconds. The material density considered is \f$ \rho = 2500 \f$ kg/m\f$^3\f$, and the elastic behavior is defined by Young's modulus\f$ E = 100e6 \f$ MPa and Poisson ratio equal to \f$ \nu = 0.25 \f$. The figure below shows the body in the space.

!["Geometry of the body"](/manual/images/geometry-body.png)

### MPM-Model

With these data we will to create an MPM model consisting in uniformly distributed particles inside the geometry of the body. For do so, we used the "body" and "cuboid" keywords. When body type is "cuboid" we can create a body with the lower and highest coordinate point P1 (lower) and P2 (hight) of the geometry. The material established using the "material_id" keyword. This material must to be correctly defined using the keyword "materials". When we create a body using uniformly distributed particles, eight in the case of a regular element mesh, the number of total particles in the body depends of the mesh grid cell dimension. In this case we will consider a uniform mesh consisting in elements of dimension \f$ \Delta x = \Delta y = \Delta z = 0.1 \f$ m. In a MPM model the mesh must to cover the whole domain, covering the complete amplitude of the body movement. For this example we will consider the same number of element in each direction \f$ n_x=12, n_y=12, n_z=15 \f$ to account from the origin of coordinate \f$ (0,0,0) \f$. 

The complete JSON file with the keywords we write:

    {
        "body":
        {
            "elastic-cuboid-body":
            {
            "type":"cuboid",
            "id":1,
            "point_p1":[0.4,0.4,0],
            "point_p2":[0.7,0.7,0.8],
            "material_id":1
            }
        },
            "materials":
        {
            "material-1":
            {
            "type":"elastic",
            "id":1,
            "young":100e6,
            "density":2500,
            "poisson":0.25
            }
        },
        "mesh":
        { 
            "cells_dimension":[0.1,0.1,0.1],
            "cells_number":[12,12,15],
            "origin":[0,0,0],
            "boundary_conditions":
            {
            "plane_X0":"sliding",
            "plane_Y0":"sliding",
            "plane_Z0":"earthquake",
            "plane_Xn":"sliding",
            "plane_Yn":"sliding",
            "plane_Zn":"sliding"
            }
        },
        "time":2,
        "time_step":1e-4,
        "results":
        {
            "print":40,
            "fields":["displacement","velocity"]
        }
    }

When the "earthquake" keyword is defined in any plane in the mesh, the simulator will search for the acceleration record "base_acceleration.csv" in the working directory. This record must to be the data in the following structure: time, acceleration_x, acceleration_y, acceleration_z. The five first lines of the acceleration record is:

    
        t,ax,ay,az
        0.0,-1.8849555921538759,-0.9424777960769379,-0.0
        5e-05,-1.8849554991350466,-0.9424777844495842,-0.0
        0.0001,-1.884955220078568,-0.9424777495675233,-0.0
        0.00015000000000000001,-1.8849547549844674,-0.9424776914307561,-0.0
        ...
    

Note that the base acceleration record must to contain a header, that will be ignored during the reading acceleration data.

Once we have the JSON input file, we can execute the simulator with the input file as parameter. When the simulation ends we can find the particle results in the "/particle" directory, and the grid mesh in the "/grid" directory. The particles results are written in uniformly separated times determined by the number of results defined in the "print" keyword. The number of total results must be 40. In this example, the particles results must be "particle_1.vtu", "particle_2.vtu", ..., "particle_41.vtu". Together with the "particle_i.vtu" results, a particle serie file "particleTimeSerie.pvd" is created. This time series file can be used to load all the results in the Paraview scientific visualization program.

In other to visualize the results, the "particleTimeSerie.pvd" can be open in the Paraview by "> File > Open > particleTimeSerie.pvd". And the mesh can be loaded by open the "> File > Open > eulerianGrid.vtu".

!["Particles and mesh of analyzed case"](/manual/images/mpm-model-particles-and-mesh.png)

In order to validate the implementation of the dynamic boundary condition the velocity calculated from the input function used to created the base acceleration, \f$ \dot{u} = A sin ( 2 \pi f t + \alpha ) \f$ is compared with the velocity of a particle located at the base of the MPM model. The next figure shows this comparison. The velocities obtained with MPM at the base of the models coincides with the velocities obtained from the input record.

!["Verification of velocities obtained with MPM simulation"](/manual/images/velocity-base-verification.png)
