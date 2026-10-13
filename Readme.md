# 🌱 Sistema de Irrigação Automatizada

Sistema de irrigação com **Arduino UNO** que lê a umidade do solo e abre uma válvula solenoide automaticamente quando o solo está seco, fechando-a quando a umidade volta ao nível desejado. O estado do solo é indicado por LEDs (vermelho = seco, verde = úmido) e todas as mudanças de estado aparecem no Monitor Serial.

O projeto foi validado em simulação no **Tinkercad Circuits** e foi pensado para funcionar, na versão física, de forma **100% solar**, sem ligação à rede elétrica.

---

## ✨ Funcionalidades

- Leitura da umidade do solo com conversão para porcentagem
- Acionamento automático do relé (válvula) quando o solo fica seco
- LED **vermelho** para solo seco e LED **verde** para solo úmido
- Mensagens no Monitor Serial apenas quando o estado muda:
  - `SOLO SECO` / `COMECOU A IRRIGAR`
  - `SOLO UMIDO` / `PAROU DE IRRIGAR`
- Limiar de umidade ajustável no código

---

## ⚙️ Como funciona

1. A cada ~0,7 s o Arduino lê o sensor de umidade no pino **A0**.
2. Se a leitura estiver **abaixo do limiar**, o solo é considerado seco:
   o LED vermelho acende e o pino **D13** aciona o relé, abrindo a válvula.
3. Quando a leitura volta a ficar **acima do limiar**, o solo é considerado úmido:
   o LED verde acende, o relé desliga e a válvula fecha.

```
Sensor de umidade ──► Arduino UNO ──► Transistor ──► Relé ──► Válvula solenoide
                          │
                          ├──► LED vermelho (D7)  solo seco
                          └──► LED verde    (D6)  solo úmido
```

---

## 🧰 Componentes

### Simulação (Tinkercad)

| Qtd. | Componente | Observação |
| --- | --- | --- |
| 1 | Arduino Uno R3 | Alimentado pelo USB |
| 1 | Sensor de umidade do solo | Saída analógica (0 a 876 no Tinkercad) |
| 1 | Relé SPDT (LU-5-R) | Representa o módulo relé |
| 1 | Transistor NPN | Aciona a bobina do relé |
| 1 | Diodo | Proteção da bobina (opcional na simulação) |
| 1 | Resistor 1 kΩ | Base do transistor |
| 3 | Resistor 220 Ω | Um para cada LED |
| 1 | LED (qualquer cor) | Representa a válvula |
| 1 | LED vermelho | Indica solo seco |
| 1 | LED verde | Indica solo úmido |
| 1 | Protoboard | — |

### Versão física (alimentação solar)

| Qtd. | Componente | Especificação sugerida |
| --- | --- | --- |
| 1 | Arduino UNO R3 | — |
| 1 | Sensor de umidade de solo | YL-69 / FC-28 com módulo LM393 |
| 1 | Módulo relé 1 canal | Bobina 5 V, contatos 10 A |
| 1 | Válvula solenoide | 12 V CC, normalmente fechada, 1/2" |
| 1 | Diodo 1N4007 | Em paralelo com a válvula |
| 1 | Placa solar | 12 V, 20 W |
| 1 | Controlador de carga | PWM 12 V, 10 A |
| 1 | Bateria | 12 V, 7 Ah (VRLA) |
| 1 | Regulador step-down | LM2596, saída ajustada em 5,0 V |
| 1 | Fusível | 5 A, no positivo da bateria |

---

## 🔌 Ligações (Tinkercad)

### Alimentação
| De | Para |
| --- | --- |
| Arduino 5V | Trilha + da protoboard |
| Arduino GND | Trilha − da protoboard |

### Sensor de umidade
| Pino do sensor | Para |
| --- | --- |
| VCC | Trilha + |
| GND | Trilha − |
| SIG | Arduino A0 |

### Acionamento do relé
| De | Para |
| --- | --- |
| Arduino D13 | Resistor 1 kΩ → base do transistor NPN |
| Emissor do transistor | Trilha − |
| Coletor do transistor | Terminal 1 da bobina do relé |
| Terminal 2 da bobina | Trilha + |
| Diodo | Em paralelo com a bobina (faixa no lado do +) |

