# Luz-aquário / Aquarium-lighting
Automação e controle das luzes de um aquário com um arduino UNO e um servo preso ao eixo do dimmer da lâmpada. / Automation and control of aquarium lighting, using arduino UNO and a servo attached to the light´s dimmer.

# Português

Este projeto Arduino implementa um sistema de controle de iluminação suave usando um servo motor para simular um dimmer, com controle por botão, ajustes manuais, transições automáticas baseadas no tempo (RTC), e armazenamento de configurações na EEPROM. Um LED externo indica o estado atual do sistema.

## 🔧 Funcionalidades

- Controle de brilho via servo (0 a 180 graus)
- Ajuste manual com botão (curta e longa pressão)
- Transições suaves (fade in/out) programadas
- Salvamento automático do brilho atual na EEPROM
- Horários automáticos com RTC DS1302 (11h acende, 20h apaga)
- Diagnóstico via LED externo com piscar indicando nível de brilho
- Indicador visual de estado com LED externo conectado ao pino 13

## 📦 Requisitos de Hardware

- Arduino UNO (ou compatível)
- Servo motor
- Módulo RTC DS1302
- Botão com resistor pull-up interno
- LED **externo** conectado ao pino 13
- Resistor de 220Ω (em série com o LED)
- Jumpers e protoboard

## 🖥️ Pinos Utilizados

| Componente | Pino Arduino |
|------------|--------------|
| Botão      | D2           |
| Servo      | D9           |
| LED externo| D13          |
| RTC CLK    | D3           |
| RTC DAT    | D7           |
| RTC RST    | D8           |

> ⚠️ O LED externo deve ser ligado entre o pino D13 e o GND, com um resistor de 220Ω em série.

## ⚙️ Instalação e Montagem

1. Conecte os componentes conforme a tabela acima.
2. Faça upload do código para o Arduino.
3. Na primeira execução, o horário do RTC será configurado para: **18:00h do dia 29/03/2025** (modifique se necessário).
4. Pressione o botão:
   - Curto: ajusta brilho em 20%
   - Longo (1s): liga/desliga com fade
   - Muito longo (5s): diagnóstico com LED

## ⏲️ Horários Automáticos

- **11:00** → Fade suave até brilho 100%
- **20:00** → Fade suave até brilho 0%

## 💾 EEPROM

O nível de brilho atual é salvo automaticamente ao final de cada transição.

## 🔍 Diagnóstico

Pressionando o botão por 5 segundos:
- 2 piscadas = brilho 0%
- 4 piscadas = brilho 100%

## Bibliotecas Utilizadas

- [Servo](https://www.arduino.cc/en/Reference/Servo)
- [Bounce2](https://github.com/thomasfredericks/Bounce2)
- [EEPROM](https://www.arduino.cc/en/Reference/EEPROM)
- [virtuabotixRTC](https://github.com/virtuabotix/DS1302)

# Inglês

This Arduino project implements a smooth lighting control system using a servo motor as a dimmer, a button for manual control, time-based automatic transitions (via RTC), and brightness memory via EEPROM. An external LED indicates the current system state.

## 🔧 Features

- Brightness control via servo (0 to 180 degrees)
- Manual adjustment using button (short and long press)
- Smooth fade transitions on power on/off
- Automatic brightness memory (stored in EEPROM)
- Real-time clock automation (ON at 11:00, OFF at 20:00)
- Diagnostic mode using external LED blinks
- Visual state indicator using external LED on pin 13

## 📦 Hardware Requirements

- Arduino UNO (or compatible)
- Servo motor
- DS1302 RTC module
- Push-button with internal pull-up
- **External LED** connected to pin 13
- 220Ω resistor in series with the LED
- Jumper wires and breadboard

## 🖥️ Pinout

| Component     | Arduino Pin |
|---------------|-------------|
| Button        | D2          |
| Servo         | D9          |
| External LED  | D13         |
| RTC CLK       | D3          |
| RTC DAT       | D7          |
| RTC RST       | D8          |

> ⚠️ The external LED should be connected between pin D13 and GND, with a 220Ω resistor in series.

## ⚙️ Setup Instructions

1. Wire components as shown above.
2. Upload the sketch to your Arduino.
3. On first boot, the RTC time is set to **6:00 PM on 03/29/2025** (adjust if needed).
4. Button usage:
   - Short press: adjusts brightness by 20%
   - Long press (1s): fades to ON/OFF
   - Very long press (5s): diagnostic LED blink

## ⏲️ Automatic Schedule

- **11:00 AM** → Smooth fade to 100% brightness
- **8:00 PM** → Smooth fade to 0% brightness

## 💾 EEPROM

The current brightness level is saved automatically after transitions.

## 🔍 Diagnostic Mode

Press and hold the button for 5 seconds:
- 2 blinks = brightness is 0%
- 4 blinks = brightness is 100%

## Libraries Used

- [Servo](https://www.arduino.cc/en/Reference/Servo)
- [Bounce2](https://github.com/thomasfredericks/Bounce2)
- [EEPROM](https://www.arduino.cc/en/Reference/EEPROM)
- [virtuabotixRTC](https://github.com/virtuabotix/DS1302)
