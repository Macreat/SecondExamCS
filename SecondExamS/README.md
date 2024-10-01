README - Examen Final Supletorio - Sistema de Operaciones Aritméticas
Este proyecto implementa un sistema de operaciones aritméticas utilizando un STM32, con las siguientes funcionalidades:

Entrada de valores mediante un teclado hexadecimal.
Selección de operación aritmética a través de comunicación UART.
Mostrar resultados en una pantalla OLED.
Indicador LED para mostrar el estado del sistema según el resultado de la operación.
Criterios de Evaluación
Funcionalidad (60%): Cada requisito funcional tiene una ponderación equitativa.
Arquitectura de Código (10%): Uso adecuado de interrupciones, periféricos y librerías externas.
Optimización (10%): Optimización de consumo energético y uso eficiente de memoria.
Requisitos del Sistema
Requisitos No Funcionales
Indicador LED: El sistema utiliza un LED conectado a la placa para indicar el estado del cálculo.
Teclado Hexadecimal: Un teclado externo se utiliza para la entrada de los números.
Puerto de Depuración (UART): Comunicación serie con una PC utilizando USART2 configurado a 115200 baudios.
Pantalla OLED: Visualización de valores, operaciones y resultados en una pantalla OLED conectada vía I2C.
Requisitos Funcionales
Ingreso de Valores vía Teclado:

El teclado acepta dos números de hasta 3 dígitos cada uno.
Validación básica de entradas para asegurar la captura de números correctos.
Selección de Operación vía USART2:

A través de la interfaz UART, el usuario selecciona la operación aritmética.
'+' para suma
'-' para resta
'\*' para multiplicación
'/' para división
El operador se muestra en la pantalla OLED.
Mostrar Entradas:

El sistema muestra en la pantalla OLED los dos últimos valores ingresados por el teclado y la última operación recibida vía UART.
Realizar el Cálculo:

Cuando se recibe el carácter '=' por la UART:
Se realiza la operación seleccionada entre los dos números ingresados.
El resultado se muestra en la pantalla OLED.
El resultado también se envía a la PC vía UART.
Funcionalidad de Reinicio:

Al presionar la tecla '#' en el teclado, se reinician los valores de entrada y se limpia la pantalla OLED, dejando el sistema listo para recibir nuevas entradas.
Indicador de Estado LED:

El LED se enciende (ON) si el resultado del cálculo es positivo.
El LED se apaga (OFF) si el resultado es cero o negativo.
Estructura del Código
El código está dividido en diferentes secciones para manejar periféricos, entradas y lógica del sistema. Aquí se resumen las principales funcionalidades y cómo están implementadas:

Inicialización
Los periféricos como GPIO, I2C y UART se inicializan utilizando las funciones generadas por STM32CubeMX.
La pantalla OLED se inicializa y se limpia al inicio del programa. Se muestra un mensaje de bienvenida en la pantalla OLED.
Entrada de Valores y Operadores
Teclado: Las teclas presionadas se leen a través de interrupciones generadas por los pines configurados como entradas de interrupción externa (EXTI).
UART: Los operadores aritméticos (+, -, _, /) se leen a través de interrupciones UART utilizando la función HAL_UART_RxCpltCallback().
Procesamiento de Datos
Los números ingresados se almacenan en buffers circulares utilizando una implementación de ring_buffer.
Se implementa la lógica para acumular los dígitos en cada buffer y reconstruir los números enteros cuando se completa la entrada.
Operaciones Aritméticas
Una vez que se han ingresado los números y se ha recibido un operador, el sistema realiza la operación aritmética seleccionada cuando se recibe el carácter '='.
La operación se ejecuta dentro de un switch basado en el operador recibido.
Indicador de Estado LED
El sistema activa o desactiva un LED según el resultado de la operación:
Si el resultado es positivo, el LED se enciende.
Si el resultado es cero o negativo, el LED se apaga.
Visualización en la Pantalla OLED
Los números ingresados, el operador y el resultado final se muestran en la pantalla OLED, utilizando la biblioteca SSD1306.
Requisitos de Hardware
STM32 Nucleo Board: Usamos la familia STM32 con I2C para la pantalla OLED y GPIO para la lectura del teclado y el control del LED.
Teclado Hexadecimal: Se conecta a los pines GPIO configurados como entradas con interrupciones.
Pantalla OLED (128x64): Utilizada para mostrar los números, operador y resultados.
LED: Conectado a un pin GPIO de salida para mostrar el estado del sistema.
Requisitos de Software
STM32CubeMX: Para la configuración de los periféricos.
HAL (Hardware Abstraction Layer): Librerías STM32 HAL para manejo de periféricos.
Biblioteca SSD1306: Para la comunicación con la pantalla OLED.
Keil uVision/STM32CubeIDE: Herramientas de desarrollo para compilar y cargar el código en la placa STM32.
Cómo Usar el Sistema
Encendido: Al encender el sistema, la pantalla OLED mostrará un mensaje de bienvenida.
Ingresar Valores: Utilice el teclado para ingresar dos números. El sistema almacenará hasta 3 dígitos por número.
Seleccionar Operación: Ingrese un operador aritmético (+, -, _, /) utilizando el puerto UART.
Realizar Cálculo: Presione el carácter '=' para realizar la operación seleccionada. El resultado aparecerá en la pantalla OLED y se enviará vía UART.
Reiniciar: Presione '#' en el teclado para reiniciar las entradas y comenzar una nueva operación.
Conclusión
Este proyecto demuestra la integración de múltiples periféricos y dispositivos externos en un sistema STM32, proporcionando una solución para realizar operaciones aritméticas básicas. Las entradas del teclado y UART, junto con la visualización en la pantalla OLED, ofrecen una interfaz intuitiva para el usuario. El uso de interrupciones y buffers circulares asegura un manejo eficiente de las entradas y operaciones.
