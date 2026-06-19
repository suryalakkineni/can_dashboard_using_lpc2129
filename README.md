# CAN Dashboard System using LPC2129 and CAN Protocol

## Overview

The CAN Dashboard System is a distributed embedded system developed using LPC2129 microcontrollers and CAN (Controller Area Network) communication. The project simulates an automotive dashboard where different nodes perform dedicated tasks and exchange information through the CAN bus.

The system consists of three independent nodes:

1. Main Dashboard Node
2. Fuel Monitoring Node
3. Indicator Control Node

The Main Dashboard Node receives data from other nodes and displays vehicle information such as fuel level, engine temperature, and indicator status on an LCD.

---

# Project Objectives

* Learn CAN Bus communication in embedded systems.
* Implement distributed node architecture.
* Interface sensors with LPC2129.
* Display real-time vehicle parameters on LCD.
* Control indicator LEDs through CAN messaging.
* Use external interrupts for switch detection.

---

# Hardware Requirements

| Component                  | Purpose                        |
| -------------------------- | ------------------------------ |
| LPC2129 Microcontroller    | Main Controller                |
| MCP2551 CAN Transceiver    | CAN Communication              |
| DS18B20 Temperature Sensor | Engine Temperature Measurement |
| Potentiometer              | Fuel Level Simulation          |
| 16x2/20x4 LCD              | Dashboard Display              |
| LEDs (8 Nos.)              | Indicator Display              |
| Push Buttons               | Left/Right Indicator Control   |
| Crystal Oscillator         | System Clock                   |
| Power Supply (5V/3.3V)     | Circuit Power                  |

---

# Software Requirements

* Embedded C
* Keil uVision
* Flash Magic
* Proteus (Optional for Simulation)

---

# System Architecture

```text
                  CAN BUS
--------------------------------------------------

     Fuel Node  --------\
                          \
                           \
                            --> Main Dashboard Node
                           /
                          /
 Indicator Node --------/

--------------------------------------------------
```

---

# Node Descriptions

## 1. Main Dashboard Node

### Functions

* Reads engine temperature from DS18B20.
* Receives fuel percentage from Fuel Node.
* Detects LEFT and RIGHT switch presses.
* Sends indicator commands through CAN.
* Displays all information on LCD.

### Connected Devices

| Device        | LPC2129 Pin   |
| ------------- | ------------- |
| LCD Data Pins | P0.0 - P0.7   |
| LCD RW        | P0.9          |
| LCD EN        | P0.10         |
| LCD RS        | P0.11         |
| Left Switch   | P0.14 (EINT0) |
| DS18B20       | P0.15         |
| Right Switch  | P0.16 (EINT1) |
| CAN TXD1      | P0.24         |
| CAN RXD1      | P0.25         |

### LCD Display Example

```text
<<< DASH BOARD >>>

Fuel : 75%
Indicator  < >
Temp : 28 C
```

---

## 2. Fuel Monitoring Node

### Functions

* Reads fuel sensor value using ADC.
* Converts ADC value into fuel percentage.
* Sends fuel percentage to Main Dashboard Node using CAN.

### ADC Working

The fuel sensor is simulated using a potentiometer.

```text
ADC Range     : 0 - 1023
Voltage Range : 0V - 3.3V
```

Fuel Percentage Calculation:

```c
Fuel_Percentage = (ADC_Value * 100) / 1023;
```

### ADC Channels

| Channel | Pin   |
| ------- | ----- |
| AD0.0   | P0.27 |
| AD0.1   | P0.28 |
| AD0.2   | P0.29 |
| AD0.3   | P0.30 |

---

## 3. Indicator Control Node

### Functions

* Receives CAN messages from Main Node.
* Controls LED indicator animations.
* Supports LEFT, RIGHT, and OFF modes.

### CAN Commands

| Data | Function        |
| ---- | --------------- |
| 'L'  | Left Indicator  |
| 'R'  | Right Indicator |
| 'O'  | Indicator OFF   |

### LED Connections

```text
P0.0 → LED0
P0.1 → LED1
P0.2 → LED2
P0.3 → LED3
P0.4 → LED4
P0.5 → LED5
P0.6 → LED6
P0.7 → LED7
```

### Left Indicator Pattern

```text
LED7 → LED6 → LED5 → LED4 → LED3 → LED2 → LED1 → LED0
```

### Right Indicator Pattern

```text
LED0 → LED1 → LED2 → LED3 → LED4 → LED5 → LED6 → LED7
```

---

# CAN Communication

## CAN Message IDs

| CAN ID    | Purpose                   |
| --------- | ------------------------- |
| 0x11      | Indicator Control Command |
| Custom ID | Fuel Data Transmission    |

---

## Indicator Data Frame

```text
ID      : 0x11
DLC     : 1

Data:
'L' -> Left
'R' -> Right
'O' -> Off
```

---

# Temperature Monitoring

The DS18B20 sensor provides temperature data.

Example:

```text
Raw Data = Temperature × 16

25°C = 25 << 4
```

Conversion:

```c
Temperature = RawValue >> 4;
```

---

# Interrupt Configuration

## External Interrupts

| Interrupt | Pin   | Function               |
| --------- | ----- | ---------------------- |
| EINT0     | P0.14 | Left Indicator Switch  |
| EINT1     | P0.16 | Right Indicator Switch |

### Operation

Pressing a switch:

1. Generates External Interrupt.
2. Updates indicator state.
3. Sends CAN message.
4. Indicator Node receives command.
5. LEDs animate accordingly.

---

# Project Flow

```text
Start

 ↓

Initialize LCD

 ↓

Initialize CAN

 ↓

Read Temperature

 ↓

Receive Fuel Data

 ↓

Display Information

 ↓

Check Interrupt Events

 ↓

Send Indicator Commands

 ↓

Indicator Node Controls LEDs

 ↓

Repeat Forever
```

---

# Features

* CAN Bus Communication
* Distributed Node Architecture
* Real-Time Temperature Monitoring
* Fuel Level Monitoring
* LCD Dashboard Display
* External Interrupt Handling
* Sequential LED Indicators
* Modular Embedded C Design

---

# Learning Outcomes

After completing this project, you will understand:

* CAN Protocol Fundamentals
* LPC2129 Peripheral Programming
* ADC Interfacing
* DS18B20 Interfacing
* LCD Interfacing
* External Interrupts
* Embedded C Programming
* Automotive Dashboard Concepts
* Distributed Embedded Systems

---

# Author

**Naga Sai Durga Surya Lakkineni**

Embedded Systems Project

**CAN Dashboard System using LPC2129**
