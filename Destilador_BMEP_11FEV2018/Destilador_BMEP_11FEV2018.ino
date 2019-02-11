#include <TimerOne.h>   // inclui biblioteca Timer1(Dimmer)
#include <Adafruit_MAX31865.h> //Biblioteca módulo temp

//Definições sobre o termopar
Adafruit_MAX31865 max = Adafruit_MAX31865(10, 11, 12, 13);
#define RREF      430.0
#define RNOMINAL  99.75

//Definições do dimmer
#define triacApin 3 // Define que o Dimmer será comandado pelo pino 3
int frequencia = 60;
int stateTriacA = 0;
int power = 0;  //inicializa variavel que controla potencia na lampada com 0 (lampada apagada)
int destilador = 0;
int flag = 0;
unsigned long t0 = 0;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete]


void setup() {
pinMode(triacApin, OUTPUT);
digitalWrite(triacApin, LOW);
max.begin(MAX31865_3WIRE); //Inícia o termopar.
Timer1.initialize(); // Inicializa a biblioteca timer 1(Dimmer)
attachInterrupt(0, zero_cross_detect, FALLING); // Interrupção para o pino 2(Zero cross detection, Dimmer)
Serial.begin(9600);
inputString.reserve(200);
}


int stringToNumber(String thisString)
{
  int i, value = 0, length;
  length = thisString.length();
  for(i=0; i<length; i++)
  {
    value = (10*value) + thisString.charAt(i)-(int) '0';;
  }
  return value;
}     


void loop() {
unsigned long t1 = millis();

while((millis() - t1) <= 2000){
  
}
  if(destilador == 1){
    static unsigned long ult_tempo=0;
    unsigned long tempo = millis();
    unsigned long tempo_on = tempo - t0;
    int tempo_on_min = tempo_on/60000;
    Serial.println("Destilador Ligado");
    Serial.print(power);
    Serial.print(";");
    temperatura();
    Serial.print("Tempo: ");
    Serial.println(tempo_on_min);

    if(tempo_on/1000 <= 90){
      power = 50.00;
    }
    else if(tempo_on/1000 > 90 && tempo_on/1000 < 600){
      power = 35.00;
    }

    else if(tempo_on/1000 > 600 && tempo_on/1000 < 1200){
      power = 45.00;
    }

    else if(tempo_on/1000 > 1200 && tempo_on/1000 < 1320){
      power = 55.00;
    }

    else if(tempo_on/1000 > 1320 && tempo_on/1000 < 2100){
      power = 75.00;
    }

    else if(tempo_on/1000 > 2100){
      power = 10.00;
      Serial.println("Tempo excedido");
    }
  }
    
  else{
    Serial.println("Destilador Desligado");
  }


  // Recebimento de dados pela a porta serial (Tratamento)
  if (stringComplete) 
  {
    int sizer=sizeof(inputString);
    if(inputString.startsWith("#pwr"))
    {
      // Pega o valor numero da potência a partir da posição 4 depois do #pwr
      power=stringToNumber(inputString.substring(4,sizer));
    }
    if(inputString.startsWith("#data"))
    {
      //Serial.print("stepCount = ");
      Serial.print(power);
      Serial.print(";");
      temperatura();
    }
    if(inputString.startsWith("#pwr"))
    {
      // Pega o valor numero da potência a partir da posição 4 depois do #pwr
      power=stringToNumber(inputString.substring(4,sizer));
    }
    if(inputString.startsWith("#on"))
    {
      destilador = 1;
      t0 = millis();
      //Colocar aqui a rampa de temperatura
    }
    if(inputString.startsWith("#off"))
    {
      destilador = 0;
      power = 01.00;
    }
    if(inputString.startsWith("#temp"))
    {
      temperatura();
    }
    
    // limpa a string e libera a nova recepção de dados
    inputString = "";
    stringComplete = false;
  }
}

// Função responsável por tratar o recebimento de dados da serial
void serialEvent() 
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:valv_in
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') 
    {
      stringComplete = true;
    }
  }
}

//Função responsável por ler a temperatura.
int temperatura()
{
  //Definições sobre a leitura de temperatura.
  uint16_t rtd = max.readRTD();
  float ratio = rtd;
  ratio /= 32768;
  Serial.println(max.temperature(RNOMINAL, RREF));
  
}
//Função que sincroniza o arduino com a rede elétrica para controle do dimmer.
void zero_cross_detect()
{
  if(power > 0){
    long dimtime = int(map(power,0,100,8000,150));  // calcula o tempo de delay para o disparo do TRIAC 7950,100
    Timer1.attachInterrupt(gateTRIAC, dimtime);  //associa a funcao gateTRIAC com Interrupcao do TIMER1
    Timer1.start();  // inicia contagem TIMER1
  }

  else{
    digitalWrite(triacApin, LOW);   // mantem gate do TRIAC desativado.
    Timer1.stop();
  }
}

//Função que dispara o triac.
void gateTRIAC ()
{  
// trata interrupcao do TIMER1 gerando pulso no gate do TRIAC
  digitalWrite(triacApin, HIGH);  // dispara o Triac
  delayMicroseconds(5);  // aguarda 5 microsegundos para garantir disparo do TRIAC
  digitalWrite(triacApin, LOW);   // desabibilta gate do TRIAC
  Timer1.stop();
}
 
int Concatena_Numeros(int variavel1, int variavel2)
{
  int resultado;
  resultado = ((variavel1-'0')*10)+(variavel2-'0');
  return resultado;

}
