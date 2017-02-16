/*
 * This program sets up the Arduino to read the ambient temperature and then
 * display the temperature on two seven segment displays.
 * 
 * Author: Michael Mackliet
 * Last Modified March 6, 2016
 */
 
 
 class ShiftRegister {

  private:
    int led_state[8];
    int data;
    int clock;
    int latch;
    
    void updateLEDs();

    
  public:
    ShiftRegister(int data, int clock, int latch);
    void change_output(int output_number, int on_or_off);
    void changeNumDisplay(int number);
    
 };

/*
 * This is a constructor
 * 
 * Creates a ShiftRegister object given the pin numbers used as data, clock, 
 * and latch in the circuit.  The constructor sets all LED states to LOW and
 * sets the pins on the Arduino as output pins.
 * 
 * Parameters
 * int data_pin--the pin number on the Arduino to be used as the data output
 * int clock_pin--the pin number on the Arduino to be used as the clock output
 * int latch_pin--the pin number on the Arduino to be used as the latch output
 */
ShiftRegister::ShiftRegister(int data_pin, int clock_pin, int latch_pin){
  for(int i=0;i<8;i++){
    led_state[i]=LOW;
  }
  data = data_pin;
  clock = clock_pin;
  latch = latch_pin;
  pinMode(data, OUTPUT);
  pinMode(clock, OUTPUT);  
  pinMode(latch, OUTPUT); 
}

/*
 * This is a member function
 * 
 * This function changes the state in one of the LEDs given its position in the array
 * holding LED states in the ShiftRegister object and the desired state of the LED, 
 * HIGH or LOW.
 * 
 *Parameters:
 *int output_number--the position of the led's state stored in the array
 *int on_or_off--the desired state of the LED after the change
 *
 *Returns:
 *none
 */
void ShiftRegister::change_output(int output_number, int on_or_off){
   led_state[output_number] = on_or_off;
   updateLEDs();
 }

 /*
  * This function displays a number on the 7 segment common cathode
  * display hooked to the shift register
  * 
  * Parameters
  * int number: the number to be displayed
  * 
  * Returns:
  * none
  */
 void ShiftRegister::changeNumDisplay(int number){
  if(number<0 && number>9){
    return;
  }
  switch (number){
    case 0:
    change_output(6, HIGH);
    change_output(5, HIGH);
    change_output(4,HIGH);
    change_output(3,HIGH);
    change_output(2, HIGH);
    change_output(1,HIGH);
    change_output(0, LOW);
    break;
    
    case 1: 
    change_output(1,LOW);
    change_output(2, LOW);
    change_output(3, LOW);
    change_output(4,HIGH);
    change_output(5,HIGH);
    change_output(6,LOW);
    change_output(0,LOW);
    break;

    case 2:
    change_output(6, HIGH);
    change_output(5, HIGH);
    change_output(4,LOW);
    change_output(3,HIGH);
    change_output(2, HIGH);
    change_output(1,LOW);
    change_output(0, HIGH);
    break;

    case 3:
    change_output(6, HIGH);
    change_output(5, HIGH);
    change_output(4,HIGH);
    change_output(3,HIGH);
    change_output(2, LOW);
    change_output(1,LOW);
    change_output(0, HIGH);
    break;

    case 4:
    change_output(6, LOW);
    change_output(5, HIGH);
    change_output(4,HIGH);
    change_output(3,LOW);
    change_output(2, LOW);
    change_output(1,HIGH);
    change_output(0, HIGH);
    break;

    case 5:
    change_output(6, HIGH);
    change_output(5, LOW);
    change_output(4,HIGH);
    change_output(3,HIGH);
    change_output(2, LOW);
    change_output(1,HIGH);
    change_output(0, HIGH);
    break;

    case 6:
    change_output(6, HIGH);
    change_output(5, LOW);
    change_output(4,HIGH);
    change_output(3,HIGH);
    change_output(2, HIGH);
    change_output(1,HIGH);
    change_output(0, HIGH);
    break;

    case 7:
    change_output(6, HIGH);
    change_output(5, HIGH);
    change_output(4,HIGH);
    change_output(3,LOW);
    change_output(2, LOW);
    change_output(1,LOW);
    change_output(0, LOW);
    break;

    case 8:
    change_output(6, HIGH);
    change_output(5, HIGH);
    change_output(4,HIGH);
    change_output(3,HIGH);
    change_output(2, HIGH);
    change_output(1,HIGH);
    change_output(0, HIGH);
    break;

    case 9:
    change_output(6, HIGH);
    change_output(5, HIGH);
    change_output(4,HIGH);
    change_output(3,LOW);
    change_output(2, LOW);
    change_output(1,HIGH);
    change_output(0, HIGH);
    break;
  }
 }