### Válvula (LED)
| De | Para |
| --- | --- |
| COM do relé | Trilha + |
| NO do relé | Resistor 220 Ω → ânodo do LED |
| Cátodo do LED | Trilha − |

### LEDs indicadores
| De | Para |
| --- | --- |
| Arduino D7 | Resistor 220 Ω → ânodo do LED vermelho → cátodo na trilha − |
| Arduino D6 | Resistor 220 Ω → ânodo do LED verde → cátodo na trilha − |

> **Dicas do Tinkercad**
> - A perna **dobrada** do LED é o ânodo (+).
> - Confira se os resistores estão em **Ω** e não em **kΩ**.
> - Os terminais do relé aparecem como "Terminal 01, 05, 06, 07, 08 e 12". Use o **multímetro** no modo resistência para identificar a bobina, o COM, o NO e o NC.

---

## 💻 Código

O código está em [`sistema_irrigacao.ino`](sistema_irrigacao.ino).

### Parâmetros ajustáveis

| Constante | Valor padrão | Função |
| --- | --- | --- |
| `limiar` | `400` | Abaixo deste valor o solo é considerado seco |
| `leituraMaxima` | `876` | Valor máximo do sensor (usado no cálculo da %) |
| `mostrarLeituras` | `true` | `false` mostra apenas as mudanças de estado |

### Pinagem

| Pino | Função |
| --- | --- |
| A0 | Leitura do sensor de umidade |
| D13 | Acionamento do relé |
| D7 | LED vermelho (solo seco) |
| D6 | LED verde (solo úmido) |

---

## ▶️ Como executar

### No Tinkercad
1. Monte o circuito conforme a seção **Ligações**.
2. Clique em **Código**, selecione **Texto** e cole o conteúdo de `sistema_irrigacao.ino`.
3. Clique em **Iniciar simulação** e abra o **Monitor serial**.
4. Clique no sensor de umidade e arraste o controle entre seco e úmido.

### No Arduino físico
1. Abra `sistema_irrigacao.ino` na Arduino IDE (o arquivo deve estar em uma pasta chamada `sistema_irrigacao`).
2. Selecione a placa **Arduino Uno** e a porta correta.
3. Clique em **Carregar** e abra o Monitor Serial em **9600 baud**.
4. Calibre o `limiar` e o `leituraMaxima` com os valores do seu sensor no solo real.

---

## 🖥️ Exemplo de saída no Monitor Serial

```
=== Sistema de Irrigacao Automatizada ===
Irriga quando a umidade ficar abaixo de 45%
Monitorando o solo...

Umidade: 69%
[ok] SOLO UMIDO - LED verde - nao precisa irrigar
Umidade: 0%
[!] SOLO SECO - LED vermelho
[>] COMECOU A IRRIGAR - valvula aberta
Umidade: 92%
[ok] SOLO UMIDO - LED verde
[x] PAROU DE IRRIGAR - valvula fechada
```

---

## ⚠️ Observações para a versão física

- No sensor real, os valores de leitura são diferentes dos do Tinkercad: **recalibre** o `limiar`.
- O **diodo 1N4007** em paralelo com a válvula é obrigatório para proteger o relé e o Arduino.
- Ligue o regulador de 5 V no **pino 5V** do Arduino e confira a tensão com um multímetro antes.
- Sondas resistivas (YL-69) corroem com o tempo; um **sensor capacitivo** dura muito mais.
- Não use o USB e o regulador ao mesmo tempo no pino 5V.

---

## 📁 Estrutura do repositório

```
sistema_irrigacao/
├── sistema_irrigacao.ino   # código do Arduino
└── README.md               # este arquivo
```

---

## 🚀 Melhorias futuras

- [ ] Tempo máximo de rega e pausa de segurança
- [ ] Modo de baixo consumo (sleep) para a versão solar
- [ ] Monitoramento da tensão da bateria
- [ ] Display LCD com a umidade em tempo real
- [ ] Monitoramento remoto com ESP8266/ESP32