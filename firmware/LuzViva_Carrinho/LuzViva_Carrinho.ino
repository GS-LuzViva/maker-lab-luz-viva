#include <WiFi.h>
#include <WebServer.h>
#include <LuzViva_inferencing.h>
#include <cstring>

// LuzViva: telemetria de luminosidade com TinyML + controle manual do carrinho.
// O modelo classifica a luz para exibição no painel. Ele não controla os motores.
// A proteção frontal continua independente, usando o HC-SR04.

// Ponte H: pinagem preservada do firmware CP2.
#define ENA_PIN 13
#define IN1_PIN 14
#define IN2_PIN 12
#define ENB_PIN 25
#define IN3_PIN 27
#define IN4_PIN 26

// HC-SR04: usado como proteção de proximidade, sem alimentar o modelo de luz.
#define TRIG_PIN 5
#define ECHO_PIN 2
#define DISTANCIA_SEGURANCA_CM 30.0f

// LDR: divisor 3V3 -> LDR -> junção (GPIO 32) -> resistor 10 kΩ -> GND.
#define LDR_PIN 32
#define PWM_AUTONOMO 255
#define PWM_GIRO 190
#define TEMPO_PAUSA_MS 250UL
#define TEMPO_RECUO_MS 220UL
#define TEMPO_GIRO_MS 650UL

const char* ssid = "ESP32_RC";
const char* password = "esp32pass";

WebServer server(80);

float amostrasLdr[EI_CLASSIFIER_RAW_SAMPLE_COUNT];
size_t numeroAmostras = 0;
unsigned long ultimaAmostraMs = 0;
unsigned long ultimaDistanciaMs = 0;
unsigned long ultimoDisparoSensorMs = 0;

int ultimaLeituraLdr = 0;
uint32_t ultimaLeituraMv = 0;
float ultimaDistancia = -1.0f;
float confiancaModelo = 0.0f;
int velocidadeAtual = 0;
bool andandoFrente = false;
bool modoAutonomo = false;
bool bloqueioPerigo = false;
uint8_t leiturasLivresSeguidas = 0;

enum EstadoEvasao { SEM_EVASAO, PAUSA_ANTES_RECUO, RECUANDO, GIRANDO };
EstadoEvasao estadoEvasao = SEM_EVASAO;
unsigned long fimEtapaEvasaoMs = 0;

char classeModelo[24] = "inicializando";
char decisao[64] = "aguardando janela do modelo";

void copiarTexto(char* destino, size_t tamanho, const char* origem) {
  strncpy(destino, origem, tamanho - 1);
  destino[tamanho - 1] = '\0';
}

void iniciarEvasao();

void pararMotores(const char* motivo = "STOP") {
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  andandoFrente = false;
  velocidadeAtual = 0;
  copiarTexto(decisao, sizeof(decisao), motivo);
}

void aplicarFrente(int pwm) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, pwm);
  analogWrite(ENB_PIN, pwm);
  andandoFrente = true;
  velocidadeAtual = pwm;
  copiarTexto(decisao, sizeof(decisao), modoAutonomo
      ? "AUTO: avancando com caminho livre"
      : "Movimento manual para frente");
}

float medirDistancia() {
  while (ultimoDisparoSensorMs != 0 &&
         millis() - ultimoDisparoSensorMs < 60UL) {
    delay(1);
  }

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  ultimoDisparoSensorMs = millis();

  const unsigned long duracao = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (duracao == 0) return -1.0f;
  return duracao * 0.0343f / 2.0f;
}

