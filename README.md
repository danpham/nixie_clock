# Nixie Clock ESP32-C3

A **Nixie tube clock project** based on the **ESP32-C3**, using **IN14** and **IN3** tubes driven by **HV5622** high-voltage drivers.

![3D View](https://github.com/danpham/nixie_clock/raw/master/3d_view.png)

![3D View](https://github.com/danpham/nixie_clock/raw/master/pcb_view.png)

## Hardware

- **Microcontroller:** ESP32-C3  
- **Nixie tubes:** 6 × IN14 and 2 × IN3  
- **Driver:** HV5622 for cathode switching  
- **Rotary encoder:** EC11 for time configuration  
- **3.3V regulator:** AP63203WU to power the ESP32-C3 and logic components  
- **PCB design:** SMD (Surface-Mount) components

> ⚠️ **Warning:** This project involves high voltage. Follow proper electrical safety procedures!


## Software

The code is written in **C for the ESP32-C3**, organized as follows:

```
software/nixie_clock_esp32_project/
├── components/
│   ├── uart/
│   ├── gpio_driver/
│   ├── display/
│   ├── clock/
│   └── hv5622/
└── main/
```

### Current Features

- Nixie tube display control via HV5622  
- Rotary encoder input (EC11) for setting the time  
- Internal clock management  
- **Anti–cathode poisoning routine:** periodic cycling and cathode management to prevent oxide buildup and extend tube lifespan  

### Future Features

- Wi-Fi connectivity for automatic time synchronization  
- Web interface for configuration: timezone, brightness, night mode, and display enable/disable  

## Compilation

The project uses **ESP-IDF for ESP32-C3**. Make sure your ESP32-C3 development environment is properly set up.

```bash
cd software/nixie_clock_esp32_project
idf.py build
idf.py flash
```

## License

This project is released under the **GPL v3** license.  
A separate `LICENSE` file contains the complete text of the license.


## Safety

⚠️ This project involves **high voltage (~170V for Nixie tubes)**.  
- Never touch the circuit while it is powered
- Work carefully and only with proper knowledge of high-voltage electronics

