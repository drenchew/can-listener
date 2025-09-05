# CAN Listener

A real-time CAN bus analyzer built with ESP32, featuring an ST7735 TFT display and FreeRTOS 

## Features

- **Real-time CAN bus monitoring** with MCP2515 CAN controller
- **ST7735 TFT display** for visual message display
- **FreeRTOS multi-tasking** for concurrent operations
- **ESP32 utilization** for better performance

## Hardware Requirements

### Components
- ESP32 development board (esp32 wroom dev kit)
- MCP2515 CAN bus module
- ST7735 TFT display (128x160 pixels)


### Wiring Diagram




## Installation

### Prerequisites
- [PlatformIO](https://platformio.org/) IDE or CLI
- ESP32 development environment

### Setup
1. Clone the repository:
   ```bash
   git clone https://github.com/drenchew/can-listener.git
   cd can-listener
   ```

2. Open the project in PlatformIO

3. Build and upload:
   ```bash
   pio run --target upload
   ```

## Configuration

### CAN Bus Settings
The default configuration uses:
- **Baud rate**: 500 kbps
- **Crystal frequency**: 8 MHz
- **Mode**: Normal operation




### Display Output
The TFT shows:
```
CAN Messages:
ID:123 L:8
01 02 03 04 05 06 07 08
ID:456 L:4
AA BB CC DD
```

### Serial Output
```
ID: 0x123 DLC: 8 Data: 1 2 3 4 5 6 7 8
ID: 0x456 DLC: 4 Data: AA BB CC DD
```

## Dependencies

- `coryjfowler/mcp_can@^1.5.1` - MCP2515 CAN controller library
- `adafruit/Adafruit ST7735 and ST7789 Library@^1.10.3` - Display driver
- `adafruit/Adafruit GFX Library@^1.11.9` - Graphics library
- `FreeRTOS` - Real-time operating system (built into ESP32)



## License

This project is open source. Feel free to use, modify, and distribute.


## Acknowledgments

- Adafruit
- Cory J. Fowler 
