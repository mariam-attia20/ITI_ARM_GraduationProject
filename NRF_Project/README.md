# nRF24L01 Button -> LED Link (STM32F103, bare-metal)

Hold the button on the transmitter board, the LED on the receiver board is ON.
Release it, the LED goes OFF.

## Wiring

### Transmitter board
| Signal | STM32F103 pin | Note |
|--------|---------------|------|
| Button | PA0 | button between PA0 and GND, internal pull-up enabled (pressed = 0) |
| nRF CE | PA3 | |
| nRF CSN| PA4 | |
| nRF SCK| PA5 | SPI1_SCK |
| nRF MISO| PA6 | SPI1_MISO |
| nRF MOSI| PA7 | SPI1_MOSI |
| nRF VCC| 3.3V | **never 5V** |
| nRF GND| GND | |
| nRF IRQ| not connected | driver is polling-based |

### Receiver board
Same nRF wiring, plus:

| Signal | STM32F103 pin | Note |
|--------|---------------|------|
| LED | PA1 | PA1 -> 220R/330R -> LED anode, cathode -> GND (HIGH = ON) |

## Hardware notes that actually decide whether this works
1. Solder/glue a **10 uF electrolytic + 100 nF ceramic capacitor directly across the
   module's VCC and GND pins**. The nRF draws current spikes during transmission and
   the Blue Pill 3.3V regulator is weak. Most "it doesn't receive anything" cases are this.
2. Keep the SPI jumper wires short (< 15 cm).
3. PA+LNA modules must be powered from a separate 3.3V supply (they need ~250 mA peaks).

## Software configuration
- `MCAL/RCC/RCC_interface.h` -> `RCC_CLOCK_SOURCE`
  - `RCC_HSI` (default): internal 8 MHz, works without a crystal.
  - `RCC_HSE_PLL_72MHZ`: use only if an 8 MHz crystal is fitted.
  `F_CPU` follows automatically and SysTick delays stay correct.
- `HAL/NRF24/NRF24_config.h` -> CE/CSN pins, RF channel, address, payload size.
  **These must be identical on both boards.**

## Protocol
Payload is 2 bytes: `[0] = 0xA5` header, `[1] = 0` released / `1` pressed.
The transmitter streams the current state every 20 ms instead of sending one
"pressed" event, so:
- a lost packet self-heals on the next 20 ms frame,
- the receiver turns the LED off after 300 ms of silence (transmitter off / out of range).

Auto-ACK and auto-retransmit are disabled on purpose - the repeated streaming already
gives robustness, and there is no MAX_RT stall to recover from.

## Build
Add all `.c` files to your project (`RCC_program.c`, `GPIO_program.c`, `SPI_program.c`,
`STK_program.c`, `NRF24_program.c`) plus **one** of the two application files:
- transmitter board -> `APP/main_TX.c`
- receiver board -> `APP/main_RX.c`

Never compile both `main_TX.c` and `main_RX.c` into the same image.

## Quick bring-up check
Before debugging the RF link, verify SPI first:

```c
u8 ch = HNRF24_u8ReadRegister(0x05);   /* RF_CH */
/* must read back 10 (NRF_RF_CHANNEL). 0x00 or 0xFF = SPI/wiring problem. */
```