/*
 * This function sends the LED state data to the shift register and updates the display
 * 
 * Parameters:
 * none
 * 
 * Returns:
 * none
 */
void ShiftRegister::updateLEDs(){
 digitalWrite(latch,LOW);
 for(int i=0;i<8;i++){
  digitalWrite(data,led_state[i]);

  digitalWrite(clock,HIGH);
  delay(1);
  digitalWrite(clock,LOW);
 }
 digitalWrite(latch,HIGH);
}




//Declare global variables to be used throughout program as shift registers and the
//pin at which the temperature will be read in at
ShiftRegister reg1(2,3,4);
ShiftRegister reg2(5,3,6);
int temperaturePin;



/*
 * This function reads the voltage from a given pin and then uses some equations
 * derived through MATLAB to get the temperature being read on a thermistor
 * that's in series with a 51k resistor.
 * 
 * Returns:
 * float - the temperature being read by the thermistor
 */
float getTemperature(){
  float voltage = getVoltage(temperaturePin);
  float logResistance = log(getResistance(voltage));
  float coefficients[4] = {-0.139660890449635, 2.56407142372048, -32.1255766341304, 87.0068146625893};
  float temperature = 0;
  int exp = 3;
  for(int i=0; i<4; i++){
    temperature += coefficients[i]*pow(logResistance,exp);
    exp--;
  }
  return (1.8*temperature + 32);
}

/*
 * This helper function is used to get the current resistance of the thermistor
 * based of the voltage across it when it is in series with a 51k resistor.
 * 
 * Parameters
 * float voltage - the voltage across the thermistor
 * 
 * Returns
 * float - the current resistance of the thermistor
 */
float getResistance(float voltage){
  float Vsrc = 5; //power supply voltage
  float R1 = 50.6 *pow(10,3); //resistor in series with thermistor
  float resistance = ((voltage/Vsrc)*R1)/(1-(voltage/Vsrc));
  resistance = resistance/1000; //resistance in kOhms
  return resistance;
}

/*
 * This function displays the current temperature on two common cathode seven segment
 * displays hooked up two shift registers.
 * 
 * Parameters
 * float temperature - temperature to be displayed by the seven segment displays
 * 
 * Returns
 * none
 */
void displayTemperature(float temperature){
  int hundredsPlace = temperature/100;
  int tensPlace = (temperature - hundredsPlace*100)/10;
  int onesPlace = (temperature - hundredsPlace*100 - tensPlace*10);
  if((temperature-100*hundredsPlace-10*tensPlace-onesPlace)>=0.5){
    onesPlace++;
    if(onesPlace == 10){
      tensPlace++;
      onesPlace = 0;
      if(tensPlace == 10){
        hundredsPlace++;
        tensPlace = 0;
      }
    }
  }
  reg1.changeNumDisplay(tensPlace);
  reg2.changeNumDisplay(onesPlace);
}
void setup()
{
  temperaturePin = 0; //Analog pin number to be used to read temperature
  Serial.begin(9600);  //Start the serial connection with the copmuter
                       //to view the result open the serial monitor 
                       //last button beneath the file bar (looks like a box with an antenae)
}
 
void loop()
{
 float temperature = getTemperature();
 displayTemperature(temperature);
 delay(1000);
}

/*
 * getVoltage() - returns the voltage on the analog input defined by
 * int pin - the pin being used to read the voltage
 */
float getVoltage(int pin){
 return (analogRead(pin) * .004882814); //converting from a 0 to 1023 digital range
                                        // to 0 to 5 volts (each 1 reading equals ~ 5 millivolts
}

