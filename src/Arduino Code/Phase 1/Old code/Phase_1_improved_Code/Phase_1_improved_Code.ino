#include <Servo.h>
#include <Wire.h>
const int delay_uni = 200;
//I2C:
const int i2c_dir = 8;
const int BUFFER_SIZE = 1; // Tamaño del buffer para almacenar los datos recibidos
int receivedData[BUFFER_SIZE];
bool go = false;

//Ultrasonic pins:
const int ultra_R_echo = 7;
const int ultra_R_trig = 8;

const int ultra_F_echo = 9;
const int ultra_F_trig = 10;

const int ultra_L_echo = 11;
const int ultra_L_trig = 12;

const int Light1 = 13;
const int Light2 = 14;

unsigned long time_a, time_b, time_c, time_d;

//Motor pins:
const int IN1 = 2;  //AIN1
const int ENA = 3;  // PWMA
const int IN2 = 4;  //AIN2

//Steering servo pin:
int max_R_angle = 125; //max giro R
int max_L_angle = 67; //max giro L
Servo myservo;
const int servo_pin = 6;

// Defining class for PD regulation
class Regulator {
public:
  // Defining parameterized constructor with Kp, Kd, and Target (The target distance between walls) as parameters
  Regulator(float kp, float kd, float target)
    : Kp(kp), Kd(kd), Target(target), Pre_ErrorR(0), Pre_ErrorL(0) {} // Defining all the parameters we will use for the steer

  // Actual Function that will return the steer for the robot
  float Steer(float Distance_R, float Distance_L){

    // Proportional Error
    float errorR = Target - Distance_R;
    float errorL = Target - Distance_L;

    // Derivative Error
    float derivativeR = errorR - Pre_ErrorR;
    float derivativeL = errorL - Pre_ErrorL;

    // PD for Right Sensor
    float R_Correction = Kp * errorR + Kd * derivativeR;

    // PD for Left Sensor
    float L_Correction = Kp * errorL + Kd * derivativeL;

    float F_Correction = L_Correction - R_Correction;
    // The bigger the distance right the more it should turn right, thats why we subtract the (-)R_Correction to the 90
    // Meanwhile if the distance to the left was real big, it means we have to add subtract the 90, which is why we add (-)L_Correction

    if (F_Correction > 0){
      F_Correction = pow(F_Correction, 1.1);
    }
    
    // Defining final servo position
    float Servo_Pos = 85 + F_Correction;
    // 90 is the where it should stay

    if (Servo_Pos < 70){
      Servo_Pos = 70;
    }
    else if (Servo_Pos > 120){
      Servo_Pos = 120;
    }

    // Although the Kp and Kd are designed to limit the Servo to a range of 80-120 degrees, the Derivative can increase enough to nullify that, for this reason we add a cut off

    return Servo_Pos;
  }

private:
  float Kp;
  float Kd;
  float Target;

  float Pre_ErrorR;
  float Pre_ErrorL;
};

void setup() {
  //Comunication setup:
  Serial.begin(115200);
  Wire.begin(i2c_dir);
  Wire.onReceive(receiveEvent);
  pinMode(5, INPUT);

  //motor pin setup:
  for(int i = 2; i<5; i++){
    pinMode(i, OUTPUT);
  }

  //ultrasonic pins setup:
  for (int i = 7; i < 13; i++){
    if(i % 2 == 0) pinMode(i, OUTPUT);
    else pinMode(i, INPUT);
  }  

  //light indicators
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);

  //Steering servo setup:
  myservo.attach(servo_pin);
  myservo.write(80);
  delay(500);
}

void loop() {
  while(go == false){
    if(digitalRead(5)) go = true;
    delay(100);
  }

  float dis_F_inicial = medir_distancia('F');
  float dis_R_inicial = medir_distancia('R');
  float dis_L_inicial = medir_distancia('L');

  for(int i=0; i<12; i++){
    acelerar_recto(85, 120);
    auto_giro(134);
  }
  
  delay(500);
  go = false;
}

