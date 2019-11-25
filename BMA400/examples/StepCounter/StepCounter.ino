#include "BMA400.h"

uint32_t cnt = 0;

void setup(void)
{
    Wire.begin();
    
    Serial.begin(115200);
    while(!Serial);
    Serial.println("BMA400 Raw Data");

    while(1)
    {
        if(bma400.isConnection())
        {
            bma400.initialize();
            Serial.println("BMA400 is connected");
            break;
        }
        else Serial.println("BMA400 is not connected");
        
        delay(2000);
    }
}

void loop(void)
{
    bma400.EnableStepCounter();
    cnt = bma400.getStepCounter();
    Serial.print("Step: ");
    Serial.println(cnt);
    
    Serial.println();
    
    delay(50);
}
