////////////////////////////////////////////////////////////////
//      programme principale
//      date de création    :12/06/2016
//      date de mise à jour :12/06/2016
//      détails             :
//Hystérésis(seuil de basculement)
//+ Transmission Quartus
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//Debut//
////////////////////////////////////////////////////////////////

//Bibliothéque(s)
#include "mbed.h"

#define MAINPRG
#ifdef MAINPRG
//---------//

//Communication pc (UART)
Serial pc(USBTX, USBRX); // tx, rx - Liaison série

////////////////////////////////////////////////////////////////
//Constante(s)//
////////////////////////////////////////////////////////////////
//Port(s) de communication(s)---------------------------------//
#define ENABLE D6
//GPIO_05 > Pin_B4 > 08----//
#define SIGNAL A5
//SPI---------------------------------------------------------//
#define CS  D10  // cs Output //pas besoin
#define DI  D11  // MOSI  SPI 
//GPIO_00 > Pin_D3 > 02----//
#define DO  D12  // MISO  SPI
#define CLK D13  // Clock SPI
//GPIO_01 > Pin_C3 > 04----//
//Constante(s)------------------------------------------------//
#define HIGH        1
#define LOW         0
#define MAX         5
#define MIN         1
//seuil(s)----------------------------------------------------//
#define SEUILUP     0.6
#define SEUILDOWN   0.6
////////////////////////////////////////////////////////////////
//Déclaration(s)//
////////////////////////////////////////////////////////////////
//Port(s) de communication(s)---------------------------------//
AnalogIn   signal(SIGNAL);
DigitalOut Hearth (ENABLE);
DigitalOut cs (CS);
SPI        spi(DI,DO,CLK);

//Prototype(s)------------------------------------------------//
void task1(void);
void task2 (void);
//événement(s) temporel(s)------------------------------------//
Timer time1;
Ticker count;
Ticker ticker1;
Ticker ticker2;

//Variable(s) globale(s)--------------------------------------//
int g_count=LOW;
int g_cpt=0;
float sinus[20]= {1.00,0.98,0.90,0.79,0.65,0.50,0.35,0.21,
                  0.10,0.02,0.00,0.02,0.10,0.21,0.35,0.50,
                  0.65,0.79,0.90,0.98
                 };
////////////////////////////////////////////////////////////////
//Programme principal//
////////////////////////////////////////////////////////////////
int main()
{
    char freq;
    float bmin[5]= {NULL};
    int bat=0,battement=0;;
    float temps=0;
    float intensite=MIN;
    spi.format(8,0);//taille du bus SPI : 8 bits
    spi.frequency(10000); //fréquance du bus SPI : 10 KHz
    cs=HIGH;//Disable communication
    //count.attach(&task1,1);
    ticker1.attach(&task2,0.05);
    time1.reset();
    time1.start();
    int etats=0;
    while(1) {
        //machine à états qui vérifie les conditions
        switch (etats) {
            case 0:
                if(/*sinus[g_cpt]*/signal.read()>=SEUILUP) {
                    etats=1;
                }
                break;
            case 1:
                if(/*sinus[g_cpt]*/signal.read()<=SEUILDOWN) {
                    etats=0;
                    bat++;
                }
                break;
        }
        //machine à états pour l'affectations
        switch (etats) {
            case 0:
                Hearth=HIGH;
                break;
            case 1:
                Hearth=LOW;
                break;
        }
        if(time1.read_ms()>=3000) {
            bmin[4]=bmin[3];
            bmin[3]=bmin[2];
            bmin[2]=bmin[1];
            bmin[1]=bmin[0];
            bmin[0]=bat;
            battement=((bmin[4]+bmin[3]+bmin[2]+bmin[1]+bmin[0])/5)*20;
            bat=0;
            time1.reset();
        }
        cs=LOW;
        spi.write(battement);
        cs=HIGH;
        if(pc.readable()) {
            freq = pc.getc();
            wait(0.001);
            if((freq == 'm') && (intensite < MAX)) {
                intensite +=0.05;
            }
            if((freq == 'l') && (intensite > MIN)) {
                intensite -=0.05;
            }
            temps=(1/intensite)/20;
            ticker1.detach();
            ticker1.attach(&task2,temps);
        }
        printf("freq = %02f\ttemps = %02f\tbattement/min = %d\n\r",intensite,(temps*20),battement);
        wait(0.005);
    }
}
////////////////////////////////////////////////////////////////
//Fonction(s)//
////////////////////////////////////////////////////////////////
void task1(void)
{
    g_count++;
    //pc.printf("count = %d\n\r",g_count);
    if(g_count>=150)
        g_count=0;
}
void task2(void)
{
    g_cpt++;
    if(g_cpt>=20) {
        g_cpt=0;
    }
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//Fin//
////////////////////////////////////////////////////////////////
#endif //MAINPRG