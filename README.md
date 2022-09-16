# About
Hey Buddy is a robotic car that provides control to the user via voice activation or built in buttons.
<br>Project demonstration https://youtu.be/RHQObHKx_kw

![image](https://user-images.githubusercontent.com/46685749/190571954-c90767b4-258f-4ae5-ac3c-83f1d5685245.png)
![image](https://user-images.githubusercontent.com/46685749/190572064-a5356cd5-33a0-4a8d-b13e-13f5ffe0a999.png)
![image](https://user-images.githubusercontent.com/46685749/190572087-9118c413-f76c-4e45-8c6f-41c83ce6e943.png)


# Software
Run /rpi/vroom.py onl a raspberry pi 4 with rpi camera, connected to MSP432 via serial input

Run main.c on MSP432 with its respective sensors connected. 

# Hardware

## Right ultrasonic
```bash
vcc green->brown to 5v
trigger purple to gpio5.0
echo    white to gpio5.1
gnd yellow->yellow
```

## Left ultrasonic
```bash
vcc white to 3v
trigger grey to gpio5.2
echo    purple to gpio3.5
gnd blue->blue
```
## Front ultrasonic
```bash
vcc red->purpLe to 5v
trigger yellow to gpio3.6
echo    orange to gpio3.7
gnd brown->brown
```
## raspberry pi
```bash
raspberry pi -> re speaker mic array
raspberry pi - > MSP432 via USB
```

## motor 
```bash
white enA 2.4
blue IN1 4.5
purple IN2 4.4 
orange IN3 4.2
yellow IN4 4.0
grey enB 2.5
```

## Wheel Encoder
```bash
left wheel encoder - green to 6.4
right wheel encoder - green to 5.5
```


