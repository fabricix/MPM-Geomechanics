# Verificação da coluna poroelástica 1D — USL

A comparação adicional dos tipos e coeficientes de amortecimento no n20 está
em [DAMPING.md](DAMPING.md).

Ensaio executado em 03/11/2026 com o código local, incluindo as alterações não
commitadas do solver USL. A compilação Release x64 passou. Os fontes do solver
e o esboço `consolidation.json` não foram modificados nesta verificação.

O resultado é favorável no regime de drenagem lenta, mas não constitui aprovação
irrestrita: com a condutividade original há oscilações iniciais de pressão que
não desaparecem ao reduzir o passo de tempo.

## Modelo e condições iniciais

- Coluna de altura H = 1 m e área A = 1 m², sem gravidade.
- Uma célula em cada direção lateral; 20 ou 40 células na vertical; oito
  partículas por célula (160 ou 320 partículas).
- Deslocamento lateral e fluxo lateral impedidos; base fixa e impermeável.
- Drenagem pela camada superior de partículas, com pressão prescrita zero.
- E = 50 MPa, nu = 0,3, n = 0,3, rho_s = 2600 kg/m³,
  rho_w = 1000 kg/m³, K_w = 500 MPa.
- Pressão inicial p0 = 1000 Pa no interior; tensão efetiva inicial zero.
- Carga vertical constante q = p0 = 1000 Pa, total de 1000 N sobre a área de 1 m².
- Sem amortecimento adicional; esquema USL. MUSL e contato não são avaliados.
- Duração: 0,16 s nos casos normais e 1,6 s nos casos `_slow`, mantendo os
  passos de tempo da tabela. A duração lenta foi estendida dez vezes para
  comparar aproximadamente o mesmo intervalo adimensional Tv.

Este ensaio parte de uma coluna **pré-pressurizada**, cujo estado interior está
inicialmente em equilíbrio sob q = p0. Não é uma simulação da fase de aplicação
instantânea da carga sobre uma coluna inicialmente descarregada. O deslocamento
analítico é o incremento relativo à configuração pré-pressurizada inicial.
A liberação da drenagem é representada pela pressão zero na camada superior já
na configuração inicial. A posição da drenagem e a carga distribuída nas
partículas introduzem uma aproximação espacial que melhora com o refinamento.

O esboço original tem H = 10 m, duração 0,001 s e carga desativada pela chave
`NO_load_distributed_box`. Ele foi executado separadamente, mas não serve como
comparação do ensaio carregado: apresentou expansão, não o recalque sob carga
constante. Sua faixa de drenagem contém a camada superior de partículas.
Para observar dissipação em uma execução curta foram criadas variantes menores;
o material original foi mantido, exceto pela condutividade nos casos `_slow`.

## Solução de referência

