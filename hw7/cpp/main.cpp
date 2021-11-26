/* ----------------------------------------------------------------------
** Include Files
** ------------------------------------------------------------------- */
#include "mbed.h"
#include "arm_math.h"
#include "math_helper.h"
#include <cstdio>

#define SEMIHOSTING

#include <stdio.h>


/* ----------------------------------------------------------------------
** Macro Defines
** ------------------------------------------------------------------- */

#define TEST_LENGTH_SAMPLES  320
/*
This SNR is a bit small. Need to understand why
this example is not giving better SNR ...
*/
#define SNR_THRESHOLD_F32    75.0f
#define BLOCK_SIZE            32

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
/* Must be a multiple of 16 */
#define NUM_TAPS_ARRAY_SIZE              32
#else
#define NUM_TAPS_ARRAY_SIZE              29
#endif

#define NUM_TAPS              29

/* -------------------------------------------------------------------
 * The input signal and reference output (computed with MATLAB)
 * are defined externally in arm_fir_lpf_data.c.
 * ------------------------------------------------------------------- */

extern float32_t testInput_f32_1kHz_15kHz[TEST_LENGTH_SAMPLES];
extern float32_t refOutput[TEST_LENGTH_SAMPLES];

/* -------------------------------------------------------------------
 * Declare Test output buffer
 * ------------------------------------------------------------------- */

static float32_t testOutput[TEST_LENGTH_SAMPLES];

/* -------------------------------------------------------------------
 * Declare State buffer of size (numTaps + blockSize - 1)
 * ------------------------------------------------------------------- */
#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
static float32_t firStateF32[2 * BLOCK_SIZE + NUM_TAPS - 1];
#else
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
#endif 


const float32_t firCoeffs32[29] = {
    +0.00157110313822663f,+0.00193587740164167f,+0.000f,-0.00450800417827176,-0.00696141204220354f,+0.00f,+0.0150407722350994f,+0.0212098923660594f,+0.000f,-0.0406688345893480f,-0.0568310931756990f,+0.00f,+0.131209102667399f,+0.271772086903490f,	+0.332461018547214f,	+0.271772086903490f,	+0.131209102667399f,	+0.00f,	-0.0568310931756990f,	-0.0406688345893480f,	+0.000f,	+0.0212098923660594f,	+0.0150407722350994f,	+0.000f,	-0.00696141204220354f,	-0.00450800417827176f,	+0.000f,	+0.00193587740164167f,	+0.00157110313822663f
};

/* ------------------------------------------------------------------
 * Global variables for FIR LPF Example
 * ------------------------------------------------------------------- */

uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;

float32_t  snr;

/* ----------------------------------------------------------------------
 * FIR LPF Example
 * ------------------------------------------------------------------- */

int32_t main(void)
{
  uint32_t i;
  arm_fir_instance_f32 S;
  arm_status status;
  float32_t  *inputF32, *outputF32;

  /* Initialize input and output buffer pointers */
  inputF32 = &testInput_f32_1kHz_15kHz[0];
  outputF32 = &testOutput[0];

  /* Call FIR init function to initialize the instance structure. */
  arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], blockSize);

  /* ----------------------------------------------------------------------
  ** Call the FIR process function for every blockSize samples
  ** ------------------------------------------------------------------- */

  for(i=0; i < numBlocks; i++)
  {
    arm_fir_f32(&S, inputF32 + (i * blockSize), outputF32 + (i * blockSize), blockSize);
  }

  /* ----------------------------------------------------------------------
  ** Compare the generated output against the reference output computed
  ** in MATLAB.
  ** ------------------------------------------------------------------- */

  snr = arm_snr_f32(&refOutput[0], &testOutput[0], TEST_LENGTH_SAMPLES);
  
  status = (snr < SNR_THRESHOLD_F32) ? ARM_MATH_TEST_FAILURE : ARM_MATH_SUCCESS;
  printf("test!!!\r\n");
  printf("snr : %f\n",snr);
  for(int i = 0;i<315;i++){
      printf("%f, ",testOutput[i]);
  }
  printf("\n");
  if (status != ARM_MATH_SUCCESS)
  {
#if defined (SEMIHOSTING)
    printf("FAILURE\n");
#else
    while (1);                             /* main function does not return */
#endif
  }
  else
  {
#if defined (SEMIHOSTING)
    printf("SUCCESS\n");
#else
    while (1);                             /* main function does not return */
#endif
  }
}