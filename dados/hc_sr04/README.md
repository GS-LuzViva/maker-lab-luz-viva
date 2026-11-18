# Dataset — CP2 TinyML

## Sensor e dados

- Sensor: HC-SR04
- Microcontrolador: ESP32
- Grandeza medida: distância
- Unidade: centímetros
- Colunas: `timestamp` e `distance_cm`

## Método de coleta
As medições foram organizadas em arquivos CSV por condição do sensor.

## Classes do modelo

| Classe | Significado |
|---|---|
| `atencao` | Obstáculo a média distância |
| `livre` | Caminho desobstruído |
| `perigo` | Obstáculo próximo |

## Arquivos coletados

Os arquivos CSV estão organizados nesta pasta, separados por classe.

- `atencao_001.csv` até `atencao_010.csv`
- `livre_001.csv` até `livre_010.csv`
- `perigo_001.csv` até `perigo_010.csv`

Total: 30 arquivos CSV.

## Preparação para o Edge Impulse
Os CSVs estão separados por classe e usam o nome do arquivo para identificar a condição durante a importação.
