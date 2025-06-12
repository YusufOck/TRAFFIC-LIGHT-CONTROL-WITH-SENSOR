# 🚦 Traffic Light Controller Project – FSM & Arduino

This project implements a traffic light control system using two complementary approaches:

## 🔧 Part 1: Digital System Design (Hardware-Only FSM)

A finite state machine (FSM)-based controller is designed using basic digital components:
- **D-type Flip-Flops**
- **555 Timer (Astable mode)**
- **Logic Gates (AND, OR, NOT)**
- **Sensor Input (Button)**

The system simulates a two-way intersection, managing traffic flow using pure hardware logic without any microcontroller or software. The controller responds to a button press (e.g., side-road vehicle request) and cycles through traffic light states accordingly.

### Features:
- Fully hardware-based state transitions
- Adjustable clock via 555 timer
- Simulated in **Proteus**
- Final circuit fabricated on **PCB**

---

## 💻 Part 2: Microprocessor-Based Design (Arduino Uno)

The same traffic scenario is implemented using an **Arduino Uno**, demonstrating software-based flexibility.

### Features:
- **Automatic Mode**: Cycles through traffic lights in a loop
- **Override Mode**: Priority given to emergency vehicles via **IR remote**
- **IR Sensor Module** for command reception
- Real-time state control using code-based FSM

---

## 🛠 Technologies Used

| Platform        | Tools / Components                  |
|----------------|--------------------------------------|
| Digital Logic  | Proteus, D Flip-Flops, 555 Timer     |
| Microcontroller| Arduino Uno, IR Sensor, Remote       |
| Design Tools   | Proteus Simulation, PCB Fabrication  |
| Programming    | C/C++ (Arduino IDE)                  |

---

## 📁 File Structure

