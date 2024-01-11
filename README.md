## Intrucciones de make

1. make build -> buildea el programa
2. make debug -> corre el programa en qemu/gdb
3. make clean -> limpia archivos elf, lst, bin, etc...
4. make clean build -> se recomienda si se desea buildear con alguna modificacion


## NOTA
Para el ejercicio 5 considere que al entrar en la tarea 1 o 2 llame a svc y este le cambie el modo. 
Ya que se pedia que corra en modo svc, lo puse en este modo y luego se queda corriendo en el mismo. No vuelve a user. 
La otra idea que tenia para implementarlo pero no llegue con el tiempo era dejar que la tarea intente leer, eso tire un data abort y ahi determinar la direccion donde fue generada esta excepcion y segun esto tirar un svc para cambiar el modo de la tarea.


