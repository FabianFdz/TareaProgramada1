//------------------------------------- INCLUCION DE LIBRERIAS A UTILIZAR ------------------------------------------------------------
#include <stdio.h>    //funciones estándar de entrada/salida
//#include <conio.h>    //para utilizar getche
#include<stdlib.h>   //para utilizar system
#include<string.h>   //manejo de cadenas
#define TEXT_L 500   //costante para la cadena del texto plano

//-----------------------------------------   FUNCION CIFRAR   -----------------------------------------------------------------------
void
Cifrar_Texto(void){
	
    int longitud_cadena,longitud_clave,i,j,argc;
    unsigned char alfabeto[]= "ABCDEFGHIJKLMN\xA5OPQRSTUVWXYZ. \0"; 
    int alf_len   = strlen((char*)alfabeto);
    int alf_A_pos = 0;
    int alf_Z_pos = alf_len-3;
    unsigned char cadena[TEXT_L]; // se usa unsigned para que la Ñ tome el valor 165 en lugar de -91
    /*
    unsigned char clave[100];
	memset((void*)clave,'\0',100);
    */
	unsigned char cifrado[TEXT_L];  
	memset((void*)cifrado,'\0',TEXT_L);
	unsigned char char_string[] = "\0";
   
    printf("\nIntroduce el texto a cifrar:");
    scanf("%[^\n]", cadena);
	fflush(stdin);  
	longitud_cadena = strlen ( (char *) cadena);    // para que funcione strlen con un unsigned char 
    printf("\nLongitud del texto:%d",longitud_cadena);
    
    /* 
    printf("\n\nIntroduce la clave(KEY):");
	scanf("%[^\n]", clave);
	fflush(stdin); 
	longitud_clave = strlen( (char *) clave);    // para que funcione strlen con un unsigned char 
    printf("\nLongitud de la clave:%d\n",longitud_clave);
    */

    char clave[100] = "cifradordecontenido";

   	printf("\nalfabeto[0]=%c, alfabeto[alf_len]=%c\n",alfabeto[alf_A_pos],alfabeto[alf_Z_pos]);
    printf("alfabeto[0]=%i, alfabeto[alf_len]=%i\n",alfabeto[alf_A_pos],alfabeto[alf_Z_pos]);
        
    for(i=0; i<longitud_cadena; i++){
		    	
    	if(cadena[i] >= alfabeto[alf_A_pos] && cadena[i] <= alfabeto[alf_Z_pos]){
    		j = i % longitud_clave;
			char_string[0] = ((cadena[i] - alfabeto[alf_A_pos]) + (clave[j] - alfabeto[alf_A_pos]))   % (alf_Z_pos) + alfabeto[alf_A_pos]; 
    		strncat((char*)cifrado,(char*)char_string,2);  

            /*  		 
			printf("Letra original: %c",cadena[i]);  
			printf(" Letra clave: %c",clave[j]); 				
			printf("  Letra cifrada: %c\n",cifrado[i]);
    		*/


    	}else{
    		char_string[0] = cadena[i];
    		strncat((char*)cifrado,(char*)char_string,2);    		
    	}
   }
    printf("\n\nEl texto cifrado en vigenere es: %s",cifrado);    
    return;
}       
//--------------------------------------------  FUNCION DESCIFRAR  ----------------------------------------------------------------------
void
Descifrar_Texto(void){
    int longitud_cadena,longitud_clave,i,j,argc;
    unsigned char alfabeto[]= "ABCDEFGHIJKLMN\xA5OPQRSTUVWXYZ .\0"; 
    int alf_len   = strlen((char*)alfabeto);
    int alf_A_pos = 0;
    int alf_Z_pos = alf_len-3;
    unsigned char cadena[TEXT_L]; // se usa unsigned para que la Ñ tome el valor 165 en lugar de -91
    unsigned char clave[100];
	memset((void*)clave,'\0',100);
	unsigned char descifrado[TEXT_L];  
	memset((void*)descifrado,'\0',TEXT_L);
	unsigned char char_string[] = "\0";
   
    printf("\nIntroduce el texto a cifrar:");
    scanf("%[^\n]", cadena);
	fflush(stdin);  
	longitud_cadena = strlen ( (char *) cadena);    // para que funcione strlen con un unsigned char 
    printf("\nLongitud del texto:%d",longitud_cadena);
    
    printf("\n\nIntroduce la clave(KEY):");
	scanf("%[^\n]", clave);
	fflush(stdin); 
	longitud_clave = strlen( (char *) clave);    // para que funcione strlen con un unsigned char 
    printf("\nLongitud de la clave:%d\n",longitud_clave);
    
   	printf("\nalphabeto[o]=%c, alphabeto[alf_len]=%c\n",alfabeto[alf_A_pos],alfabeto[alf_Z_pos]);
    printf("alphabeto[o]=%i, alphabeto[alf_len]=%i\n",alfabeto[alf_A_pos],alfabeto[alf_Z_pos]);
        
    for(i=0; i<longitud_cadena; i++){
	    	
    	if(cadena[i] >= alfabeto[alf_A_pos] && cadena[i] <= alfabeto[alf_Z_pos]){
    		j = i % longitud_clave;
			char_string[0] = ((cadena[i] - alfabeto[alf_A_pos]) - (clave[j] - alfabeto[alf_A_pos]))   % (alf_Z_pos) + alfabeto[alf_A_pos]; 
    		strncat((char*)descifrado,(char*)char_string,2);    		 
			printf("Letra original: %c",cadena[i]);  	

			//printf("  Letra clave: %c",clave[j]); 
			
            printf(" Letra descifrada: %c\n",descifrado[i]);		
    		
    	}else{
    		char_string[0] = cadena[i];
    		strncat((char*)descifrado,(char*)char_string,2);    		
    	}
    	
   }
    printf("\n\nEl texto descifrado en vigenere es: %s",descifrado);    
    return;
}   



/* 
//------------------------ FUNCION CRIPTOANALIZAR ----------------------------------------------------------------
void                                                                                                                               
Criptoanalizar(void){    
                                                                                                                                                                                                           
    return;   
}           



//------------------------ FUNCION MENU --------------------------------------------------------------------------
void
Menu(void){
	system("color 1F");  //Para poner color al fondo y letras
    char opc[1];
    do{
	printf("\n");
    printf("\t\t-----------------------------------------------\n");
    printf("\t\t      PROGRAMA CRIPTOGRAFICO DE VIGENERE       \n");
    printf("\t\t-----------------------------------------------\n");
    printf("\t\t(1).-Cifrar Texto\n");
    printf("\t\t(2).-Descifrar Texto\n");
    printf("\t\t(3).-Criptoanalizar\n");
    printf("\t\t(4).-Salir\n");
    printf("\t\t------------------------------------------------\n");
    printf("\n\t\tElija una Opcion:");    
    gets(opc);   
    
    if ( opc[0]=='1' ){
        Cifrar_Texto();
    }     
    	if ( opc[0]=='2' ){
            Descifrar_Texto();
    	}
   			 if ( opc[0]=='3' ){
           		 Criptoanalizar();
   			}
    			if ( opc[0]=='4' ){
    				printf("\t\t\t\nFin del programa");      				
    			}    		
   }while(opc[0]!='4');	
}

//-----------------------------FUNCION MAIN ----------------------------------------------
int
main(void){
   Menu();      
   getche();
   return(0);
}

*/