void atualizarProtecaoFrontal() {
  if (millis() - ultimaDistanciaMs < 100UL) return;
  ultimaDistanciaMs = millis();
  ultimaDistancia = medirDistancia();

  if (estadoEvasao != SEM_EVASAO) return;

  if (ultimaDistancia < 0.0f) {
    leiturasLivresSeguidas = 0;
    if (andandoFrente && modoAutonomo) {
      pararMotores("Seguranca: HC-SR04 sem eco; carrinho parado");
    }
    return;
  }

  if (ultimaDistancia <= DISTANCIA_SEGURANCA_CM) {
    leiturasLivresSeguidas = 0;
    if (modoAutonomo) {
      iniciarEvasao();
    } else if (andandoFrente) {
      pararMotores("Seguranca: obstaculo a 30 cm ou menos");
    }
  } else if (bloqueioPerigo && ultimaDistancia > DISTANCIA_SEGURANCA_CM) {
    if (leiturasLivresSeguidas < 3) leiturasLivresSeguidas++;
    if (leiturasLivresSeguidas >= 3) {
      bloqueioPerigo = false;
      leiturasLivresSeguidas = 0;
      copiarTexto(decisao, sizeof(decisao), "Caminho livre; modo automatico retomado");
    }
  }
}

void iniciarEvasao() {
  if (estadoEvasao != SEM_EVASAO) return;
  pararMotores("AUTO: obstaculo proximo; preparando evasao");
  bloqueioPerigo = true;
  leiturasLivresSeguidas = 0;
  estadoEvasao = PAUSA_ANTES_RECUO;
  fimEtapaEvasaoMs = millis() + TEMPO_PAUSA_MS;
}

void atualizarEvasao() {
  if (estadoEvasao == SEM_EVASAO || (long)(millis() - fimEtapaEvasaoMs) < 0) return;

  if (estadoEvasao == PAUSA_ANTES_RECUO) {
    estadoEvasao = RECUANDO;
    fimEtapaEvasaoMs = millis() + TEMPO_RECUO_MS;
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
    analogWrite(ENA_PIN, PWM_GIRO);
    analogWrite(ENB_PIN, PWM_GIRO);
    andandoFrente = false;
    velocidadeAtual = PWM_GIRO;
    copiarTexto(decisao, sizeof(decisao), "AUTO: recuando antes de virar");
  } else if (estadoEvasao == RECUANDO) {
    estadoEvasao = GIRANDO;
    fimEtapaEvasaoMs = millis() + TEMPO_GIRO_MS;
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
    analogWrite(ENA_PIN, PWM_GIRO);
    analogWrite(ENB_PIN, PWM_GIRO);
    copiarTexto(decisao, sizeof(decisao), "AUTO: girando para desviar");
  } else if (estadoEvasao == GIRANDO) {
    estadoEvasao = SEM_EVASAO;
    pararMotores("AUTO: manobra concluida; verificando novo caminho");
    numeroAmostras = 0;
    ultimaDistanciaMs = 0;
  }
}

void atualizarModoAutonomo() {
  if (!modoAutonomo) return;
  if (estadoEvasao != SEM_EVASAO) return;
  if (ultimaDistancia < 0.0f) {
    if (andandoFrente) pararMotores("AUTO: sem eco do HC-SR04; parado");
    else copiarTexto(decisao, sizeof(decisao), "AUTO: aguardando eco do HC-SR04");
    return;
  }
  if (ultimaDistancia <= DISTANCIA_SEGURANCA_CM) {
    iniciarEvasao();
    return;
  }
  if (bloqueioPerigo) {
    if (andandoFrente) pararMotores("AUTO: validando caminho livre");
    return;
  }
  if (!andandoFrente) aplicarFrente(PWM_AUTONOMO);
}

int obterDadosLdr(size_t offset, size_t length, float* saida) {
  if (offset + length > EI_CLASSIFIER_RAW_SAMPLE_COUNT) return -1;
  memcpy(saida, amostrasLdr + offset, length * sizeof(float));
  return 0;
}

