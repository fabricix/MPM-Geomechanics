// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Integration.h"

void Integration::nodalMomentum(Mesh* mesh, double dt) {

	// get mesh
	vector<Node*>* nodes = mesh->getNodes();

	// for each node
	#pragma omp parallel for shared(nodes, dt)
	for (int i = 0; i < static_cast<int>(nodes->size()); ++i) {

		if (!nodes->at(i)->getActive()){ continue; }
			
		// integrate nodal momentum
		nodes->at(i)->integrateMomentum(dt);
	}

	// get contactNodes
	unordered_map<int, Mesh::ContactNodeData>& contactNodes = mesh->getContactNodes();

	// for each contact node
	for (auto it = contactNodes.begin(); it != contactNodes.end(); ++it) {
		Mesh::ContactNodeData& contactNodesData = it->second;

		// integrate nodal momentum for each body at contact nodes
		contactNodesData.momentumMaster += contactNodesData.totalForceMaster * dt;
		contactNodesData.momentumSlave += contactNodesData.totalForceSlave * dt;
	}
}
