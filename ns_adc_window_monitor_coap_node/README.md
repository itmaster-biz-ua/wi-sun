# Wi-SUN ADC Window Monitor CoAP node
Wi-SUN ADC Window Monitor CoAP node is a project on KIT LP-CC1312R7, which implements a CoAP node of the Wi-SUN network, which measures voltage.

In this project, the CoAP node connects to the border gateway (BR). It sends the value of the analog-to-digital converter (ADC) and a flag that shows whether the ADC value is above or below the limit. The output of the ADC value outside the limit is also signaled by green and red LEDs located on the board. BR can also set these limits and control the LEDs on the board.

ADC gain and offset errors are also corrected.

## Main features and characteristics of the project
Border router (BR) architecture: network coprocessor (NWP);
Exchange protocol between BR and host: Spinel;
CoAP node architecture: built-in device in active mode;
Exchange protocol between BR and CoAP node: CoAP;
ADC operating mode: 12-bit, active, conversion every 125 ms;
ADC input: pin DIO23;
Reading data from the CoAP node: ADC value (adcValue), ADC window state flag (bvWindowState) and LED states;
Controlled parameters on the CoAP node: upper and lower limits of the ADC window (adcWindowHigh and adcWindowLow).
Current consumption when the LEDs are off: 9 mA (at V = 3.3 V);
Current consumption with one LED on: 12 mA (at V = 3.3 V).

## Architecture of the Wi-SUN FAN network in the project
The Wi-SUN ADC Window Monitor CoAP node project is based on two examples in the LP-CC1312R7 SDK: ADC Window Monitor for LaunchPad and CoAP node (ns_coap_node). We launched the first project in ADC lessons: https://www.itmaster.biz.ua/electronics/ti-micro/adc1.html

We did the second project while studying the Wi-SUN FAN node router: https://www.itmaster.biz.ua/electronics/ti-micro/ti-micro-lp-cc1312r7-wi-sun-fan5.html

Our project will contain the functionality of both projects, where the CoAP node will be the server, and the BR will be the client.

For BR, we use the ns_br example in the TI SDK.

We control the network using the modified Spinel CLI utility, which, in addition to the standard functionality (LED control), will have the ability to read the value of the ADC - adcValue and the value of the limit-exceeding flag - bvWindowState. This utility can also be used to change the values of the upper and lower limits: adcWindowHigh and adcWindowLow.

Changing the value of bvWindowState will overwrite the states of the red and green LEDs. At the same time, we will be able to overwrite these LED states with the help of CoAP protocol commands.

## Code Wi-SUN ADC Window Monitor CoAP node
ns_adc_node_LP_CC1312R7 - Wi-SUN ADC Window Monitor CoAP project for Code Composer Studio (CCS);

adc_window_monitor_launchpad - project for Sensor Controller Studio (SCS);

spinel-cli.py - Updated Spinel CLI utility file that we use to control BR and send commands via CoAP protocol.

## Examples of Spinel CLI commands
The general appearance of the commands:

coap <ipv6 address> <coap request code (get|put|post)> <coap request type (con|non)> <uri_path>
      [--led_state <led_target (r|g)> <led_state (0|1)>]
      [--adc_window_state <adcWindow (low|high)> <value>]
	  
### Get the state of the LEDs on the board:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 get con led

2020:abcd:0000:0000:0212:4b00:29c6:ff79 is the IP6 address of the CoAP node that you will receive at the stage of joining it;
get - GET request;
con - means a confirmation message;
led is the LED resource URI on the board.
Light up the red LED:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 post con led --led_state r 1

### Turn off the red LED:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 put con led --led_state r 0

### Light up the green LED:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 post con led --led_state g 1

### Turn off the green LED:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 post con led --led_state g 0

### Get the ADC value:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 get con adc_voltage

### Get the value of the flag of the ADC window:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 get con adc_window

### Set the lower limit of the ADC window to 1000:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 post con adc_window --adc_window_state low 1000

### Set the upper border of the ADC window to 2000:

coap 2020:abcd:0000:0000:0212:4b00:29c6:ff79 put con adc_window --adc_window_state high 2000

## Quick start
Below are the steps to get started. They are similar to those performed in the TI Wi-SUN FAN network learning lessons: https://www.itmaster.biz.ua/electronics/ti-micro/lp-cc1312r7-wi-sun-fan1.html

1. Download the ns_br example project to the first LP-CC1312R7 board.

2. Download the ns_adc_node_LP_CC1312R7 project from GIT, compile it, and upload it to the second LP-CC1312R7 board.

3. Download the file spinel-cli.py from GIT, overwrite the old original file in the directory of this utility with it and run it for the COM port of the ns_br project (eg. python spinel-cli.py -u COM9).

4. Start the Wi-SUN network. These are the following commands: ifconfig up, wisunstack start, wisunstack, routerstate and connecteddevices. Wait for the ns_adc_node to connect to the network. When the ns_adc_node connects to the network, the red LED will stop blinking and the green LED will light up.

5. Now you can execute CoAP commands, examples of which are shown in the previous header.