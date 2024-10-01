# 4100901 - Examen Final - Versión Supletorio

Sistema de operaciones aritméticas.

## Criterios de Evaluación
Funcionalidad: 60% (ponderación igual para cada requisito)
Arquitectura de Código: 10% (uso adecuado de interrupciones y librerías)
Optimización: 10% (consumo de energía y uso de memoria)

## Instrucciones
Cree un nuevo proyecto utilizando STM32CubeMX e inicialice todos los periféricos de acuerdo con los requisitos a continuación.

## Requisitos del Sistema
### Requisitos No Funcionales
Indicador LED: Utilice un LED integrado para mostrar el estado del sistema.
Teclado Hexadecimal: Implemente un teclado para la entrada de valores numéricos por parte del usuario.
Puerto de Depuración: Configure una interfaz de comunicación con una PC utilizando USART2 a 115200 baudios.
Pantalla OLED: Utilice una pantalla OLED para mostrar información del sistema.

### Requisitos Funcionales
1. Ingreso de Valores vía Teclado
* El teclado debe aceptar dos valores separados, cada uno de hasta 3 dígitos.
* Implemente validación de entrada para asegurar datos correctos.

2. Selección de Operación vía USART2
* El puerto USART2 debe recibir un carácter único que indique la operación aritmética:
  * '+' para suma
  * '-' para resta
  * '*' para multiplicación
  * '/' para división
* Muestre la operación seleccionada en la pantalla OLED.

3. Mostrar Entradas
* Muestre los dos últimos valores ingresados vía el teclado y la última operación recibida vía USART2 en la pantalla OLED.

4. Realizar el Cálculo
* Cuando se reciba el carácter '=' a través de USART2:
  * Realice la operación aritmética seleccionada sobre los dos valores de entrada.
  * Muestre el resultado en la pantalla OLED.
  * Envíe el resultado a la PC vía USART2.

5. Funcionalidad de Reinicio
* Al presionar la tecla '#' en el teclado, se deben reiniciar los valores de entrada.
* Asegure que el sistema esté listo para aceptar nuevas entradas después del reinicio.

6. Indicador de Estado LED
* El LED debe encenderse (ON) si el resultado del cálculo es positivo.
* El LED debe apagarse (OFF) si el resultado es cero o negativo.
  
