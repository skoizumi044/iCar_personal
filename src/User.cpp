#include <board.h>

void sensorMonitor(int speed, float pGain, float dGain);

void setup() {
}

void loop() {
	int lineL, lineR;
	int error;
	int controlValue;
	int speedL, speedR;
	float pGain, dGain;
    int min_speed = 0;
    int max_speed = 100;
    int base_speed = 40;
    int dynamic_speed;
    int gain_switch = 0;
    int last_error = 0;
    int diff;

    /* BTN_1を押すまで待つ */
    while (digitalRead(PIN_BTN1) == LOW) {}

    while (1) {
        lineL = analogRead(PIN_LINE_L); //地面の明るさ左 0~4095
        lineR = analogRead(PIN_LINE_R); //地面の明るさ右 0~4095

        gain_switch = analogRead(PIN_TOGGLE);

        switch(gain_switch){
        	case 0 :
        		pGain = (float)analogRead(PIN_VOLUME) / 100.0f;
        		break;

        	case 1 :
        		dGain = (float)analogRead(PIN_VOLUME) / 50.0f;
        		break;

        	case 2 :
        		base_speed = analogRead(PIN_VOLUME);
        		break;

        	default :
        		break;
        }

        error = lineL - lineR;  //偏差 -4095~4095
        diff = error - last_error;
        last_error = error;

        controlValue = (int)(((float)error * pGain + (float)diff * dGain) / 40.0f);

        dynamic_speed = base_speed - (abs(controlValue) / 2);
        speedL = dynamic_speed - controlValue;
        speedR = dynamic_speed + controlValue;



        if(speedL < min_speed){
        	speedL = min_speed;
        } else if (speedL > max_speed){
        	speedL = max_speed;
        }

        if(speedR < min_speed){
        	speedR = min_speed;
        } else if (speedR > max_speed){
        	speedR = max_speed;
        }

        analogWrite(PIN_MOTOR_L, speedL);
        analogWrite(PIN_MOTOR_R, speedR);

        sensorMonitor(base_speed, pGain, dGain);

        delay(1);
    }
}

void sensorMonitor(int speed, float pGain, float dGain){
    static uint32_t lastLcdUpdate = 0;

    /* 50[ms]毎に処理を実行 */
    /* ※早すぎると数字が読み取れなくなるため */
    if ((millis() - lastLcdUpdate) >= 50) {
        lastLcdUpdate = millis();

        LcdDrv_setCursor(0, 0);
        LcdDrv_print("L");
        LcdDrv_setNum(analogRead(PIN_LINE_L), 4);

        LcdDrv_print(" R");
        LcdDrv_setNum(analogRead(PIN_LINE_R), 4);

        LcdDrv_print(" T");
        LcdDrv_setNum(analogRead(PIN_TOGGLE), 1);

        LcdDrv_setCursor(1, 0);
        LcdDrv_print("P");
        LcdDrv_setNum(pGain * 100, 3);

        LcdDrv_print(" D");
        LcdDrv_setNum(dGain * 100, 3);

        LcdDrv_print(" S");
        LcdDrv_setNum(speed, 3);

        LcdDrv_update();
    }
}
