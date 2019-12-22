#INCLUDE <16F887.H>
#device adc=10
#USE DELAY(CLOCK=4000000)
#FUSES XT,NOPROTECT,NOWDT,NOBROWNOUT,NOPUT,NOLVP
//#DEFINE USE_PORTB_LCD TRUE
#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7)   // Pines para establecer comunicacion rs232
#DEFINE USE_PORTB_KBD   //Por defecto el teclado se conecta al puerto D,
                        //como el microcontrolador que se esta usando
                        //no tiene puerto D se conecta al puerto B.
  
#include <stdlib.h>

#INCLUDE <LCD.C>
#INCLUDE <KBD4x4.C>  //Incluir en el encabezado el driver para
                      //manejar el teclado telef�nico MODIFICADO
#use     standard_io(b) 
#define  KEYHIT_DELAY   200    //Tiempo de espera del teclado en milisegundos
#byte PORTB= 6
#byte PORTC= 7
#BYTE PORTA= 5
#BYTE PORTD= 8
 
long bits;     //Variable almacena los bits
float tem;     //Almacena la temperatura


  INT DIR;

/*===========================================================================*/
/*=======================       FUNCION TECLA         =======================*/
/*===========================================================================*/
//Funcion encargada de esperar a que se presione una tecla 
char tecla(void){
   char c;
   do{ //espera hasta que se presione una tecla
      c=kbd_getc(); //Captura valor del teclado
     }
   while(c=='\0');
   return(c);
}

/*===========================================================================*/
/*=======================    FUNCION TECLA CON TIMER  =======================*/
/*===========================================================================*/
// Pregunta por una Tecla por un tiempo, si no hay actividad, deja de preguntar
// y deja que el PIC continue con su trabajo

char tecla_time(void) {
   char c='\0';
   unsigned int16 timeout;
   timeout=0;
   c=kbd_getc(); //Captura valor del teclado
   while(c=='\0' && (++timeout< (KEYHIT_DELAY*100)))
   {
      delay_us(10);
      c=kbd_getc(); //Captura valor del teclado
   }
   return(c);
}

/*===========================================================================*/
/*=======================       PROGRAMA PRINCIPAL    =======================*/
/*===========================================================================*/

VOID MAIN()
{
  CHAR K;
  
   char aux[2];//declaramos una variable auxiliar para guardar datos del teclado
   
   
   port_b_pullups (0xFF);  //Utiliza las resistencias PULL UP internas del puerto B
   
   set_tris_a(0b00000001);          //Pongo el RA0 como entrada
   SET_TRIS_B(1);
   SET_TRIS_C(0);    //Puerto C como Salida
   
   setup_adc(adc_clock_internal);   //Selecciono reloj interno para conversion
   
   LCD_INIT();       //Inicializar el driver del lcd
   KBD_INIT();       //Inicializar el driver del teclado
   LCD_PUTC("\f");   //Limpia el LCD
   
   
   
   WHILE(1)
   {
   
      int i = 0;//val del vector
      DIR=9;
      LCD_GOTOXY(1,2);  //Ubica el cursor del LCD
      LCD_PUTC("Temp.A= ");
      LCD_GOTOXY(1,2);  //Ubica el cursor del LCD
      aux="";//valor auxiliar para comparar temperatura iniciado en 100 para que se apague el ventilador
      while (DIR < 17)
      {
            if(DIR <= 10){
               if(k!='\0'){
                  
                  if (k == '*' || k == '#'){
                     k='\0';
                  }
                  
                  else{
                     LCD_GOTOXY(DIR,2);
                     lcd_putc(k);
                     aux [i]= k;//guardando los datos de teclado en variable auxiliar
                     i++;//incrementador del puntero de la variable auxiliar
                     k='\0';
                     DIR++;
                     
                  }
                  
               }
            }else if(k=='#'){
                     k='\0';
                     DIR = 17;
                  }
        
         
 // ============================================================================= //        
         //A continuaci�n se muestran diferentes formas para leer los valores
         //del teclado matricial, descomenta la forma de almacenar el dato
         //en la variable "k" y observa las diferencias de cada llamado
         
         
         
         //k=tecla();      //Lee el valor del teclado y espera hasta que alguna tecla se pulse
         k=tecla_time();   //Lee el valor del teclado pero solo espera un tiempo determinado
         //k=kbd_getc();   //Funci�n del ccs c para leer el valor del teclado (sin control)
         
  // ============================================================================= //   
         if(DIR>16)
            LCD_PUTC("\f");
         
       //Lectura ADC
        set_adc_channel(0);          //Selecciono el canal 0 (RA0)
        delay_ms(1);                 //llamo retardo de 1 ms
        bits=read_adc();             //Guarde el dato del LM en tempe
        tem=bits*0.0488758553;              //Conversion de bits a temperatura
        lcd_gotoxy(1,1);             //Ubiquese en la posicion 2,2
        printf(lcd_putc,"Temp. R= %f    ",tem);  //Muestra el valor numerico de la conversion
        
        
             //Codigo del ventilador
        float comp = atol(aux);
         if(tem >= comp){                       // comparacion de la temperatura real a la temperatura ajustada 
            output_high(pin_a2);//encendera ventelidar en caso de sobre pasar la ajustada
            
         }else{
            output_low(pin_a2);//de lo contrario la apagara
            
         }
      }
      
   }
}
