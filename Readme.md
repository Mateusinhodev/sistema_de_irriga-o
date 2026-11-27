# 🌱👨🏽‍💻 Sistema de Irrigação Automatizada Aplicada no Campo

Projeto interdisciplinar entre os cursos de **Informática** e **Agricultura**. Um Arduino UNO lê a umidade do solo e aciona a irrigação sozinho: liga quando a terra está seca e desliga quando ela volta a ficar úmida.

---

## Equipe

| Nome | Função / Curso |
|---|---|
| **Prof. Cesar Henrique** | Professor orientador (Agricultura) |
| **Prof. Mateus Rodrigues** | Professor orientador (Informática) |
| **Gustavo Milhomens** | Aluno (Curso de Informática) |
| **Luiz Henrique** | Aluno (Curso de Agricultura) |
| **Miguel Barros** | Aluno (Curso de Agricultura) |

---

## Objetivo

Automatizar a irrigação de uma área de cultivo para que a planta receba água só quando precisa. Isso evita desperdício e excesso de água no solo e diminui o trabalho manual. O projeto une a parte de programação e eletrônica (Informática) com o manejo do solo e das culturas (Agricultura).

---

## Materiais utilizados

| Componente | Função no projeto |
|---|---|
| Arduino UNO | Microcontrolador que lê o sensor e controla o relé |
| Protoboard | Montagem e interligação do circuito |
| Sensor de umidade do solo (com módulo comparador) | Mede a umidade da terra (saída analógica **AO**) |
| Módulo relé | Liga e desliga a válvula solenoide |
| Válvula solenoide | Abre e fecha a passagem de água para o aspersor |
| Aspersor | Distribui a água sobre a área de cultivo |
| Bomba d'água | Puxa a água do poço artesiano e manda para a tubulação |
| Poço artesiano | Fonte de água da irrigação |
| Mangueiras / canos | Levam a água da bomba até a válvula e o aspersor |
| Fonte 9V | Alimentação do Arduino |
| Fonte de alimentação de nobreak | Alimentação da válvula solenoide (pelo relé) |
| Jumpers / fios | Conexões |

---

## Sistema hidráulico

A água sai do **poço artesiano**, é levada pela **bomba d'água** até a **válvula solenoide** e, quando a válvula abre, segue para o **aspersor**:

O Arduino não controla a bomba: ele só abre ou fecha a válvula solenoide, que libera ou corta a água para o aspersor.

## Ligações

| Componente | Pino do componente | Pino do Arduino |
|---|---|---|
| Sensor de umidade | VCC | 5V |
| Sensor de umidade | GND | GND |
| Sensor de umidade | **AO** (saída analógica) | **A0** |
| Relé | VCC | 5V |
| Relé | GND | GND |
| Relé | IN | **D3** |
| Bateria 9V | (+) | **VIN** (ou conector P4) |
| Bateria 9V | (−) | GND |

> ⚠️ Nunca ligue os 9V no pino **5V** do Arduino.

### Circuito da válvula solenoide (potência)

| De | Para |
|---|---|
| Fonte do nobreak (+) | **COM** do relé |
| **NA** (normalmente aberto) do relé | Terminal 1 da válvula solenoide |
| Terminal 2 da válvula solenoide | Fonte do nobreak (−) |

Quando o Arduino aciona o relé, o contato COM fecha com o NA, a válvula recebe energia e **abre a passagem de água**. Quando o relé desliga, a válvula **fecha**.

---

## Como funciona

1. O Arduino faz **10 leituras** do sensor (uma a cada 10 ms) e calcula a **média**, para estabilizar o valor.
2. A leitura bruta (0 a 1023) é convertida em **porcentagem de umidade**: solo seco = 0 %, solo encharcado = 100 %.
3. O sistema usa **histerese** (dois limites diferentes):
   - Se a umidade cair para **30 % ou menos** → o relé liga e a **válvula solenoide abre**.
   - A válvula só **fecha** quando a umidade chegar a **60 % ou mais**.
4. A cada 1 segundo o estado aparece no **Monitor Serial**.

### Por que usar histerese?

Com um único limite (por exemplo, 50 %), a umidade ficaria oscilando em volta desse valor e o relé ligaria e desligaria sem parar. Isso desgasta o relé e a válvula. Com dois limites, o sistema liga com o solo seco e só desliga quando ele estiver bem úmido.

