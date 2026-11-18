# LuzViva — robô CitizenTech de monitoramento da iluminação

O LuzViva é um protótipo MakerLab para identificar pontos de iluminação pública que podem precisar de manutenção. A proposta é apoiar quem circula à noite, encontrando falhas antes de depender apenas de reclamações.

> **Protótipo:** o TinyML classifica a leitura do LDR e o painel local mostra classe e confiança. O painel de controle do ESP32 fica em `http://192.168.4.1`.

## Link do vídeo

https://www.youtube.com/watch?v=IoHceU1G-Fc

## Ficha técnica do MVP

- **Produto:** LuzViva (nome provisório).
- **Cidadãos e território:** pessoas que circulam à noite pelas áreas-piloto Sé, Pinheiros e Campo Limpo, em São Paulo.
- **Problema:** cadastros de luminárias, telegestão e chamados não equivalem, por si só, a uma medição da luz ao longo do percurso de pedestres. O projeto investiga como dados de campo podem complementar os sistemas existentes.
- **Proposta de valor:** medir e classificar a luminosidade em uma rota demonstrativa e organizar evidências para apoiar a vistoria e a priorização de manutenção.
- **Componente MakerLab:** carrinho com ESP32, LDR e classificador TinyML; HC-SR04 para proteção e evasão de obstáculos.
- **Fontes do MVP:** cadastro de iluminação GeoSampa/ILUME, SP156, Open-Meteo e dados coletados pelo robô e pelo app, conforme as entregas das demais disciplinas.
- **ODS relacionados:** 11 (Cidades e comunidades sustentáveis), 9 (Indústria, inovação e infraestrutura) e 10 (Redução das desigualdades).

## Equipe

| Integrante | RM | Responsabilidade no projeto |
|---|---:|---|
| Caíque Walter Silva | 550693 | App LuzViva e RPA |
| Matheus José de Lima Costa | 551157 | Secure DevOps e nuvem |
| João Paulo Fonseca Zamperlini | 99279 | Visão computacional e GenAI |
| Guilherme Monteiro Espim | 99499 | MakerLab e BI & Startup |
| Guilherme Nobre Bernardo | 98604 | Big Data e integração de dados |

## Estrutura

```text
.
├── README.md
├── docs/
│   ├── arquitetura.md
│   ├── dados-e-modelo.md
│   └── hardware.md
├── evidencias/
│   ├── treinamento_ldr.json
│   ├── ldr/
│   └── hc_sr04/
├── firmware/
│   └── LuzViva_Carrinho/
│       └── LuzViva_Carrinho.ino
├── model/biblioteca-edge-impulse/
│   ├── README.md
│   ├── ei-luzviva-arduino-1.0.2-impulse-#1.zip
│   └── ei-guiespim18-project-1-arduino-1.0.3-impulse-#1.zip
└── dados/
    ├── ldr/       # 60 CSVs: 20 por condição
    └── hc_sr04/   # 30 CSVs: 10 por condição
```

## Executar o protótipo local

1. Instale o Arduino IDE e o suporte à placa ESP32 correspondente à placa usada.
2. Instale o ZIP versionado em `model/biblioteca-edge-impulse/ei-luzviva-arduino-1.0.2-impulse-#1.zip` pelo menu **Sketch → Include Library → Add .ZIP Library…**. Confira o SHA-256 e as instruções em [model/biblioteca-edge-impulse/README.md](model/biblioteca-edge-impulse/README.md).
3. Abra `firmware/LuzViva_Carrinho/LuzViva_Carrinho.ino`, selecione a placa ESP32-S3 e a porta serial e grave o programa.
4. Conecte o celular ou computador à rede Wi-Fi local do carrinho (`ESP32_RC`, senha `esp32pass`) e abra `http://192.168.4.1` para acessar o painel.
5. Observe a classificação e a confiança do LDR, o controle manual e a proteção do HC-SR04.

## Documentação

- [Arquitetura e estado das integrações](docs/arquitetura.md)
- [Hardware e ligações](docs/hardware.md)
- [Dados, classes e resultados do modelo](docs/dados-e-modelo.md)
- [Bibliotecas Edge Impulse do LDR e do HC-SR04/CP2](model/biblioteca-edge-impulse/README.md)
- [Biblioteca versionada do Edge Impulse](model/biblioteca-edge-impulse/README.md)

## Evidências do treinamento LDR

- [Relatório com resultados do treinamento](docs/evidencias-treinamento-ldr.md)
- [Registro JSON dos parâmetros e métricas](evidencias/treinamento_ldr.json)
