void AgregarContactos();//Funcion para agregar contactos al archivo .txt
int CargaContactos();//Funcion que carga los contactos desde el txt al inicio del programa
int TotalContactos();//Funcion para contar la cantida de contactos

typedef struct{ //struct para almacenar los contactos con alias contacto
	char ip[256];
	char nombre[256];
	char puerto[8];
}contacto;
