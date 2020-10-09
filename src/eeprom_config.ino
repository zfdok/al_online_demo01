#include "config.h"
/************************dataType_bytes************************
 * dataType                 bytes
 * ------------------------------------------------------------
 * char                     8
 * short                    2
 * int                      4
 * time_t                   4
 * int32_t                  8
 * long                     4
 * float                    4
 * double                   8
 * ************************************************************/

/************************EEPROM_table***************************
 * adress                   length(byte)        description
 * ------------------------------------------------------------
 * 1                        1                   firstLoad_flag
 * 2                        8                   sleeptime
 * 10                       1                   temp Upper/Lower limit enabled
 * 11                       4                   temp Upper limit
 * 15                       4                   temp Lower limit
 * 
 * ************************************************************/
void get_eeprom_firstBootFlag()
{
  Serial.printf("EEPROM 1: %d \r\n", EEPROM.read(1));
  firstBootFlag = EEPROM.read(1) == 1 ? false : true;
}

void eeprom_config_init()
{
  if (firstBootFlag)
  {
    Serial.println("this is the first load,begin to write default:");
    EEPROM.write(1, 1);
    EEPROM.writeInt(2, FIRST_SLEEPTIME);
    EEPROM.write(10, FIRST_TEMP_LIMIT_ENABLE);
    EEPROM.writeFloat(11, FIRST_TEMP_UPPER_LIMIT);
    EEPROM.writeFloat(15, FIRST_TEMP_LOWER_LIMIT);
    EEPROM.commit();
    firstBootFlag = false;
  }
  else
  {
    Serial.println("this is not the first load");
    sleeptime = (time_t)EEPROM.readInt(2);
    Serial.printf("sleeptime:%ld\r\n", sleeptime);
    tempLimit_enable = EEPROM.read(10) == 0 ? false : true;
    Serial.printf("sleeptime:%s\r\n", (String)tempLimit_enable);
    tempUpperLimit = EEPROM.readFloat(11);
    Serial.printf("tempUpperLimit:%.2f\r\n", tempUpperLimit);
    tempLowerLimit = EEPROM.readFloat(15);
    Serial.printf("tempLowerLimit:%.2f\r\n", tempLowerLimit);
  }
}

void eeprom_config_set_sleeptime(time_t time1)
{
  EEPROM.writeInt(2, time1);
  EEPROM.commit();
}