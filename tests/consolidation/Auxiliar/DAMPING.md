# Comparação de amortecimento no n20

Foram executados seis casos com o mesmo executável da verificação anterior,
sem modificar o solver: nenhum amortecimento, local com alpha = 0,05, 0,10,
0,20 e 0,50, e relaxação dinâmica cinética (`kinetic`). Todos terminaram
normalmente, com valores finitos, partículas ativas e deslocamento lateral nulo.

Mantiveram-se H = 1 m, 20 células verticais, 160 partículas, dt = 1e-5 s,
duração = 0,16 s, k_h = 1e-3 m/s, pressão inicial e carga de 1 kPa, GIMP e USL.
Variou-se apenas a configuração `damping`. Em todos os casos a saída foi
adensada igualmente (`results.print = 800`, resultando em 843 arquivos incluindo
o inicial). Por isso os máximos amostrados diferem ligeiramente da comparação
anterior com cerca de 80 saídas. O caso sem amortecimento foi executado novamente.

## Implementação avaliada

- `none`: não aplica força artificial de amortecimento nem relaxação cinética.
- `local`: aplica uma força de magnitude alpha vezes a norma da força
  desequilibrada e direção contrária à velocidade nodal. Alpha é adimensional,
  não um tempo nem uma razão de amortecimento crítico.
- `kinetic`: detecta a diminuição da energia cinética entre passos e zera a
  velocidade das partículas sólidas. O critério usa a energia cinética apenas
  do sólido e não zera a velocidade do fluido. Não há parâmetro de tempo ou
  alpha para esse tipo no leitor de entrada atual.

Há limitações importantes no código avaliado: o amortecimento local é calculado
antes da atualização das velocidades nodais, portanto usa valores anteriores.
Além disso, nos ramos em que a velocidade é exatamente zero, as funções locais
não zeram explicitamente a força de amortecimento, que pode manter seu valor
anterior. A relaxação cinética atua somente no sólido. A comparação caracteriza
essas implementações atuais, não implementações corrigidas ou um modelo físico
de viscosidade. Nenhuma dessas rotinas foi alterada para o ensaio.

## Resultados

A referência analítica e a correção do rótulo de tempo são as mesmas descritas
em `README.md`. O máximo de pressão abaixo considera apenas saídas com
Tv >= 0,05 e é RMSE do perfil dividido por p0, não erro relativo em cada ponto.
O erro de recalque é relativo ao recalque analítico da camada superior no último
arquivo (aproximadamente 13,780 µm), não ao recalque final de tempo infinito.

| Tipo | alpha | Máximo erro de pressão | Erro de pressão no último arquivo | Recalque no último arquivo (µm) | Erro relativo do recalque |
|---|---:|---:|---:|---:|---:|
| Nenhum | — | 9,204% | 0,137% | 13,648 | 0,958% |
| Local | 0,05 | 3,857% | 1,057% | 13,052 | 5,283% |
| Local | 0,10 | 6,638% | 2,127% | 12,468 | 9,521% |
| Local | 0,20 | 10,258% | 4,205% | 11,333 | 17,758% |
| Local | 0,50 | 16,698% | 10,845% | 8,142 | 40,916% |
| Cinético | — | 22,157% | 0,752% | 13,311 | 3,404% |

O local com alpha = 0,05 reduziu as oscilações e o pico de erro inicial, mas
introduziu um desvio persistente na pressão e no recalque. Valores maiores
modificaram progressivamente a evolução do problema. Com alpha = 0,50,
apareceram também oscilações tardias. A relaxação cinética não melhorou o pico
de erro neste ensaio e apresentou oscilações acentuadas. Não se deve inferir que
uma curva mais suave seja necessariamente mais correta.

O caso sem amortecimento teve a melhor concordância no último instante. No
intervalo 0,05 <= Tv <= 0,3, a média temporal amostrada do RMSE(p)/p0 foi
3,030% sem amortecimento e 2,486% com alpha = 0,05; para Tv > 0,3, esses valores
foram 0,447% e 1,232%. Isso evidencia a troca entre reduzir o transiente inicial
e alterar a evolução posterior. Os resultados não justificam escolher um
coeficiente universal nem declarar validado o amortecimento bifásico.

## Arquivos e repetição

Os resultados estão em `C:/MPM/artifacts/consolidation-review/damping-n20`:

- `comparison.png`: pressão inicial e completa, recalque e RMSE do perfil.
- `pressure_profiles.png`: perfis próximos de Tv = 0,1, 0,5 e 1,0.
- `comparison.csv`, `metrics.json`, `run_metadata.json`: dados e identificação
  do executável utilizado.
- Subpastas `alpha_0`, `alpha_005`, `alpha_010`, `alpha_020`, `alpha_050` e
  `kinetic`: entradas, logs e arquivos ParaView de cada caso.

Somente refazer a análise:

```powershell
python -B tests/consolidation/Auxiliar/compare_damping.py --output C:/MPM/artifacts/consolidation-review/damping-n20
```

Executar novamente os seis casos e a análise (requer a entrada de referência
na pasta irmã `n20`):

```powershell
python -B tests/consolidation/Auxiliar/compare_damping.py --output C:/MPM/artifacts/consolidation-review/damping-n20 --exe C:/MPM/artifacts/usl-review/bin/MPM-Geomechanics.exe --run
```
