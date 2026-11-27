// =============================================================
//  Irrigação automatizada com sensor de umidade do solo
//  - Sensor: saída ANALÓGICA (pino AO do módulo) ligada no A0
//  - Relé: pino 3 (ativo em LOW, como no código original)
// =============================================================

#define rele   3
#define sensor A0          // AO do módulo de umidade (não o DO)

// ---------- CALIBRAÇÃO (ajuste com os valores do seu sensor) ----------
// Abra o Monitor Serial (9600) e anote a "Leitura bruta":
//   VALOR_SECO    -> sensor no ar ou em terra totalmente seca
//   VALOR_MOLHADO -> sensor em terra encharcada (ou no copo d'água)
const int VALOR_SECO    = 1023;
const int VALOR_MOLHADO = 800;

// ---------- LIMITES DE UMIDADE (%) ----------
// Liga quando o solo ficar seco e só desliga quando estiver úmido.
// A diferença entre os dois (histerese) evita o relé ficar
// ligando/desligando sem parar perto de um único valor.
const int UMIDADE_LIGA     = 30;   // abaixo disso: solo seco -> liga
const int UMIDADE_DESLIGA  = 60;   // acima disso: solo úmido -> desliga

const int NUM_LEITURAS = 10;       // média para estabilizar a leitura

bool irrigando = false;

int lerUmidade()
{
  long soma = 0;
  for (int i = 0; i < NUM_LEITURAS; i++)
  {
    soma += analogRead(sensor);
    delay(10);
  }
  int bruto = soma / NUM_LEITURAS;

  Serial.print("Leitura bruta: ");
  Serial.print(bruto);

  // Converte para porcentagem: seco = 0 %, molhado = 100 %
  int umidade = map(bruto, VALOR_SECO, VALOR_MOLHADO, 0, 100);
  umidade = constrain(umidade, 0, 100);
  return umidade;
}

void setup()
{
  Serial.begin(9600);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);        // começa com a válvula fechada
}

void loop()
{
  int umidade = lerUmidade();

  if (!irrigando && umidade <= UMIDADE_LIGA)
  {
    irrigando = true;
    digitalWrite(rele, LOW);       // abre a válvula
  }
  else if (irrigando && umidade >= UMIDADE_DESLIGA)
  {
    irrigando = false;
    digitalWrite(rele, HIGH);      // fecha a válvula
  }

  Serial.print("  | Umidade: ");
  Serial.print(umidade);
  Serial.print(" %  | Irrigacao: ");
  Serial.println(irrigando ? "LIGADA" : "DESLIGADA");

  delay(1000);
}