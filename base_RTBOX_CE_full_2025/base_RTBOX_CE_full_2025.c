// Cria cabeçalho


#include "F28x_Project.h"
#include "math.h"
#include "Peripherals.h"
#include "Utils.h"
#include "stateMachine.h"




// Measurements (from ADCResultsRegs)
Uint16 sensorA = 0, sensorB = 0, sensorC = 0, sensorD = 0;
Uint16 sensorE = 0, sensorF = 0, sensorG = 0, sensorH = 0;
Uint16 sensorI = 0, sensorJ = 0, sensorK = 0, sensorL = 0;
Uint16 sensorM = 0;

// system variables
float igA = 0, igB = 0, igC = 0;
float vcapA = 0, vcapB = 0, vcapC = 0;
float itA = 0, itB = 0, itC = 0;
float vpacA = 0, vpacB = 0, vpacC = 0;
float vdc = 0, vdcAnterior = 0;
float Defasado = 0;
float vdc_filter = 0;
float ig_final=0, vcf_final=0, it_final=0;

// Clarke and Park Transform
 Clarke I_Clarke, V_Clarke, U_Clarke;
 Park I_Park, V_Park;

// Control variables
float Vdc_U = 311, Vdc_Ref = 311;
float iq_ref = 0, id_ref = 0;
float uq_ref = 0, ud_ref = 0;

float ig_ref = 0;
float ig_ref_ampli = 0;

float error = 0, inte_error = 0;
float error_after_sum = 0, inte_error_after_sum = 0, inte2_error_after_sum = 0;
float after_wn2 = 0, after_xi = 0;

float element1 = 0, element2 = 0, element3 = 0, element4 = 0, element5 = 0, element6 = 0;

float m = 0;
float m_PWM = 0;

float K1 = -0.060880252044768*10;
float K2 = -0.01766405787119710;
float K3 = 0.002176039647572*10;
float K4 = 0.031622776601686*10;
float K5 = -1.915782131413451e+03*10;
float K6 = 99.428309138826780*10;

float display_itA = 0;
float display_itA_hat = 0;

float display_vcf = 0;
float display_vcf_hat = 0;

float display_ig = 0;
float display_ig_ref = 0;

float display_Vg_est = 0;


// SOGI

float z_SOGI = 0;
float V_linha_SOGI = 0;
float qV_linha_SOGI = 0;
float inte_a_SOGI = 0;


// PLL
float omega = 0, angulo = 0;
int16 angulo_saida;
// Transformada inversa de Clarke
float ua = 0, ub = 0, uc = 0;

// MÃ¡quina de estados
 StateMachine stateMachine;

// Flags de estado
 int dcbus = 0, erro = 0;
// Declaracao de Funcoes
 void App_transPLL2SHUNT();
 void App_transSHUNT2BYPASS();
 void App_transBYPASS2DCBUS();
 void App_transDCBUS2WORKING();
 void App_transAnyToError();
__interrupt void adca1_isr(void);

void main(void)
{

    // Passo 1 - Inicializa sistema de controle

    // PLL, WatchDog, enable Peripheral Clocks
    InitSysCtrl();

    // Passo 2 - Inicializa GPIO
    InitGpio();

    EALLOW;
        GpioCtrlRegs.GPADIR.bit.GPIO6 = 1; // Corrente
        GpioCtrlRegs.GPADIR.bit.GPIO7 = 1; // Switch rede
        GpioCtrlRegs.GPADIR.bit.GPIO8 = 1; // Switch Bypass
    EDIS;

    InitEPwm1Gpio(); // Lembrar do F2837xD_EPwm.c (EPWM1)
    InitEPwm2Gpio();
    InitEPwm3Gpio();
    InitEPwm4Gpio();
    InitEPwm5Gpio();
    InitEPwm6Gpio();

    ConfiguraPWM1();
    ConfiguraPWM2();
    ConfiguraPWM3();
    ConfiguraPWM4();
    ConfiguraPWM5();
    ConfiguraPWM6();
    ConfiguraDAC();
    ConfiguraADC();
    SM_init(&stateMachine);

    DINT;

    InitPieCtrl();
    IER = 0;
    IFR = 0;
    InitPieVectTable();

    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; // Mapeia a int para ADCA1

    IER |= M_INT1;

    EINT;
    ERTM;

    // Loop infinito
    while (1){}
}

