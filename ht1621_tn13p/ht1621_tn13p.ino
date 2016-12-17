//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8..
// 2016.12.17 copyright BotanicFields
// demonstration of aitendo TN13P, an LCD for KENWOOD portable audio player
// using HOLTEK HT1621 LCD controller

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8..
// driver for HOLTEK HT1621 RAM mapping 32x4 LCD controller
// LCD RAM image
const int HT1621_RAM_SIZE = 32;
unsigned char HT1621_ram[HT1621_RAM_SIZE];

// pin assignment of arduino uno
const int HT1621_CS_N = 7;
const int HT1621_RD_N = 6;
const int HT1621_WR_N = 5;
const int HT1621_DATA = 4;

// driver prototypes
void HT1621_initialize(void);
void HT1621_command(const unsigned char command_code);
void HT1621_write(const unsigned char address_min, const unsigned char address_max, const unsigned char data[]);
void HT1621_write_bit(const unsigned char data_masked);

//..:....:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8..
// driver for HOLTEK HT1621 RAM mapping 32x4 LCD controller
void HT1621_initialize(void) {
  pinMode(HT1621_CS_N, OUTPUT);
  pinMode(HT1621_RD_N, OUTPUT);
  pinMode(HT1621_WR_N, OUTPUT);
  pinMode(HT1621_DATA, OUTPUT);
  
  digitalWrite(HT1621_CS_N, HIGH);
  digitalWrite(HT1621_RD_N, HIGH);
  digitalWrite(HT1621_WR_N, HIGH);
  digitalWrite(HT1621_DATA, HIGH);
  
//  HT1621_command(B00000000); //SYS DIS(default)
  HT1621_command(B00000001); //SYS EN

//  HT1621_command(B00000010); //LCD OFF(default)
  HT1621_command(B00000011); //LCD ON

//  HT1621_command(B00010100); //XTAL 32K
  HT1621_command(B00011000); //RC 256K(default)
//  HT1621_command(B00011100); //EXT 256K

  HT1621_command(B00000100); //TIMER DIS
//  HT1621_command(B00000110); //TIMER EN
//  HT1621_command(B00001100); //CLR TIMER

  HT1621_command(B00000101); //WDT DIS
//  HT1621_command(B00000111); //WDT EN
//  HT1621_command(B00001110); //CLR WDT

  HT1621_command(B10000000); //IRQ_N DIS(default)
//  HT1621_command(B10001000); //IRQ_N EN

//  HT1621_command(B10100000); //F1
//  HT1621_command(B10100001); //F2
//  HT1621_command(B10100010); //F4
//  HT1621_command(B10100011); //F8
//  HT1621_command(B10100100); //F16
//  HT1621_command(B10100101); //F32
//  HT1621_command(B10100110)；//F64
  HT1621_command(B10100111); //F128(default)

  HT1621_command(B00001000); //TONE OFF(default)
//  HT1621_command(B00001001); //TONE ON
//  HT1621_command(B01000000); //TONE 4K
//  HT1621_command(B01100000); //TONE 2K

//  HT1621_command(B11100000); //TOPT
  HT1621_command(B11100011); //TNORMAL(default)
    
//  HT1621_command(B00100000); //BIAS 1/2 2 commons option
//  HT1621_command(B00100100); //BIAS 1/2 3 commons option
//  HT1621_command(B00101000); //BIAS 1/2 4 commons option
//  HT1621_command(B00100001); //BIAS 1/3 2 commons option
  HT1621_command(B00100101); //BIAS 1/3 3 commons option
//  HT1621_command(B00101001); //BIAS 1/3 4 commons option

  for (int i = 0; i < HT1621_RAM_SIZE; i++) {
    HT1621_ram[i] = 0;
  }
  HT1621_write(0, HT1621_RAM_SIZE - 1, HT1621_ram);
}

// command_code: xxxx-xxxx-x
void HT1621_command(const unsigned char command_code) {
  digitalWrite(HT1621_CS_N, LOW);

  // command: B100
  HT1621_write_bit(1);
  HT1621_write_bit(0);
  HT1621_write_bit(0);

  // command_code: xxxx-xxxx
  for (unsigned char bit_mask = B10000000; bit_mask > 0; bit_mask >>= 1) {
    HT1621_write_bit(command_code & bit_mask);
  }
  // command_code: -x
  HT1621_write_bit(0);

  digitalWrite(HT1621_CS_N, HIGH);
}

// write multiple data into HT1621
// address_min: B000xxxxx   0..31
// address_max: B000xxxxx   0..31 >= address_min
// data[0..31]: B0000xxxx
void HT1621_write(const unsigned char address_min, const unsigned char address_max, const unsigned char data[]) {
  digitalWrite(HT1621_CS_N, LOW);

  // command: B101
  HT1621_write_bit(1);
  HT1621_write_bit(0);
  HT1621_write_bit(1);

  // address: A5..A0
  for (unsigned char bit_mask = B00100000; bit_mask > 0; bit_mask >>= 1) {
    HT1621_write_bit(address_min & bit_mask);
  }
  // data: D0..D3
  for (int i = address_min; i <= address_max; i++) {
    for (unsigned char bit_mask = B00000001; bit_mask <= B00001000; bit_mask <<= 1) {
      HT1621_write_bit(data[i] & bit_mask);
    }
  }
  digitalWrite(HT1621_CS_N, HIGH);
}

