# Cubo unidad importado desde Gmsh

Una geometría: cubo de 1 m de lado, dividido por Gmsh en 6 tetraedros lineales. Dos configuraciones: Gaussian y su control baricéntrico. La suma de los volúmenes tetraédricos es 1 m³. La medición MPM se realiza al crear el cuerpo, sin resolver la evolución temporal.

## Repetir la medición

Requiere un checkout del proyecto, Python 3.8 o posterior y GCC con C++17 y OpenMP en Linux o WSL. No requiere Gmsh instalado, CMake ni paquetes Python adicionales para usar la malla incluida.

Desde la raíz del checkout de esta rama, ejecutar:

```bash
python3 tests/gmsh_tet4_volume/reproduce.py --repo .
```

Se puede ejecutar desde otra ubicación indicando el checkout:

```bash
python3 /ruta/tests/gmsh_tet4_volume/reproduce.py --repo /ruta/MPM-Geomechanics
```

Se compilan fuentes originales y el programa `volume_probe.cpp` en una carpeta nueva `.runtime/run-.../` del paquete. El probe utiliza `Input::readInputFile`, `getMaterialList`, `getBodyList` y `BodyGmsh::create` originales. El binario principal del simulador no se modifica ni instrumenta. La compilación puede demorar varios minutos.

Con el código reportado se espera un control correcto y un fallo Gaussian; el runner termina con código 1. Error de compilación, lectura o ejecución termina con código 2. Cada JSON se ejecuta en un proceso nuevo para evitar reutilización de la caché de malla.

## Comparar el candidato

```bash
python3 tests/gmsh_tet4_volume/reproduce.py --repo . --compare-candidate
```

El script cambia una sola llamada TET4 en una copia temporal de BodyGmsh.cpp, compila esa copia y repite los dos ensayos. No aplica el parche al repositorio. En este modo termina con código 0 si la masa y el volumen actual pasan en el candidato. No se exige que el volumen inicial pase: esa propiedad sigue sin corregir.

## Comprobar la geometría

```bash
python3 tests/gmsh_tet4_volume/check_mesh.py
```

El archivo `.geo` define el cubo completo. La malla `.msh` se generó con la API oficial Gmsh 4.15.2: 8 nodos y 6 TET4. Cada tetraedro ocupa 1/6 m³. El verificador estándar también comprueba las 12 caras exteriores, dos por cada plano del cubo, y elementos a lados opuestos de las 6 caras internas. `unit_cube.svg` muestra una proyección de esa conectividad; sirve como vista de la malla, no como referencia de volumen.

Si se desea regenerarla, con Gmsh y su módulo Python disponibles:

```bash
python3 tests/gmsh_tet4_volume/generate_mesh.py
```

También puede abrirse `fixtures/unit_cube.geo` en Gmsh y generar la malla 3D. Documentación oficial de la API y formato: https://gmsh.info/doc/texinfo/

## Interpretar los volúmenes

- `current_volume`: suma de `getCurrentVolume()`, es decir `m/ρ`. Es el volumen usado para fuerzas internas. Resultado original: 1 m³ baricéntrico, 0.25 m³ Gaussian.
- `initial_volume`: suma de productos de tamaños. Con celda 1³, se obtiene 6 o 24 m³, incluso tras el candidato. Se mide y comunica por separado.
- `mass_and_current_volume_ok`: exige únicamente masa y volumen actual con tolerancia relativa 1e-12 y cantidades 6/24 partículas. No significa validación de toda la geometría de partículas ni de GIMP.

Se incluyen las fuentes del ensayo, la malla, las configuraciones, el parche candidato y los resultados de antes y después. El código del simulador permanece sin cambios en esta rama; el candidato se compila únicamente en una copia temporal. Los fuentes originales del checkout se verifican por hash antes y después de la ejecución.

El parche cambia también la masa que recibe `ParticleMixture`, pues la llamada se comparte con el modo bifásico. Los ensayos incluidos son monofásicos; antes de integrar una corrección debe verificarse por separado esa ruta.
