#include <Thermistor.h> 
#include <Wire.h>
  

#define MPU 0x68
 

#define A_R 16384.0 // 32768/2
#define G_R 131.0 // 32768/250
 
//
#define RAD_A_DEG = 57.295779
 


int16_t AcX, AcY, AcZ, GyX, GyY, GyZ,Tmp;
 
//Angulos
float Acc[2];
float Gy[3];
float Angle[3];

String valores;

long tiempo_prev;
float dt;

void setup()
{
Wire.begin(); // D2(GPIO4)=SDA / D1(GPIO5)=SCL
Wire.beginTransmission(MPU);
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission(true);
Serial.begin(9600);
pinMode(13, INPUT); // Setup for leads off detection LO +
pinMode(12, INPUT); // Setup for leads off detection LO -
}

void loop()
{
 //Monitoramento de Temperatura:
//  temperatura_ntc()
  monitoramento_ecg();
 
 //Monitoramento de Posição 
   Wire.beginTransmission(MPU);
   Wire.write(0x3B);
   Wire.endTransmission(false);
   Wire.requestFrom(MPU,6,true);   
   AcX=Wire.read()<<8|Wire.read(); 
   AcY=Wire.read()<<8|Wire.read();
   AcZ=Wire.read()<<8|Wire.read();
   Tmp=Wire.read()<<8|Wire.read();

   Tmp = Serial.print(Tmp/340.00+36.53);
 
   // Calculo dos Angulos
   Acc[1] = atan(-1*(AcX/A_R)/sqrt(pow((AcY/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;
   Acc[0] = atan((AcY/A_R)/sqrt(pow((AcX/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;
 
   //Valores do Giroscópio
   Wire.beginTransmission(MPU);
   Wire.write(0x43);
   Wire.endTransmission(false);
   Wire.requestFrom(MPU,6,true);   
   GyX=Wire.read()<<8|Wire.read();
   GyY=Wire.read()<<8|Wire.read();
   GyZ=Wire.read()<<8|Wire.read();
 
   //Calculo del angulo del Giroscopio
   Gy[0] = GyX/G_R;
   Gy[1] = GyY/G_R;
   Gy[2] = GyZ/G_R;

   dt = (millis() - tiempo_prev) / 1000.0;
   tiempo_prev = millis();
 
   //Aplicando o Filtro
   Angle[0] = 0.98 *(Angle[0]+Gy[0]*dt) + 0.02*Acc[0];
   Angle[1] = 0.98 *(Angle[1]+Gy[1]*dt) + 0.02*Acc[1];

  
   Angle[2] = Angle[2]+Gy[2]*dt;
 
   valores = "90, " +String(Angle[0]) + "," + String(Angle[1]) + "," + String(Angle[2]) + ", -90";
   Serial.println(valores);
   
   delay(10);

}

void monitoramento_ecg(){
if((digitalRead(12) == 1)||(digitalRead(13) == 1)){
Serial.println('!');
}
else{
// send the value of analog input 0:
Serial.println(analogRead(A0));
}
//Wait for a bit to keep serial data from saturating
delay(1);
}
