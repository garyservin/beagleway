#include "segway.h"

DigitalOut led(LED1);

//void RxInterrupt() {
	//int temp = bt.getc();
//	return;
//}

int main() {
    int timerPrint = 0;

    InitSegway();					// Init modules used in this prototype

	for (;;) {
        Timers(START);				
		GetAngle();

		if (fabs(angle) > 30.0)		// Stop if falling or laying down
		{
			LeftMotor.speed(0.0);
			RightMotor.speed(0.0);
			controller.reset();
		} else {
			SetOutput();
		}

		//master.stop();
		//master_timer = master.read();

        if (++timerPrint > 0){
            timerPrint = 0;
    	    pc.printf("%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\r\n", acc_angle, gyr_x, angleCompl, angle_kalman, output);
	        //pc.printf("%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t", acc_angle, gyr_x, angle, output, u, master_timer);

        }
		Timers(STOP);

		if (pc.readable())			// For setting the PID values
			RxData();
	}
}

void InitSegway(void) {
	PrintHeader();
	InitOffsets();
	InitMotors();
	InitPID();
	//processing();
}

void PrintHeader(void) {
	pc.baud(115200); // Interface with the BBB

//	bt.baud(115200); // Interface with Remote Controller
//	bt.attach(&RxInterrupt, Serial::RxIrq);

	lcd.baud(19200); // Interface with LCD
    lcd.printf("$1   Beagleway    \n");
    lcd.printf("$2 192.168.1.250  \n");

	// Print header for the graphics names and units
	//bt.printf("accelerometer,gyroscope,complementary,kalman,difference,output,time,Ts,loop\n");
	//bt.printf("degrees,degrees/sec,ddegrees,degrees,degrees,pwm,seconds,seconds,seconds\n");

	//bt.printf("\r\n\r\n");
	//bt.printf("#    Prototipo de un Transportador Personal  \r\n");
	//bt.printf("#       con Sistema de Balance Automatico    \r\n");
	//bt.printf("#  Gary Servin Cardozo - garyservin@ieee.org \r\n");
	//bt.printf("#   Luis Sandoval - lsandovalvera@gmail.com  \r\n");
	//bt.printf("#       Universidad Nacianal de Asuncion     \r\n");
	//bt.printf("#             Facultad Politecnica           \r\n");
	//bt.printf("#Ingenieria Electronica - Enfasis Mecatronica\r\n");

	//wait(0.5);
	//wait(6.0); // Wait for the LCD to display all the info
}

void InitOffsets(void) {
	CalibrateSensors();
}

void InitMotors(void) {
	LeftMotor.speed(0.0);
	RightMotor.speed(0.0);
}

void InitPID(void) {

    // LTI Controller
//	controllerGain 	<< -6.19 << -0.80 << -0.062;

	controller.setInputLimits(-90.0, 90);	// Input [-90, 90] degrees
	controller.setOutputLimits(-1.0, 1.0);
	controller.setBias(0.0);
	controller.setMode(AUTO_MODE);
	controller.setSetPoint(0.0);		
}

void CalibrateSensors(void) {

	float accy = 0.0, gyrx = 0.0;
	accy_offset = gyrx_offset = 0.0;

	for (int i = 0; i < 128; i++) {
		accy += accelerometerY.read();
		gyrx += gyroscopeX.read();
	}

	accy_offset = accy / 128.0;
	gyrx_offset = gyrx / 128.0;
}

void ReadSensors(void) {
	float accy = 0.0, gyrx = 0.0;
	acc_y = gyr_x = 0.0;

	for (int i = 0; i < 16; i++) {
		gyrx += gyroscopeX.read();
		accy += accelerometerY.read();
	}

	accy /= 16.0;
	gyrx /= 16.0;

	acc_y = (accy - accy_offset) * (3.3) / acc_scale;
	gyr_x = (gyrx - gyrx_offset) * (3.3) / gyr_scale;

	if (acc_y > 1.0)
		acc_y = 1.0;
	else if (acc_y < -1.0)
		acc_y = -1.0;
	if (gyr_x > 2000.0)
		gyr_x = 2000.0;
	else if (gyr_x < -2000.0)
		gyr_x = -2000.0;
}

void GetAngle(void) {
	double acc_rad = 0.0;

	ReadSensors();
	acc_rad = asin(acc_y);              // Get angle from accleration - in radians
    acc_angle = acc_rad * TODEG;        // Convert gyro rate to radians/seconds
	
    // Complementary Filter (Sample rate = 0.01 seconds, result in radians)
	angleCompl = 0.98 * (angle + 0.01 * gyr_x) + 0.02 * acc_angle; // Takes 0.000007 seconds or 7 uS
	angle_kalman = kalman(acc_angle, gyr_x, loop_time); // Takes 0.000051 seconds or 51 uS
    angle = angle_kalman;
    
    //systemOutput.add(1, 1, angle);
    //systemOutput.add(2, 1, gyr_x);
    //systemOutput.add(3, 1, 0);
}

