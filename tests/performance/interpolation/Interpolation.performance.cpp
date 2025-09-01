#include <gtest/gtest.h>
#include <chrono>
#include <random>
#include <vector>

#include "Mesh.h"
#include "Particle.h"
#include "ShapeGimp.h"
#include "Interpolation.h"
#include "Body/BodyParticle.h"
#include "configuration.performance.h"

using namespace std;

TEST(InterpolationPerformance, NodalMass_nParticles)
{

    const int numParticles = Configuration::getNumParticles();
    const int numThreads = Configuration::getNumThreads();
    const Vector3d particleSize = Configuration::getParticleSize();
    const double particleMass = Configuration::getParticleMass();
    const Vector3d cellDimension = Configuration::getCellDimension();
    const Vector3i numCells = Configuration::getNumCells();
    const int randomSeed = Configuration::getRandomSeed();

#ifdef _OPENMP
    std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(numThreads);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#else
    std::cout << "[ INFO ] _OPENMP is NOT defined" << std::endl;
#endif

    // create the mesh
    Mesh mesh;
    mesh.setNumCells(numCells(0), numCells(1), numCells(2));
    mesh.setCellDimension(cellDimension(0), cellDimension(1), cellDimension(2));
    mesh.createGrid();

    // create particles
    vector<Particle*> particles;
    particles.reserve(numParticles);

    std::mt19937 gen(randomSeed);
    std::uniform_real_distribution<double> dist(0.0, cellDimension.maxCoeff());

    std::cout << "[ INFO ] Total particles: " << numParticles << std::endl;

    for (int i = 0; i < numParticles; ++i) {
        Particle* p = new Particle(Vector3d(dist(gen), dist(gen), dist(gen)), NULL, particleSize);
        p->setId(i);
        p->setMass(particleMass);
        p->setShape(new ShapeGimp);
        p->updateContributionNodes(&mesh);
        particles.push_back(p);
    }

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // measure execution time of nodalMass
    auto t0 = std::chrono::high_resolution_clock::now();
    Interpolation::nodalMass(&mesh, &particles);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[ PERF ] nodalMass took " << duration << " ms" << std::endl;

    // optional: check total nodal mass
    double totalMass = 0.0;
    vector<Node*>* nodes = mesh.getNodes();
    for (size_t j = 0; j < nodes->size(); ++j) {
        totalMass += nodes->at(j)->getMass();
    }
    EXPECT_NEAR(totalMass, numParticles * particleMass, 1e-8);

    // free memory
    for (auto* p : particles)
        delete p;
}

TEST(InterpolationPerformance, NodalMomentum_nParticles)
{

    const int numParticles = Configuration::getNumParticles();
    const int numThreads = Configuration::getNumThreads();
    const Vector3d particleSize = Configuration::getParticleSize();
    const double particleMass = Configuration::getParticleMass();
    const Vector3d cellDimension = Configuration::getCellDimension();
    const Vector3i numCells = Configuration::getNumCells();
    const int randomSeed = Configuration::getRandomSeed();

#ifdef _OPENMP
    std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(numThreads);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#endif

    Vector3d particleVelocity(1.0, 2.0, 3.0);

    // create the mesh
    Mesh mesh;
    mesh.setNumCells(numCells(0), numCells(1), numCells(2));
    mesh.setCellDimension(cellDimension(0), cellDimension(1), cellDimension(2));
    mesh.createGrid();

    // create particles
    std::vector<Particle*> particles;
    particles.reserve(numParticles);

    std::mt19937 gen(randomSeed);
    std::uniform_real_distribution<double> dist(0.0, cellDimension.maxCoeff());

    for (int i = 0; i < numParticles; i++) {
        Vector3d pos(dist(gen), dist(gen), dist(gen));
        Particle* p = new Particle(pos, nullptr, particleSize);
        p->setId(i);
        p->setMass(particleMass);
        p->setVelocity(particleVelocity);
        p->setShape(new ShapeGimp);
        particles.push_back(p);
    }

    // Update contribution nodes
    for (auto& p : particles)
        p->updateContributionNodes(&mesh);

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // Start timing
    auto t1 = std::chrono::high_resolution_clock::now();

    // Interpolate momentum
    Interpolation::nodalMomentum(&mesh, &particles);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = t2 - t1;

    std::cout << "[ PERF ] nodalMomentum took " << elapsed.count() << " ms" << std::endl;

    // Optional: sum total nodal momentum to verify correctness
    Vector3d totalMomentum = Vector3d::Zero();
    for (Node* n : *mesh.getNodes())
        totalMomentum += n->getMomentum();

    EXPECT_NEAR(totalMomentum(0), particleVelocity(0) * numParticles, 1e-5);
    EXPECT_NEAR(totalMomentum(1), particleVelocity(1) * numParticles, 1e-5);
    EXPECT_NEAR(totalMomentum(2), particleVelocity(2) * numParticles, 1e-5);

    // Clean up
    for (auto& p : particles)
        delete p;
}

