"""Regenerate the provided cube mesh with the official Gmsh Python API.

Gmsh is optional for reproducing MPM results: unit_cube.msh is included.
This helper requires the gmsh Python module and its matching shared library.
"""
from pathlib import Path
import hashlib
import json
import math
import gmsh

ROOT = Path(__file__).resolve().parent

def det(a,b,c):
    return a[0]*(b[1]*c[2]-b[2]*c[1])-a[1]*(b[0]*c[2]-b[2]*c[0])+a[2]*(b[0]*c[1]-b[1]*c[0])

def main():
    fixture = ROOT/'fixtures/unit_cube.msh'
    gmsh.initialize()
    try:
        gmsh.open(str(ROOT/'fixtures/unit_cube.geo'))
        gmsh.model.mesh.generate(3)
        gmsh.write(str(fixture))
        node_ids, coordinates, _ = gmsh.model.mesh.getNodes()
        coords = {int(tag): [float(x) for x in coordinates[3*i:3*i+3]] for i,tag in enumerate(node_ids)}
        types, tags_by_type, nodes_by_type = gmsh.model.mesh.getElements(3)
        assert list(types) == [4], 'Expected only linear tetrahedral volume elements'
        tet_tags = [int(t) for t in tags_by_type[0]]
        flat = [int(t) for t in nodes_by_type[0]]
        tets = [flat[i:i+4] for i in range(0,len(flat),4)]
        volumes = []
        for tet in tets:
            a,b,c,d = [coords[tag] for tag in tet]
            vectors = [[point[j]-a[j] for j in range(3)] for point in [b,c,d]]
            volumes.append(abs(det(*vectors))/6)
        cad_volume = gmsh.model.occ.getMass(3,1)
        gmsh_volumes = [float(v) for v in gmsh.model.mesh.getElementQualities(tet_tags,'volume')]
        assert len(tets) == 6 and len(coords) == 8
        assert all(math.isclose(v,1/6,rel_tol=1e-12) for v in volumes)
        assert math.isclose(sum(volumes),1,rel_tol=1e-12)
        assert math.isclose(sum(gmsh_volumes),1,rel_tol=1e-12)
        assert math.isclose(cad_volume,1,rel_tol=1e-12)
        report = {'generator':'Official Gmsh Python API','gmsh_version':gmsh.__version__,
                  'geo':'fixtures/unit_cube.geo','mesh':'fixtures/unit_cube.msh',
                  'nodes':len(coords),'tetrahedra':len(tets),'element_type':'TET4',
                  'cad_volume':cad_volume,'sum_tet_volumes_determinant':sum(volumes),
                  'sum_tet_volumes_gmsh':sum(gmsh_volumes),'tet_volumes':volumes,
                  'coordinates':coords,'connectivity':tets,
                  'sha256':hashlib.sha256(fixture.read_bytes()).hexdigest()}
        evidence = ROOT/'evidence'
        evidence.mkdir(exist_ok=True)
        (evidence/'mesh_validation.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
        print(json.dumps({k:report[k] for k in ['gmsh_version','nodes','tetrahedra','cad_volume','sum_tet_volumes_determinant','sum_tet_volumes_gmsh']}))
    finally:
        gmsh.finalize()

if __name__ == '__main__':
    main()
