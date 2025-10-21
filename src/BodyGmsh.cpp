// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Body/BodyGmsh.h"
#include "Particle/Particle.h"
#include "Particle/ParticleMixture.h"
#include "Model.h"
#include "GmshMeshReader.h"
#include "Warning.h"

#include <unordered_set>
#include <cmath>
#include <memory>

namespace {
    std::shared_ptr<const GMSH::GmshMesh> s_mesh;
    std::string s_meshPath;
}

BodyGmsh::BodyGmsh(
					const std::string& mesh_file,
					std::map<std::string, int> physical_to_material,
                   	const std::string& particle_distribution)
				   :
				    meshFile(mesh_file),
				    physicalToMaterial(std::move(physical_to_material)),
					particleDistribution(particle_distribution),
					materialId(-1) 
{
    // empty
}

BodyGmsh::~BodyGmsh() { }


  static inline void tetCentroid(const double A[3], const double B[3],
                                 const double C[3], const double D[3],
                                 double Xc[3]) {
    Xc[0]=(A[0]+B[0]+C[0]+D[0])/4.0;
    Xc[1]=(A[1]+B[1]+C[1]+D[1])/4.0;
    Xc[2]=(A[2]+B[2]+C[2]+D[2])/4.0;
  }

  static inline double tetVolume(const double A[3], const double B[3],
                                 const double C[3], const double D[3]) {
    double BA[3]={B[0]-A[0],B[1]-A[1],B[2]-A[2]};
    double CA[3]={C[0]-A[0],C[1]-A[1],C[2]-A[2]};
    double DA[3]={D[0]-A[0],D[1]-A[1],D[2]-A[2]};
    double cx=BA[1]*CA[2]-BA[2]*CA[1];
    double cy=BA[2]*CA[0]-BA[0]*CA[2];
    double cz=BA[0]*CA[1]-BA[1]*CA[0];
    double triple=fabs(cx*DA[0]+cy*DA[1]+cz*DA[2]);
    return triple/6.0;
  }

  static inline void tet4rule(const double A[3], const double B[3],
                              const double C[3], const double D[3],
                              double X[4][3]) {
    const double a=0.58541020, b=0.13819660;
    double w[4][4]={{a,b,b,b},{b,a,b,b},{b,b,a,b},{b,b,b,a}};
    for (int i=0;i<4;++i){
      X[i][0]=w[i][0]*A[0]+w[i][1]*B[0]+w[i][2]*C[0]+w[i][3]*D[0];
      X[i][1]=w[i][0]*A[1]+w[i][1]*B[1]+w[i][2]*C[1]+w[i][3]*D[1];
      X[i][2]=w[i][0]*A[2]+w[i][1]*B[2]+w[i][2]*C[2]+w[i][3]*D[2];
    }
  }