Usou-se a solução por séries da difusão poroelástica 1D com base impermeável e
topo drenado, conforme a [documentação técnica da Itasca](https://docs.itascacg.com/pfc600/flac3d/zone/test3d/Fluid/1DConsolidation/1dconsolidation.html).
A compressibilidade do fluido é mantida, com coeficiente de Biot igual a 1.

Definindo o módulo confinado D e o armazenamento S:

```text
D  = E (1 - nu) / [(1 + nu)(1 - 2 nu)] = 67,307692 MPa
S  = 1/D + n/K_w = 1,545714286e-8 Pa^-1
cv = k_h / (rho_w g S)
Tv = cv t / H²
```

Aqui `hydraulic_conductivity` é k_h em m/s. Não é a mobilidade em
m²/(Pa s); a conversão requer rho_w g. O arrasto do código utiliza g = 9,81
mesmo quando a gravidade como força de corpo está desligada.

Para z medido a partir da base e a_m = (2m+1) pi/2:

```text
p(z,t) = 2 p0 sum[(-1)^m cos(a_m z/H) exp(-a_m² Tv) / a_m]
u(z,t) = {2 p0 H sum[(-1)^m sin(a_m z/H) exp(-a_m² Tv)/a_m²] - p0 z}/D
```

O script usa 400 termos, compara nas posições iniciais das partículas e avalia
o recalque na camada superior de partículas, sem confundi-la com a superfície
geométrica z = H. O recalque final da superfície seria p0 H / D = 14,857143 µm.
Com k_h = 1e-3 m/s, cv = 6,594802 m²/s e H²/cv = 0,151635 s;
com k_h = 1e-4 m/s, esses valores são 0,659480 m²/s e 1,516346 s.

O solver registra o estado atualizado antes de incrementar `iTime`. Apenas na
análise, acrescentou-se dt ao rótulo dos arquivos não iniciais. Não houve alteração
do solver nem interpolação da velocidade em meio passo para tempo inteiro.

## Resultados

Critério declarado no script: RMSE(p)/p0 < 5% e RMSE(u)/(p0 H/D) < 5% nos
arquivos com Tv >= 0,05, além de valores finitos, partículas ativas e deslocamento
lateral inferior a 1e-8 m. O limiar é um critério deste ensaio, não uma norma.
O início abrupto, a discretização do dreno e a inércia limitam a comparação com
a referência quase estática. Não se exige concordância no instante inicial.

| Caso | Células z | dt (s) | k_h (m/s) | Máximo RMSE(p)/p0, Tv >= 0,05 | RMSE(p)/p0 no último arquivo | Critério |
|---|---:|---:|---:|---:|---:|---|
| n20 | 20 | 1e-5 | 1e-3 | 9,226% | 0,141% | Não passou |
| n40 | 40 | 5e-6 | 1e-3 | 9,136% | 0,082% | Não passou |
| n40_dt_half | 40 | 2,5e-6 | 1e-3 | 9,127% | 0,081% | Não passou |
| n20_slow | 20 | 1e-5 | 1e-4 | 1,701% | 0,132% | Passou no intervalo avaliado |
| n40_slow | 40 | 5e-6 | 1e-4 | 0,922% | 0,066% | Passou no intervalo avaliado |

Os máximos são calculados entre os arquivos salvos, não em todos os passos.
Na extensão dos casos lentos para 1,6 s foram mantidas 80 saídas, agora espaçadas
por aproximadamente 0,02 s. A amostragem em Tv fica próxima à dos casos normais;
por isso os máximos amostrados não são diretamente equivalentes aos da execução
curta anterior, que tinha saídas dez vezes mais frequentes no tempo físico.

Todos os casos permaneceram finitos, com todas as partículas ativas e sem
deslocamento lateral. O maior erro de deslocamento normalizado ficou abaixo
de 0,44% da escala de recalque final. Esse percentual não é erro relativo ao
pequeno recalque instantâneo.

Nos casos com k_h = 1e-3, após Tv >= 0,3, o máximo erro de pressão caiu para
1,45%, 1,22% e 1,18%, respectivamente. A diminuição de dt praticamente não mudou
as oscilações iniciais. Reduzir a condutividade melhorou a concordância, o que é
compatível com a influência da inércia na comparação com uma solução quase
estática; não demonstra que a inércia seja a única origem da diferença.

Os casos `_slow` tiveram sua duração ampliada de 0,16 s para 1,6 s, mantendo
seus passos de tempo, para alcançar aproximadamente Tv = 1,05, como os casos
normais. Isso permite comparar também a dissipação mais avançada. Duas malhas
não estabelecem uma ordem formal de convergência.

Na execução estendida, o maior erro de deslocamento normalizado dos casos
`n20_slow` e `n40_slow` foi, respectivamente, 0,157% e 0,057% da escala final.
Para Tv >= 0,3, os máximos erros de pressão desses casos foram 0,838% e 0,419%.
Ambos mantiveram todas as partículas ativas, valores finitos e deslocamento
lateral nulo até o final. A referência é quase estática e de pequenas deformações;
o ensaio não valida todos os regimes dinâmicos ou configurações do solver.

## Arquivos e repetição

- `consolidation.json`: esboço original, preservado.
- `consolidation_1d_usl.json`: entrada pronta do caso n40_slow.
- `verify_consolidation.py`: gera e executa cinco variantes, calcula as séries,
  exporta CSV, métricas JSON e gráficos. Requer NumPy e Matplotlib.
- Resultados desta execução: `C:/MPM/artifacts/consolidation-review`.
- `comparison.png`: pressão média, recalque e erro dos perfis.
- `pressure_profiles.png`: perfis em tempos adimensionais selecionados.
- `comparison.csv`, `metrics.json`, `run_metadata.json`: valores e identificação
  do executável utilizado. A subpasta `original` contém a execução do esboço.

Nesta organização do repositório, os scripts e documentos ficam em
`tests/consolidation/Auxiliar`. As entradas dos cinco casos estão nas pastas
irmãs, como [n20/input.json](../n20/input.json), e as cópias dos gráficos estão
em [comparison.png](../comparison.png) e
[pressure_profiles.png](../pressure_profiles.png). Os caminhos em
`C:/MPM/artifacts/consolidation-review` abaixo apontam para os resultados
completos da execução local, incluindo CSV e métricas.

Executar a partir da raiz do repositório, substituindo o caminho do executável:

```powershell
python tests/consolidation/Auxiliar/verify_consolidation.py --exe C:/MPM/artifacts/usl-review/bin/MPM-Geomechanics.exe --output C:/MPM/artifacts/consolidation-review
```

Ou apenas analisar as saídas existentes:

```powershell
python tests/consolidation/Auxiliar/verify_consolidation.py --output C:/MPM/artifacts/consolidation-review --analyze-only
```

O comando retorna código 1 quando qualquer caso não atende ao critério, como
nesta execução: os três casos com maior condutividade não passaram no intervalo
inicial avaliado. Os casos lentos passaram. Esse resultado não equivale a um
erro de compilação ou a uma interrupção das simulações.