bool classificarLuz() {
  signal_t sinal = {};
  sinal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
  sinal.get_data = obterDadosLdr;

  ei_impulse_result_t resultado = {0};
  const EI_IMPULSE_ERROR erro = run_classifier(&sinal, &resultado, false);
  if (erro != EI_IMPULSE_OK) {
    copiarTexto(classeModelo, sizeof(classeModelo), "erro");
    confiancaModelo = 0.0f;
    Serial.printf("Erro do Edge Impulse: %d\n", erro);
    return false;
  }

  const char* melhorClasse = "indefinido";
  confiancaModelo = 0.0f;
  for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    const float probabilidade = resultado.classification[i].value;
    if (probabilidade > confiancaModelo) {
      confiancaModelo = probabilidade;
      melhorClasse = resultado.classification[i].label;
    }
  }
  copiarTexto(classeModelo, sizeof(classeModelo), melhorClasse);

  Serial.printf("LDR ADC=%d | classe=%s | confianca=%.1f%%\n",
                ultimaLeituraLdr, classeModelo, confiancaModelo * 100.0f);
  return true;
}

void atualizarLeituraLdr() {
  if (millis() - ultimaAmostraMs < EI_CLASSIFIER_INTERVAL_MS) return;
  ultimaAmostraMs = millis();

  ultimaLeituraLdr = analogRead(LDR_PIN);
  ultimaLeituraMv = analogReadMilliVolts(LDR_PIN);

  if (numeroAmostras < EI_CLASSIFIER_RAW_SAMPLE_COUNT) {
    amostrasLdr[numeroAmostras++] = static_cast<float>(ultimaLeituraLdr);
  } else {
    memmove(amostrasLdr, amostrasLdr + 1,
            (EI_CLASSIFIER_RAW_SAMPLE_COUNT - 1) * sizeof(float));
    amostrasLdr[EI_CLASSIFIER_RAW_SAMPLE_COUNT - 1] =
        static_cast<float>(ultimaLeituraLdr);
  }

  if (numeroAmostras == EI_CLASSIFIER_RAW_SAMPLE_COUNT) {
    classificarLuz();
  }
}

void frente() {
  ultimaDistancia = medirDistancia();
  if (ultimaDistancia < 0.0f) {
    aplicarFrente(255);
    copiarTexto(decisao, sizeof(decisao),
                "Manual: sem eco; avance sob supervisao");
  } else if (ultimaDistancia <= DISTANCIA_SEGURANCA_CM) {
    pararMotores("Seguranca: obstaculo a 30 cm ou menos");
  } else {
    aplicarFrente(255);
  }
}

void re() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, 255);
  analogWrite(ENB_PIN, 255);
  andandoFrente = false;
  velocidadeAtual = 255;
  copiarTexto(decisao, sizeof(decisao), "Movimento manual para tras");
}

void esquerda() {
  analogWrite(ENA_PIN, 0);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENB_PIN, 255);
  andandoFrente = false;
  velocidadeAtual = 255;
  copiarTexto(decisao, sizeof(decisao), "Curva manual a esquerda");
}

void direita() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, 255);
  analogWrite(ENB_PIN, 0);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  andandoFrente = false;
  velocidadeAtual = 255;
  copiarTexto(decisao, sizeof(decisao), "Curva manual a direita");
}

