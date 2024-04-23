# neptune_pink

Cobolt 4 - Marine Vehicle AIS Transponder

## CSSE4011 Project  
Team Members:  
1. Marcus Von Rausch
2. Cameron Miller
3. Connor Davis
4. Joshua Smith

## Tasks
1. Write/Finalise Project Overview Report (Hardware Architecture, Data Tree, Flowcharts, Group Wiki, KPIs, etc.) for Milestone. See full list of tasks below.
2. Identify the Communication Protocol made by the CityCats (AIS A / B)
3. Figure out how to use the radio receiver for these two channels and frequencies
4. Transfer radio from raspberry pi to the nrf52840 using uart
5. Receive on nrf52840 and process data in some way (movement prediction, Kalman filter, etc.)
6. Identify bluetooth send/receive method for the PC (PyBluez Module looks promising)
7. Make a bluetooth communication protocol between the nrf52840 and the PC
8. Receive and process the bluetooth data on PC such that it is appropraite for display (this would be the step to implement any kind of blockchain or something for immutability)
9. Develop PC app to display the data as appropriate
10. Make the Poster / Props / Presentation outline for Final Presentation

## Requirements
- At least one sensor -> Radio Receiver
- At least one actuaor -> Display
- PC/Mobile App of some kind -> Will be done on the display
- Non-trivial wireless networking, at least 3 nodes -> Base (Raspberry Pi), NRF52840, Display (PC/M5)
    - Base will receive radio waves using sensor, Lime SDR Mini is currently desired 
    - nrf52840 will processed info from base using uart 
    - Display will receive info from nrf52940 via bluetooth
- Non-trivial techniques from Lectures 1-9
    - Localisation
    - Mobility / Tracking
    - Time Synchronisation?
    - Sensing or Signal Processing
    - Kalman Filter?
    - Blockchain?
- Poster
    - Aims (KPI)
    - System Overview
    - Results
    - Conclusions (WRT the KPI)
    - Presentation Format
- Creativity -> Cool app design?
- Props -> City cat 3D model, idk what else we could do.
- Maintain a group wiki -> This github will need to have a wiki pages as seen in project overview tasks.

### Project Overview Tasks for Group Wiki - Must be done for Milestone!
- Write a project description
- Need 5 KPIs -> As seen above
- Write a system overview -> Hardware Architecture, Top Level Flowchart
- Snesor Integration - What sensors, what data, etc.
- Wireless Network, IoT Protocols, or Web Dashboards
- Algorithm Schemes Used
- DIKW Pyramid Abstraction
- Daily project updates