TEST(InterpolationPerformance, NodalInternalForce_nParticles)
{

    const int numParticles = Configuration::getNumParticles();
    const int numThreads = Configuration::getNumThreads();
    const Vector3d particleSize = Configuration::getParticleSize();
    const double particleMass = Configuration::getParticleMass();
    const Vector3d cellDimension = Configuration::getCellDimension();
    const Vector3i numCells = Configuration::getNumCells();
    const int randomSeed = Configuration::getRandomSeed();

#ifdef _OPENMP
    std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(numThreads);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#endif

    // create the mesh
    Mesh mesh;
    mesh.setNumCells(numCells(0), numCells(1), numCells(2));
    mesh.setCellDimension(cellDimension(0), cellDimension(1), cellDimension(2));
    mesh.createGrid();

    // create particles
    vector<Particle*> particles;
    particles.reserve(numParticles);
    std::mt19937 gen(randomSeed);
    std::uniform_real_distribution<double> dist(0.0, cellDimension.maxCoeff());

    for (int i = 0; i < numParticles; ++i) {
        Vector3d pos(dist(gen), dist(gen), dist(gen));
        Particle* p = new Particle(pos, nullptr, particleSize);
        p->setShape(new ShapeGimp());
        p->setActive(true);
        p->setStress(Matrix3d::Identity());
        p->setPorePressure(0.0);
        particles.push_back(p);
    }

    // Update contributions
    for (auto& p : particles) {
        p->updateContributionNodes(&mesh);
    }

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // Measure performance of nodalInternalForce
    auto start = std::chrono::high_resolution_clock::now();
    Interpolation::nodalInternalForce(&mesh, &particles);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[ PERF ] nodalInternalForce took " << duration_ms << " ms" << std::endl;

    // Clean up
    for (auto p : particles)
        delete p;
}

TEST(InterpolationPerformance, InterpolationPerformance_NodalExternalForce_nParticles)
{

    const int numParticles = Configuration::getNumParticles();
    const int numThreads = Configuration::getNumThreads();
    const Vector3d particleSize = Configuration::getParticleSize();
    const double particleMass = Configuration::getParticleMass();
    const Vector3d cellDimension = Configuration::getCellDimension();
    const Vector3i numCells = Configuration::getNumCells();
    const int randomSeed = Configuration::getRandomSeed();

#ifdef _OPENMP
    std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(numThreads);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#endif

    const Vector3d appliedForce(1.0, 2.0, 3.0);

    // create the mesh
    Mesh mesh;
    mesh.setNumCells(numCells(0), numCells(1), numCells(2));
    mesh.setCellDimension(cellDimension(0), cellDimension(1), cellDimension(2));
    mesh.createGrid();

    // create particles
    vector<Particle*> particles;
    std::mt19937 gen(randomSeed);
    std::uniform_real_distribution<double> dist(0.0, cellDimension.maxCoeff());
    particles.reserve(numParticles);

    for (int i = 0; i < numParticles; ++i)
    {
        Vector3d pos(dist(gen), dist(gen), dist(gen));
        Particle* p = new Particle(pos, nullptr, particleSize);
        p->setId(i);
        p->setMass(particleMass);
        p->setShape(new ShapeGimp);
        p->addExternalForce(appliedForce);
        particles.push_back(p);
    }

    // Update contribution nodes
    for (auto& p : particles)
        p->updateContributionNodes(&mesh);

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // Interpolate external force
    auto start = std::chrono::high_resolution_clock::now();
    Interpolation::nodalExternalForce(&mesh, &particles);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "[ PERF ] nodalExternalForce took " << duration.count() << " ms\n";

    // Verify result
    Vector3d totalForce = Vector3d::Zero();
    for (const auto& node : *mesh.getNodes())
        totalForce += node->getExternalForce();

    Vector3d expectedForce = appliedForce * static_cast<double>(numParticles);
    EXPECT_NEAR(totalForce(0), expectedForce(0), 1e-5);
    EXPECT_NEAR(totalForce(1), expectedForce(1), 1e-5);
    EXPECT_NEAR(totalForce(2), expectedForce(2), 1e-5);

    // Clean up
    for (auto& p : particles)
        delete p;
}

