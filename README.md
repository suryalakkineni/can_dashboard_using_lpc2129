# 🚗 CAN Dashboard System Using LPC2129

## 📌 Project Title

**CAN-Based Automotive Dashboard Monitoring System Using LPC2129 Microcontrollers**

---

# 📖 Abstract

Modern automobiles use multiple Electronic Control Units (ECUs) that communicate through a Controller Area Network (CAN) bus. CAN provides reliable, high-speed communication between different vehicle subsystems while reducing wiring complexity.

This project implements a simplified automotive dashboard using three LPC2129 microcontrollers connected through a CAN network.

The system continuously monitors:

* Engine Temperature
* Fuel Level
* Indicator Status

The information is transmitted through the CAN bus and displayed on an LCD dashboard. Indicator control is performed using external interrupts and dedicated LED animations.

This project demonstrates real-time embedded communication, distributed control systems, sensor interfacing, interrupt handling, and CAN protocol implementation.

---

# 🎯 Objectives

The main objectives of this project are:

* To understand CAN Bus communication.
* To implement distributed node architecture.
* To monitor fuel level using ADC.
* To monitor engine temperature using DS18B20.
* To display vehicle information on LCD.
* To control indicators using external interrupts.
* To exchange real-time data between multiple nodes.

---

# 🚘 System Overview

The system consists of three independent nodes:

1. Main Dashboard Node
2. Fuel Monitoring Node
3. Indicator Control Node

Each node performs a dedicated task and communicates through the CAN network.

---

# 🏗 System Architecture

```text
                         CAN BUS
--------------------------------------------------------------------------------

                    +----------------------+
                    |  MAIN DASHBOARD NODE |
                    |      LPC2129         |
                    +----------+-----------+
                               |
                +--------------+--------------+
                |                             |
                |                             |
                V                             V

     +------------------+         +------------------+
     |    FUEL NODE     |         | INDICATOR NODE   |
     |     LPC2129      |         |    LPC2129       |
     +------------------+         +------------------+

--------------------------------------------------------------------------------
```

---

# 🔧 Hardware Components

| Component                  | Quantity | Purpose                 |
| -------------------------- | -------- | ----------------------- |
| LPC2129 Microcontroller    | 3        | Main Controllers        |
| MCP2551 CAN Transceiver    | 3        | CAN Communication       |
| DS18B20 Temperature Sensor | 1        | Temperature Measurement |
| Potentiometer              | 1        | Fuel Sensor Simulation  |
| 16x2/20x4 LCD              | 1        | Dashboard Display       |
| LEDs                       | 8        | Indicator Display       |
| Push Buttons               | 2        | Indicator Switches      |
| Crystal Oscillator         | 3        | Clock Generation        |
| Power Supply               | 1        | System Power            |

---

# 💻 Software Requirements

| Software     | Purpose               |
| ------------ | --------------------- |
| Keil uVision | Program Development   |
| Embedded C   | Firmware Development  |
| Flash Magic  | Program Download      |
| Proteus      | Simulation (Optional) |

---

# 📡 Why CAN Protocol?

Controller Area Network (CAN) is widely used in automotive applications because:

* High Reliability
* Error Detection Mechanism
* Multi-Master Communication
* Reduced Wiring
* Real-Time Performance
* Low Cost Implementation

---

# 🔹 Node 1 : Main Dashboard Node

## Responsibilities

The Main Dashboard Node acts as the central controller of the system.

### Functions

* Reads engine temperature from DS18B20
* Receives fuel percentage from Fuel Node
* Detects Left and Right switch presses
* Sends indicator commands through CAN
* Displays complete dashboard information on LCD

---

## Hardware Connections

### LCD Interface

| LCD Pin | LPC2129 Pin |
| ------- | ----------- |
| D0-D7   | P0.0 - P0.7 |
| RW      | P0.9        |
| EN      | P0.10       |
| RS      | P0.11       |

### Switch Connections

| Switch       | LPC2129 Pin   |
| ------------ | ------------- |
| Left Switch  | P0.14 (EINT0) |
| Right Switch | P0.16 (EINT1) |

### Sensor Connection

| Sensor  | LPC2129 Pin |
| ------- | ----------- |
| DS18B20 | P0.15       |

### CAN Connection

| Signal   | LPC2129 Pin |
| -------- | ----------- |
| CAN TXD1 | P0.24       |
| CAN RXD1 | P0.25       |

---

# 🌡 Temperature Monitoring

The DS18B20 digital temperature sensor continuously measures engine temperature.

### Advantages

* Digital Sensor
* High Accuracy
* One-Wire Interface
* No ADC Required

### Temperature Calculation

DS18B20 returns:

```text
Temperature × 16
```

Example:

```text
25°C → 400
```

