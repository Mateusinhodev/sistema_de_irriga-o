#define rele   13
#define sens   A0

const int  limiar          = 400;   // abaixo deste valor = solo seco
const int  leituraMaxima   = 876;   // maximo do sensor no Tinkercad
const bool mostrarLeituras = true;  // false = mostra so as mudancas de estado

bool irrigando = false;
bool primeiraLeitura = true;

int paraPorcentagem(int leitura) {
  return map(leitura, 0, leituraMaxima, 0, 100);
}

void setup() {
  pinMode(rele, OUTPUT);
  digitalWrite(rele, LOW);
  Serial.begin(9600);
  Serial.println("=== Sistema de Irrigacao Automatizada ===");
  Serial.print("Irriga quando a umidade ficar abaixo de ");
  Serial.print(paraPorcentagem(limiar));
  Serial.println("%");
  Serial.println("Monitorando o solo...");
  Serial.println();
}

void loop() {
  int leitura = analogRead(sens);
  int porcentagem = paraPorcentagem(leitura);
  bool seco = leitura < limiar;

  if (mostrarLeituras) {
    Serial.print("Umidade: ");
    Serial.print(porcentagem);
    Serial.println("%");
  }

  // Solo ficou seco: comeca a irrigar
  if (seco && !irrigando) {
    Serial.println("[!] SOLO SECO");
    Serial.println("[>] COMECOU A IRRIGAR - valvula aberta");
    digitalWrite(rele, HIGH);
    irrigando = true;
  }
  // Solo ficou umido: para de irrigar
  else if (!seco && irrigando) {
    Serial.println("[ok] SOLO UMIDO");
    Serial.println("[x] PAROU DE IRRIGAR - valvula fechada");
    digitalWrite(rele, LOW);
    irrigando = false;
  }
  // Primeira leitura com o solo ja umido
  else if (!seco && primeiraLeitura) {
    Serial.println("[ok] SOLO UMIDO - nao precisa irrigar");
  }

  primeiraLeitura = false;
  delay(741);
}