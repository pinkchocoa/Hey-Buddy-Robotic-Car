# About
Hey Buddy is a robotic car that provides control to the user via voice activation or built in buttons.
Project demonstration https://youtu.be/RHQObHKx_kw

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