__interrupt void adca1_isr(void)
{
    static int InterruptCount = 0; // Variavel de debug
    InterruptCount++;

    static float tempo = 0;
    tempo += DELT; // Incrementa tempo

    // Sensores recebem os valores do ADCs
    sensorA = AdcaResultRegs.ADCRESULT0; // ADCINA2
    sensorB = AdcaResultRegs.ADCRESULT1; // ADCINA3
    sensorC = AdcaResultRegs.ADCRESULT2; // ADCINA4
    sensorD = AdcaResultRegs.ADCRESULT3; // ADCINA5

    sensorE = AdcbResultRegs.ADCRESULT0; // ADCINB2
    sensorF = AdcbResultRegs.ADCRESULT1; // ADCINB3
    sensorG = AdcbResultRegs.ADCRESULT2; // ADCINB4
    sensorH = AdcbResultRegs.ADCRESULT3; // ADCINB5

    sensorI = AdccResultRegs.ADCRESULT0; // ADCINC2
    sensorJ = AdccResultRegs.ADCRESULT1; // ADCINC3
    sensorK = AdccResultRegs.ADCRESULT2; // ADCINC4
    sensorL = AdccResultRegs.ADCRESULT3; // ADCINC5

    sensorM = AdcdResultRegs.ADCRESULT0; // ADCIN14

    // Conversao de bit para valor real das variaveis

    igA = CondicionaSinal(sensorA, 2048, 0.048828125);


    vcapA = CondicionaSinal(sensorD, 2048, 0.09765625);


    itA = CondicionaSinal(sensorG, 2048, 0.048828125);


    vpacA = CondicionaSinal(sensorJ, 2048, 0.09765625);


    vdc = CondicionaSinal(sensorM, 0, 0.2442002);

    Defasado = CondicionaSinal(sensorL, 2048, 0.09765625);

    LPFilter(&vdc_filter, vdc, 0.99252);



    // Realiza controle ///////////////////////////////////////////////////////////

    it_final = itA;
    vcf_final=vcapA;
    ig_final = igA;

    if(tempo>=0.01){

    //S0GI
    z_SOGI = 0.1*(vpacA-V_linha_SOGI);
    V_linha_SOGI += (z_SOGI-qV_linha_SOGI)*377*DELT;
    inte_a_SOGI  += V_linha_SOGI*DELT;
    qV_linha_SOGI = 377*inte_a_SOGI;

    // Referencia
    if (tempo < 20){
         ig_ref = (vpacA/180.0) * ig_ref_ampli;
     }
     else{
         Defasado = qV_linha_SOGI;
         ig_ref = (Defasado/180.0) * ig_ref_ampli;
     }

    // Modulo ressonante com integrador
    error = ig_ref - igA;

    // Integrador
    inte_error += error*DELT;
    element4 = inte_error*K4; //pos ganho 16

    // Ressonante
    error_after_sum = error + after_wn2 + after_xi;
    inte_error_after_sum += error_after_sum*DELT;

    after_xi = inte_error_after_sum*(-2)*0.01*376.9911;
    element6 = inte_error_after_sum*K6;

    inte2_error_after_sum += inte_error_after_sum*DELT;
    after_wn2 = -(376.9911*376.9911)*inte2_error_after_sum;

    element5 = inte2_error_after_sum*K5;

   // Ganhos de realimentacao

    element1 = K1*it_final;
    element2 = K2*vcf_final;
    element3 = K3*ig_final;

    // Entrada de controle

    m = (element1 + element2 + element3 + element4 + element5 + element6)/(vdc*0.5);

    EPwm1Regs.CMPA.bit.CMPA = pwmConvert(m);

    EPwm2Regs.CMPA.bit.CMPA = pwmConvert(m);
    EPwm3Regs.CMPA.bit.CMPA = pwmConvert(m);
    EPwm4Regs.CMPA.bit.CMPA = pwmConvert(m);
    EPwm5Regs.CMPA.bit.CMPA = pwmConvert(m);
    EPwm6Regs.CMPA.bit.CMPA = pwmConvert(m);


       display_Vg_est = vpacA*10 + 2048;
       DacaRegs.DACVALS.all = (Uint16)(display_Vg_est);

       display_ig_ref = ig_ref*20 + 2048;
       DacbRegs.DACVALS.all = (Uint16)(display_ig_ref);

    }

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Clear ADC INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

 void App_transPLL2SHUNT()
{
     GpioDataRegs.GPASET.bit.GPIO7 = 1; // Conectou a rede
 }
 void App_transSHUNT2BYPASS()
{
     GpioDataRegs.GPASET.bit.GPIO8 = 1; // Bypass
 }
 void App_transBYPASS2DCBUS()
{
     Vdc_Ref = 450;
 }
 void App_transDCBUS2WORKING()
{
     GpioDataRegs.GPASET.bit.GPIO6 = 1; // Working
 }
 void App_transAnyToError()
{
     dcbus = 0;
     GpioDataRegs.GPACLEAR.bit.GPIO6 = 1; // Not Working
     GpioDataRegs.GPACLEAR.bit.GPIO7 = 1; // Desconectou da rede
     GpioDataRegs.GPACLEAR.bit.GPIO8 = 1; // Fechou Bypass
 }
