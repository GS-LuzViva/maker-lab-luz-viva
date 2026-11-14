# LuzViva — robô CitizenTech de monitoramento da iluminação

O LuzViva é um protótipo MakerLab para identificar pontos de iluminação pública que podem precisar de manutenção. A proposta é apoiar quem circula à noite, encontrando falhas antes de depender apenas de reclamações.

> **Estado atual:** este repositório contém o firmware do carrinho e os CSVs de coleta. O firmware lê o LDR, executa o classificador TinyML embarcado e mostra classe e confiança no painel Wi-Fi do ESP32. O controle manual funciona; o modo automático usa o HC-SR04 para avançar e desviar de obstáculos. O resultado do LDR ainda não altera o movimento do robô e o firmware ainda não envia medições para a API do app. Consulte [o estado de integração](docs/arquitetura.md).

## Link do vídeo

https://www.youtube.com/watch?v=IoHceU1G-Fc

## Ficha técnica do MVP

- **Produto:** LuzViva (nome provisório).
- **Cidadãos e território:** pessoas que circulam à noite pelas áreas-piloto Sé, Pinheiros e Campo Limpo, em São Paulo.
- **Problema:** a cidade não tem uma indicação aberta e atualizada de quais luminárias estão apagadas; a falha pode permanecer até alguém reportá-la.
- **Proposta de valor:** detectar e priorizar falhas de iluminação com evidências de sensores e relatos, apoiando a manutenção.
- **Componente MakerLab:** carrinho com ESP32, LDR e classificador TinyML; HC-SR04 para proteção e evasão de obstáculos.
- **Integração pretendida:** enviar medições do robô ao app LuzViva por `POST /api/medicoes`, conforme o contrato acordado pela equipe. Essa integração ainda precisa ser implementada e demonstrada.
- **Fontes do MVP:** cadastro de iluminação GeoSampa/ILUME, SP156, Open-Meteo e dados coletados pelo robô e pelo app, conforme as entregas das demais disciplinas.
- **ODS relacionados:** 11 (Cidades e comunidades sustentáveis), 9 (Indústria, inovação e infraestrutura) e 10 (Redução das desigualdades).
- **URL pública do MVP:** a preencher quando publicada.

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
│   ├── entrega-makerlab.md
│   └── hardware.md
├── firmware/
│   └── LuzViva_Carrinho/
│       └── LuzViva_Carrinho.ino
└── dados/
    ├── ldr/       # 60 CSVs: 20 por condição
    └── hc_sr04/   # 30 CSVs: 10 por condição
```

## Executar o protótipo local

1. Instale o Arduino IDE e o suporte à placa ESP32 correspondente à placa usada.
2. Instale a biblioteca de inferência Arduino exportada pelo Edge Impulse para este projeto. O sketch inclui `LuzViva_inferencing.h`; essa biblioteca/modelo não está versionada nesta pasta no momento.
3. Abra `firmware/LuzViva_Carrinho/LuzViva_Carrinho.ino`, selecione a placa e a porta serial e grave o programa.
4. Conecte o celular ou computador ao ponto de acesso Wi-Fi criado pelo ESP32 e abra `http://192.168.4.1`.
5. Use o painel para observar classificação/confiança, leituras e distância, controlar o carrinho manualmente e alternar o modo automático.

As credenciais do ponto de acesso estão definidas no sketch para a demonstração local. Antes de publicar o código, a equipe deve decidir se as mantém como credenciais públicas de demonstração ou as move para configuração local fora do Git.

## Documentação

- [Arquitetura e estado das integrações](docs/arquitetura.md)
- [Hardware e ligações](docs/hardware.md)
- [Dados, classes e limitações do modelo](docs/dados-e-modelo.md)
- [Checklist e roteiro da entrega MakerLab](docs/entrega-makerlab.md)

## Entrega MakerLab

A entrega oficial é o link público do GitHub com o código e a documentação reproduzível, acompanhado de um vídeo de até 6 minutos demonstrando o problema CitizenTech, o sensor adicional, a integração com IA e o robô funcionando. Consulte [o checklist completo](docs/entrega-makerlab.md).

## Evidências do treinamento LDR

- [Relatório com resultados e limitações](docs/evidencias-treinamento-ldr.md)
- [Registro JSON dos parâmetros e métricas](evidencias/treinamento_ldr.json)
