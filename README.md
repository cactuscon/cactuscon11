# CactusCon 11 Badge Tutorial

## Introduction
Thanks for participating in Cactuscon 11 and for purchasing a badge! While it may have been a blast making the badges run an LED party in person, this tutorial will walk you through how to program the badge yourself and go through the services/components we used to run the badge this year. To begin you'll want to ensure you get a development environment and the platformio tools set up and running if you don't already have them.

### Installing Platform IO

1. Install [VS Code](https://code.visualstudio.com/download) if you don't already have it installed

2. Install the PlatformIO plugin

![alt text](https://github.com/cactuscon/cactuscon8/raw/master/img/install_platformio.png "Install PlatformIO")

3. Open up the home page for Platform IO if not already open by clicking the alien on the left bar, then under PIO home "Open". 

![alt text](images/pio_home.png "Navigate to PIO Home")

4. Click "New project", give it a name, and select "Espressif ESP32-S2-Saola-1" as the board (you can begin to type the name in for autocomplete).

![alt text](images/pio_new_project.png "New project")

![alt text](images/pio_project_wizard.png "Project Wizard")

## Testing basic badge operation

We'll look at using the some more sophisticated functionality in a bit, but first let's start with the simplest program that can run the badge. We'll do most of our work in this tutorial within main.cpp. Open the file and add the following code to the top which will set some constant variables that we can use to reference the pins tied to the LEDs on the badge.

``` c
#include <Ardunio.h>

const int DRONE_CAMERA = 10; // Drone Camera LED
const int HOUSE_RIGHT_EYE_RED = 13; // Set the pin for the red LED
const int HOUSE_RIGHT_EYE_GREEN = 12; // Set the pin for the green LED
const int HOUSE_RIGHT_EYE_BLUE = 11; // Set the pin for the  blue LED
const int HOUSE_LEFT_EYE_RED = 5; // Set the pin for the red LED
const int HOUSE_LEFT_EYE_GREEN = 4;// Set the pin for the green LED
const int HOUSE_LEFT_EYE_BLUE = 3;  // Set the pin for the  blue LED
```

Once the constants have been defined, during setup these pins will need to be initalized to set them as OUTPUTs. We do this with the pinMode() function in ardunio. We'll also enable the serial console by calling Serial.begin with a baud rate of 115,200 (so it can be monitored by clicking the serial icon next to the beaker in the bottom left bar of VScode).

``` c
setup () {
    pinMode(DRONE_CAMERA, OUTPUT);
    pinMode(HOUSE_RIGHT_EYE_RED, OUTPUT);
    pinMode(HOUSE_RIGHT_EYE_GREEN, OUTPUT);
    pinMode(HOUSE_RIGHT_EYE_BLUE, OUTPUT);
    pinMode(HOUSE_LEFT_EYE_RED, OUTPUT);
    pinMode(HOUSE_LEFT_EYE_GREEN, OUTPUT);
    pinMode(HOUSE_LEFT_EYE_BLUE, OUTPUT);

    // Due to the way the hardware is designed, LOW means ON for a pin. Set all pins to high (OFF) at start
    digitalWrite(DRONE_CAMERA, HIGH);
    digitalWrite(HOUSE_RIGHT_EYE_RED, HIGH);
    digitalWrite(HOUSE_RIGHT_EYE_GREEN, HIGH);
    digitalWrite(HOUSE_RIGHT_EYE_BLUE, HIGH);
    digitalWrite(HOUSE_LEFT_EYE_RED, HIGH);
    digitalWrite(HOUSE_LEFT_EYE_GREEN, HIGH);
    digitalWrite(HOUSE_LEFT_EYE_BLUE, HIGH);

    Serial.begin(115200);
}
```

Now that everything has been set up, you can add whatever code within the loop function to make your badge do many different things. Here is just a simple example of blinking the house eyes red.

``` c
loop () {
    digitalWrite(HOUSE_RIGHT_EYE_RED, LOW);
    digitalWrite(HOUSE_LEFT_EYE_RED, LOW);
    delay(500); // sleep for 500 ms

    digitalWrite(HOUSE_RIGHT_EYE_RED, HIGH);
    digitalWrite(HOUSE_LEFT_EYE_RED, HIGH);
    delay(500); // sleep for 500 ms
}
```
Now plug in the USB cable on your badge, turn the ON/OFF switch to on, and then hold the boot button and press reset. After completed, click the right arrow in the bottom bar of VS Code to push the firmware to the badge.

![alt text](https://github.com/cactuscon/cactuscon8/raw/master/img/push_firmware.png "Push firmware")

It'll take a little bit to run, especially for the first time, but you should get a success message at the end. If you do, press the reset button on the badge and the eyes should start blinking.

If you get a failure and VS code complains that it can't find the USB port, check to ensure the badge is found. From the Platform IO home screen check the devices section from the Platform IO home screen to see if the badge is listed.

![alt text](images/pio_devices.png "Devices section")

If you do not see the badge listed, ensure the ON/OFF switch on the side of the badge is set to on and the battery is installed + charged. Also ensure that you held the boot button and then reset.

If that still doesn't work, sometimes the USB port can be a bit sensitive, so try a different cable, or ensure the cable is fully seated. 

Lastly, you may also want to ensure that you have the [CH340 drivers](http://www.wch-ic.com/downloads/CH341SER_ZIP.html) for your operating system. They should be there by default on most systems, but you never know.

If you still can't get it working, please feel free to reach out for help on the hardware-hacking channel on the Cactuscon Community discord.

---- 
## Setting up MQTT

MQTT is a lightwight messaging protocol with characteristics that make it well suited for unreliable networks and the Internet of Things (IoT). There are many [MQTT brokers](https://mqtt.org/software/) (servers) out there, but for this tutorial we'll focus on the [AWS MQTT](https://aws.amazon.com/what-is/mqtt/) implementation. The free tier of AWS IoT core provides more than sufficent resources to not only play with the badge, but also to begin to build other IoT implementations in your home. Before starting this tutorial ensure to have an AWS account set up. Your usage for the tutorial will likely be covered by the AWS free tier, so no need to worry too much about charges.

### Creating a thing policy
<table>
<tr>
    <td>
    Once logged into AWS, navigate to the AWS IoT page. You can find it by typing IoT in search and selecting IoT Core. 
    </td>
    <td>
    <img src="images/AWS_iot_start.png" alt="AWS IoT Core">
    </td>
</tr>
<tr>
    <td>
    Once on the page expand the Security section, and click Policies.
    </td>
    <td>
    <img src="images/AWS_iot_policies.png" alt="AWS IoT Core - Policies page" height="300px">
    </td>
</tr>
<tr>
    <td>
    Under policies click Create Policy, which will open a new window.<br><br>
    Give the policy a name (for example default-policy), under Policy Statments switch from "Builder" to "JSON" view.
    </td>
    <td>
    <img src="images/AWS_iot_policy.png" alt="AWS IoT Create Policy page">
    </td>
</tr>
<tr>
    <td>
    Copy and paste the following policy. Replace the <code>#region</code> and <code>#account</code> values for your AWS account. Then click Create.<br><br>
    The policy here does 4 things:<br>
    <ul>
        <li>Allow client connect only if the Certificate matches the provided Thing Name</li>
        <li>Allow the Thing to Publish, Subscribe, and Receive messages on <b>any</b> topic.
    </ul>
    <br>
    Note: for better security, you may want to further reduce the access to topics for device policies.
    </td>
    <td>
    <pre><code lang="json">{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:#region:#account:client/${iot:Connection.Thing.ThingName}"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": "*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": "*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": "*"
    }
  ]
}</code></pre>
    </td>
</tr>
</table>


### Creating a thing

<table>
<tr>
    <td>
    Once a policy has been created, on the left nav expand "All devices" under Manage, then click Things.
    </td>
    <td>
    <img src="images/AWS_iot_things.png" alt="AWS IoT All Devices - Things" height="300px">
    </td>
</tr>
<tr>
    <td>
    On the things page, click "Create things"
    </td>
    <td>
    </td>
</tr>
<tr>
    <td>
    Leave "Create single thing" selected and click next.
    </td>
    <td>
    <img src="images/AWS_iot_things2.png" alt="AWS IoT Create Thing - Single Thing">
    </td>
</tr>
<tr>
    <td>
    Enter a thing name, leave no shadow selected, then click next.
    </td>
    <td>
    <img src="images/AWS_iot_create_thing.png" alt="AWS IoT Create Thing - Thing details">
    </td>
</tr>
<tr>
    <td>
    Under device certificate, leave auto-generate a new certificate selected and hit next.
    </td>
    <td>
    </td>
</tr>
<tr>
    <td>
    Checkmark the box next to the policy you just made and click create thing.
    </td>
    <td>
    <img src="images/AWS_iot_thing_policy.png" alt="AWS IoT Create Thing - Thing Policy - Check created policy">
    </td>
</tr>
<tr>
    <td>
    <font color="red">!Important: </font> You will get a popup that contains the device certificate, key files, and root CA. Don't move forward without downloading the device certificate, the device public key & private key, and the Amazon Root CA 1.
    </td>
    <td>
    </td>
</tr>
</table>

### Publishing to MQTT

MQTT works like many other different message brokers, where a message is published on a topic that subscribers can recieve. It allows for any type of data to be published to it. In this case we'll use JSON since it makes life simple and allows for testing on the AWS console.

To get started, we'll need to pull in a few libaries one for JSON and one for MQTT. Open up the file platform.io in the main project folder and add these lib_deps in.

``` ini
[env:esp32-s2-saola-1]
platform = espressif32
board = esp32-s2-saola-1
framework = arduino
lib_deps = 
	knolleary/PubSubClient@^2.8
	bblanchon/ArduinoJson@^6.19.4
```

This is where things will get a bit interesting. You will need to take the certificates you downloaded and convert them into C style string variables. You can put this in main.cpp, but it will make things hard to read. Instead open the include folder, right click, then create a new file and name it "aws.hpp". Add your certificates and private key to it in the form below.

``` c
const char AWS_PUBLIC_CERT[] = "-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";

const char AWS_DEVICE_CERT[] = "-----BEGIN CERTIFICATE-----\n" \
"SIMILAR TO ABOVE, EACH LINE FROM THE CERT FILE ON ITS OWN LINE WITH\n" \
"NEWLINE CHARACTERS INCLUDED.\n" \
"-----END CERTIFICATE-----\n";

const char AWS_PRIVATE_KEY[] = "-----BEGIN RSA PRIVATE KEY-----\n" \
"SIMILAR TO ABOVE, EACH LINE FROM THE CERT FILE ON ITS OWN LINE WITH\n" \
"NEWLINE CHARACTERS INCLUDED.\n" \
"-----END RSA PRIVATE KEY-----\n";
```

Now that the certificates are there, go back to the main.ccp file and at the top add the following..

``` c
#include <Ardunio.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "aws.hpp"
```

That will pull those variables in from the header file you created and allow you to use them within main, but without cluttering things up. Now let's establish connectivity to MQTT. You'll want to create a few objects that will be used to connect to MQTT. You can put these just below the includes.

``` c
WiFiClientSecure secure_client = WiFiClientSecure();
PubSubClient mqtt_client = PubSubClient(secure_client);
```

Before we go much further, we'll need some boilerplate code to start wifi up on the device. You can dig further into this if you want on the [ESP-32 documentation page](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html), but for now just gloss over it.

``` c
void start_wifi() {
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.begin("YOUR SSID FOR WIFI HERE", "YOUR PASSWORD FOR WIFI HERE");

  Serial.print("WiFi is connecting");
  for (int i = 0; i < 25 && WiFi.status() != WL_CONNECTED; i++){
    Serial.print('.');
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    Serial.println(" failed!");
    ESP.restart();
  }

  Serial.println(" connected!");
}
```

We'll also define a function that handles establishing a connection to MQTT. This will continue to try and connect multiple times, and ultimately restart the device if it fails to connect.

One important note -- The `MyDeviceID` needs to match your `thing_name` from the AWS configuration. The policy set on the thing requires these values to match. To simplify this, you can edit the policy from earlier and change the `resource` for Action `iot:Connect` to `*` to allow any device with the certificate to connect.

``` c
void connect_mqtt() {
    Serial.println("Connecting to MQTT");
    for (int i = 0; i < 25 && !mqtt_client.connect("MyDeviceID"); i++) {
        Serial.println("Failed connection, retrying.");
        mqtt_client.disconnect();
        delay(1000);
    } 

    if (!mqtt_client.connected()) {
        Serial.println("Connection to MQTT failed!");
        ESP.restart();

        return;
    }
    Serial.println("Connected to MQTT");
}
```

Within the setup() function is where we'll do all of the magic to start wifi, establish a connection, and then publish a message to a topic. Be sure to update the code below to point to the URL for your MQTT service endpoint, which you can grab from AWS IoT Core > "Connect one device" page.

``` c
void setup() {
    StaticJsonDocument<200> doc;
    byte json_buf[200];
    int msg_len;
    bool results; 

    // other code to set up pins and serial communications omitted
    
    start_wifi();

    secure_client.setCACert(AWS_PUBLIC_CERT);
    secure_client.setCertificate(AWS_DEVICE_CERT);
    secure_client.setPrivateKey(AWS_PRIVATE_KEY);
    // update the URL below to match the service name in your AWS IoT Core > "Connect one device" page
    mqtt_client.setServer("YourIoTServiceName-ats.iot.us-east-1.amazonaws.com", 8883);
    connect_mqtt();


    doc["party"] = true;
    msg_len = serializeJson(doc, json_buf);
    results = mqtt_client.publish("myTopicName", json_buf, msg_len);

    if (results) {
        Serial.println("Transmitted");
    }
    else {
        Serial.println("Failed to transmit");
    }
}
```

Upload the code to the badge and if all goes well, when it turns on it will connect to wifi, MQTT, and publish a message that looks like:

``` json
{
    "party": true
}
```

You can watch this happen by using the AWS IOT test client within the AWS console. To get to it, click the 'MQTT test client' link.

![alt text](images/aws_subscribe_client.png "AWS IOT Console Subscriber Client")

We leave it as an exercise for the reader to have a message triggered by a button press on the badge. If you have never worked with buttons in Ardunio before, there are several [ardunio button tutorials](https://arduinogetstarted.com/tutorials/arduino-button) out there which can give a place to start. The pin mappings for the buttons on the badge are as follows.

``` c
const int down_btn_pin = GPIO_NUM_38;
const int up_btn_pin = GPIO_NUM_37;
const int center_btn_pin = GPIO_NUM_36;
const int right_btn_pin = GPIO_NUM_35;
const int left_btn_pin = GPIO_NUM_34;
```

### Subscribing to MQTT
When a message is published to a topic it is useless unless there is something subscribed to that topic to recieve it. We'll create a function that instructs our MQTT client to execute whenever a message is received on the topic. At the conference we had the big red button as a MQTT publisher, and every badge as a subscriber.

First before subscribing to the topic we must define the function to execute whenever a message is received. The arguments for this function are fixed by the library being used and are passed in to share details about where the message came from with the function. For this example we'll ignore the topic and just respond to any topic, but this is where more complicated logic could be put in place to respond to a message directed at a specific badge in a diferent way.

``` c
void handle_message(const char* topic, byte* payload, unsigned int length) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);

    if (doc["party"].is<bool>()){ // check to ensure that the party key is set and is a bool
        if (doc["party"].as<bool>()) {
            for (int i = 0; i < 6; i++) { // blink the eyes red 6 times
                    digitalWrite(HOUSE_RIGHT_EYE_RED, LOW);
                    digitalWrite(HOUSE_LEFT_EYE_RED, LOW);
                    delay(500); // sleep for 500 ms

                    digitalWrite(HOUSE_RIGHT_EYE_RED, HIGH);
                    digitalWrite(HOUSE_LEFT_EYE_RED, HIGH);
                    delay(500); // sleep for 500 ms
            }
        }
    }
}
```

In our setup, this is where we'll tell the MQTT client to execute our function whenever a message is received.

``` c
void setup(){
    // other setup code omittet for brevity
    start_wifi();

    secure_client.setCACert(AWS_PUBLIC_CERT);
    secure_client.setCertificate(AWS_DEVICE_CERT);
    secure_client.setPrivateKey(AWS_PRIVATE_KEY);
    // update the URL below to match the service name in your AWS IoT Core > "Connect one device" page
    mqtt_client.setServer("YourIoTServiceName-ats.iot.us-east-1.amazonaws.com", 8883);

    connect_mqtt();
    mqtt_client.setCallback(&handle_message);
    mqtt_client.subscribe("myTopicName", 0); // the 0 denotes the QOS level in MQTT, this has to do with message acknowledgement and you can ignore for now.
}
```

We'll also need to be sure that we call the mqtt client loop method within the main loop as this method checks for new messages and calls our handle message function if a new one has arrived.

``` c
void loop() {
    // eat sleep rave repeat
    mqtt_client.loop()
}
```

And that should do it. Publish the code to the badge, and then use the AWS MQTT test client to publish a message.

![alt text](images/aws_publish_client.png "AWS IOT Console Subscriber Client")

You'll note that the setup code in the subscriber didn't have the publish code from above. For the sake of sanity as you begin to learn, it is recommended that you either code your badge to publish OR recieve. You can certainly do both, but that adds a bit more complexity that is also left as an exercise for the reader to venture into if they dare :).

MQTT is very versitle and supported in many different home automation platforms like home assistant. Another interesting exercise would be to use your badge and MQTT to trigger automations in your home.

## Interfacing with the badge hardware

There are many different ways to interface with the hardware, including directly manipulating the GPIO pins. We have provided a higher level  that provides for some predefined functionality. To begin to use these functions, we must first pull in a needed library. Open the platformio.ini file in the project directory and modify it to add the lib_deps for FastLED.

``` ini
[env:esp32-s2-saola-1]
platform = espressif32
board = esp32-s2-saola-1
framework = arduino
lib_deps = 
	knolleary/PubSubClient@^2.8
	bblanchon/ArduinoJson@^6.19.4
	fastled/FastLED@^3.5.0
```

After that is completed, switch back to main.cpp. The hardware must first be initalized by calling the HacktarHouse::setup method from within the main setup function.

``` c
void setup() {
    HacktarHouse::setup();
}
```

After the hardware has been initalized, the various different methods can be executed.

``` c
void loop() {
    HacktarHouse::party(); // eat sleep rave repeat
    HacktarHouse::cylon();

    HacktarHouse::set_eyes("ff0000");
    delay(500);
    HacktarHouse::set_eyes("000000");
    delay(500);
}
```

### Pulling it all together with MQTT
Spend some time getting a bit comfortable with the hardware and play with the different functions. Now that we've got a handle on controlling the hardware, let's tie this together MQTT. To do that we'll make a minor update the handle message function from the MQTT subscriber example above (don't forget about the setup and loop code as well).

``` c
WiFiClientSecure secure_client = WiFiClientSecure();
PubSubClient mqtt_client = PubSubClient(secure_client);

void handle_message(const char* topic, byte* payload, unsigned int length){
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);

    if (doc["party"].is<bool>()){
        if (doc["party"].as<bool>())
            HacktarHouse::party();
    }
}
```

If for some reason you are hitting some errors or having problems, feel free to [clone](https://www.howtogeek.com/451360/how-to-clone-a-github-repository/) this repo out and open the project folder in platform IO. 

The code in this repo provides a fully functional example of badge code that recieves instructions from MQTT. It is released under the Simplified BSD License, so feel free to modify and re-use as desired!
