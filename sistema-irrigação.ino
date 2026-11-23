/*
  ============================================================
   SISTEMA DE IRRIGAÇÃO AUTOMATIZADA - Arduino UNO
  ============================================================
   Ligações (conforme o esquema):
     - Relé (IN)          -> pino 8
     - Relé VCC / GND     -> 5V / GND
     - Sensor HL-01 VCC   -> pino 7  (alimentado só na leitura)
     - Sensor HL-01 GND   -> GND
     - Sensor HL-01 A0    -> A0
     - LED indicador      -> pino 12 (com resistor de 220 ohms ao GND)
     - Bomba: em série com COM/NO do relé e a fonte de 9-15V

   MODO DE TESTE (MODO_SIMULACAO = true):
     A umidade do solo é SIMULADA: o "solo" vai secando sozinho
     e, quando o sistema irriga, a umidade sobe. Assim dá para
     testar toda a lógica sem água.
     Para usar o sensor real, mude MODO_SIMULACAO para false.
  ============================================================
*/

// ---------------- CONFIGURAÇÕES ----------------
const bool MODO_SIMULACAO = false;   // true = valores simulados | false = sensor real
const bool ACIONAR_RELE   = true;   // false = só o LED indica (relé não clica)

// Alguns módulos de relé ligam com LOW (acionamento em nível baixo).
// Se o relé funcionar invertido, troque para LOW.
const int RELE_LIGADO    = HIGH;
const int RELE_DESLIGADO = (RELE_LIGADO == HIGH) ? LOW : HIGH;

// Pinos
const int PINO_RELE         = 8;
const int PINO_SENSOR_VCC   = 7;
const int PINO_SENSOR       = A0;
const int PINO_LED          = 12;

// Limites de umidade (em %) - histerese evita liga/desliga contínuo
const int UMIDADE_MINIMA = 35;   // abaixo disso: começa a irrigar
const int UMIDADE_ALVO   = 70;   // atingiu isso: para de irrigar

// Calibração do sensor real (leitura analógica)
// Sensor no ar (seco) costuma dar ~1023; na água ~300. Ajuste com seus testes.
const int LEITURA_SECO    = 1023;
const int LEITURA_MOLHADO = 300;

// Tempos (ms)
const unsigned long INTERVALO_LEITURA  = 1000;   // lê a umidade a cada 1 s
const unsigned long TEMPO_MAX_IRRIGACAO = 30000; // segurança: no máx. 30 s seguidos
const unsigned long PAUSA_APOS_LIMITE   = 15000; // espera 15 s se estourar o tempo
const unsigned long PISCA_LED           = 300;   // velocidade do pisca do LED

// Parâmetros da simulação (% por leitura)
const float SIM_SECAGEM   = 1.5;  // quanto o solo seca a cada leitura
const float SIM_IRRIGACAO = 4.0;  // quanto a umidade sobe ao irrigar

// ---------------- VARIÁVEIS ----------------
bool  irrigando = false;
float umidadeSimulada = 60.0;
int   umidadeAtual = 0;

unsigned long ultimoTempoLeitura = 0;
unsigned long inicioIrrigacao    = 0;
unsigned long bloqueadoAte       = 0;
unsigned long ultimoPisca        = 0;
bool estadoLed = false;

// ---------------- FUNÇÕES ----------------
int lerUmidadeReal() {
  digitalWrite(PINO_SENSOR_VCC, HIGH);   // liga o sensor
  delay(10);                             // estabiliza
  long soma = 0;
  for (int i = 0; i < 10; i++) {         // média de 10 leituras
    soma += analogRead(PINO_SENSOR);
    delay(2);
  }
  digitalWrite(PINO_SENSOR_VCC, LOW);    // desliga (reduz corrosão)

  int leitura = soma / 10;
  int umidade = map(leitura, LEITURA_SECO, LEITURA_MOLHADO, 0, 100);
  return constrain(umidade, 0, 100);
}

int lerUmidadeSimulada() {
  if (irrigando) {
    umidadeSimulada += SIM_IRRIGACAO;
  } else {
    umidadeSimulada -= SIM_SECAGEM;
  }
  umidadeSimulada += random(-10, 11) / 10.0;   // ruído de +-1% (realismo)
  umidadeSimulada = constrain(umidadeSimulada, 0, 100);
  return (int)umidadeSimulada;
}

void ligarIrrigacao() {
  irrigando = true;
  inicioIrrigacao = millis();
  if (ACIONAR_RELE) digitalWrite(PINO_RELE, RELE_LIGADO);
  Serial.println(F(">>> IRRIGACAO LIGADA"));
}

void desligarIrrigacao(const __FlashStringHelper* motivo) {
  irrigando = false;
  digitalWrite(PINO_RELE, RELE_DESLIGADO);
  digitalWrite(PINO_LED, LOW);
  estadoLed = false;
  Serial.print(F("<<< IRRIGACAO DESLIGADA - "));
  Serial.println(motivo);
}

void atualizarLed() {
  // LED pisca enquanto irriga
  if (irrigando && millis() - ultimoPisca >= PISCA_LED) {
    ultimoPisca = millis();
    estadoLed = !estadoLed;
    digitalWrite(PINO_LED, estadoLed);
  }
}

void imprimirStatus() {
  Serial.print(MODO_SIMULACAO ? F("[SIM] ") : F("[REAL] "));
  Serial.print(F("Umidade: "));
  Serial.print(umidadeAtual);
  Serial.print(F("% | Estado: "));
  Serial.println(irrigando ? F("IRRIGANDO") : F("aguardando"));
}

// ---------------- SETUP ----------------
void setup() {
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_SENSOR_VCC, OUTPUT);
  pinMode(PINO_LED, OUTPUT);

  digitalWrite(PINO_RELE, RELE_DESLIGADO);  // garante bomba desligada no início
  digitalWrite(PINO_SENSOR_VCC, LOW);
  digitalWrite(PINO_LED, LOW);

  Serial.begin(9600);
  randomSeed(analogRead(A5));

  Serial.println(F("=== Sistema de Irrigacao Automatizada ==="));
  Serial.println(MODO_SIMULACAO ? F("Modo: SIMULACAO (sem agua)")
                                : F("Modo: SENSOR REAL"));
  Serial.print(F("Irriga abaixo de "));
  Serial.print(UMIDADE_MINIMA);
  Serial.print(F("% e para em "));
  Serial.print(UMIDADE_ALVO);
  Serial.println(F("%"));
}

// ---------------- LOOP ----------------
void loop() {
  unsigned long agora = millis();

  if (agora - ultimoTempoLeitura >= INTERVALO_LEITURA) {
    ultimoTempoLeitura = agora;

    umidadeAtual = MODO_SIMULACAO ? lerUmidadeSimulada() : lerUmidadeReal();

    if (!irrigando) {
      // Só liga se a umidade estiver baixa e não estiver em pausa de segurança
      if (umidadeAtual < UMIDADE_MINIMA && agora >= bloqueadoAte) {
        ligarIrrigacao();
      }
    } else {
      if (umidadeAtual >= UMIDADE_ALVO) {
        desligarIrrigacao(F("umidade ideal atingida"));
      } else if (agora - inicioIrrigacao >= TEMPO_MAX_IRRIGACAO) {
        // Proteção: evita bomba ligada para sempre (sensor solto, sem água etc.)
        desligarIrrigacao(F("TEMPO MAXIMO! Verifique sensor/agua"));
        bloqueadoAte = agora + PAUSA_APOS_LIMITE;
      }
    }

    imprimirStatus();
  }

  atualizarLed();
}