const char pagina[] PROGMEM = R"rawliteral(
<!doctype html><html lang="pt-BR"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>LuzViva | Painel do carrinho</title>
<style>
:root{color-scheme:dark;--bg:#0c1420;--panel:#172334;--muted:#9aabc0;--green:#54d69a;--amber:#f5bd4f;--red:#ff7185;--blue:#66baff}
*{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at top,#182b41,var(--bg) 55%);color:#f4f7fb;font:16px system-ui,Arial,sans-serif;padding:20px}
main{max-width:760px;margin:auto}header{display:flex;justify-content:space-between;align-items:center;gap:16px;margin:4px 0 20px}h1{font-size:25px;margin:0}header small,.muted{color:var(--muted)}
.grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:14px}.card{background:var(--panel);border:1px solid #293b50;border-radius:18px;padding:18px;box-shadow:0 12px 26px #0002}.wide{grid-column:1/-1}.label{font-size:13px;color:var(--muted);text-transform:uppercase;letter-spacing:.08em}.value{font-size:30px;font-weight:750;margin-top:6px}.unit{font-size:14px;color:var(--muted);font-weight:500}.badge{display:inline-block;border-radius:99px;padding:7px 12px;margin-top:10px;background:#354257;color:#fff;font-weight:700}.badge.ok{background:#14563a;color:#a7f2cd}.badge.insuficiente{background:#694b12;color:#ffe19a}.badge.apagada{background:#273e62;color:#c6dcff}.badge.incert{background:#4d3040;color:#ffd5e0}
canvas{width:100%;height:190px;display:block;margin-top:12px}.controls{width:300px;margin:20px auto;display:grid;grid-template-columns:90px 90px 90px;grid-template-rows:90px 90px 90px;gap:15px;justify-content:center}.controls button{width:90px;height:90px;font-size:42px;border:0;border-radius:20px;background:#333;color:#fff;touch-action:manipulation;-webkit-tap-highlight-color:transparent;user-select:none}.controls .forward{grid-column:2;grid-row:1}.controls .left{grid-column:1;grid-row:2}.controls .stop{grid-column:2;grid-row:2;background:#b00000}.controls .right{grid-column:3;grid-row:2}.controls .backward{grid-column:2;grid-row:3}.auto{width:100%;height:48px;margin-top:12px;border:0;border-radius:13px;background:#176b3a;color:white;font-size:16px;font-weight:700}.auto.on{background:#74421a}.footer{font-size:12px;color:var(--muted);margin:16px 2px 4px;line-height:1.5}
@media(max-width:520px){body{padding:14px}.grid{gap:10px}.card{padding:15px}.value{font-size:25px}h1{font-size:21px}}
</style></head><body><main>
<header><div><h1>☀️ LuzViva</h1><small>Classificação TinyML no ESP32</small></div><div class="muted" id="wifi">Conectando…</div></header>
<section class="grid">
  <article class="card"><div class="label">Status da iluminação</div><div class="value" id="classe">Aguardando</div><span id="badge" class="badge">Coletando leituras</span></article>
  <article class="card"><div class="label">Confiança do modelo</div><div class="value"><span id="confianca">—</span><span class="unit"> %</span></div><div class="muted"><span id="amostras">0</span> / 18 leituras na janela TinyML</div></article>
  <article class="card"><div class="label">Leitura do LDR</div><div class="value"><span id="adc">—</span><span class="unit"> ADC</span></div><div class="muted"><span id="mv">—</span> mV</div></article>
  <article class="card"><div class="label">Proteção frontal</div><div class="value"><span id="distancia">—</span><span class="unit"> cm</span></div><div class="muted">HC-SR04 · parada a 30 cm</div></article>
  <article class="card wide"><div class="label">Histórico de luminosidade · últimas 60 leituras</div><canvas id="grafico"></canvas><div class="muted">ADC aumenta com a luz nesta montagem. A classificação vem do modelo, não de um limite fixo.</div></article>
  <article class="card wide"><div class="label">Controle do carrinho</div><button id="autoButton" class="auto" onclick="toggleAuto()">LIGAR MODO AUTOMATICO</button><div class="muted" id="modo" style="margin-top:8px">Modo manual</div><div class="controls">
    <button class="forward" type="button" onclick="sendCommand('forward')">&#8593;</button>
    <button class="left" type="button" onclick="sendCommand('left')">&#8592;</button><button class="stop" type="button" onclick="sendCommand('stop')">&#9632;</button><button class="right" type="button" onclick="sendCommand('right')">&#8594;</button>
    <button class="backward" type="button" onclick="sendCommand('backward')">&#8595;</button>
  </div><div class="muted" id="feedback" role="status" aria-live="polite" style="margin-top:10px">Pronto para receber comandos.</div></article>
</section><div class="footer">O modelo LuzViva classifica e exibe a iluminação. No modo automático, o HC-SR04 guia o avanço e a evasão de obstáculos; os comandos manuais continuam disponíveis.</div>
</main><script>
const hist=[];const labels={apagada:'Luz apagada',insuficiente:'Luz insuficiente',ok:'Luz adequada'};
function badgeClass(c,conf){if(conf<60)return'incert';return c||''}
function draw(){const c=document.getElementById('grafico'),r=c.getBoundingClientRect(),dpr=window.devicePixelRatio||1;c.width=r.width*dpr;c.height=190*dpr;const x=c.getContext('2d');x.scale(dpr,dpr);const w=r.width,h=190,p=24;x.clearRect(0,0,w,h);x.strokeStyle='#34475e';x.fillStyle='#9aabc0';x.font='11px system-ui';for(let i=0;i<=4;i++){const y=p+(h-2*p)*i/4;x.beginPath();x.moveTo(p,y);x.lineTo(w-8,y);x.stroke();x.fillText(String(Math.round(4095*(1-i/4))),2,y+4)}if(hist.length<2)return;const min=0,max=4095;x.strokeStyle='#66baff';x.lineWidth=2.5;x.beginPath();hist.forEach((v,i)=>{const px=p+(w-p-8)*i/(hist.length-1),py=h-p-(v-min)/(max-min)*(h-2*p);if(i===0)x.moveTo(px,py);else x.lineTo(px,py)});x.stroke();const last=hist[hist.length-1],ly=h-p-last/max*(h-2*p);x.fillStyle='#66baff';x.beginPath();x.arc(w-8,ly,4,0,Math.PI*2);x.fill()}
async function update(){try{const s=await fetch('/status',{cache:'no-store'}).then(r=>r.json());const c=s.classe,conf=Number(s.confianca);document.getElementById('wifi').textContent='● ESP32 conectado';document.getElementById('classe').textContent=labels[c]|| (c==='incerto'?'Leitura incerta':c);const b=document.getElementById('badge');b.textContent=conf<60?'Confiança baixa':(labels[c]||c);b.className='badge '+badgeClass(c,conf);document.getElementById('confianca').textContent=Number.isFinite(conf)?Math.round(conf):'—';document.getElementById('amostras').textContent=s.amostras;document.getElementById('adc').textContent=s.ldr_adc;document.getElementById('mv').textContent=s.ldr_mv;document.getElementById('distancia').textContent=Number(s.distancia)<0?'sem eco':Number(s.distancia).toFixed(1);document.getElementById('modo').textContent=s.auto?'Modo automático · '+s.decisao:'Modo manual · '+s.decisao;const ab=document.getElementById('autoButton');ab.textContent=s.auto?'DESLIGAR MODO AUTOMATICO':'LIGAR MODO AUTOMATICO';ab.className=s.auto?'auto on':'auto';hist.push(Number(s.ldr_adc));if(hist.length>60)hist.shift();draw()}catch(e){document.getElementById('wifi').textContent='● tentando reconectar'}}
async function sendCommand(cmd){const f=document.getElementById('feedback');f.textContent='Enviando comando '+cmd+'…';try{const r=await fetch('/command?cmd='+cmd);const t=await r.text();f.textContent=(r.ok?'Resposta do ESP32: ':'Erro HTTP '+r.status+': ')+t;await update()}catch(e){f.textContent='Sem resposta do ESP32. Confirme o Wi-Fi ESP32_RC e abra http://192.168.4.1.'}}function toggleAuto(){fetch('/command?cmd='+(document.getElementById('autoButton').textContent==='LIGAR MODO AUTOMATICO'?'auto_on':'auto_off')).then(update).catch(()=>{})}setInterval(update,500);window.addEventListener('resize',draw);update();
</script></body></html>
)rawliteral";

void handleCommand() {
  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "Comando ausente");
    return;
  }

  const String cmd = server.arg("cmd");
  if (cmd == "auto_on") {
    modoAutonomo = true;
    estadoEvasao = SEM_EVASAO;
    bloqueioPerigo = false;
    leiturasLivresSeguidas = 0;
    pararMotores("Modo automatico ligado; verificando a frente");
    ultimaDistanciaMs = 0;
    server.send(200, "text/plain", "Modo automatico ligado");
    return;
  }
  if (cmd == "auto_off") {
    modoAutonomo = false;
    estadoEvasao = SEM_EVASAO;
    pararMotores("Modo automatico desligado");
    server.send(200, "text/plain", "Modo automatico desligado");
    return;
  }

  // Um comando direcional assume o controle manual.
  modoAutonomo = false;
  estadoEvasao = SEM_EVASAO;
  if (cmd == "forward") frente();
  else if (cmd == "backward") re();
  else if (cmd == "left") esquerda();
  else if (cmd == "right") direita();
  else if (cmd == "stop") pararMotores("STOP manual");
  else {
    server.send(400, "text/plain", "Comando desconhecido");
    return;
  }
  Serial.printf("CMD %s -> %s | PWM=%d | distancia=%.1f cm | AUTO=%d\n",
                cmd.c_str(), decisao, velocidadeAtual, ultimaDistancia,
                modoAutonomo ? 1 : 0);
  server.send(200, "text/plain",
      String(decisao) + " | PWM=" + String(velocidadeAtual));
}

