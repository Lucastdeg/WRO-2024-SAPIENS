#include <Servo.h>
#include <Wire.h>

//const int delay_uni = 200;
//I2C:
const int i2c_dir = 8;
const int BUFFER_SIZE = 1; // Tamaño del buffer para almacenar los datos recibidos
int receivedData[BUFFER_SIZE];
bool go = false;

float R_tem;
float L_tem;

int umbralcount;

float PrevB;
float PrevF;
float PrevR;
float PrevL;

bool turn_direction;

bool true_turn = true;
int counting_turns = 0;

//Ultrasonic pins:
const int ultra_R_echo = 7;
const int ultra_R_trig = 8;

const int ultra_F_echo = 9;
const int ultra_F_trig = 10;

const int ultra_L_echo = 11;
const int ultra_L_trig = 12;

const int ultra_B_echo = 16;
const int ultra_B_trig = 17;

const int count_led = 14;

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
  for (int i = 7; i < 15; i++){
    if(i % 2 == 0) pinMode(i, OUTPUT);
    else pinMode(i, INPUT);
  }  

  pinMode(16, INPUT);
  pinMode(17, OUTPUT);

  //light indicators
  //pinMode(13, OUTPUT);
  //pinMode(14, OUTPUT);

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

  umbralcount = 0;

  delay(1000);
  first_turn(151, 92, 176);



  if (turn_direction) {
    while (counting_turns < 12) { // 12
      first_turn(148, 99, 165);
    }
  } else {
    while (counting_turns < 12) { // 12
      first_turn(148, 99, 165);
    }
  }

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 114);
  delay(5500);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 110);
  delay(100);
  
  breaking(300);

  delay(500);
  go = false;

}

void first_turn(int v, int umbral1, int umbral2) {
  float dis_F_tem = medir_distancia('F');
  float dis_B_tem = medir_distancia('B');
  float umbralbase = umbral1;
  R_tem = medir_distancia('R');
  L_tem = medir_distancia('L');

  Regulator regular(0.20, 0, 45, 83); // 0.14


  while (dis_F_tem > (umbral1+5) and dis_B_tem < (umbral2-5)) {

    umbralcount = 0;
    float posicion = regular.Steer(R_tem, L_tem);

    myservo.write(posicion);

    if (true_turn) {
      counting_turns = counting_turns + 1;
      true_turn = false;
      digitalWrite(count_led, HIGH);
    }

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);
    delay(20);

    digitalWrite(count_led, LOW);


    if (medir_distancia('L') > 100 or medir_distancia('R') > 100) {
      break;
    }
    L_tem = medir_distancia('L');
    R_tem = medir_distancia('R');
    dis_F_tem = medir_distancia('F');
    dis_B_tem = medir_distancia('B');

    if (medir_distancia('R') < 20 or medir_distancia('L') < 20) {
      umbral1 = umbralbase - 5;
    } else {
      umbral1 = umbralbase;
    }

    save_crash(v);
  }

  if (medir_distancia('L') > medir_distancia('R')) {
    turn_direction = true;
  }
  else {
    turn_direction = false;
  }
  if (turn_direction) {
    left_turn(v, 60, 0);
  } 
  else {
    left_turn(v, 60, 0);
    // This should be right turn
  }


}

void left_forwards(int v, int umbral) {
  float dis_F_tem = medir_distancia('F');
  R_tem = medir_distancia('R');
  L_tem = medir_distancia('L');

  Regulator regular(0.18, 0, 45, 83);

  myservo.write(86);

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
    delay(20);
    count = count-1;
  }

  left_turn_end(v, angle);

  

}

void left_turn_end(int v, int angle) {
  float R = medir_distancia('R');
  float L = medir_distancia('L');
  true_turn = true;
  if (R > 100) {
    R = 45;
    PrevR = 45;
  } else {
    PrevR = R;
  }
  if (L > 100) {
    L = 45;
    PrevL = 45;
  } else {
    PrevL = L;
  }
  float add = R+L;
  
  umbralcount = umbralcount+1;

  while ((add > 125 or add < 15) and umbralcount < 1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);
    delay(112); // 120
    myservo.write(angle);
    angle = angle + 1;
    if (angle > 84) {
      angle = 84;
    }
    R = medir_distancia('R');
    L = medir_distancia('L');


    if (L > 100 or L < 4) {
      L = PrevL;
    } else {
      PrevL = L;
    }

    if (R > 100 or R < 4) {
      R = PrevR;
    } else {
      PrevR = R;
    }
    add = R+L;
    //digitalWrite(count_led, LOW);
    
  }

}

void save_crash(int v) {
  float R = medir_distancia('R');
  if (R < 10) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    delay(1000);
    myservo.write(60);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    delay(1300);
    myservo.write(86);
    true_turn = false;
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
    case 'B':
      trig = ultra_B_trig;
      echo = ultra_B_echo;
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
