#makefile para el programa Communicator
COMMU=clienteFuncional2.c
SV=S.c
compilacion:
		@echo Creando el archivo Cliente.out
		gcc $(COMMU) -w -o Cliente.out
		@echo Creando el archivo Server.out
		gcc $(SV) -w -o Server.out
		@echo Compilacion finalizada