TEST(InterpolationPerformance, InterpolationPerformance_ParticleStrainIncrement_nParticles)
{

    const int numParticles = Configuration::getNumParticles();
    const int numThreads = Configuration::getNumThreads();
    const Vector3d particleSize = Configuration::getParticleSize();
    const double particleMass = Configuration::getParticleMass();
    const Vector3d cellDimension = Configuration::getCellDimension();
    const Vector3i numCells = Configuration::getNumCells();
    const int randomSeed = Configuration::getRandomSeed();

#ifdef _OPENMP
    std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(numThreads);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#endif

    // Set up parameters
    const double dt = 0.01;
    const Vector3d nodalVelocity(1.0, 2.0, 3.0);

    // create the mesh
    Mesh mesh;
    mesh.setNumCells(numCells(0), numCells(1), numCells(2));
    mesh.setCellDimension(cellDimension(0), cellDimension(1), cellDimension(2));
    mesh.createGrid();

    // create particles
    vector<Particle*> particles;
    std::mt19937 gen(randomSeed);
    std::uniform_real_distribution<double> dist(0.0, cellDimension.maxCoeff());
    particles.reserve(numParticles);

    for (int i = 0; i < numParticles; ++i) {
        Vector3d position(dist(gen), dist(gen), 0.0);
        Particle* p = new Particle(position, nullptr, particleSize);
        p->setId(i);
        p->setMass(particleMass);
        p->setShape(new ShapeGimp);
        p->setActive(true);
        particles.push_back(p);
    }

    // Update contribution nodes for all particles
    for (int i = 0; i < numParticles; ++i)
        particles[i]->updateContributionNodes(&mesh);

    // Assign a space variated velocity field to all nodes
    for (auto& node : *mesh.getNodes()) {
        Vector3d coords = node->getCoordinates();
        node->setVelocity(Vector3d(nodalVelocity(0) * coords(0), nodalVelocity(1) * coords(1), nodalVelocity(2) * coords(2)));
    }

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // Measure performance
    auto start = chrono::high_resolution_clock::now();
    Interpolation::particleStrainIncrement(&mesh, &particles, dt);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::cout << "[ PERF ] particleStrainIncrement took " << duration.count() << " ms" << std::endl;

    // ===================================================
    // Analytical Expectation for Strain Increment
    // ===================================================
    // Given:
    //   - nodal velocity field: v(x, y, z) = (ax, by, cz)
    //   - with a = 1.0, b = 2.0, c = 3.0
    //   - and time step dt = 0.01
    // Then:
    //   - dv_x/dx = a then strain_xx = a * dt = 0.01
    //   - dv_y/dy = b then strain_yy = b * dt = 0.02
    //   - dv_z/dz = c then strain_zz = c * dt = 0.03
    //   - all off-diagonal terms are zero due to pure diagonal velocity gradient

    // Verify numeric consistency
    for (int i = 0; i < numParticles; ++i) {
        Matrix3d strain = particles[i]->getStrainIncrement();

        // Diagonal terms (strain increment = velocity / dt)
        EXPECT_NEAR(strain(0, 0), nodalVelocity(0) * dt, 1e-5);
        EXPECT_NEAR(strain(1, 1), nodalVelocity(1) * dt, 1e-5);
        EXPECT_NEAR(strain(2, 2), nodalVelocity(2) * dt, 1e-5);

        // Off-diagonal terms (should be zero)
        EXPECT_NEAR(strain(0, 1), 0.0, 1e-5);
        EXPECT_NEAR(strain(0, 2), 0.0, 1e-5);
        EXPECT_NEAR(strain(1, 0), 0.0, 1e-5);
        EXPECT_NEAR(strain(1, 2), 0.0, 1e-5);
        EXPECT_NEAR(strain(2, 0), 0.0, 1e-5);
        EXPECT_NEAR(strain(2, 1), 0.0, 1e-5);
    }

    // Cleanup
    for (auto p : particles) delete p;
}

