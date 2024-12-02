

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>


#define LED_PIN 2           // Pino de controle da fita de LED RGB
#define LED_COUNT 12        // Número de LEDs na fita


#define leitor_red 8        // Sensor correspondente à cor vermelha
#define leitor_green 11     // Sensor correspondente à cor verde
#define leitor_blue 9       // Sensor correspondente à cor azul
#define leitor_yellow 10    // Sensor correspondente à cor amarela


LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


int contador = 0;
int recorde = 0;  // Variável para armazenar o recorde
unsigned long tempoAnterior = 0;
bool sensorAtivado = false;  // Flag para indicar se o sensor foi ativado


// Variáveis para o estado dos leitores
bool estado_leitor_red = false;
bool estado_leitor_green = false;
bool estado_leitor_blue = false;
bool estado_leitor_yellow = false;


// Variáveis para manter o estado dos LEDs
bool led_red_aceso = false;
bool led_green_aceso = false;
bool led_blue_aceso = false;
bool led_yellow_aceso = false;


// Variáveis para controle de incremento único
bool incremento_red = false;
bool incremento_green = false;
bool incremento_blue = false;
bool incremento_yellow = false;


void atualizarLCD() {
  lcd.setCursor(10, 0);
  lcd.print("  ");  // Limpa o contador
  lcd.setCursor(10, 1);
  lcd.print("  ");  // Limpa o recorde


  lcd.setCursor(10, 0);
  lcd.print(contador < 10 ? "0" : "");
  lcd.print(contador);


  lcd.setCursor(10, 1);
  lcd.print(recorde < 10 ? "0" : "");
  lcd.print(recorde);


  Serial.print("Atual: ");
  Serial.print(contador < 10 ? "0" : "");
  Serial.print(contador);
  Serial.print(" | Recorde: ");
  Serial.println(recorde < 10 ? "0" : "");
  Serial.println(recorde);
}


void setup() {
  pinMode(leitor_red, INPUT);
  pinMode(leitor_green, INPUT);
  pinMode(leitor_blue, INPUT);
  pinMode(leitor_yellow, INPUT);


  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Atual:");
  lcd.setCursor(0, 1);
  lcd.print("Recorde: ");


  strip.begin();
  strip.show();


  Serial.begin(9600);
  randomSeed(analogRead(0));
}


void acenderLEDaleatorio() {
  int ledEscolhido = random(0, 4); // Escolhe um número entre 0 e 3 para as cores


  // Apaga todos os LEDs antes de acender a nova cor
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }


  // Acende os LEDs de acordo com a cor escolhida
  if (ledEscolhido == 0) {  // Azul
    for (int i = 0; i < 3; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 255));  // Azul
    }
    led_blue_aceso = true;
    led_red_aceso = led_green_aceso = led_yellow_aceso = false; // Desativa os outros LEDs
    incremento_blue = false;  // Resetar controle de incremento
  } else if (ledEscolhido == 1) {  // Vermelho
    for (int i = 3; i < 6; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));  // Vermelho
    }
    led_red_aceso = true;
    led_blue_aceso = led_green_aceso = led_yellow_aceso = false;
    incremento_red = false;  // Resetar controle de incremento
  } else if (ledEscolhido == 2) {  // Amarelo
    for (int i = 6; i < 9; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 0));  // Amarelo
    }
    led_yellow_aceso = true;
    led_red_aceso = led_blue_aceso = led_green_aceso = false;
    incremento_yellow = false;  // Resetar controle de incremento
  } else if (ledEscolhido == 3) {  // Verde
    for (int i = 9; i < 12; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));  // Verde
    }
    led_green_aceso = true;
    led_red_aceso = led_blue_aceso = led_yellow_aceso = false;
    incremento_green = false;  // Resetar controle de incremento
  }


  // Atualiza a fita de LEDs
  strip.show();
}


void verificarRecorde() {
  if (contador > recorde) {
    recorde = contador;
    atualizarLCD();
  }
}


void zerarContadorSeMovimentoComLEDApagado() {
  if (contador > 0) {  // Verifica se o contador está maior que 0
    if ((digitalRead(leitor_red) == LOW && !led_red_aceso) ||
        (digitalRead(leitor_green) == LOW && !led_green_aceso) ||
        (digitalRead(leitor_blue) == LOW && !led_blue_aceso) ||
        (digitalRead(leitor_yellow) == LOW && !led_yellow_aceso)) {
      contador = 0;
      atualizarLCD();
    }
  }
}


void loop() {
  unsigned long tempoAtual = millis();


  // Alterna o LED aleatoriamente a cada 1.5 segundos (1500ms)
  if (tempoAtual - tempoAnterior >= 1500) {
    acenderLEDaleatorio();
    tempoAnterior = tempoAtual;
  }


  // Atualiza o estado dos sensores
  estado_leitor_red = digitalRead(leitor_red);
  estado_leitor_green = digitalRead(leitor_green);
  estado_leitor_blue = digitalRead(leitor_blue);
  estado_leitor_yellow = digitalRead(leitor_yellow);


  // Zera o contador se algum sensor detectar movimento enquanto o LED correspondente está apagado
  zerarContadorSeMovimentoComLEDApagado();


  // Verifica se o sensor detecta movimento e o LED correspondente está aceso
  if (digitalRead(leitor_red) == LOW && led_red_aceso && !incremento_red) {
    contador++;
    incremento_red = true;  // Impede o incremento múltiplo
    atualizarLCD();
    while (digitalRead(leitor_red) == LOW) {
      delay(10);
    }
  }


  if (digitalRead(leitor_green) == LOW && led_green_aceso && !incremento_green) {
    contador++;
    incremento_green = true;  // Impede o incremento múltiplo
    atualizarLCD();
    while (digitalRead(leitor_green) == LOW) {
      delay(10);
    }
  }


  if (digitalRead(leitor_blue) == LOW && led_blue_aceso && !incremento_blue) {
    contador++;
    incremento_blue = true;  // Impede o incremento múltiplo
    atualizarLCD();
    while (digitalRead(leitor_blue) == LOW) {
      delay(10);
    }
  }


  if (digitalRead(leitor_yellow) == LOW && led_yellow_aceso && !incremento_yellow) {
    contador++;
    incremento_yellow = true;  // Impede o incremento múltiplo
    atualizarLCD();
    while (digitalRead(leitor_yellow) == LOW) {
      delay(10);
    }
  }


  // Verifica se o contador ultrapassou o recorde e atualiza
  verificarRecorde();
}



