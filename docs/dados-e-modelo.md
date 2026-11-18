# Dados e modelo TinyML

## Dataset LDR

Em `dados/ldr/` há 60 arquivos CSV, 20 por condição. As colunas são `timestamp_ms`, `ldr_adc` e `ldr_mv`.

| Arquivos | Rótulo nos nomes dos CSVs | Classe esperada no modelo/painel |
|---|---|---|
| 20 | `apagado` | `apagada` |
| 20 | `insuficiente` | `insuficiente` |
| 20 | `ok` | `ok` |

`apagado` nos nomes dos arquivos e `apagada` como classe são formas diferentes do mesmo rótulo; mantenha o mapeamento explícito ao importar/treinar.

## Dataset HC-SR04

Em `dados/hc_sr04/` há 30 CSVs separados em `livre`, `atencao` e `perigo`, com 10 arquivos por condição. As colunas declaradas são `timestamp` e `distance_cm`. Esse conjunto é separado da classificação de luminosidade e não é usado pelo classificador LDR do firmware atual.

## Inferência embarcada

O sketch inclui `LuzViva_inferencing.h`, mantém uma janela do tamanho definido por `EI_CLASSIFIER_RAW_SAMPLE_COUNT` e executa `run_classifier`. A classe com maior pontuação e sua confiança são exibidas no painel. A interface apresenta `apagada`, `insuficiente` e `ok`; ela sinaliza confiança abaixo de 60% como baixa.

O ZIP da biblioteca gerada pelo Edge Impulse está versionado em `model/biblioteca-edge-impulse/`; a versão correta, o checksum e a instalação estão documentados no README dessa pasta. A exportação antiga de outro projeto não deve ser instalada.

## Resultados do treinamento

As telas salvas em `evidencias/ldr/` registram 100% de acurácia na validação e no teste do modelo. A entrada usa o canal `ldr_adc` em uma janela de 18 leituras. Consulte os parâmetros e resultados em [`evidencias/treinamento_ldr.json`](../evidencias/treinamento_ldr.json) e [`docs/evidencias-treinamento-ldr.md`](evidencias-treinamento-ldr.md).

O modelo recebe leituras ADC do LDR pelo canal `ldr_adc`.