TEST(InterpolationPerformance, InterpolationPerformance_ParticleVorticityIncrement_nParticles)
{

    const int numParticles = Configuration::getNumParticles();
    const int numThreads = Configuration::getNumThreads();
    const Vector3d particleSize = Configuration::getParticleSize();
    const double particleMass = Configuration::getParticleMass();
    const Vector3d cellDimension = Configuration::getCellDimension();
    const Vector3i numCells = Configuration::getNumCells();
    const int randomSeed = Configuration::getRandomSeed();

#ifdef _OPENMP
    std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(numThreads);
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#endif

    const double dt = 0.01;
    const Vector3d omega(1.0, 2.0, 3.0);  // Angular velocity around z, y, x

    // ================================
    // Setup mesh and particles
    // ================================
    Mesh mesh;
    mesh.setNumCells(numCells(0), numCells(1), numCells(2));
    mesh.setCellDimension(cellDimension(0), cellDimension(1), cellDimension(2));
    mesh.createGrid();

    std::vector<Particle*> particles;
    std::mt19937 gen(randomSeed);
    std::uniform_real_distribution<double> dist(0.0, cellDimension.maxCoeff());
    particles.reserve(numParticles);

    for (int i = 0; i < numParticles; ++i) {
        Vector3d pos(dist(gen), dist(gen), dist(gen));
        auto* p = new Particle(pos, nullptr, particleSize);
        p->setId(i);
        p->setMass(particleMass);
        p->setShape(new ShapeGimp);
        p->setActive(true);
        particles.push_back(p);
    }

    for (auto* p : particles)
        p->updateContributionNodes(&mesh);

    // ================================
    // Assign a velocity field v = w x r
    // ================================
    for (auto& node : *mesh.getNodes()) {
        Vector3d r = node->getCoordinates();
        Vector3d v = omega.cross(r);
        node->setVelocity(v);
    }

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // ================================
    // Performance measurement
    // ================================
    auto start = std::chrono::high_resolution_clock::now();
    Interpolation::particleVorticityIncrement(&mesh, &particles, dt);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[ PERF ] particleVorticityIncrement took " << duration.count() << " ms" << std::endl;

    // =====================================================
    // Analytical verification
    // =====================================================
    // Given a rotational velocity field v = w × r,
    // the vorticity increment tensor W is:
    //     W = 0.5 * (grad(v) - (grad(v))^T)
    // For a constant angular velocity w = (1, 2, 3),
    // and time step dt = 0.01, the expected increment is:
    //
    // grad(W)_expected = dt * 
    //  [  0   -3    2 ]
    //  [  3    0   -1 ]
    //  [ -2    1    0 ]
    //
    // The test compares this expected result against the 
    // interpolated vorticity increment in each particle.
    // Expected rotation tensor: skew-symmetric matrix

    Matrix3d expected;
    expected << 0, -omega(2) * dt, omega(1)* dt, omega(2)* dt, 0, -omega(0) * dt, -omega(1) * dt, omega(0)* dt, 0;

    for (int i = 0; i < numParticles; ++i) {
        const Matrix3d& W = particles[i]->getVorticityIncrement();
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                EXPECT_NEAR(W(r, c), expected(r, c), 1e-5);
    }

    for (auto* p : particles) delete p;
}