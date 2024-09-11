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

bool first_forwards = true;

bool turn_direction;
bool startsred = true;

int col = 2;

int count_laps = 0;
//Ultrasonic pins:
const int ultra_R_echo = 7;
const int ultra_R_trig = 8;

const int ultra_F_echo = 9;
const int ultra_F_trig = 10;

const int ultra_L_echo = 11;
const int ultra_L_trig = 12;

const int ultra_B_echo = 13;
const int ultra_B_trig = 14;

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
  //Comunication setup:
  Serial.begin(100000);
  Wire.begin();
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

  float dis_F_inic = medir_distancia('F');
  //float dis_R_inicial = medir_distancia('R');
  //float dis_L_inicial = medir_distancia('L');

  //delay(100);


  // 1 if red 0 if green
  if (dis_F_inic > 130){
    col = 2;
  }else{
    col = receiveColor();
  }
 

  // First check that gets direction and dodges in case of immeadite obstacle
  if (first_forwards) {
    first_dodge(col);
  } 

  if (startsred and turn_direction) {
    left_turn_end(130, 60, 950);
    align_back(120, 80);

  }
  if (startsred and !turn_direction) {
    left_turn_end(130, 125, 950);
    align_back(120, 80);

  }
  col = receiveColor();


  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  delay(10000);
  //go = false;

}

void align_back(int v, int umbral) {
  float temp_back = medir_distancia('B');
  while(temp_back > umbral) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, v);
    delay(20);
    temp_back = medir_distancia('B');
  }
}


void first_dodge(int color) {

  if (color == 1) { // red
    dodge_right(128, 80, 900, 18);
    startsred = true;

  } else if (color == 0) {
    dodge_left(128, 80, 800, 18);
    startsred = false;
    
  } else {
    float dis_F_tem = medir_distancia('F');

    while (dis_F_tem > 90) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 130);
      delay(20);

      myservo.write(86);
      dis_F_tem = medir_distancia('F');
    }
    startsred = true; // let luck decide
      // move forwards and turn

  }
  float dis_R_inicial = medir_distancia('R');
  float dis_L_inicial = medir_distancia('L');

  if (dis_R_inicial > dis_L_inicial) {
    turn_direction = false;
  } else {
    turn_direction = true;
  }

  
  myservo.write(86);
}

float proportional_right(float kp, float target, int stationary) {
  float error;
  error = medir_distancia('R') - target;
  float returnval = stationary + (error*kp);
  
  if (returnval > 120){
    return 120;
  } else {
    return returnval;
  }


}

float proportional_left(float kp, float target, int stationary) {
  float error;
  error = medir_distancia('L') - target;

  float returnval = stationary - (error*kp);

  if (returnval < 70){
    return 70;
  } else {
    return returnval;
  }

}

void dodge_right(int v, int umbral1, int time, float target) {
  float dis_F_tem = medir_distancia('F');
  float dis_R_tem = medir_distancia('R');

  float count = time/20;

  while ((count > 0 and dis_R_tem > 10)) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v+15);
    delay(20);

    myservo.write(proportional_right(0.8, target, 85));
    dis_R_tem = medir_distancia('R');
    dis_F_tem = medir_distancia('F');
    count = count-1;
  }
  myservo.write(75);
  while(dis_F_tem > umbral1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v+4);
    delay(20);
    dis_F_tem = medir_distancia('F');
  }

  myservo.write(86);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

}


void dodge_left(int v, int umbral1, int time, float target) {
  float dis_F_tem = medir_distancia('F');
  float dis_L_tem = medir_distancia('L');

  float count = time/20;

  while ((count > 0 and dis_L_tem > 10)) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v+15);
    delay(20);

    myservo.write(proportional_left(0.65, target, 85));
    dis_L_tem = medir_distancia('R');
    dis_F_tem = medir_distancia('F');
    count = count-1;
  }
  analogWrite(ENA, v+4);
  myservo.write(125);
  delay(350);
  while(dis_F_tem > umbral1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v+4);
    delay(20);
    dis_F_tem = medir_distancia('F');
  }

  myservo.write(86);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

}




// FUNCTION TO USE DOWN HERE AND UP HERE

void left_turn_end(int v, int angle, int time) {

  myservo.write(angle);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, v);
  delay(time);

  float R = medir_distancia('R');
  float L = medir_distancia('L');
  float add = R+L;

  while (add > 105 or add < 70) {
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
    add = R+L;
    
  }

  myservo.write(angle);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, v);
  delay(700);
  myservo.write(86);

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

  //right_turn_end(134, 125);

}

void right_turn_end(int v, int angle, int time) {

  myservo.write(angle);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, v);
  delay(time);

  float R = medir_distancia('R');
  float L = medir_distancia('L');
  float add = R+L;

  while (add > 105 or add < 70) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, v);
    delay(112); // 120
    myservo.write(angle);
    angle = angle - 1;
    if (angle < 88) {
      angle = 88;
    }
    R = medir_distancia('R');
    L = medir_distancia('L');
    add = R+L;
    
  }

  myservo.write(angle);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, v);
  delay(700);
  myservo.write(86);

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


int receiveColor() {
  Wire.requestFrom(0x08, 1); // Request 1 byte from the Nicla Vision

  if (Wire.available()) {
    int colorData = Wire.read(); // Read the received data
    return colorData;
  } else {
    return -1; // Return -1 if no data received (indicates an error)
  }
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
