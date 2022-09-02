# TP4 - Testing con Ceedling

_Nicolás Hasbún A._

Práctica de testing con **ceedling** aplicado a proyecto desarrollado en la cursada
de **RTOS II**. Este implementa una máquina que graba mensajes en código morse y 
entrega una interfaz por puerto serial que permite reproducir, grabar o borrar mensajes.

Se prueba una librería interna que aisla al resto del sistema respecto del manejo
de memoria dinámica en FreeRTOS. De este modo la librería entrega una interfaz para
crear nuevas sesiones, grabar datos, borrar sesiones, entregar una lista de sesiones
guardadas, etc.

La librería **api_samples** revisa un macro **`__API_SAMPLES_TESTING__`** para indicar
casos en que la librería es utilizada fuera de ambientes de FreeRTOS (útil para hacer
testing).

## Notas

Para la implementación de los test se aprovechó el hecho de que en **ceedling** los test
corren de forma secuencial. Esto fue necesario pues la librería no permite volver al estado
inicial de encendido sin asumir el correcto funcionamiento de partes importantes de esta
que requerían ser testeados.