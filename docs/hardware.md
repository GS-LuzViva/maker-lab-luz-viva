# Hardware e montagem

## Componentes documentados pelo firmware

- ESP32
- LDR GL5528 e resistor de 10 kΩ em divisor de tensão
- Sensor ultrassônico HC-SR04
- Ponte H e dois motores do carrinho

## Pinagem no sketch

| Função | GPIO |
|---|---:|
| LDR (ADC) | 32 |
| HC-SR04 TRIG | 5 |
| HC-SR04 ECHO | 2 |
| Ponte H ENA | 13 |
| Ponte H IN1 | 14 |
| Ponte H IN2 | 12 |
| Ponte H ENB | 25 |
| Ponte H IN3 | 27 |
| Ponte H IN4 | 26 |

Esta tabela reproduz a pinagem configurada no sketch do LuzViva.

## Divisor do LDR

```text
3V3 ── LDR ──┬── GPIO 32 (ADC)
              └── resistor 10 kΩ ── GND
```

O ponto de leitura é a junção entre o LDR e o resistor. O firmware configura ADC de 12 bits e lê `analogRead(32)` e `analogReadMilliVolts(32)`.

## Alimentação e segurança elétrica

- Compartilhe o GND entre ESP32, sensor e ponte H, conforme a montagem do carrinho.
- Não alimente motores diretamente pelo pino 3V3 do ESP32; use a alimentação prevista para a ponte H/motores.
- Verifique a tensão do sinal ECHO do HC-SR04: versões alimentadas em 5 V podem retornar nível de 5 V. Confirme o nível lógico aceito pela placa e use divisor/adequação de nível se necessário.
