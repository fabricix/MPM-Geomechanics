#include <gtest/gtest.h>

#include <vector>

#include "BodyParticle.h"
#include "ContactManager.h"
#include "Particle.h"

namespace
{
  void setContribution(Particle& particle, int nodeId, double weight, const Vector3d& gradient)
  {
    auto* contributions = particle.getContributionNodes();
    contributions->clear();
    contributions->emplace_back();
    contributions->back().setNodeId(nodeId);
    contributions->back().setWeight(weight);
    contributions->back().setGradients(gradient);
  }
}

TEST(ContactManager, Detects_Shared_Node_And_Computes_Master_Normal)
{
  Mesh mesh;
  mesh.setNumCells(1, 1, 1);
  mesh.setCellDimension(1.0, 1.0, 1.0);
  mesh.createGrid();

  Particle masterParticle(Vector3d::Zero(), nullptr, Vector3d::Ones());
  Particle slaveParticle(Vector3d::Zero(), nullptr, Vector3d::Ones());
  masterParticle.setMass(1.0);
  slaveParticle.setMass(1.0);
  setContribution(masterParticle, 0, 0.5, Vector3d(1.0, 0.0, 0.0));
  setContribution(slaveParticle, 0, 0.5, Vector3d(-1.0, 0.0, 0.0));

  BodyParticle masterBody;
  BodyParticle slaveBody;
  masterBody.setParticles({&masterParticle});
  slaveBody.setParticles({&slaveParticle});
  std::vector<Body*> bodies{&masterBody, &slaveBody};
  Contact contact(1, 0.5, 1, 2, Contact::MASTER);
  ContactManager manager({&contact}, 0.0);

  manager.contactNodalDetection(&mesh, &bodies);

  ASSERT_TRUE(manager.getContactDetectionFlag());
  ASSERT_EQ(mesh.getContactNodes().size(), 1u);
  auto& contactNode = mesh.getContactNodes().at(0);
  EXPECT_EQ(contactNode.contactId, 1);
  EXPECT_EQ(contactNode.bodyMasterId, 1);
  EXPECT_EQ(contactNode.bodySlaveId, 2);

  manager.nodalUnitNormal(&mesh, &bodies);
  EXPECT_TRUE(contactNode.normal.isApprox(Vector3d::UnitX()));
}

TEST(ContactManager, Limits_Tangential_Force_And_Balances_Momentum_Update)
{
  Mesh mesh;
  mesh.setNumCells(1, 1, 1);
  mesh.setCellDimension(1.0, 1.0, 1.0);
  mesh.createGrid();

  Contact contact(1, 0.5, 1, 2, Contact::MASTER);
  ContactManager manager({&contact}, 0.0);
  Node::ContactNodeData contactNode;
  contactNode.contactId = 1;
  contactNode.hasContact = true;
  contactNode.massMaster = 1.0;
  contactNode.massSlave = 1.0;
  contactNode.momentumMaster = Vector3d::Zero();
  contactNode.momentumSlave = Vector3d(10.0, 20.0, 0.0);
  contactNode.normal = Vector3d::UnitX();
  mesh.getContactNodes()[0] = contactNode;

  manager.computeContactForces(&mesh, 1.0);

  auto& result = mesh.getContactNodes().at(0);
  EXPECT_TRUE(result.normalContactForce.isApprox(Vector3d(5.0, 0.0, 0.0)));
  EXPECT_TRUE(result.tangentialContactForce.isApprox(Vector3d(0.0, 2.5, 0.0)));
  EXPECT_TRUE(result.totalForceMaster.isApprox(Vector3d(5.0, 2.5, 0.0)));
  EXPECT_TRUE(result.totalForceSlave.isApprox(Vector3d(-5.0, -2.5, 0.0)));

  manager.nodalMomentumContactUpdate(&mesh, 0.2);
  EXPECT_TRUE(result.momentumMaster.isApprox(Vector3d(1.0, 0.5, 0.0)));
  EXPECT_TRUE(result.momentumSlave.isApprox(Vector3d(9.0, 19.5, 0.0)));
  EXPECT_TRUE((result.momentumMaster + result.momentumSlave)
                  .isApprox(Vector3d(10.0, 20.0, 0.0)));
}
