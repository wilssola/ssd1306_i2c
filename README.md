# Projeto de Interface de Comunicação Serial com RP2040

Este projeto implementa um sistema interativo utilizando a placa BitDogLab, explorando diferentes interfaces de comunicação serial e dispositivos de entrada/saída.

## 🎯 Objetivos

- Implementar comunicação serial via UART e I2C
- Controlar LEDs WS2812 e RGB
- Gerenciar entradas via botões com tratamento de debounce
- Desenvolver interface de usuário com display OLED
- Criar caracteres personalizados para biblioteca de fontes

## 🛠️ Componentes Utilizados

- Placa BitDogLab com RP2040
- Matriz 5x5 de LEDs WS2812 (GPIO 7)
- LED RGB (GPIOs 11, 12, 13)
- 2 Botões de entrada (GPIOs 5, 6)
- Display OLED SSD1306 (I2C: GPIO 14, 15)

## ✨ Funcionalidades

### 1. Biblioteca de Fontes Expandida
- Implementação de caracteres minúsculos personalizados
- Suporte completo para caracteres alfanuméricos

### 2. Interface Serial com PC
- Recebimento de caracteres via Serial Monitor
- Exibição em tempo real no display OLED
- Representação visual de números (0-9) na matriz WS2812

### 3. Controle via Botões
- **Botão A**: Controle do LED Verde
  - Toggle on/off
  - Feedback visual no display
  - Log via serial
- **Botão B**: Controle do LED Azul
  - Toggle on/off
  - Feedback visual no display
  - Log via serial

## 🔧 Requisitos Técnicos

- Implementação de interrupções para botões
- Tratamento de debounce via software
- Controle de múltiplos tipos de LED
- Comunicação I2C com display OLED
- Comunicação UART com PC
- Código organizado e documentado

## 📦 Estrutura do Projeto

```
main.c
include/
├── led.h
├── pin.h
├── button.h
├── ssd1306_font.h
├── ssd1306_i2c.h
└── ws2812.h
```

## 🚀 Como Executar

1. Clone este repositório
2. Configure o ambiente de desenvolvimento para RP2040
3. Conecte os componentes conforme especificação
4. Compile e faça upload do código para a placa
5. Monitore via Serial Monitor (115200 baud)

## 📝 Notas de Implementação

- Interrupções configuradas para detecção de borda
- Debounce implementado com temporizador
- Atualização do display OLED

## 📄 Licença

Este projeto está sob a licença MIT.