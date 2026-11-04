#include "Plan.h"

const unsigned long PATTERN_PAUSE_US = 500000;


// ==========================================
// COOL PATTERNS
// ==========================================

const uint16_t FULL = 0x000;
const uint16_t EMPTY = 0xFFF;
const uint16_t SQUARE = 0x003;
const uint16_t CIRCLE = 0xFFE;

// LINES
const uint16_t LINE_L = 0xF87;
const uint16_t LINE_D = 0xFFF-2*2;
const uint16_t LINE_U = 0x53F;
const uint16_t LINE_R = 0x8DF;

// TRIANGLES
const uint16_t tri_down = 0x601;

// 2 LEDS :
const uint16_t L2_DL = 0x757;
const uint16_t L2_UL = 0x79B;


// 1 LED :
const uint16_t L1_VFD = 0xFFD;
const uint16_t L1_UR = 0x599;
const uint16_t L1_L = 0xFEF;
const uint16_t L1_DL = 0xFF7;
const uint16_t L1_D = 0xFFB;




// ==========================================
// COOL ANIMS
// ==========================================

const uint16_t cirsqa[] = { SQUARE, CIRCLE, FULL, EMPTY  };
const uint16_t test2[] = { EMPTY, 0xF85, 0xF84, 0x755, 0x799 , 0x849  };
const uint16_t omegas[] = { LINE_L-1, LINE_U-1, LINE_R-1, CIRCLE };
const uint16_t lines[] = { LINE_D, LINE_L, LINE_U, LINE_R, EMPTY };
const uint16_t singles[] = { L1_VFD, L1_UR-2, L1_L, L1_DL, L1_D, EMPTY };
const uint16_t test[] = { L2_DL, L2_UL, EMPTY };


const Plan P1 = {
  PLAN_D | PLAN_DL,
  false,
  true
};
const Plan P2 = {
  PLAN_D | PLAN_DL | PLAN_U,
  true,
  false
};

const Plan plan_test[] { P1, P2 };


// ==========================================
// ANIMATIONS
// ==========================================
// template <size_t N>
// void animate(const uint16_t (&anim)[N])
// {
//   int i = 0;
//   int count = N;
//   while (1)
//   {
//     uint16_t currentPattern = anim[i];
//     sendBU2090(currentPattern);
//     i = translate(i, 1, count);
//     Serial.println("[ANIM] " + String(i) + " : " + String((uint16_t)currentPattern, HEX));
//     waitAndSample(PATTERN_PAUSE_US);
//   }
// }


template <size_t N>
void animate(const Plan (&anim)[N])
{
  int i = 0;
  int count = N;
  while (1)
  {
    uint16_t currentPattern = planToBU2090(anim[i]);
    sendBU2090(currentPattern);
    i = translate(i, 1, count);
    Serial.println("[ANIM] " + String(i) + " : " + String((uint16_t)currentPattern, HEX));
    waitAndSample(PATTERN_PAUSE_US);
  }
}

void singlePattern(uint16_t pattern)
{
  sendBU2090(pattern);
  waitAndSample(PATTERN_PAUSE_US);
}

void loadPattern()
{
  uint16_t pattern = 0xFFF;
  int decalage = 0;
  while (1)
  {
    sendBU2090(pattern);
    pattern = translate(pattern,-4 - decalage);
    decalage+=4;
    waitAndSample(PATTERN_PAUSE_US/8);
  }
}
void scanOutputs()
{
    for (int bit = 0; bit < 12; bit++)
    {
        uint16_t pattern = 0xFFF ^ (1U << bit);

        Serial.print("Testing output bit: ");
        Serial.println(bit);

        sendBU2090(pattern);

        waitAndSample(1000000);
    }

    sendBU2090(0xFFF);
}



// helpers
uint16_t translate(uint16_t pattern, int delta)
{
    int32_t result = (int32_t)pattern + delta;

    result = ((result % 4096) + 4096) % 4096;

    return (uint16_t)result;
}
int translate(int thing, int delta, int max_count)
{
  int new_thing = thing + delta;

  if (new_thing >= max_count)
  {
    while (new_thing >= max_count)
    {
      new_thing -= max_count;
    }
  }
  else if (new_thing < 0)
  {
    while (new_thing < 0)
    {
      new_thing += max_count;
    }
  }

  return constrain(new_thing, 0, max_count);
}

uint16_t ledsToBU2090(uint16_t leds)
{
    // Start with all LEDs OFF (active-low)
    uint16_t pattern = 0x0FFF;

    // Input bit -> BU2090 output bit
    const uint8_t mapping[10] = {
        0,  // C
        1,  // VFD
        8,  // DR
        2,  // D
        3,  // DL
        4,  // L
        6,  // UL
        7,  // U
        9,  // UR
        10  // R
    };

    for (uint8_t i = 0; i < 10; i++)
    {
        if (leds & (1U << i))
        {
            // Clear the corresponding output bit (LED ON)
            pattern &= ~(1U << mapping[i]);
        }
    }

    return pattern;
}

uint16_t planToBU2090(const Plan& plan)
{
    uint16_t leds = plan.leds;

    // Input order: C, VFD, DR, D, DL, L, UL, U, UR, R
    uint16_t input = leds << 2;

    if (plan.C)
        input |= (1U << 0);

    if (plan.VFD)
        input |= (1U << 1);

    return ledsToBU2090(input);
}


// ==========================================
// TEST ALL 4096 PATTERNS
// ==========================================

void testAllPatterns() {

  for (uint16_t pattern = 0xFFF; pattern >= 0; pattern--) {

    sendBU2090(pattern);

    waitAndSample(PATTERN_PAUSE_US);
  }
}

// ==========================================
// MAIN LOOP
// ==========================================

void loop()
{
  // animate(test);
  // scanOutputs();
  animate(plan_test);
  // loadPattern();
}