void SetOutput(void) {
    double pidOutput = 0;
    
    // LTI Controller u = -Ky - Takes 0.000035 seconds or 35 uS
  //  controlSignal = controllerGain * systemOutput;
    //controlSignal *= -1;
    //u = controlSignal.getNumber(1,1) / 200.0;

    //if (u > 1.0)
    //    u = 1.0;
    //else if(u< -1.0)
    //    u = -1.0;
    
    // PID Controller - Takes 0.000023 seconds or 23 uS
    controller.setProcessValue(angle);
	pidOutput = controller.compute();

    output = pidOutput;
    
	LeftMotor.speed(output);
	RightMotor.speed(output);
}

void Timers(int action) {
	switch (action) {
	case START:
		// Start timers
		//master.start();
		period.start();
		t.start();
		break;
	case STOP:
		// Restart master timer
		//master.start();
		t.stop();
		Ts = RATE - t.read();
		t.reset();
		wait(Ts);

		// Delete this (only for debugging)
		period.stop();
		loop_time = period.read();
		//pc.printf("%.6f,%.6f\n", Ts, loop_time);
        //pc.printf("%.6f\t%.6f\r\n", Ts, loop_time);
		period.reset();

		break;
	}
}

void RxData(void) {
	char input[20];                     // The serial buffer is only 16 charactersa
    char ip[16];
	int i = 0;
	while (1) {
		input[i] = pc.getc();
		if (input[i] == ';' || i >= 19)
			break;
		i++;
	}
//	pc.printf("Input: %s\r\n",input);

	if (input[0] == 'T'){						// Set the target angle
		strtok(input, ","); 					// Ignore 'T'
		targetAngle = atof(strtok(NULL, ";"));
		controller.setSetPoint(targetAngle);
	} else if (input[0] == 'P') {
		strtok(input, ",");						// Ignore 'P'
		Kc = atof(strtok(NULL, ";"));
	} else if (input[0] == 'I') {
		if (input[1] == 'P'){
            strncpy(ip, &input[3],16);
            lcd.printf("$1   Beagleway    \n");
            lcd.printf("$2%s\n", ip);
        }
        else{
            strtok(input, ",");						// Ignore 'I'
		    Ti = atof(strtok(NULL, ";"));
        }
	} else if (input[0] == 'D') {
		strtok(input, ",");						// Ignore 'D'
		Td = atof(strtok(NULL, ";"));
	} else if (input[0] == 'A'){ 				// Abort
		LeftMotor.speed(0.0);
		RightMotor.speed(0.0);
		while (pc.getc() != 'C');				// Wait until continue is send
	} else if (input[0] == 'G')                 // The processing application sends this at startup
		processing();				
}

void processing(void) {
    pc.printf("Processing, %5.2f, %5.2f, %5.2f, %5.2f\r\n", Kc, Ti, Td, targetAngle);
}

double kalman(double newAngle, double newRate, double dtime) {
	// KasBot V2  -  Kalman filter module - http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1284738418 - http://www.x-firm.com/?page_id=145
	// with slightly modifications by Kristian Lauszus
	// See http://academic.csuohio.edu/simond/courses/eec644/kalman.pdf and http://www.cs.unc.edu/~welch/media/pdf/kalman_intro.pdf for more information

	// Discrete Kalman filter time update equations - Time Update ("Predict")
	// Update xhat - Project the state ahead
	angle_kalman += dtime * (newRate - bias);

	// Update estimation error covariance - Project the error covariance ahead
	P_00 += -dtime * (P_10 + P_01) + Q_angle * dtime;
	P_01 += -dtime * P_11;
	P_10 += -dtime * P_11;
	P_11 += +Q_gyro * dtime;

	// Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
	// Calculate Kalman gain - Compute the Kalman gain
	S = P_00 + R_angle;
	K_0 = P_00 / S;
	K_1 = P_10 / S;

	// Calculate angle and resting rate - Update estimate with measurement zk
	y = newAngle - angle_kalman;
	angle_kalman += K_0 * y;
	bias += K_1 * y;

	// Calculate estimation error covariance - Update the error covariance
	P_00 -= K_0 * P_00;
	P_01 -= K_0 * P_01;
	P_10 -= K_1 * P_00;
	P_11 -= K_1 * P_01;

	return angle_kalman;
}

