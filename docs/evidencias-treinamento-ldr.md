# Evidências do treinamento TinyML do LDR

Os dados abaixo foram conferidos nas capturas do Edge Impulse Studio do projeto LuzViva (projeto 1160482, impulse 1). O arquivo estruturado com os valores está em [`evidencias/treinamento_ldr.json`](../evidencias/treinamento_ldr.json).

## Dataset e separação

- 60 amostras no projeto: 48 de treino e 12 de teste (80/20).
- Classes: `apagada`, `insuficiente` e `ok`.
- Os exemplos de teste mostrados têm 2 segundos e quatro amostras por classe.

## Configuração e validação

- Treinamento com 100 ciclos, taxa de aprendizado 0,01 e CPU.
- Rede: um canal de entrada (`ldr_adc`), janela temporal de 18 leituras, camadas densas de 20 e 10 neurônios e saída com 3 classes. “1 feature” é o canal do sensor; o bloco Flatten recebe a sequência temporal.
- Resultado mostrado na validação: 100,0% de acurácia e loss 0,05.
- Matriz de confusão da validação: 100% correto nas três classes.
- AUC, precisão, recall e F1 ponderados: 1,00.

## Teste separado

A tela **Model testing** mostra 100,00% no modelo **Unoptimized (float32)**, com matriz de confusão sem erros nas classes `apagada`, `insuficiente` e `ok`.

### Como falar da precisão no pitch

Formulação para a apresentação: “No teste do Edge Impulse, o modelo obteve 100% de acerto nas classes apagada, insuficiente e ok.”

## Estimativa de implantação

Na seção de desempenho embarcado com EON Compiler, o Studio estima 1 ms de inferência, pico de RAM de 1,4K e flash de 14,3K.

## Acesso às telas

- [Dataset de treino](https://studio.edgeimpulse.com/studio/1160482/acquisition/training)
- [Dataset de teste](https://studio.edgeimpulse.com/studio/1160482/acquisition/testing)
- [Parâmetros e validação](https://studio.edgeimpulse.com/studio/1160482/impulse/1/learning/keras/3)
- [Teste do modelo](https://studio.edgeimpulse.com/studio/1160482/impulse/1/validation)
- [Deployment](https://studio.edgeimpulse.com/studio/1160482/impulse/1/deployment)

As capturas originais usadas neste resumo estão versionadas em [`evidencias/ldr/`](../evidencias/ldr/). A biblioteca gerada para o firmware também está versionada; consulte [`model/biblioteca-edge-impulse/README.md`](../model/biblioteca-edge-impulse/README.md) para instalar a exportação correta e conferir seu SHA-256.

## Histórico do treinamento HC-SR04 — CP2

O CP2 foi um modelo separado do classificador LDR do LuzViva. Ele usa leituras de distância do sensor ultrassônico HC-SR04 para reconhecer a situação à frente do carrinho, com as classes `livre`, `atencao` e `perigo`. A exportação versionada registra uma entrada (`distance_cm`) amostrada a 10 Hz e uma janela de 21 leituras (2,1 s). O firmware mantém uma janela deslizante e chama `run_classifier()` quando ela está completa.

A biblioteca Arduino correspondente, com o cabeçalho `guiespim18-project-1_inferencing.h`, está versionada neste repositório. Consulte [a documentação das bibliotecas Edge Impulse](../model/biblioteca-edge-impulse/README.md) para metadados, checksum e instruções de instalação.

### Dados e procedimento registrados

- A documentação original do CP2 descreve uma coleta inicial de 9 arquivos CSV: 3 por classe, com 21 leituras por arquivo e intervalo de 100 ms. Os nomes dos arquivos forneciam o rótulo durante a importação pelo CSV Wizard.
- A pasta atual [`dados/hc_sr04/`](../dados/hc_sr04/) contém 30 CSVs, organizados em 10 arquivos por classe.
- Os CSVs usam as colunas `timestamp` e `distance_cm`; a distância está em centímetros.
- A documentação do CP2 descreve as condições como caminho desobstruído (`livre`), obstáculo a média distância (`atencao`) e obstáculo próximo (`perigo`).

### Uso do modelo no carrinho

No firmware CP2, a classe com maior probabilidade é selecionada e comparada com uma confiança mínima de 60%. `livre` permite retomar o avanço depois de três inferências seguras consecutivas; `atencao` interrompe o avanço; `perigo` bloqueia a frente e, no modo autônomo ou durante avanço, inicia uma manobra temporizada de pausa, recuo e giro. O código também mantém proteções independentes do classificador: bloqueia avanço manual sem eco ou a 30 cm ou menos, e usa uma zona de emergência a 8 cm.

### Evidências

- [Documentação do ciclo TinyML do CP2](https://github.com/MatheusCosta616/entrega-projeto-maker-lab-carrinho/blob/main/docs/tinyml.md)
- [Firmware CP2 com inferência HC-SR04](https://github.com/MatheusCosta616/entrega-projeto-maker-lab-carrinho/blob/main/src/cp2/carrinho-cp2.ino)
- [Dataset HC-SR04 disponível neste repositório](../dados/hc_sr04/README.md)
