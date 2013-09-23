#ifndef SEGWAY_H
#define SEGWAY_H

#include "mbed.h"
#include "Motor.h"      // Library for the motor class
#include "PID.h"
//#include "matrix/Matrix.h"

#include <math.h>

// Mbed Pin Definitions

// Motors
// Left Motor
#define FWD_L   p21
#define PWM_L   p22
#define REV_L   p23
// Right Motor
#define FWD_R   p24
#define PWM_R   p25
#define REV_R   p26

// IMU
// Accelerometer
#define ACC_X   p16
#define ACC_Y   p18
#define ACC_Z   p20
// Gyroscope
// 1X
#define GYR_X   p19
#define GYR_Z   p17
// 4X - NOT USED @ THE MOMENT
//#define GYR_4X   p20
//#define GYR_4Y   p20

// Bluetooth
#define BT_TX   p13
#define BT_RX   p14

// Serial LCD
#define LCD_TX   p28
#define LCD_RX   p27

// PID
#define RATE	0.01

#define PI 3.14159265358979
#define TODEG 57.29577951

#define START	0
#define STOP	1
#define PAUSE	2
#define RUN	3
#define CAL	4

#define TRUE	1
#define FALSE	0

// Objects Declaration

// LEDs
//DigitalOut led1 ( LED1 );
//DigitalOut led2 ( LED2 );
//DigitalOut led3 ( LED3 );
//DigitalOut led4 ( LED4 );
//DigitalOut probe ( p5 );

//PwmOut led1 ( LED1 );
//PwmOut led2 ( LED2 );
//PwmOut led3 ( LED3 );

// IMU
AnalogIn accelerometerX ( ACC_X );
AnalogIn accelerometerY ( ACC_Y );
AnalogIn accelerometerZ ( ACC_Z );
AnalogIn gyroscopeX ( GYR_X );
AnalogIn gyroscopeZ ( GYR_Z );

// Serial Communications
Serial pc ( USBTX, USBRX );	// Serial Comm to PC
Serial bt ( BT_TX, BT_RX );	// Serial Comm to PC/Control via Bluetooth
Serial lcd ( LCD_TX, LCD_RX );	// Serial Comm to Serial LCD on top

//Motors
Motor LeftMotor ( PWM_L, FWD_L, REV_L ); // Left Motor
Motor RightMotor ( PWM_R, FWD_R, REV_R ); // Right Motor

// Timers
Timer t;
Timer master;
Timer period;

// Global Variables
double acc_scale = 0.8; // Accelerometer scale is 800 mV/g
double gyr_scale = 0.0005; // Gyroscope scale is 0.5 mV/dps (1x OUT)

double accy_offset = 0.5;
double gyrx_offset = 0.372727;

double acc_y = 0.0;
double gyr_x = 0.0;

double Ts = 0.0;

double lastError;
double iTerm;

double Kc = 50.0;
double Ti = 0.5;
double Td = 0.0;


// LTI Controller
//Matrix controllerGain(1,3);
//Matrix systemOutput(3,1);
//Matrix controlSignal(3,1);      // Control signal -  in our case u = -Ky as x = y
//double u = 0.0;     // Control signal -  in our case u = -Ky as x = y

// PID Controller
PID controller(Kc, Ti, Td, RATE); //Kc, Ti, Td, RATE

// Kalman filter variables and constants
const float Q_angle = 0.001; // Process noise covariance for the accelerometer - Sw
const float Q_gyro = 0.003; // Process noise covariance for the gyro - Sw
const float R_angle = 0.03; // Measurement noise covariance - Sv

double bias = 0;
double P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;
double y, S;
double K_0, K_1;

double loop_time = 0.0;
double angle_kalman = 0.0;
double angleCompl = 0.0;
float master_timer = 0.0;

double acc_angle = 0.0;
double gyr_rad = 0.0;
double angle = 0.0;

double targetAngle = 0.0;
double output = 0.0;

// Function Prorotypes
void InitSegway ( void );
void CalibrateSensors ( void );
void ReadSensors ( void );
void GetAngle(void);
void SetOutput(void);
void InitPID(void);
void InitMotors(void);
void InitOffsets(void);
void PrintHeader(void);
void Timers (int action);

double kalman(double newAngle, double newRate, double dtime);

void RxData(void);
void processing(void);

#endif

