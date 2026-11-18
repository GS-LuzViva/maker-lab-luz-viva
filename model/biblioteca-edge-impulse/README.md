# Bibliotecas Edge Impulse

Este diretório contém duas exportações Arduino do Edge Impulse para modelos e sensores diferentes. Instale a biblioteca que corresponde ao sketch que será compilado.

## LuzViva — LDR

Usada pelo sketch `firmware/LuzViva_Carrinho/LuzViva_Carrinho.ino`.

- Arquivo: `ei-luzviva-arduino-1.0.2-impulse-#1.zip`
- Projeto/impulse: LuzViva, impulse 1
- Cabeçalho incluído pelo sketch: `LuzViva_inferencing.h`
- Entrada: canal `ldr_adc`, janela de 18 leituras a cada 100 ms
- Classes: `apagada`, `insuficiente`, `ok`
- SHA-256: `b05f558231ae75ca8c2db296eb9c8b30eb5a7d45437803d22f29aec54f7817c9`

## CP2 — HC-SR04

Esta exportação corresponde ao modelo TinyML do CP2, treinado com leituras do sensor ultrassônico. O sketch correspondente no repositório de origem é `src/cp2/carrinho-cp2.ino`.

- Arquivo: `ei-guiespim18-project-1-arduino-1.0.3-impulse-#1.zip`
- Biblioteca Arduino: `guiespim18-project-1_inferencing`, versão 1.0.3
- Projeto Edge Impulse nos metadados do ZIP: proprietário `guiespim18`, projeto `guiespim18-project-1`, ID 1137337, versão de deployment 3
- Cabeçalho incluído pelo sketch CP2: `guiespim18-project-1_inferencing.h`
- Sensor/eixo de entrada: HC-SR04, `distance_cm`
- Janela do modelo: 21 leituras, intervalo de 100 ms (10 Hz)
- Classes: `atencao`, `livre`, `perigo`
- Formato do modelo: TFLite quantizado em int8; limiar configurado nos metadados: 0,6
- SHA-256: `83305f60ec04477f7781756226f8f0ece833f8c92b8e90e6b001626c58ad0c1f`

## Instalação no Arduino IDE

1. Abra **Sketch → Include Library → Add .ZIP Library…**.
2. Selecione o ZIP correspondente ao sketch: a exportação LuzViva para o firmware LDR ou a exportação `guiespim18-project-1` para o CP2 HC-SR04.
3. Confira se o `#include` do sketch corresponde ao cabeçalho indicado na seção da biblioteca.

As duas exportações não são intercambiáveis: cada uma contém um modelo, classes e nome de cabeçalho próprios. Os arquivos ZIP estão versionados neste diretório para permitir a instalação sem uma nova exportação da conta Edge Impulse.
