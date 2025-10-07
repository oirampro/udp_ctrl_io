# UDP Control I/O (udp_ctrl_io)

Sistema de control de periféricos para ESP32 mediante comandos UDP en texto plano. El proyecto permite controlar GPIOs digitales, PWM y ADC a través de la red local.

## 📋 Características

- **Control de GPIO Digital**: Configuración y control de 4 pines GPIO (lectura/escritura)
- **Control PWM**: Generación de señal PWM con ciclo de trabajo configurable (0-100%)
- **Lectura ADC**: Conversión analógica-digital de 12 bits
- **Comunicación UDP**: Puerto 8266, comandos en texto plano
- **Arquitectura Modular**: Componentes independientes para GPIO, PWM y ADC

## 🔧 Hardware Soportado

- **Plataforma**: ESP32
- **Framework**: ESP-IDF

### Mapeo de Pines

| Índice Usuario | GPIO Físico | Función |
|----------------|-------------|---------|
| 0 | GPIO18 | Digital I/O |
| 1 | GPIO19 | Digital I/O |
| 2 | GPIO21 | Digital I/O |
| 3 | GPIO22 | Digital I/O |
| - | GPIO5 | Salida PWM |
| - | GPIO36 | Entrada ADC (ADC1_CH0) |

## 📦 Estructura del Proyecto

```
udp_ctrl_io/
├── main/
│   └── udp_server.c          # Programa principal
├── components/
│   ├── gpio_ctrl/
│   │   ├── gpio_ctrl.h       # Header control GPIO
│   │   └── gpio_ctrl.c       # Implementación control GPIO
│   ├── pwm_ctrl/
│   │   ├── pwm_ctrl.h        # Header control PWM
│   │   └── pwm_ctrl.c        # Implementación control PWM
│   └── adc_ctrl/
│       ├── adc_ctrl.h        # Header control ADC
│       └── adc_ctrl.c        # Implementación control ADC
├── CMakeLists.txt
└── README.md
```

## 🚀 Instalación y Compilación

### Requisitos Previos

- ESP-IDF v4.4 o superior instalado
- Herramientas de compilación configuradas
- ESP32 conectado al sistema

### Pasos de Instalación

1. **Clonar el repositorio:**
```bash
git clone https://github.com/oirampro/udp_ctrl_io.git
cd udp_ctrl_io
```

2. **Configurar el proyecto:**
```bash
idf.py menuconfig
```

Configurar las opciones de red en:
- `Example Connection Configuration` → Configurar WiFi SSID y contraseña

3. **Compilar el proyecto:**
```bash
idf.py build
```

4. **Flashear el ESP32:**
```bash
idf.py -p /dev/ttyUSB0 flash
```

5. **Monitorear la salida (opcional):**
```bash
idf.py -p /dev/ttyUSB0 monitor
```

## 📡 Protocolo de Comunicación

### Formato General

- **Puerto UDP**: 8266
- **Formato**: Comandos en texto plano ASCII
- **Respuesta Exitosa**: `ACK:<valor>`
- **Respuesta Error**: `NACK`

### Comandos Disponibles

#### 1. Configurar GPIO (`cf`)

Configura un GPIO como entrada o salida.

**Sintaxis:**
```
cf <num_bit> <valor>
```

**Parámetros:**
- `num_bit`: Índice del GPIO (0-3)
- `valor`: 0 = salida, 1 = entrada

**Ejemplos:**
```
cf 0 0    → ACK:0  (GPIO 0 como salida)
cf 1 1    → ACK:1  (GPIO 1 como entrada)
cf 5 0    → NACK   (índice inválido)
```

---

#### 2. Escribir GPIO (`wb`)

Escribe un valor lógico en un GPIO configurado como salida.

**Sintaxis:**
```
wb <num_bit> <valor>
```

**Parámetros:**
- `num_bit`: Índice del GPIO (0-3)
- `valor`: 0 o 1

**Ejemplos:**
```
wb 0 1    → ACK:1  (Escribe 1 en GPIO 0)
wb 0 0    → ACK:0  (Escribe 0 en GPIO 0)
wb 1 1    → NACK   (Error si GPIO 1 no es salida)
```

---

#### 3. Leer GPIO (`rb`)

Lee el valor físico actual de un GPIO.

**Sintaxis:**
```
rb <num_bit>
```

**Parámetros:**
- `num_bit`: Índice del GPIO (0-3)

**Ejemplos:**
```
rb 0      → ACK:1  (GPIO 0 lee valor 1)
rb 2      → ACK:0  (GPIO 2 lee valor 0)
rb 5      → NACK   (índice inválido)
```

---

#### 4. Configurar PWM (`wp`)

Configura el ciclo de trabajo del PWM.

**Sintaxis:**
```
wp <porcen>
```

**Parámetros:**
- `porcen`: Porcentaje de ciclo de trabajo (0-100)

**Ejemplos:**
```
wp 50     → ACK:50   (PWM al 50%)
wp 0      → ACK:0    (PWM apagado)
wp 100    → ACK:100  (PWM al 100%)
wp 150    → NACK     (valor fuera de rango)
```

**Especificaciones PWM:**
- Frecuencia: 1 kHz
- Resolución: 13 bits
- GPIO: GPIO5

---

#### 5. Leer PWM (`rp`)

Lee el valor actual del ciclo de trabajo del PWM.

**Sintaxis:**
```
rp
```

