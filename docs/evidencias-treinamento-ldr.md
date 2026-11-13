# Evidências do treinamento TinyML do LDR

Os dados abaixo foram conferidos nas telas do Edge Impulse Studio do projeto LuzViva (projeto 1160482, impulse 1). O arquivo estruturado com os valores está em [`evidencias/treinamento_ldr.json`](../evidencias/treinamento_ldr.json).

## Dataset e separação

- 60 amostras no projeto: 48 de treino e 12 de teste (80/20).
- Classes: `apagada`, `insuficiente` e `ok`.
- Os exemplos de teste mostrados têm 2 segundos e quatro amostras por classe.

## Configuração e validação

- Treinamento com 100 ciclos, taxa de aprendizado 0,01 e CPU.
- Rede: entrada com 1 feature, camadas densas de 20 e 10 neurônios e saída com 3 classes.
- Resultado mostrado na validação: 100,0% de acurácia e loss 0,05.
- Matriz de confusão da validação: 100% correto nas três classes.
- AUC, precisão, recall e F1 ponderados: 1,00.

## Teste separado

A tela **Model testing** mostra 100,00% no modelo **Unoptimized (float32)**: 12/12 corretos, quatro por classe (`apagada`, `insuficiente`, `ok`). A matriz não mostra erros para nenhuma classe.

## Estimativa de implantação

Na seção de desempenho embarcado com EON Compiler, o Studio estima 1 ms de inferência, pico de RAM de 1,4K e flash de 14,3K. São estimativas do Studio; não são uma medição independente no carrinho.

## Limitações a declarar

O conjunto de teste tem somente 12 amostras. Portanto, a acurácia perfeita vale para esse conjunto pequeno e não garante o mesmo resultado em outras ruas, distâncias, horários ou sensores. O teste exibido é float32; a estimativa de implantação EON é outra configuração. Ainda é necessário guardar evidências de testes físicos em vários ambientes e documentar a calibração de ADC para lux.

## Acesso às telas

- [Dataset de treino](https://studio.edgeimpulse.com/studio/1160482/acquisition/training)
- [Dataset de teste](https://studio.edgeimpulse.com/studio/1160482/acquisition/testing)
- [Parâmetros e validação](https://studio.edgeimpulse.com/studio/1160482/impulse/1/learning/keras/3)
- [Teste do modelo](https://studio.edgeimpulse.com/studio/1160482/impulse/1/validation)
- [Deployment](https://studio.edgeimpulse.com/studio/1160482/impulse/1/deployment)

> **Nota sobre imagens:** conferi visualmente essas telas, mas a captura do navegador não foi exportada como arquivo de imagem neste ambiente. Este relatório e o JSON registram os valores observados; não são screenshots originais.

## Histórico do treinamento HC-SR04 — CP2

O CP2 foi um modelo separado do classificador LDR do LuzViva. Ele usa leituras de distância do sensor ultrassônico HC-SR04 para reconhecer a situação à frente do carrinho, com as classes `livre`, `atencao` e `perigo`. A documentação do CP2 registra uma entrada (`distance_cm`) amostrada a 10 Hz, janela de 20 leituras (aproximadamente 2 s) e classificação de três classes no Edge Impulse. O firmware mantém uma janela deslizante e chama `run_classifier()` quando ela está completa.

### Dados e procedimento registrados

- A documentação original do CP2 descreve uma coleta inicial de 9 arquivos CSV: 3 por classe, com 21 leituras por arquivo e intervalo de 100 ms. Os nomes dos arquivos forneciam o rótulo durante a importação pelo CSV Wizard.
- A pasta atual [`dados/hc_sr04/`](../dados/hc_sr04/) contém 30 CSVs: 10 por classe. Esse conjunto é uma versão disponível no repositório LuzViva e não comprova, por si só, que todos os 30 arquivos foram usados no treinamento original do CP2.
- Na revisão dos arquivos, `livre_001.csv` e `livre_003.csv` apareceram com marcadores de conflito de merge; mantenha-os fora de qualquer novo treinamento até recuperar e validar os dados.
- Os CSVs usam as colunas `timestamp` e `distance_cm`; a distância está em centímetros.
- A documentação do CP2 descreve as condições como caminho desobstruído (`livre`), obstáculo a média distância (`atencao`) e obstáculo próximo (`perigo`).

### Uso do modelo no carrinho

No firmware CP2, a classe com maior probabilidade é selecionada e comparada com uma confiança mínima de 60%. `livre` permite retomar o avanço depois de três inferências seguras consecutivas; `atencao` interrompe o avanço; `perigo` bloqueia a frente e, no modo autônomo ou durante avanço, inicia uma manobra temporizada de pausa, recuo e giro. O código também mantém proteções independentes do classificador: bloqueia avanço manual sem eco ou a 30 cm ou menos, e usa uma zona de emergência a 8 cm.

### Evidências e limitações do registro

A documentação disponível comprova a configuração geral descrita acima e a integração da inferência no firmware, mas deixa em branco a proporção treino/teste, os blocos exatos do impulso, a acurácia, o loss e a matriz de confusão do CP2. Portanto, não há aqui uma métrica de desempenho verificável para esse modelo. A documentação original também alerta que a coleta inicial tinha somente 3 arquivos por classe; a cópia atual tem 10 por classe, mas a qualidade e a divisão efetivamente usada no treinamento precisam ser confirmadas no Edge Impulse.

- [Documentação do ciclo TinyML do CP2](</Users/guiespim/Documents/pasta-do-gui/entrega-projeto-maker-lab-carrinho/docs/tinyml.md>)
- [Firmware CP2 com inferência HC-SR04](</Users/guiespim/Documents/pasta-do-gui/entrega-projeto-maker-lab-carrinho/src/cp2/carrinho-cp2.ino>)
- [Dataset HC-SR04 disponível neste repositório](../dados/hc_sr04/README.md)
