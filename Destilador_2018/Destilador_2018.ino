#include <TimerOne.h>   // inclui biblioteca Timer1(Dimmer)
#include <Adafruit_MAX31865.h> //Biblioteca módulo temp

//Definições sobre o termopar
Adafruit_MAX31865 max = Adafruit_MAX31865(10, 11, 12, 13);
#define RREF      430.0
#define RNOMINAL  99.75

#define valv_in 9
#define valv_out 8

//Definições do dimmer
#define triacApin 3 // Define que o Dimmer será comandado pelo pino 3
int frequencia = 60;
int stateTriacA = 0;
int power = 0;  //inicializa variavel que controla potencia na lampada com 0 (lampada apagada)
float volume = 0;

int destilador = 0;

//Sensor de luz 
#define sensor A1


String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete]

int flag = 0;

void setup()
{
  pinMode(sensor, INPUT);
  pinMode(triacApin, OUTPUT);
  pinMode(valv_in, OUTPUT);
  pinMode(valv_out, OUTPUT);
  
  max.begin(MAX31865_3WIRE); //Inícia o termopar.
  digitalWrite(triacApin, LOW);
  Serial.begin(9600);
  //Serial.println("Controle de TRIAC.\r\nDigite o Percentual de Brilho (duas casas) Ex.: 25");

  Timer1.initialize(); // Inicializa a biblioteca timer 1(Dimmer)

  attachInterrupt(0, zero_cross_detect, FALLING); // Interrupção para o pino 2(Zero cross detection, Dimmer)
  //attachInterrupt(digitalPinToInterrupt(interruptFimCurso), fim_curso, LOW); //Interrupção fim de curso

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

 
void loop()
{
  if(destilador == 1){
    digitalWrite(valv_in, HIGH);
  }
  else{
    digitalWrite(valv_in, LOW);
  }
  //Serial.println(analogRead(sensor));
  if(analogRead(sensor) > 22)  //Verifica se o volume de água está acima do nível do sensor.
  {
    //Serial.print("#");
    delay(50);
    if(analogRead(sensor) > 22){
      //Serial.print("#");
      volume += 2;
      Serial.print(power);
      Serial.print(";");
      Serial.print(volume);
      Serial.print(";");
      Serial.print(analogRead(sensor));
      Serial.print(";");
      temperatura();
      digitalWrite(valv_in, LOW);
      digitalWrite(valv_out, HIGH);
      delay(1500);
      digitalWrite(valv_out, LOW);
      delay(2000);
      digitalWrite(valv_out, HIGH);
      delay(1500);
      digitalWrite(valv_out, LOW);
      delay(2000);
      digitalWrite(valv_out, HIGH);
      delay(1500);
      digitalWrite(valv_out, LOW);
    }
    }
  else{
    digitalWrite(valv_in, HIGH);
    digitalWrite(valv_out, LOW);
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
    if(inputString.startsWith("#test"))
    {
      // Envia a string de teste que confirdema que os            time.sleep(1) sistemas estão funcionando
      Serial.println("TESTOK!");
    }
    if(inputString.startsWith("#data"))
    {
      //Serial.print("stepCount = ");
      Serial.print(power);
      Serial.print(";");
      Serial.print(volume);
      Serial.print(";");
      Serial.print(analogRead(sensor));
      Serial.print(";");
      temperatura();
    }
    if(inputString.startsWith("#reset"))
    {
      inputString = "";
      stringComplete = false;
    }
    if(inputString.startsWith("#on"))
    {
      destilador = 1;
    }
    if(inputString.startsWith("#off"))
    {
      destilador = 0;
    }
    if(inputString.startsWith("#clean"))
    {
      digitalWrite(valv_in, LOW);
      digitalWrite(valv_out, HIGH);
      delay(1500);
      digitalWrite(valv_out, LOW);
      delay(2000);
      digitalWrite(valv_out, HIGH);
      delay(1500);
      digitalWrite(valv_out, LOW);
      delay(2000);
      digitalWrite(valv_out, HIGH);
      delay(1500);
      digitalWrite(valv_out, LOW);
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