**Ejemplos:**
```
rp        → ACK:50   (PWM configurado al 50%)
rp        → ACK:0    (PWM apagado)
```

---

#### 6. Leer ADC (`ra`)

Lee el valor del conversor ADC.

**Sintaxis:**
```
ra
```

**Respuesta:**
- Valor en formato hexadecimal (0-FFF para 12 bits)

**Ejemplos:**
```
ra        → ACK:800  (ADC lee 2048 en decimal)
ra        → ACK:FFF  (ADC lee 4095 en decimal - máximo)
ra        → ACK:0    (ADC lee 0)
```

**Especificaciones ADC:**
- Resolución: 12 bits (0-4095)
- Rango de voltaje: 0-3.3V (atenuación 11dB)
- Canal: ADC1_CH0 (GPIO36)
- Calibración: Automática si está disponible

## 🧪 Pruebas con Packet Sender

### Configuración

1. Abrir Packet Sender
2. Configurar:
   - **Address**: Dirección IP del ESP32 (ver en monitor serial)
   - **Port**: 8266
   - **Protocol**: UDP
   - **Mode**: ASCII

### Secuencia de Prueba Completa

```
1. cf 0 0      → Configurar GPIO 0 como salida
2. wb 0 1      → Escribir 1 en GPIO 0
3. rb 0        → Leer GPIO 0 (debe ser 1)
4. cf 1 1      → Configurar GPIO 1 como entrada
5. rb 1        → Leer GPIO 1
6. wp 75       → Configurar PWM al 75%
7. rp          → Leer PWM (debe ser 75)
8. ra          → Leer valor ADC
9. wp 0        → Apagar PWM
10. wb 0 0     → Apagar GPIO 0
```

## 🔍 Validaciones y Manejo de Errores

El sistema valida automáticamente:

- ✅ Rangos de índices GPIO (0-3)
- ✅ Valores lógicos (0 o 1)
- ✅ Porcentajes PWM (0-100)
- ✅ Formato de comandos
- ✅ Configuración de GPIO antes de escritura
- ✅ Comandos reconocidos

Responde `NACK` en casos de:
- ❌ Comando no reconocido
- ❌ Parámetros fuera de rango
- ❌ Formato incorrecto
- ❌ Intentar escribir en GPIO configurado como entrada
- ❌ Error de hardware

## 📊 Diagrama de Flujo

```
Cliente UDP → Envía Comando (texto plano)
              ↓
          Servidor UDP (ESP32:8266)
              ↓
          Parseo y Validación
              ↓
      ┌───────┴───────┐
      ↓               ↓
   Válido          Inválido
      ↓               ↓
  Componente       NACK
  (GPIO/PWM/ADC)
      ↓
   ACK:<valor>
      ↓
  Respuesta UDP → Cliente
```

## 🐛 Debugging

### Ver logs en tiempo real:
```bash
idf.py monitor
```

### Logs importantes:
- `gpio_ctrl`: Operaciones GPIO
- `pwm_ctrl`: Operaciones PWM
- `adc_ctrl`: Operaciones ADC
- `gpio_udp_server`: Comunicación UDP y procesamiento de comandos

### Comandos de monitor:
- `Ctrl + ]` para salir del monitor
- `Ctrl + T` → `Ctrl + H` para ver ayuda

## 🔒 Consideraciones de Seguridad

- ⚠️ El sistema no implementa autenticación
- ⚠️ Usar solo en redes locales confiables
- ⚠️ No exponer el puerto UDP a Internet
- ⚠️ Validar voltajes antes de conectar al ADC (máximo 3.3V)
- ⚠️ Verificar corriente máxima de GPIOs (40mA por pin)

## 📝 Configuración Avanzada

### Cambiar Puerto UDP

En `udp_server.c`:
```c
#define PORT 8266  // Cambiar al puerto deseado
```

### Cambiar GPIO PWM

En `pwm_ctrl.c`:
```c
#define LEDC_OUTPUT_IO  (5)  // Cambiar al GPIO deseado
```

### Cambiar Canal ADC

En `adc_ctrl.c`:
```c
#define ADC_CHANNEL  ADC_CHANNEL_0  // Cambiar al canal deseado
```

### Cambiar Frecuencia PWM

En `pwm_ctrl.c`:
```c
#define LEDC_FREQUENCY  (1000)  // Cambiar frecuencia en Hz
```

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor:

1. Fork del repositorio
2. Crear rama para tu feature (`git checkout -b feature/AmazingFeature`)
3. Commit de cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abrir Pull Request

## 📄 Licencia

Este proyecto está bajo licencia de Dominio Público o CC0 (a tu elección).

## ✨ Características Futuras Posibles

- [ ] Autenticación básica
- [ ] Múltiples canales PWM
- [ ] Múltiples canales ADC
- [ ] Configuración dinámica de pines
- [ ] Modo servidor TCP
- [ ] Interface web de configuración
- [ ] Soporte para más periféricos (I2C, SPI, UART)
- [ ] Comandos de diagnóstico del sistema

## 📧 Contacto

Para preguntas o sugerencias, por favor abrir un issue en el repositorio.

## 🙏 Agradecimientos

Basado en ejemplos oficiales de ESP-IDF:
- UDP Server Example
- LEDC Basic Example
- ADC Oneshot Read Example

---

**Versión**: 1.0.0  
**Fecha**: Octubre 2025  
**Framework**: ESP-IDF  
**Plataforma**: ESP32