void BodyGmsh::create(Mesh& bg_mesh, Material* material)
{
    if (!material) {
        Warning::printMessage("BodyGmsh::create: null material pointer");
        return;
    }

    const int thisMatId = material->getId();
    const double rho = material->getDensity();

    Vector3d cellDim = bg_mesh.getCellDimension();
    Vector3d particleSize(cellDim.x(), cellDim.y(), cellDim.z());
    const bool twoPhase = ModelSetup::getTwoPhaseActive();

    // read the Gmsh mesh only once
    if (!s_mesh) {
        auto m = std::make_shared<GMSH::GmshMesh>();
        if (!GMSH::GmshMeshReader::load(meshFile, *m)) {
            Warning::printMessage("BodyGmsh::create: can't read " + meshFile);
            return;
        }
        s_mesh = std::move(m);
        s_meshPath = meshFile;
    } else if (meshFile != s_meshPath) {
        Warning::printMessage(
            "BodyGmsh::create: mesh already loaded from '" + s_meshPath +
            "', skipping reload of '" + meshFile + "'.");
    }

    // constant reference to the mesh
    const GMSH::GmshMesh& gm = *s_mesh;

    // name and physical id for the material
    std::unordered_map<std::string,int> name2id;
    name2id.reserve(gm.physicals.size());
    for (auto &kv : gm.physicals)
        if (!kv.second.name.empty())
            name2id[kv.second.name] = kv.first;

    // Physical groups associated with this material
    std::unordered_set<int> physForThisMat;
    for (auto &kv : physicalToMaterial) {
        if (kv.second == thisMatId) {
            auto it = name2id.find(kv.first);
            if (it != name2id.end()) physForThisMat.insert(it->second);
        }
    }
    if (physForThisMat.empty()) return;

    // rapid nodeTag and index lookup
    std::unordered_map<int,int> tag2idx;
    tag2idx.reserve(gm.nodes.size()*13/10);
    for (int i=0;i<(int)gm.nodes.size();++i)
        tag2idx[gm.nodes[i].tag] = i;

    // helper to add a particle
    auto addParticle = [&](const Vector3d& pos, double mass, double volume){
        (void)volume;// unused for now
        if (twoPhase){
            ParticleMixture* ipm = new ParticleMixture(pos, material, particleSize);
            ipm->setMass(mass);
            Body::particles.push_back(ipm);
        }
        else
        {
            Particle* ip = new Particle(pos, material, particleSize);
            ip->setMass(mass);
            Body::particles.push_back(ip);
        }
    };

    size_t seeds = 0;

    // get the list of volume entities associated with the material physical groups
    std::unordered_set<int> goodEntities;
    for (auto& kv : gm.entities) {
        const auto& E = kv.second;
        if (E.dim != 3) continue; // volumes only
        for (int pid : E.physicals) {
            if (physForThisMat.count(pid)) {
                goodEntities.insert(E.tag);
                break;
            }
        }
    }

    // go through the elements and create particles in TET4 and HEX8 elements
    // the particles are created in barycentric coordinates (1 particle per element) or Gauss points (4 points in TET4 or 8 in hexahedron).
    for (const auto& e : gm.elements) {

        if (!goodEntities.count(e.entityTag)) continue;

        // ---------- TET4 ----------
        if (e.type == 4 && e.nodeTags.size() == 4) {
            const auto& A = gm.nodes[tag2idx[e.nodeTags[0]]];
            const auto& B = gm.nodes[tag2idx[e.nodeTags[1]]];
            const auto& C = gm.nodes[tag2idx[e.nodeTags[2]]];
            const auto& D = gm.nodes[tag2idx[e.nodeTags[3]]];
            Vector3d a(A.x,A.y,A.z), b(B.x,B.y,B.z), c(C.x,C.y,C.z), d(D.x,D.y,D.z);

            double V = std::fabs((b-a).cross(c-a).dot(d-a)) / 6.0;
            const int nppc = particleDistribution == "barycentric" ? 1 : 4;
            const double Vp = V / nppc;
            const double Mp = rho * Vp;

            if (particleDistribution == "barycentric") {
                // barycentric
                Vector3d xc = 0.25*(a+b+c+d);
                addParticle(xc, Mp, Vp);
                ++seeds;
            }
            else {
                // four Gauss points in TET4
                const double aG=0.58541020, bG=0.13819660;
                const double w[4][4]={{aG,bG,bG,bG},{bG,aG,bG,bG},
                                      {bG,bG,aG,bG},{bG,bG,bG,aG}};
                for (int i=0;i<4;++i){
                    Vector3d p = w[i][0]*a + w[i][1]*b + w[i][2]*c + w[i][3]*d;
                    addParticle(p, Mp/4.0, Vp/4.0);
                    ++seeds;
                }
            }
        }

        // ---------- HEX8 ----------
        else if (e.type == 5 && e.nodeTags.size() == 8) {
            Vector3d P[8];
            for (int k=0;k<8;++k) {
                const auto& N = gm.nodes[tag2idx[e.nodeTags[k]]];
                P[k] = Vector3d(N.x,N.y,N.z);
            }

            // compute volume by decomposing into 5 tetrahedra
            const int T[5][4]={{0,1,3,4},{1,2,3,6},{1,3,4,6},{3,4,6,7},{1,4,5,6}};
            double V=0;
            for (const int(&t)[4] : T) {
                V += std::fabs(
                    (P[t[1]] - P[t[0]])
                    .cross(P[t[2]] - P[t[0]])
                    .dot(P[t[3]] - P[t[0]])
                ) / 6.0;
            }
               
            const int nppc = particleDistribution == "barycentric" ? 1 : 8;
            const double Vp = V / nppc;
            const double Mp = rho * Vp;

            if (particleDistribution == "barycentric") {
                // barycentric point
                Vector3d xc(0,0,0);
                for (int i=0;i<8;++i) xc += P[i];
                xc /= 8.0;
                addParticle(xc, Mp, Vp);
                ++seeds;
            } else {
                // eight Gauss points
                const double g = 1.0/std::sqrt(3.0);
                const double gp[8][3]={{-g,-g,-g},{ g,-g,-g},{ g, g,-g},{-g, g,-g},
                                       {-g,-g, g},{ g,-g, g},{ g, g, g},{-g, g, g}};
                for (int i=0;i<8;++i){
                    // trilinear shape functions for interpolate coordinates
                    double xi=gp[i][0], eta=gp[i][1], zeta=gp[i][2];
                    double N[8] = {
                        0.125*(1-xi)*(1-eta)*(1-zeta),
                        0.125*(1+xi)*(1-eta)*(1-zeta),
                        0.125*(1+xi)*(1+eta)*(1-zeta),
                        0.125*(1-xi)*(1+eta)*(1-zeta),
                        0.125*(1-xi)*(1-eta)*(1+zeta),
                        0.125*(1+xi)*(1-eta)*(1+zeta),
                        0.125*(1+xi)*(1+eta)*(1+zeta),
                        0.125*(1-xi)*(1+eta)*(1+zeta)
                    };
                    Vector3d xp(0,0,0);
                    for (int k=0;k<8;++k) xp += N[k]*P[k];
                    addParticle(xp, Mp/8.0, Vp/8.0);
                    ++seeds;
                }
            }
        }
    }

    if (seeds == 0) {
        Warning::printMessage("BodyGmsh::create: can't create material points in material id="
                              + std::to_string(thisMatId));
    }
}

void BodyGmsh::resetSharedMesh() {

    // warn if mesh is still referenced
    if (s_mesh && s_mesh.use_count() > 1) {
        Warning::printMessage("BodyGmsh::resetSharedMesh: mesh still referenced (" +
                              std::to_string(s_mesh.use_count()) + " bodies)");
    }

    // reset shared mesh pointer
    s_mesh.reset();
    s_meshPath.clear();
}
