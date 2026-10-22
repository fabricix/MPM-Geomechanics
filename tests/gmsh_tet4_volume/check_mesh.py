"""Independent ASCII MSH4.1 TET4 volume check, using only Python standard library."""
from collections import Counter
from itertools import combinations
import hashlib
import json
import math
from pathlib import Path

def determinant(a,b,c):
    return a[0]*(b[1]*c[2]-b[2]*c[1])-a[1]*(b[0]*c[2]-b[2]*c[0])+a[2]*(b[0]*c[1]-b[1]*c[0])

def validate(path):
    text = Path(path).read_text(encoding='utf-8')
    assert text.split('$MeshFormat\n')[1].splitlines()[0] == '4.1 0 8'
    tokens = iter(text.split('$Nodes\n')[1].split('$EndNodes')[0].split())
    blocks,count,_,_ = [int(next(tokens)) for _ in range(4)]
    nodes = {}
    for _ in range(blocks):
        dim,entity,parametric,n = [int(next(tokens)) for _ in range(4)]
        assert parametric == 0
        tags = [int(next(tokens)) for _ in range(n)]
        for tag in tags:
            nodes[tag] = [float(next(tokens)) for _ in range(3)]
    assert len(nodes) == count == 8
    tokens = iter(text.split('$Elements\n')[1].split('$EndElements')[0].split())
    blocks,count,_,_ = [int(next(tokens)) for _ in range(4)]
    tets = []
    for _ in range(blocks):
        dim,entity,element_type,n = [int(next(tokens)) for _ in range(4)]
        assert dim == 3 and element_type == 4
        for _ in range(n):
            tag = int(next(tokens))
            tets.append([int(next(tokens)) for _ in range(4)])
    assert len(tets) == count == 6
    assert set(tuple(p) for p in nodes.values()) == set(__import__('itertools').product([0.,1.],repeat=3))
    volumes = []
    faces = Counter()
    face_tips = {}
    for tet in tets:
        a,b,c,d = [nodes[tag] for tag in tet]
        vectors = [[point[j]-a[j] for j in range(3)] for point in [b,c,d]]
        volumes.append(abs(determinant(*vectors))/6)
        for face in combinations(tet,3):
            key = tuple(sorted(face))
            faces[key] += 1
            face_tips.setdefault(key, []).append(next(tag for tag in tet if tag not in face))
    boundary = [face for face,n in faces.items() if n == 1]
    assert len(boundary) == 12 and all(n in [1,2] for n in faces.values())
    # Every boundary triangle lies on a face of the unit cube.
    assert all(any(all(nodes[tag][axis] == side for tag in face)
                   for axis in range(3) for side in [0,1]) for face in boundary)
    for axis in range(3):
        for side in [0,1]:
            assert sum(all(nodes[tag][axis] == side for tag in face) for face in boundary) == 2
    internal_faces = [face for face,n in faces.items() if n == 2]
    assert len(internal_faces) == 6
    for face in internal_faces:
        a,b,c = [nodes[tag] for tag in face]
        u,v = [[point[j]-a[j] for j in range(3)] for point in [b,c]]
        signs = [determinant(u,v,[nodes[tag][j]-a[j] for j in range(3)]) for tag in face_tips[face]]
        assert signs[0]*signs[1] < 0
    assert all(math.isclose(v,1/6,rel_tol=1e-12) for v in volumes)
    assert math.isclose(sum(volumes),1,rel_tol=1e-12)
    return {'nodes':len(nodes),'tetrahedra':len(tets),'tet_volumes':volumes,
            'sum_geometric_volume':sum(volumes),'boundary_triangles':len(boundary),
            'boundary_on_cube_faces':True,'internal_faces_on_opposite_sides':True,
            'two_boundary_triangles_per_cube_face':True,
            'sha256':hashlib.sha256(Path(path).read_bytes()).hexdigest()}

if __name__ == '__main__':
    print(json.dumps(validate(Path(__file__).resolve().parent/'fixtures/unit_cube.msh'),indent=2))
