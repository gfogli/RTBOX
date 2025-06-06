/*
 *  Utils.h
 *  Copyright (C) 2025
 *  JoÃ£o Vitor de M.G. Rosmaninho <jvrosmaninho@ufmg.br>,
 *  Gustavo Miranda Auler <gustavoauler@ufmg.br>,
 *
 *  Version 1.0 - API with the following implemented function:
 *  float CondicionaSinal(Uint16 sinal, Uint16 offset, float gain);
 *  void clarkeTransform(*Clarke clarke, float a, float b, float c);
 *  void parkTransform(Park *park, Clarke clarke, float angulo);
 *  void inverseClarkeTransform(Park park, Clarke *clarke);
 *  void inverseParkTransform(Clarke clarke, float *a, float *b, float *c);
 *  void executePLL(Park park, float *omega, float *angulo);
 *  Based on notes from professor Gabriel Fogli <gabrielfogli@ufmg.br> for real time control with DSPs course.
 *
 *
 *  Created on: 2025/01/31
 *  Institution: UFMG
 */

#ifndef UTILS_H_
#define UTILS_H_
#include "F28x_Project.h"
#include "math.h"

// PerÃ­odo de amostragem
#define DELT 0.00005
// Definicoes da geracao do PWM
#define PWM_TBPRD 2500 // Define a freq. de amostragem PWMTBPRD = (10000*2500)/(freq_desejada*0.5)   (20kHz => 2500 [Modo UP-DOWN]) [pag. 1914]
#define PWM_CMPR25 0   // PWM initial duty cycle = 25%
// PWM
//#define Periodo_PWM 100000000 * 0.5 * 0.5 * DELT // 0.5(up_down)*Freq do CPU * Period da interrupcao (em sec)
#define DUTYCYCLE 750
// PLL
#define Kp_PLL 2.93
#define Ki_PLL 793
// Controle
#define Id_REF 20
#define Iq_REF 0
#define Kp_CONT 2.2
#define Ki_CONT 200

#define Ki_VDC 0.04
#define Kp_VDC 0.0001396

typedef struct
{
    float alfa;
    float beta;
} Clarke;

typedef struct
{
    float D;
    float Q;
} Park;

float CondicionaSinal(Uint16 sinal, int offset, float gain);
void clarkeTransform(Clarke *clarke, float a, float b, float c);
void parkTransform(Park *park, Clarke *clarke, float angulo);
void inverseParkTransform(Clarke *clarke, float vdc, float udRef, float uqRef);
void inverseClarkeTransform(Clarke *clarke, float *a, float *b, float *c);
void executePLL(Park *park, float *omega, float *angulo);
void dcVoltageControl(float *idRef, float *Vdc_U, float vdc, float vdcRef);
void currentControl(Park *park, float idRef, float iqRef, float *udRef, float *uqRef);
Uint16 convertSine2PWM(float u);
Uint16 pwmConvert(float u);
void LPFilter(float *anterior, float atual, float alfa);
#endif /* UTILS_H_ */
