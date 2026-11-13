# Entrega MakerLab — checklist

## O que a disciplina pede

A equipe deve evoluir o robô anterior para contribuir com a identificação, o monitoramento ou a resolução de um problema real CitizenTech. O objetivo inclui pelo menos um sensor adicional e integração com IA, para perceber o ambiente, interpretar dados e executar ações ou fornecer respostas contextualizadas. TinyML é uma das técnicas aceitas.

Para o LuzViva, a demonstração deve ligar a leitura do LDR à classificação TinyML e mostrar uma resposta relevante à condição de iluminação. O modo automático atual evita obstáculos com o HC-SR04; como o resultado do LDR ainda não altera o comportamento do robô, essa ligação deve ser fortalecida/documentada para evidenciar autonomia e tomada de decisão.

## Entregáveis

- Link de repositório público GitHub com código-fonte, instruções de instalação e execução, lista/descrição dos componentes e documentação da integração entre sensores e IA. Deve ser possível compreender e reproduzir o projeto.
- Vídeo demonstrativo de até 6 minutos, apresentando o problema CitizenTech, a solução e o robô funcionando. Deve evidenciar o sensor adicional, sua integração com IA e as principais funcionalidades.
- O vídeo deve estar no README ou ser acessível por link do YouTube.
- A documentação oficial da disciplina é o link GitHub; ZIP, RAR, DOC, DOCX, PDF e outros arquivos não substituem o repositório.
- Um integrante envia a entrega; o README precisa trazer nomes de todos os integrantes.

## Critérios de avaliação

- Sensor instalado corretamente, com leituras confiáveis e uso efetivo.
- IA integrada aos dados sensoriais, com classificações/respostas coerentes.
- Integração hardware/software entre sensores, atuadores, processamento e IA.
- Relevância do problema para cidadãos, comunidades ou serviços urbanos/rurais.
- Autonomia: o robô adapta seu comportamento às condições detectadas sem depender só de comandos manuais.
- Organização, robustez, segurança e apresentação do protótipo.
- Demonstração prática em diferentes condições e situações de falha, quando aplicável.
- Criatividade e possibilidade de ampliar a solução.

## Roteiro sugerido para o vídeo (até 6 min)

1. **Problema e cidadão (30–45 s):** pessoas que circulam à noite e a dificuldade de identificar luminárias apagadas nas áreas-piloto.
2. **Solução (45–60 s):** explicar o percurso demonstrativo e o papel do LDR, TinyML, painel e HC-SR04.
3. **Demonstração (2–3 min):** mostrar condições `apagada`, `insuficiente` e `ok`, classe/confiança e reação do dispositivo; mostrar controle manual e modo automático com obstáculo.
4. **Integração (45–60 s):** mostrar o formato de medição e, quando implementado, o envio ao endpoint do app. Se não estiver implementado, declarar a limitação com clareza.
5. **Testes e fechamento (45–60 s):** cobrir LDR, apresentar condição de falha (por exemplo, sem eco ou perda de Wi-Fi), limitações e benefício CitizenTech.

## Conferência antes de enviar

- [ ] README com nomes completos e RMs dos cinco integrantes.
- [ ] Link público do GitHub abre em janela anônima.
- [ ] Firmware, dependências e passos de compilação estão claros.
- [ ] Modelo Edge Impulse é disponibilizado ou sua obtenção está documentada.
- [ ] Diagrama/fotos da montagem e pinagem correspondem ao protótipo real.
- [ ] Dataset e classes estão descritos; os dois CSVs HC-SR04 com conflito estão corrigidos ou excluídos do uso.
- [ ] O vídeo mostra sensor, TinyML e respostas reais do robô sem alegar funcionalidades ainda não implementadas.
- [ ] Link do vídeo foi testado e está no README.