```
Umidade (%)
 100 ┤
  60 ┤- - - - - - - - - - - ●───── desliga aqui
     │                   ╱
     │                 ╱   (irrigando)
  30 ┤- - - ●─────────╱- - - - - - liga aqui
     │     ╱
   0 ┼─────────────────────────────► tempo
```

---

## Calibração do sensor

Cada sensor e cada tipo de solo dão valores um pouco diferentes, então é importante calibrar:

1. Carregue o código no Arduino e abra o **Monitor Serial** em **9600 baud**.
2. Deixe o sensor **no ar** ou em **terra totalmente seca** e anote a `Leitura bruta` → esse é o `VALOR_SECO`.
3. Coloque o sensor em **terra encharcada** (ou em um copo d'água) e anote a `Leitura bruta` → esse é o `VALOR_MOLHADO`.
4. Altere as constantes no código:

```cpp
const int VALOR_SECO    = 1023;  // troque pelo valor medido no seco
const int VALOR_MOLHADO = 800;   // troque pelo valor medido no molhado
```

5. Se quiser, ajuste os limites de acordo com a cultura plantada (orientação da equipe de Agricultura):

```cpp
const int UMIDADE_LIGA    = 30;  // liga a irrigação
const int UMIDADE_DESLIGA = 60;  // desliga a irrigação
```

---

## Como usar

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software).
2. Monte o circuito conforme a tabela de ligações.
3. Abra o arquivo `.ino` do projeto na IDE.
4. Em **Ferramentas → Placa**, selecione **Arduino UNO** e escolha a porta correta.
5. Clique em **Carregar (Upload)**.
6. Abra o **Monitor Serial (9600 baud)** para acompanhar o funcionamento.

### Exemplo de saída no Monitor Serial

```
Leitura bruta: 1010  | Umidade: 5 %  | Irrigacao: LIGADA
Leitura bruta: 935  | Umidade: 38 %  | Irrigacao: LIGADA
Leitura bruta: 862  | Umidade: 70 %  | Irrigacao: DESLIGADA
```

---

## Parâmetros do código

| Constante | Valor padrão | Descrição |
|---|---|---|
| `rele` | `3` | Pino digital do relé |
| `sensor` | `A0` | Pino analógico do sensor |
| `VALOR_SECO` | `1023` | Leitura bruta com o solo seco |
| `VALOR_MOLHADO` | `800` | Leitura bruta com o solo encharcado |
| `UMIDADE_LIGA` | `30` | Umidade (%) em que a irrigação liga |
| `UMIDADE_DESLIGA` | `60` | Umidade (%) em que a irrigação desliga |
| `NUM_LEITURAS` | `10` | Quantidade de leituras para a média |

---

## Problemas comuns

| Problema | Possível causa / solução |
|---|---|
| Umidade sempre 0 % ou 100 % | Calibração errada ou sensor ligado no **DO** em vez do **AO** |
| Relé funciona invertido | O módulo é ativo em HIGH: inverta os `digitalWrite` do relé |
| Relé liga e desliga sem parar | Aumente a diferença entre `UMIDADE_LIGA` e `UMIDADE_DESLIGA` |
| Leituras muito instáveis | Aumente `NUM_LEITURAS` e confira as conexões na protoboard |
| Relé estala, mas a água não passa | Confira a tensão da válvula solenoide, as ligações COM/NA e a pressão da água (algumas válvulas precisam de pressão mínima para abrir) |
| Arduino reinicia ao acionar o relé | Separe a alimentação do relé/carga da alimentação do Arduino |

---

## Melhorias futuras

- Usar sensor de umidade **capacitivo**, que sofre menos corrosão que o resistivo.
- Adicionar um **display LCD** para mostrar a umidade no campo.
- Registrar os dados (cartão SD ou envio via Wi-Fi com ESP8266/ESP32).
- Usar vários sensores para áreas maiores.
- Adicionar um tempo máximo de irrigação como proteção em caso de falha do sensor.

---

## Créditos

Projeto desenvolvido em parceria entre os cursos de **Informática** e **Agricultura**, sob orientação dos professores **Cesar Henrique** (Agricultura) e **Mateus Rodrigues** (Informática), com os alunos **Gustavo Milhomens**, **Luiz Henrique** e **Miguel Barros**.