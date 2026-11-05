#ifndef MID_H
#define MID_H

#include <Arduino.h>
#include "Low.h"
#include "Plan.h"
#include "Anim.h"





// ==========================================
// HELPERS METHODS
// ==========================================

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

// ==========================================
// CONVERSION METHODS
// ==========================================

uint16_t ledsToBU2090(uint16_t leds)
{
	// StarL with all LEDs OFF (active-low)
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
// ANIMATION METHODS
// ==========================================

void singlePlan(Plan plan)
{
	sendBU2090(planToBU2090(plan));
	waitAndSample(500*ms);
}


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
		waitAndSample(500*ms); // default time delay
	}
}

template <size_t N, size_t M>
void animate(const Plan (&anim)[N], const int (&timings)[M])
{
	if (N != M)
	{
		Serial.println("[ERROR] Could not play anim since the timings table does not have the same size as the plans one");
		return;
	}

	int i = 0;
	int count = N;
	while (1)
	{
		uint16_t currentPattern = planToBU2090(anim[i]);
		sendBU2090(currentPattern);
		Serial.println("[ANIM] " + String(i) + " : " + String((uint16_t)currentPattern, HEX));
		waitAndSample(timings[i]*tick);
		i = translate(i, 1, count);
	}
}

template <size_t N>
void animate(const Anim<N>& anim)
{
	size_t i = 0;
	while (i < N)
	{
		uint16_t pattern = planToBU2090(anim.plans[i]);
		sendBU2090(pattern);
		// Serial.println("[ANIM] " + String(i) + " : " + String((uint16_t)pattern, HEX) + ".  Now waiting for " + String());
		waitAndSample(anim.times[i]*tick);
		i = (i + 1) % N;
	}
}




#endif