//Funciones:
void acelerar_recto(float umbral, int v){
  float dis_F_actual = medir_distancia('F');
  float dis_R_actual = medir_distancia('R');
  float dis_L_actual = medir_distancia('L');
  digitalWrite(Light1, HIGH);

  float dis_R_tem, dis_L_tem;

  // Here goes the Kp and Kd values with the target distance
  // Regulator regular(0.43, -0.07, 40);
  Regulator regular(0.15, -0.01, 40);

  while(dis_F_actual > umbral+2){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);
    delay(20);
    dis_R_tem = medir_distancia('R');
    dis_L_tem = medir_distancia('L');

    // Mejorar correcion de posicion
    float posicion = regular.Steer(dis_R_tem, dis_L_tem);

    myservo.write(posicion);

    dis_F_actual = medir_distancia('F');

  } 
  digitalWrite(Light1, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(delay_uni);
}

void acelerar(int v){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, v);
}

void auto_giro(int v){
  float time = 1380 * (v/100);
  float angle;

  float dis_R_actual = medir_distancia('R');
  delay(5);
  float dis_L_actual = medir_distancia('L');
  delay(5);
  float dis_F_actual = medir_distancia('F');
  delay(5);
  digitalWrite(Light2, HIGH);
  if(dis_R_actual > dis_L_actual){
    float angle = max_R_angle - (max_R_angle * v/1000);
    myservo.write(angle-4);
    delay(10);
  }
  else{
    float angle = max_L_angle - (max_L_angle * v/1000);
    myservo.write(angle-4);
    delay(10);
  }
  acelerar(v);
  delay(time);
  myservo.write(91);
  delay(20);

  float dis_LAT_tem = medir_distancia('R') + medir_distancia('L');
  while(dis_LAT_tem < 120 and dis_LAT_tem > 80){
    acelerar(v)
    myservo.write(84);
    delay(20);
    myservo.write(86);
    delay(20);
    dis_LAT_tem = medir_distancia('R') + medir_distancia('L');
  }
  digitalWrite(Light2, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(10);
}

/*
void estacionarse(float dis_F, float dis_B){
  float dis_f_tem = dis_F / medir_distancia('F') - 10;
  delay(5);
  float dis_b_tem = dis_B / medir_distancia('B') - 10;
  delay(5);

  while(dis_f_tem < 1.2 and dis_f_tem > 0.8 or dis_b_tem > 1.2 and dis_b_tem < 0.8){
    acelerar(90);
    myservo.write(95);
    delay(20);
    myservo.write(85);
    delay(20);
    dis_f_tem = dis_F / medir_distancia('F') - 10;
    dis_b_tem = dis_B / medir_distancia('B') - 10;
  }
  stop();
}
*/

float medir_distancia(char dir){
  int trig = 0;
  int echo = 0;
  switch(dir){
    case 'F':
      trig = ultra_F_trig;
      echo = ultra_F_echo;
      break;
    case 'R':
      trig = ultra_R_trig;
      echo = ultra_R_echo;
      break;
    case 'L':
      trig = ultra_L_trig;
      echo = ultra_L_echo;
      break;
    default:
      return 0.0;
  }
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  float distance_cm = duration * 0.034 / 2;
  return distance_cm * 0.95;
}

void receiveEvent(int numBytes) {
  Serial.print("numbytes = ");
  Serial.println(numBytes);
  if(numBytes <= BUFFER_SIZE) { // Asegurarse de que la cantidad de bytes recibidos no exceda el tamaño del buffer
    for(int i = 0; i < numBytes; i++) {
      receivedData[i] = Wire.read(); // Leer los datos recibidos y almacenarlos en el buffer
    }
  }
  else{
    // Si se reciben más datos de los que puede manejar el buffer, descartar los datos adicionales
    while (Wire.available()) {
      Wire.read();
    }
  }
  
  if(receivedData[0] == 1){
    myservo.write(130);
    }
  else if(receivedData[0] == 0){
    myservo.write(70);
    }
      
}

void break_car(){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH );
      delay(700);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      delay(100);
}

void stop(){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH );
      delay(500);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      delay(100);
      exit(0);
}

long medir_tiempo(char dir){
  int trig = 0;
  int echo = 0;
  switch(dir){
    case 'F':
      trig = ultra_F_trig;
      echo = ultra_F_echo;
      break;
    case 'R':
      trig = ultra_R_trig;
      echo = ultra_R_echo;
      break;
    case 'L':
      trig = ultra_L_trig;
      echo = ultra_L_echo;
      break;
    default:
      return 0.0;
  }
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return duration/2;
}







