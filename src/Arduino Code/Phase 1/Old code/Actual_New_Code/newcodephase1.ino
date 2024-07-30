#include <Servo.h>
#include <Wire.h>
const int delay_uni = 200;
//I2C:
const int i2c_dir = 8;
const int BUFFER_SIZE = 1; // Tamaño del buffer para almacenar los datos recibidos
int receivedData[BUFFER_SIZE];
bool go = false;

float R_tem;
float L_tem;

bool turn_direction;

//Ultrasonic pins:
const int ultra_R_echo = 7;
const int ultra_R_trig = 8;

const int ultra_F_echo = 9;
const int ultra_F_trig = 10;

const int ultra_L_echo = 11;
const int ultra_L_trig = 12;

//const int Light1 = 13;
//const int Light2 = 14;

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
  Regulator(float kp, float kd, float target, int stationary)
    : Kp(kp), Kd(kd), Target(target), Stationary(stationary), Pre_ErrorR(0), Pre_ErrorL(0) {} // Defining all the parameters we will use for the steer

  // Actual Function that will return the steer for the robot
  float Steer(float Distance_R, float Distance_L){

    // Proportional Error
    float errorR = Distance_R - Distance_L;

    // Derivative Error
    float derivativeR = errorR - Pre_ErrorR;

    // PD for Right Sensor
    float R_Correction = Kp * errorR + Kd * derivativeR;

    // PD for Left Sensor

    float F_Correction = R_Correction;
    // The bigger the distance right the more it should turn right, thats why we subtract the (-)R_Correction to the 90
    // Meanwhile if the distance to the left was real big, it means we have to add subtract the 90, which is why we add (-)L_Correction

    if (F_Correction > 0){
      F_Correction = pow(F_Correction, 1.05);
    }
    
    // Defining final servo position
    float Servo_Pos = Stationary + F_Correction;
    // 90 is the where it should stay

    if (Servo_Pos < 70){
      Servo_Pos = 70;
    }
    else if (Servo_Pos > 120){
      Servo_Pos = 120;
    }

    // Although the Kp and Kd are designed to limit the Servo to a range of 60-120 degrees, the Derivative can increase enough to nullify that, for this reason we add a cut off

    return Servo_Pos;
  }

private:
  float Kp;
  float Kd;
  float Target;
  int Stationary;

  float Pre_ErrorR;
  float Pre_ErrorL;
};

void setup() {
  //Comunication setup:
  Serial.begin(115200);
  Wire.begin(i2c_dir);
  //Wire.onReceive(receiveEvent);
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
  myservo.write(87);
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
  first_turn(110, 80);



  if (turn_direction) {
    for(int i=0; i<11; i++){

      //change this to an infinite loop next time to check
      first_turn(110, 75);
      //left_forwards(100, 80); //speed, umbral
      //left_turn(105, 60, 1400); //speed, angle, time
      //eventually add save crash Im adding this comment here so you dont forget
    }
  }
  else {
      for(int i=0; i<11; i++){
        first_turn(110, 75);
        //breaking(3000);
      //left_forwards(100, 80); //speed, umbral
      //left_turn(105, 60, 1400); //speed, angle, time
      //eventually add save crash Im adding this comment here so you dont forget
    }
  }
  
  breaking(100);

  delay(500);
  go = false;

}

void first_turn(int v, int umbral) {
  float dis_F_tem = medir_distancia('F');
  R_tem = medir_distancia('R');
  L_tem = medir_distancia('L');

  Regulator regular(0.18, 0, 45, 86);


  while (dis_F_tem+10 > umbral) {

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);
    delay(20);
    float posicion = regular.Steer(R_tem, L_tem);

    myservo.write(posicion);


    if (medir_distancia('L') > 120 or medir_distancia('R') > 120) {
      break;
    }
    L_tem = medir_distancia('L');
    R_tem = medir_distancia('R');
    dis_F_tem = medir_distancia('F');
  }

  if (L_tem > R_tem) {
    turn_direction = true;
  }
  else {
    turn_direction = false;
  }
  if (turn_direction) {
    left_turn(v, 60, 1320);
  } 
  else {
    left_turn(v, 60, 1320);
    // This should be right turn
  }


}

void left_forwards(int v, int umbral) {
  float dis_F_tem = medir_distancia('F');
  R_tem = medir_distancia('R');
  L_tem = medir_distancia('L');

  Regulator regular(0.18, 0, 45, 86);

  myservo.write(88);

  while (dis_F_tem > umbral) {

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);

    float posicion = regular.Steer(R_tem, L_tem);

    myservo.write(posicion);


    if (medir_distancia('L') > 95 or medir_distancia('R') > 95) {
      break;
    }
    L_tem = medir_distancia('L');
    R_tem = medir_distancia('R');
    dis_F_tem = medir_distancia('F');
  }
}

void left_turn(int v, int angle, int time) {
  float kp = 4;
  // time = get_time(v, time, R_tem, L_tem, 4, turn_direction); // velocity, base time, right reading, left reading, kp the bigger the more effect

  myservo.write(angle);
  
  float count = time/20;
  while (count > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);
    count = count-1;
    delay(20);
  }

  left_turn_end(v, angle);

  

}

void left_turn_end(int v, int angle) {
  float R = medir_distancia('R');
  float L = medir_distancia('L');
  float add = R+L;

  while (add > 110 or add < 30) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);
    delay(60); // 120
    myservo.write(angle);
    angle = angle + 1;
    if (angle > 88) {
      angle = 88;
    }
    R = medir_distancia('R');
    L = medir_distancia('L');
    add = R+L;
    
  }

}


void breaking(int time) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(time);
}

// I am only copy pasting to allow for more specifications later on, either way left turn is the main function
void right_turn(int v, int angle, int time) {
  float kp = 4;
  time = get_time(v, time, R_tem, L_tem, 4, turn_direction); // velocity, base time, right reading, left reading, kp the bigger the more effect

  myservo.write(angle);
  acelerar(v);
  delay(time);

}

float get_time(int v, int base_time, float R, float L, float kp, bool left) {
  int speed_mod = (100/v);
  float error;


  if (left) {
    error = L - R;
  }
  else {
    error = R - L;
  }

  return (base_time + (error*kp))*speed_mod;

}

void acelerar(int v){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, v);
}


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
  float distance_cm = (duration*0.0343)/2;
  return distance_cm * 1; //change the 1 for any modification
}