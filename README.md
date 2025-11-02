# Smart Traffic Light Controller (Dual Implementation)

This repository documents two distinct methods for creating an intelligent traffic light control system for a pedestrian crosswalk.

1.  **Hardware-Only (Digital Logic):** A "smart" system built without any programmable microcontroller. The logic is implemented directly in hardware using a Finite State Machine (FSM) composed of logic gates and flip-flops.
2.  **Microprocessor-Based (Software):** A "smart" system where the logic is controlled by software (C++) running on a programmable microcontroller (e.g., Arduino).

---

## Part 1: Hardware-Only Digital Logic Controller (No Microcontroller)

This is the primary focus of the project: creating an intelligent system using only discrete digital logic components. The system's "brain" is a hardware-based **Finite State Machine (FSM)**.

### Core Concept: The State Machine

Instead of software, this system uses a "state machine" to manage the traffic lights.
* **Flip-Flops** (e.g., JK or D-type) are used to "remember" the current state (e.g., "Vehicle Go" or "Pedestrian Go").
* **Logic Gates** (e.g., 74xx series AND, OR, NOT) decide when to change states (e.g., *if* the 'Vehicle Go' state is active *and* the pedestrian button is pressed, *then* transition to the 'Vehicle Wait' state).
* **A Timer (e.g., 555 Timer IC)** is used to create precise delays for the yellow light and the pedestrian crossing time.

The "sensor" (push button) acts as a physical input that triggers the state machine to transition from the vehicle cycle to the pedestrian cycle.

### Hardware Implementation: From Prototype to PCB

#### 1. Breadboard Prototype
The initial circuit was prototyped on a breadboard. This phase was critical for testing the state machine logic, ensuring the flip-flops transitioned correctly, and calibrating the timer durations.

![Breadboard Layout](https://github.com/YusufOck/TRAFFIC-LIGHT-CONTROL-WITH-SENSOR/blob/main/PCB%20and%20BREADBARD%20images/BREADBARD.png)

#### 2. Final PCB (Printed Circuit Board)
After validating the logic, a custom **Printed Circuit Board (PCB)** was designed. This eliminates wiring errors and creates a permanent, robust, and compact final product. The PCB itself is the physical embodiment of the finite state machine.

![PCB Final Design](https://github.com/YusufOck/TRAFFIC-LIGHT-CONTROL-WITH-SENSOR/blob/main/PCB%20and%20BREADBARD%20images/PCB.jpg)

![PCB Copper Layout](https://github.com/YusufOck/TRAFFIC-LIGHT-CONTROL-WITH-SENSOR/blob/main/PCB%20and%20BREADBARD%20images/PCB%20Layout.png)

---

### Initial Design Phase

This image captures the early planning and design stages for the circuit logic and layout.

![Design Phase](https://github.com/YusufOck/TRAFFIC-LIGHT-CONTROL-WITH-SENSOR/blob/main/PCB%20and%20BREADBARD%20images/Design%20Phase.jpg)

---

## Part 2: Microprocessor-Based Controller (Programmable)

This is the alternative, software-based implementation of the same problem. The control logic is not built with gates but is written in **C++** and executed by a microprocessor (like the Atmel chip on an Arduino).

* **Logic:** Handled by `if`, `else`, and `delay()` functions within the `.ino` sketch.
* **Flexibility:** This method is more flexible, as the timing and logic can be changed just by uploading new code, without redesigning the hardware.
* **Components:** Requires only the microcontroller, LEDs, resistors, and the button.

### Microprocessor Prototype

This prototype shows the circuit built around a programmable chip, demonstrating the simplicity of the hardware when the logic is managed by software.

![Microprocessor Breadboard](https://github.com/YusufOck/TRAFFIC-LIGHT-CONTROL-WITH-SENSOR/blob/main/PCB%20and%20BREADBARD%20images/Microprocessor%20Design%20on%20Breadboard.jpg)

---

## 🚀 How to Replicate

### Version 1: Hardware-Only (Digital Logic)
1.  **Gather Components:** Collect the logic ICs, 555 timer, flip-flops, LEDs, and support components (resistors/capacitors) as per the design.
2.  **Assemble Circuit:**
    * **For testing:** Use the **Breadboard Layout** image to build the prototype.
    * **For final product:** Use the **PCB Layout** and **PCB** images to fabricate and solder your own board.
3.  **Power On:** Provide the required DC voltage (e.g., 5V) to the circuit.
4.  **Test:** The Green LED (vehicle traffic) should be ON by default. Press the push button to trigger the pedestrian cycle.

### Version 2: Microprocessor (Software)
1.  **Gather Components:** Arduino UNO (or similar), LEDs, resistors, push button, breadboard.
2.  **Assemble Circuit:** Build the simple circuit shown in the **Microprocessor Breadboard** image.
3.  **Upload Code:**
    * Open the `.ino` file from the `Arduino Code` folder in this repository using the Arduino IDE.
    * Connect your Arduino and upload the sketch.
4.  **Test:** The system will now run the logic from the code.

## 🧑‍💻 Author

**Yusuf Ocak**
* GitHub: [@YusufOck](https://github.com/YusufOck)