Conversion:

```c
Temperature = RawValue >> 4;
```

---

# ⛽ Node 2 : Fuel Monitoring Node

## Responsibilities

* Read fuel sensor value
* Convert ADC value into percentage
* Send fuel percentage to Main Node through CAN

---

## Fuel Sensor

A potentiometer is used to simulate the fuel tank sensor.

### ADC Specifications

```text
Resolution : 10-bit
Range      : 0 – 1023
Reference  : 3.3V
```

### Fuel Calculation

```c
fuelPercent = (adcValue * 100) / 1023;
```

### Examples

| ADC Value | Fuel % |
| --------- | ------ |
| 0         | 0%     |
| 256       | 25%    |
| 512       | 50%    |
| 768       | 75%    |
| 1023      | 100%   |

---

## ADC Connection

| ADC Channel | LPC2129 Pin |
| ----------- | ----------- |
| AD0.0       | P0.27       |

---

# 🚦 Node 3 : Indicator Node

## Responsibilities

* Receive commands from Main Node
* Control LED indicators
* Generate left/right animations

---

## LED Connections

| LED  | Pin  |
| ---- | ---- |
| LED0 | P0.0 |
| LED1 | P0.1 |
| LED2 | P0.2 |
| LED3 | P0.3 |
| LED4 | P0.4 |
| LED5 | P0.5 |
| LED6 | P0.6 |
| LED7 | P0.7 |

---

# 🔄 Indicator Operation

## LEFT Indicator

```text
LED7 → LED6 → LED5 → LED4 → LED3 → LED2 → LED1 → LED0
```

CAN Command:

```text
'L'
```

---

## RIGHT Indicator

```text
LED0 → LED1 → LED2 → LED3 → LED4 → LED5 → LED6 → LED7
```

CAN Command:

```text
'R'
```

---

## OFF State

```text
All LEDs OFF
```

CAN Command:

```text
'O'
```

---

# 📨 CAN Communication

## Fuel Data Frame

```text
ID      : 0x01
DLC     : 4
Data1   : Fuel Percentage
```

Example:

```text
ID = 0x01
Data1 = 75
```

---

## Indicator Command Frame

```text
ID      : 0x11
DLC     : 1
```

Commands:

```text
'L' = Left Indicator
'R' = Right Indicator
'O' = Indicator OFF
```

---

# ⚡ Interrupt Handling

Two external interrupts are used.

| Interrupt | Pin   | Function        |
| --------- | ----- | --------------- |
| EINT0     | P0.14 | Left Indicator  |
| EINT1     | P0.16 | Right Indicator |

### Interrupt Process

```text
Button Press
      ↓
External Interrupt
      ↓
Update Indicator State
      ↓
Transmit CAN Message
      ↓
Indicator Node Receives Command
      ↓
LED Pattern Executes
```

---

# 📟 LCD Output

Normal Display:

```text
--------------------------------
<<< DASH BOARD >>>

Fuel : 75 %

Indicator  < >

Temp : 30 C
--------------------------------
```

---

# 🔄 Complete Project Flow

```text
START
   │
   ▼
Initialize LCD
   │
Initialize CAN
   │
Initialize ADC
   │
Initialize DS18B20
   │
Wait for Events
   │
 ┌───────────────┬────────────────┐
 │               │                │
 ▼               ▼                ▼

Read Fuel    Read Temp      Switch Press

 │               │                │

 ▼               ▼                ▼

Send CAN      Display       Send Indicator
Message       Data          Command

 │               │                │

 └───────────────┴────────────────┘

               │

               ▼

          Update LCD

               │

               ▼

            Repeat
```

---

# ✅ Features

* CAN Based Communication
* Distributed Node Architecture
* Real-Time Fuel Monitoring
* Real-Time Temperature Monitoring
* LCD Dashboard Interface
* Interrupt-Based Indicator Control
* LED Animation
* Embedded C Firmware
* Automotive Dashboard Simulation

---

# 🎓 Learning Outcomes

Through this project, we learned:

* CAN Protocol Implementation
* LPC2129 Programming
* ADC Interfacing
* DS18B20 Interfacing
* LCD Interfacing
* Interrupt Programming
* Embedded C Development
* Automotive Embedded Systems
* Real-Time Communication Networks

---

# 🔮 Future Enhancements

* Vehicle Speed Monitoring
* RPM Measurement
* OBD Integration
* GPS Tracking
* GSM Alerts
* Touchscreen Dashboard
* Data Logging to SD Card
* Bluetooth Connectivity
* Mobile App Integration

---

# 👨‍💻 Author

**Surya Lakkineni**

Embedded Systems Project

**CAN Dashboard System Using LPC2129 and CAN Protocol**

Year: 2026