// write 1 bit into HT1621
void HT1621_write_bit(const unsigned char data_masked) {
  unsigned char data_out = (data_masked == 0) ? LOW : HIGH ;
  digitalWrite(HT1621_WR_N, LOW);
  digitalWrite(HT1621_DATA, data_out);
  digitalWrite(HT1621_WR_N, HIGH);
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8..
// driver for aitendo TN13P
// image buffer of LCD
unsigned char TN13P_kenwood  = 0;  // -kenwood 
unsigned char TN13P_leftauml = 0;  // -----uml (left arrows: upper middle lower)
unsigned char TN13P_larfuk12 = 0;  // larfuk12 (loop auto rev fwd use key 1h 2h)
unsigned int TN13P_i = 0;

// driver prototypes
void TN13P_demo1(void);
void TN13P_update(void);

// map HT1621 RAM images to TN13P pins of segment and common
// TN13P pin  1  HT1621_ram[0] = B00000xxx;  // -    -    -
// TN13P pin  2  HT1621_ram[1] = B00000xxx;  // -    -    -
// TN13P pin  3  HT1621_ram[2] = B00000xxx;  // low  up   mid
// TN13P pin  4  HT1621_ram[3] = B00000xxx;  // -    -    -
// TN13P pin  5  HT1621_ram[4] = B00000xxx;  // -    -    loop
// TN13P pin  6  HT1621_ram[5] = B00000xxx;  // auto E    K
// TN13P pin  7  HT1621_ram[6] = B00000xxx;  // <R   W    N
// TN13P pin  8  HT1621_ram[7] = B00000xxx;  // F>   O1   key
// TN13P pin  9  HT1621_ram[8] = B00000xxx;  // 2H  USE   1H
// TN13P pin 10  HT1621_ram[9] = B00000xxx;  // -    D    O2
// TN13P pin 11  common                                   D0
// TN13P pin 12  common                              D1
// TN13P pin 13  common                         D2
void TN13P_update(void) {
  unsigned char image = 0;
  if (TN13P_leftauml & B00000001) { image |= B00000100; }  // lower
  if (TN13P_leftauml & B00000100) { image |= B00000010; }  // upper
  if (TN13P_leftauml & B00000010) { image |= B00000001; }  // middle
  HT1621_ram[2] = image;

  image = 0;
  if (TN13P_larfuk12 & B10000000) { image |= B00000001; }  // loop
  HT1621_ram[4] = image;

  image = 0;
  if (TN13P_larfuk12 & B01000000) { image |= B00000100; }  // auto
  if (TN13P_kenwood  & B00100000) { image |= B00000010; }  // -E-----
  if (TN13P_kenwood  & B01000000) { image |= B00000001; }  // K------
  HT1621_ram[5] = image;

  image = 0;
  if (TN13P_larfuk12 & B00100000) { image |= B00000100; }  // <R
  if (TN13P_kenwood  & B00001000) { image |= B00000010; }  // ---W---
  if (TN13P_kenwood  & B00010000) { image |= B00000001; }  // --N----
  HT1621_ram[6] = image;

  image = 0;
  if (TN13P_larfuk12 & B00010000) { image |= B00000100; }  // F>
  if (TN13P_kenwood  & B00000100) { image |= B00000010; }  // ----O--
  if (TN13P_larfuk12 & B00000100) { image |= B00000001; }  // key
  HT1621_ram[7] = image;

  image = 0;
  if (TN13P_larfuk12 & B00000001) { image |= B00000100; }  // 2H
  if (TN13P_larfuk12 & B00001000) { image |= B00000010; }  // USE
  if (TN13P_larfuk12 & B00000010) { image |= B00000001; }  // 1H
  HT1621_ram[8] = image;

  image = 0;
  if (TN13P_kenwood  & B00000001) { image |= B00000010; }  // ------D
  if (TN13P_kenwood  & B00000010) { image |= B00000001; }  // -----O-
  HT1621_ram[9] = image;

  HT1621_write(2, 9, HT1621_ram);
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8..
// demo for TN13P Kenwood LCD
void TN13P_demo1(void) {
  const unsigned char TN13P_kenwood_pattern[18] = {
    B01000000,
    B01100000,
    B01110000,
    B01111000,
    B01111100,
    B01111110,
    B01111111,
    B00000000,
    B01111111,
    B00000000,
    B01110000,
    B00001111,
    B00110001,
    B00001110,
    B00010100,
    B00001000,
    B00000000,
    B00000000,
  };
  TN13P_kenwood = TN13P_kenwood_pattern[TN13P_i % 18];
  TN13P_leftauml++;
  TN13P_larfuk12 = random(0x100);
  TN13P_i++;
  TN13P_update();
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8..
// demonstration main
void setup() {
  HT1621_initialize();
}
void loop() {
  TN13P_demo1();
  delay(500);
}

