# GeoFetch ESP32 Firmware

This repository contains the ROS2-integrated firmware for the GeoFetch module. Running on an **ESP32**, it manages the low-level control of the drilling and container subsystems while communicating with a ROS2 environment via **micro-ROS**.

## 📂 Repository Structure

The project follows the standard PlatformIO directory layout:

* **`src/`**: Contains the core application logic and communication management.
    * `main.cpp`: Initializer for hardware, subsystem managers, and micro-ROS services.
    * **`system/`**: High and low-level logic managers for the module.
    * **`components/`**: Hardware abstraction wrappers for motors (Auger/Press), Load Cells, and End Switches.
    * **`communication/`**: Handles micro-ROS node management, service registration, and telemetry publishing.
* **`include/`**: Global configuration and header files.
    * `Config.h`: Defines all hardware pin assignments and system constants like speeds and durations.
* **`platformio.ini`**: Project configuration file including library dependencies and build flags.
* **`custom.meta`**: Specific micro-ROS configurations, such as the maximum number of publishers and services.

---

## 🛠 Setup Instructions

### 1. Prerequisites
* Install [Visual Studio Code](https://code.visualstudio.com/).
* Install the **PlatformIO IDE** extension from the VS Code Marketplace.

### 2. Project Initialization
1. Clone this repository to your local machine.
2. Open VS Code and click on the **PlatformIO icon** (the ant head) in the side bar.
3. Select **"Open Project"** and navigate to the cloned repository folder.
4. PlatformIO will automatically detect the `platformio.ini` file and download the required libraries (micro-ROS, HX711, AccelStepper, etc.).

---

## 🚀 Usage & Commands

You can manage the project using the PlatformIO toolbar (the status bar at the bottom of VS Code) or via the integrated terminal.

### Within VS Code (GUI)
* **Build**: Click the **Checkmark** icon to compile the code and verify there are no errors.
* **Upload (Flash)**: Click the **Right Arrow** icon to compile and upload the firmware to your ESP32 via USB.
* **Clean**: Click the **Trash Can** icon to remove old build artifacts.
* **Serial Monitor**: Click the **Plug** icon to view debug output (configured for 115200 baud).

### Command Line (PlatformIO Core)
Run these commands from the project root directory:

| Task | Command |
| :--- | :--- |
| **Build Project** | `pio run` |
| **Upload Firmware** | `pio run --target upload` |
| **Clean Build Files** | `pio run --target clean` |
| **Monitor Serial** | `pio device monitor` |

---

## ⚙️ Configuration Notes
* **Micro-ROS Distribution**: The system is configured for **ROS2 Humble**.
* **Transport**: Communication is handled via **Serial transport** at 115200 baud.
* **Resource Limits**: If you add new ROS services, you must update the `RMW_UXRCE_MAX_SERVICES` value in `custom.meta`. Also run a full clean (not the same as clean) and rebuild.