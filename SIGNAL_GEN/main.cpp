/*
 * SIGNAL_GEN.cpp
 *
 * Created: 18/04/2018 12:43:19 a. m.
 * Author : PC
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


long int spwm [30]= {128,154,180,203,223,238,249,255,255,249, 238,223, 203, 180,154,128,101, 75,52,32, 17,6,0,0,6,17,32,52,75,101};
long int sierra[30] ={0, 8,17, 26, 35, 43, 52, 61, 70, 79, 87, 96,105, 114, 123, 131,140, 149, 158, 167, 175, 184, 193, 202, 211, 219, 228, 237, 246, 255};
long int tri[30]={0,17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255,238,221,204,187,170,153,136,119,102,85,68,51,34,17};
long int *out;
int n,aux,col,fila,valor,frec,f2,n_tecla;
double m;
long int t;

void recorre_puerto();
void lee_puerto();

ISR(TIMER1_OVF_vect)
{
	//recorre_puerto();
	if (n<(29)){n++;}
	else{n=0;}
	OCR0=out[n];
    TCNT1 = -t;	
}

ISR(TIMER2_OVF_vect)
{
	recorre_puerto();
	TCNT2=-45;
}

ISR(INT0_vect)
{
	lee_puerto();
	if ((valor>10) & (valor<16))
	{
		switch(valor)
		{
			case 11:
			out = spwm;
			break;
			
			case 12:
			out = sierra;
			break;
			
			case 13:
			out = tri;
			break;
		}
	}
	
	else
	{
		switch(n_tecla)
		{
			case 0:
				n_tecla++;
				f2=valor*10;
			break;
			
			case 1:
				n_tecla++;
				if (valor==10){valor=0;} 
				//else{valor=valor;}
				f2+=valor;
			break;
			
			case 2:
				if (valor==16)
				{frec=f2;}
				else{frec=100;}
			    n_tecla=0;
				n=0;
				//comp = -0.0024*frec+1.24;
				PORTC=frec;
				//m=(double) 1000000/(frec*30);
				m=(double) 33333.33/frec;
				t = round(m)-90;
			break;
			
		}//switch
		PORTC=n_tecla;
	}//else
}

void recorre_puerto()
{
	
	if ((aux&0xF0)!=0x80)
	{
		aux=aux<<1;
	}
	else
	{
		aux=0x10;
	}
	PORTA=~aux;
}

void lee_puerto()
{
	col= (((aux>>4)&0x08)>>3)*4 + (((aux>>4)&0x04)>>2)*3 + ((aux>>4)&0x02) + ((aux>>4)&0x01);
	fila=PINA&0x0F;
	switch (fila)
	{
		case 0x07:
		valor=0;
		break;
		
		case 0x0B:
		valor=4;
		break;
		
		case 0x0D:
		valor=8;
		break;
		
		case 0x0E:
		valor=12;
		break;
	}
	
	valor+=col;
	
}

int main(void)
{
    /* Replace with your application code */
	DDRB |= (1 <<PINB3);  //  output
	DDRA=0xF0; //NIBLE ALTO SALIDAS
	DDRC=0XFF;
	PORTA=0x0F; //ACTIVA PULL-UPS
	PORTC=0x00;
	
	TCCR0|= (1<<WGM01)|(1<<WGM00) | (1<<COM01) | (1<<CS00); //PWM
	
	TCCR2|= 0x07;  //preescaler a 1024
	TIMSK|= (1<<TOIE2);
	TCNT2=-25;			//TECLADO
	
	
	TCCR1B|=(1<<CS10);
	TIMSK|=(1<<TOIE1); //CAMBIO DE ESTADO DE SEÑALES
	
	MCUCR=0x02;	//INTERRUPCION POR FLANCO DE BAJADA
	GICR|=(1<<INT0);	//HABILITA INTERRUPCION
	sei();
	
	aux=0x10;
	PORTA=~aux;
	PORTC=0x00;
	n=0;
	frec=100;
	out=sierra;
	n_tecla=0;
	f2=0;
	m=(double) 1000000/(frec*30);
	t = round(m)-80;
	OCR0=0;
	TCNT1=-100;

    while (1) 
    {
		
    }
}