void handleStatus() {
  char json[320];
  if (ultimaDistancia < 0.0f) {
    snprintf(json, sizeof(json),
      "{\"ldr_adc\":%d,\"ldr_mv\":%lu,\"classe\":\"%s\",\"confianca\":%.1f,\"amostras\":%u,\"distancia\":-1,\"decisao\":\"%s\",\"pwm\":%d,\"auto\":%s}",
      ultimaLeituraLdr, (unsigned long)ultimaLeituraMv, classeModelo,
      confiancaModelo * 100.0f, (unsigned)numeroAmostras, decisao, velocidadeAtual,
      modoAutonomo ? "true" : "false");
  } else {
    snprintf(json, sizeof(json),
      "{\"ldr_adc\":%d,\"ldr_mv\":%lu,\"classe\":\"%s\",\"confianca\":%.1f,\"amostras\":%u,\"distancia\":%.1f,\"decisao\":\"%s\",\"pwm\":%d,\"auto\":%s}",
      ultimaLeituraLdr, (unsigned long)ultimaLeituraMv, classeModelo,
      confiancaModelo * 100.0f, (unsigned)numeroAmostras, ultimaDistancia, decisao, velocidadeAtual,
      modoAutonomo ? "true" : "false");
  }
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(LDR_PIN, INPUT);

  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  pararMotores("Motores parados ao iniciar");

  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.println("LuzViva | classificacao TinyML da luminosidade");
  Serial.printf("Wi-Fi: %s | IP: %s\n", ssid, WiFi.softAPIP().toString().c_str());
  Serial.printf("LDR no GPIO %d | janela: %u leituras | intervalo: %u ms\n",
                LDR_PIN, (unsigned)EI_CLASSIFIER_RAW_SAMPLE_COUNT,
                (unsigned)EI_CLASSIFIER_INTERVAL_MS);
  Serial.println("Conecte ao Wi-Fi ESP32_RC e abra http://192.168.4.1");

  server.on("/", []() { server.send_P(200, "text/html; charset=UTF-8", pagina); });
  server.on("/status", handleStatus);
  server.on("/command", handleCommand);
  server.begin();
}

void loop() {
  server.handleClient();
  atualizarEvasao();
  atualizarProtecaoFrontal();
  atualizarModoAutonomo();
  atualizarLeituraLdr();
  delay(1);
}
