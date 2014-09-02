#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N 80

void encriptarFrase();
void desencriptarFrase();

int largo;
char frase[N];

int main ()
{
    printf("Ingrese la frase: ");
    scanf("%10000[^\n]", frase);

    largo = strlen(frase);

    encriptarFrase();
    printf("La frase encriptada es: %s \n",frase);

    desencriptarFrase();
    printf("La frase desencriptada es: %s \n",frase);
    return 0;
}

void encriptarFrase ()
{
    int i;

    for(i=0 ; i<largo ; ++i)
    {
        //convierte las minusculas a mayuscula.
        //*(frase + i) = toupper(*(frase + i));

        //descarta los digitos
        if(isalpha(*(frase + i)))
        {
            //verifica los casos especiales X-Y-Z
            //if(*(frase + i) > 87)
            {
                *(frase + i) = *(frase + i) - 23;
            }
            //else
            //{
                //*(frase + i) = *(frase + i) + 3;
            //}
        }
        
        if(isdigit(*(frase + i)))
		{
				*(frase + i) = *(frase + i) + 6;
			
		}
		
		else
		{
				*(frase + i) = *(frase + i) - 4;
		}
		
    }
}

void desencriptarFrase ()
{
    int i;

    for(i=0 ; i<largo ; ++i)
    {
        if(isalpha(*(frase + i)))
        {
            //if(*(frase + i) < 68)
            {
                *(frase + i) = *(frase + i) + 23;
            }
            //else
            //{
                //*(frase + i)= *(frase + i) - 3;
            //}
        }
        
        if(isdigit(*(frase + i)))
		{
				*(frase + i) = *(frase + i) - 6;
			
		}
		
		else
		{
				*(frase + i) = *(frase + i) + 4;
		}
    }
}
