#define REGISTER_U8(varName, address) \
  volatile unsigned char *varName = (volatile unsigned char *)address;
#define REGISTER_U16(varName, address) \
  volatile unsigned int *varName = (volatile unsigned int *)address;

#define SET_AS_OUTPUT(port, bit) *port |= (0x01 << bit);
#define SET_AS_INPUT(port, bit) *port &= ~(0x01 << bit);

#define ACTIVATE_PULLUP(port, bit) *port |= (0x01 << bit);

#define SET_BIT_HIGH(port, bit) *port |= (0x01 << bit);
#define SET_BIT_LOW(port, bit) *port &= ~(0x01 << bit);

#define CHECK_BIT_HIGH(port, bit) ((*port & (0x01 << bit)) != 0)

REGISTER_U8(mainPORTA, 0x22);
REGISTER_U8(mainDDRA, 0x21);
REGISTER_U8(mainPINA, 0x20);

REGISTER_U8(mainPORTB, 0x25);
REGISTER_U8(mainDDRB, 0x24);
REGISTER_U8(mainPINB, 0x23);

REGISTER_U8(mainPORTH, 0x102);
REGISTER_U8(mainDDRH, 0x101);
REGISTER_U8(mainPINH, 0x100);

REGISTER_U8(mainPORTC, 0x28);
REGISTER_U8(mainDDRC, 0x27);
REGISTER_U8(mainPINC, 0x26);

REGISTER_U8(mainTCCR1B, 0x81);
REGISTER_U16(mainTCNT1, 0x84);
REGISTER_U8(mainTIFR1, 0x36);

#define CONTROL_BUTTON_PIN 4

#define YELLOW_LED_BIT 1
#define GREEN_LED_BIT 3
#define BLUE_LED_BIT 5
#define RED_LED_BIT 7

enum STATUS {
  INACTIVE = 0,
  STANDBY = 1,
  ACTIVE = 2,
  FAULT = 3
};

volatile STATUS currentStatus = INACTIVE;

bool prevButtonStatus = false;

void changeStatus(STATUS updatedStatus) {
  switch (currentStatus) {
  default:
  case INACTIVE:
    SET_BIT_LOW(mainPORTA, YELLOW_LED_BIT);
    break;
  case STANDBY:
    SET_BIT_LOW(mainPORTA, GREEN_LED_BIT);
    break;
  case ACTIVE:
    SET_BIT_LOW(mainPORTA, BLUE_LED_BIT);
    break;
  case FAULT:
    SET_BIT_LOW(mainPORTA, RED_LED_BIT);
    break;
  }

  currentStatus = updatedStatus;

  switch (updatedStatus) {
  default:
  case INACTIVE:
    SET_BIT_HIGH(mainPORTA, YELLOW_LED_BIT);
    break;
  case STANDBY:
    SET_BIT_HIGH(mainPORTA, GREEN_LED_BIT);
    break;
  case ACTIVE:
    SET_BIT_HIGH(mainPORTA, BLUE_LED_BIT);
    break;
  case FAULT:
    SET_BIT_HIGH(mainPORTA, RED_LED_BIT);
    break;
  }
}

void setup() {
  SET_AS_INPUT(mainDDRC, CONTROL_BUTTON_PIN);
  ACTIVATE_PULLUP(mainPORTC, CONTROL_BUTTON_PIN);

  SET_AS_OUTPUT(mainDDRA, YELLOW_LED_BIT);
  SET_AS_OUTPUT(mainDDRA, GREEN_LED_BIT);

  Serial.begin(9600);

  changeStatus(INACTIVE);
}

void loop() {
  bool currentButtonStatus = !CHECK_BIT_HIGH(mainPINC, CONTROL_BUTTON_PIN);

  if (currentButtonStatus && currentButtonStatus != prevButtonStatus) {
    if (currentStatus != INACTIVE) {
      changeStatus(INACTIVE);
    } else {
      changeStatus(STANDBY);
    }
  }
  prevButtonStatus = currentButtonStatus;

  switch (currentStatus) {
  case INACTIVE:
    Serial.println("INACTIVE");
    break;
  case STANDBY:
    Serial.println("STANDBY");
    break;
  case ACTIVE:
    Serial.println("ACTIVE");
    break;
  case FAULT:
    Serial.println("FAULT");
    break;
  default:
    changeStatus(INACTIVE);
  }
}
