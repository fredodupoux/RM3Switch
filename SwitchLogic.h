/*
Switch logic class, to trigger and setup relays
by simply creating instances and calling their functions
Author: Frederic Dupoux
Date : March 27 2024
Version: 1.0

How to use it:
-- Define the pins for the relay and virtual button
const int RELAY_PIN = 2;        // Example pin for the relay
const int VBUTTON_PIN = V1;     // Example virtual pin for the button in Blynk

-- Create an instance of SwitchLogic with custom pin configuration
SwitchLogic switch2(RELAY_PIN, VBUTTON_PIN, "Switch2");

-- Call the methods
switch2.start();
switch2.stop();
switch2.getRelayState();

*/

class SwitchLogic {
private:
    int relayPin;
    int VbuttonPin;
    String eventName;
    bool relayState; // Store the current state of the relay

public:
    // Constructor 
    SwitchLogic(int relay, int Vbutton, const String& event, bool initialState = LOW) : relayPin(relay), VbuttonPin(Vbutton), eventName(event), relayState(initialState) {
        // Set pin mode for the relay pin
        pinMode(relayPin, OUTPUT);
    }

    void start() {
        digitalWrite(relayPin, HIGH);
        Blynk.virtualWrite(VbuttonPin, HIGH);
        Blynk.logEvent(eventName + "_start");

        // Update the relay state
        relayState = HIGH;
        Terminal.println(eventName + " -> ON.\n");
    }

    void stop() {
        digitalWrite(relayPin, LOW);
        Blynk.virtualWrite(VbuttonPin, LOW);
        Blynk.logEvent(eventName + "_stop");

        // Update the relay state
        relayState = LOW;
        Terminal.println(eventName + " -> OFF.\n");
       
    }

    // Getter for relay state
    bool getRelayState() const {
        return relayState;
    }

    // Setter for event name
    void setEventName(const String& event) {
        eventName = event;
    }

    // Getter for event name
    String getEventName() const {
        return eventName;
    }

    // Setter for relay pin
    void setRelayPin(int pin) {
        relayPin = pin;
        pinMode(relayPin, OUTPUT);
    }

    // Getter for relay pin
    int getRelayPin() const {
        return relayPin;
    }

    // Setter for virtual button pin
    void setVbuttonPin(int pin) {
        VbuttonPin = pin;
    }

    // Getter for virtual button pin
    int getVbuttonPin() const {
        return VbuttonPin;
    }

    // Setter for relay state
    void setRelayState(bool state) {
        relayState = state;
    }
};
