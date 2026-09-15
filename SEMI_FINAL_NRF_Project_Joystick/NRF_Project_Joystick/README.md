# nRF24L01 Joystick -> 4 LEDs (STM32F103, bare-metal)

Move the joystick on the transmitter board, the matching LED on the receiver
board lights up and stays on for as long as you hold the stick there.

| Stick direction | Transmitted value | LED on receiver |
|-----------------|-------------------|-----------------|
| VRx positive    | +1 .. +125        | LED1 **PA8**    |
| VRx negative    | -1 .. -125        | LED2 **PA9**    |
| VRy positive    | +1 .. +125        | LED3 **PA10**   |
| VRy negative    | -1 .. -125        | LED4 **PA11**   |
| centered        | 0                 | all off         |

PB0 -> SHEMAL (FO2 W T7T(TALT PIN)) -> VRx
PB1 -> YMEN (YMEN W SHEMAL(RAB3 PIN)) -> VRy

## Pin change you need to know about

**PB6 and PB7 cannot be analog inputs on the STM32F103.** ADC1 is wired only to
PA0..PA7 (channels 0..7) and PB0, PB1 (channels 8, 9). PB6/PB7 are I2C1 / TIM4
pins with no ADC path, so putting them in analog mode just leaves them floating.

PB0 and PB1 are the only analog capable pins on port B and are what this code
uses. To move them, edit the six `JOY_*` defines at the top of `APP/main_TX.c`.

## Wiring

### Transmitter board
| Signal | STM32F103 pin | Note |
|--------|---------------|------|
| Joystick VRx | PB0 | analog, ADC1 channel 8 |
| Joystick VRy | PB1 | analog, ADC1 channel 9 |
| Joystick VCC | 3.3V | **not 5V** - a 5V-fed pot would push >3.3V into the ADC |
| Joystick GND | GND | |
| nRF CE  | PA3 | |
| nRF CSN | PA4 | |
| nRF SCK | PA5 | SPI1_SCK |
| nRF MISO| PA6 | SPI1_MISO |
| nRF MOSI| PA7 | SPI1_MOSI |
| nRF VCC | 3.3V | **never 5V** |
| nRF GND | GND | |
| nRF IRQ | not connected | driver is polling-based |

The joystick's SW (push) pin is unused here.

### Receiver board
Same nRF wiring, plus four LEDs:

| Signal | STM32F103 pin | Note |
|--------|---------------|------|
| LED1 | PA8  | pin -> 220R/330R -> LED anode, cathode -> GND (HIGH = ON) |
| LED2 | PA9  | same |
| LED3 | PA10 | same |
| LED4 | PA11 | same - see the note below |

`PA11` is also USB_DM on a Blue Pill. It works fine as a plain output as long as
you are not using USB. If your board has anything on the USB connector, move
LED4 to a free pin by editing `LED_YN_PIN` in `APP/main_RX.c`.

## Hardware notes that actually decide whether this works
1. Solder/glue a **10 uF electrolytic + 100 nF ceramic capacitor directly across
   the nRF module's VCC and GND pins**. The radio draws current spikes during
   transmission and the Blue Pill 3.3V regulator is weak. Most "it doesn't
   receive anything" cases are this.
2. Keep the SPI jumper wires short (< 15 cm).
3. PA+LNA modules must be powered from a separate 3.3V supply (~250 mA peaks).
4. **Do not touch the joystick while the transmitter powers up.** The first
   thing it does is average 64 samples per axis to learn where that particular
   stick rests. Holding it off-center during boot makes that position the new
   zero.

## How an axis becomes -125 .. 0 .. +125

1. Four ADC readings are averaged per axis per loop.
2. The learned center is subtracted, giving a signed deviation.
3. Inside +/- `AXIS_DEADZONE` (150 counts) the result is forced to exactly **0**.
4. Outside it, the remaining travel is scaled to 1..125, with the positive and
   negative halves scaled separately so both reach full 125 even on a joystick
   whose center does not sit at 2048.

Tuning, all in `APP/main_TX.c`:
- LEDs flicker with the stick at rest -> raise `AXIS_DEADZONE`.
- Stick has to travel too far before anything happens -> lower it.
- An axis moves the wrong way -> set `JOY_X_INVERT` or `JOY_Y_INVERT` to 1.

## Software configuration
- `MCAL/RCC/RCC_interface.h` -> `RCC_CLOCK_SOURCE`
  - `RCC_HSI` (default): internal 8 MHz, works without a crystal. ADC clock
    ends up at 4 MHz.
  - `RCC_HSE_PLL_72MHZ`: use only if an 8 MHz crystal is fitted. The ADC
    prescaler switches to /6 automatically so the ADC stays under its 14 MHz
    limit.
  `F_CPU` follows automatically and SysTick delays stay correct.
- `HAL/NRF24/NRF24_config.h` -> CE/CSN pins, RF channel, address, payload size.
  **These must be identical on both boards.**
  Current settings: channel 108, 1 Mbps, auto-ACK on, 3-byte payload,
  address `E7 E7 E7 E7 E7`.

## Protocol
Payload is 3 bytes:

| Byte | Meaning |
|------|---------|
| 0 | `0xA5` header, used to reject garbage frames |
| 1 | VRx as a signed 8-bit value, -125 .. +125 |
| 2 | VRy as a signed 8-bit value, -125 .. +125 |

The transmitter streams the current state every 10 ms rather than sending
one-shot events, and fires a burst of 6 frames spaced 2 ms apart whenever the
direction changes. So:
- a lost packet self-heals on the next frame,
- a direction change is never carried by a single packet,
- the receiver clears all four LEDs after 1000 ms of silence (transmitter off or
  out of range).

Leaving the center is accepted instantly; returning to the center is only
accepted after the axis has been steadily centered for 50 ms, so noise at the
deadzone edge cannot make an LED flicker off mid-hold.

## Build
Add all `.c` files to your project:

```
LIB/          (headers only)
MCAL/RCC/RCC_program.c
MCAL/GPIO/GPIO_program.c
MCAL/SPI/SPI_program.c
MCAL/STK/STK_program.c
MCAL/ADC/ADC_program.c        <-- new
HAL/NRF24/NRF24_program.c
```

plus **one** of the two application files:
- transmitter board -> `APP/main_TX.c`
- receiver board -> `APP/main_RX.c`

Never compile both `main_TX.c` and `main_RX.c` into the same image.

## Quick bring-up check

**Radio / SPI**, before debugging anything RF:

```c
u8 ch = HNRF24_u8ReadRegister(0x05);   /* RF_CH */
/* must read back 108 (NRF_RF_CHANNEL). 0x00 or 0xFF = SPI/wiring problem. */
```

**ADC**, before debugging the scaling: read the raw value straight after
`MADC1_voidInit()` and look at it in the debugger.

```c
u16 raw = MADC1_u16ReadChannel(ADC1_CH8);
/* centered stick should read somewhere around 1900..2200.
 * 0 or 4095 that never moves = wrong pin, pin not in analog mode,
 * or the joystick is not powered. */
```

## Changed from the button version
- `MCAL/ADC/` added (new driver).
- `MCAL/RCC/` gained `MRCC_voidSetAdcPrescaler()`.
- `NRF24_config.h`: `NRF_PAYLOAD_SIZE` 2 -> **3**. Must be reflashed on **both**
  boards, a mismatch here means nothing is received at all.
- `APP/main_TX.c` and `APP/main_RX.c` rewritten.
- Everything else (GPIO, SPI, STK, the nRF24 driver itself) is untouched.
