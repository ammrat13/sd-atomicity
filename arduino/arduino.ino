//! \file arduino.ino
//! \brief Arduino "coprocessor" code for the atomicity test.
//! \author Ammar Ratnani <ammrat13@gmail.com>
//!
//! The program on the RPi will signal the start of the write by setting a GPIO
//! pin high. After that, it expects an external device (this code) to
//! power-cycle the device. This is some very simple code to do that. It will
//! read a voltage on a pin, and if it's high, it will set another pin after
//! some fixed delay.

//! \brief The pin to read the signal from the RPi.
#define SIGNAL_PIN (2u)

//! \brief The pin to set.
#define SET_PIN (3u)
//! \brief The voltage to set the set pin to.
#define SET_VOLTAGE (HIGH)
//! \brief The delay to wait before setting the set pin.
#define SET_DELAY_US (1000u)

//! \brief How long to wait after setting the set pin to reset it.
#define RESET_DELAY_MS (500u)

//! \brief Invert a voltage level.
//!
//! For some reason, there doesn't seem to be a built-in function or operator
//! for this. We'll write our own.
//!
//! \param[in] v The voltage level to invert.
//! \return The inverted voltage level.
uint8_t VOLTAGE_INVERT(uint8_t v) {
  return (v == HIGH) ? LOW : HIGH;
}

void setup() {

  // Serial for debugging. We shouldn't use this in time-critical code.
  Serial.begin(115200);

  // Note: No internal pull-down resistors on the Arduino. We need to use an
  // external one.
  pinMode(SIGNAL_PIN, INPUT);
  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, VOLTAGE_INVERT(SET_VOLTAGE));

  Serial.println("Starting atomicity test coprocessor...");
}

void loop() {

  // Wait for the signal. We don't want to deal with interrupts, so we'll just
  // busy-wait.
  while (digitalRead(SIGNAL_PIN) == LOW)
    ;

  delayMicroseconds(SET_DELAY_US);
  digitalWrite(SET_PIN, SET_VOLTAGE);

  Serial.println("TRIGGERED!");

  delay(RESET_DELAY_MS);
  digitalWrite(SET_PIN, VOLTAGE_INVERT(SET_VOLTAGE));
}
