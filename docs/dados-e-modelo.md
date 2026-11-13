# Dados e modelo TinyML

## Dataset LDR

Em `dados/ldr/` há 60 arquivos CSV, 20 por condição. As colunas são `timestamp_ms`, `ldr_adc` e `ldr_mv`.

| Arquivos | Rótulo nos nomes dos CSVs | Classe esperada no modelo/painel |
|---|---|---|
| 20 | `apagado` | `apagada` |
| 20 | `insuficiente` | `insuficiente` |
| 20 | `ok` | `ok` |

`apagado` nos nomes dos arquivos e `apagada` como classe são formas diferentes do mesmo rótulo; mantenha o mapeamento explícito ao importar/treinar. As primeiras linhas mostram valores diferentes entre os ambientes coletados, mas isso por si só não comprova qualidade nem acurácia do classificador.

## Dataset HC-SR04

Em `dados/hc_sr04/` há 30 CSVs separados em `livre`, `atencao` e `perigo`, com 10 arquivos por condição. As colunas declaradas são `timestamp` e `distance_cm`. Esse conjunto é separado da classificação de luminosidade e não é usado pelo classificador LDR do firmware atual.

**Pendência de integridade:** `livre_001.csv` e `livre_003.csv` contêm marcadores de conflito de merge (`<<<<<<<`, `=======`, `>>>>>>>`). Não os use em treinamento até recuperar a versão correta e validar o conteúdo. Os arquivos foram preservados sem alteração.

## Inferência embarcada

O sketch inclui `LuzViva_inferencing.h`, mantém uma janela do tamanho definido por `EI_CLASSIFIER_RAW_SAMPLE_COUNT` e executa `run_classifier`. A classe com maior pontuação e sua confiança são exibidas no painel. A interface apresenta `apagada`, `insuficiente` e `ok`; ela sinaliza confiança abaixo de 60% como baixa.

A biblioteca gerada pelo Edge Impulse não está presente na pasta do projeto; portanto, a compilação depende de instalá-la separadamente no Arduino IDE. Este repositório ainda não contém métricas de validação do modelo, matriz de confusão ou evidências reproduzíveis de desempenho. Não atribua acurácia sem registrar a avaliação real.

## Limitações e próximos registros

- ADC/mV não equivale a lux: documentar calibração comparando com luxímetro em múltiplas condições.
- Registrar como cada CSV foi coletado, distância/orientação do sensor, fonte de luz e variação entre ambientes.
- Separar dados por sessão/ambiente na validação para reduzir risco de o modelo memorizar uma única configuração.
- Guardar no README/relatório a configuração do Edge Impulse, janela, taxa de amostragem, divisão treino/validação/teste e métricas obtidas.
- Distinguir leitura diurna de noturna e aplicar o horário de operação definido no contrato da equipe, se confirmado pelo app/pipeline.
