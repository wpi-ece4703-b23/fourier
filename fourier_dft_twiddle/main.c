#include "xlaudio.h"
#include "xlaudio_armdsp.h"
#include <stdio.h>

#define BBUFLEN_64
#define BUFLEN_SZ 64
#define N 64

typedef struct {
  float32_t real;
  float32_t imag;
} COMPLEX;

COMPLEX samples[N];
COMPLEX twiddle[N];

void inittwiddle() {
    int i;
    for(i=0 ; i<N ; i++) {
        twiddle[i].real =  cos(2*M_PI*i/N);
        twiddle[i].imag = -sin(2*M_PI*i/N);
    }
}

void dft() {
  COMPLEX result[N];
  int k,n;

  for (k=0 ; k<N ; k++)  {

    result[k].real = 0.0;
    result[k].imag = 0.0;

    for (n=0 ; n<N ; n++) {
         result[k].real += samples[n].real*twiddle[(n*k)%N].real
                         - samples[n].imag*twiddle[(n*k)%N].imag;
         result[k].imag += samples[n].imag*twiddle[(n*k)%N].real
                         + samples[n].real*twiddle[(n*k)%N].imag;
    }
  }

  for (k=0 ; k<N ; k++) {
    samples[k] = result[k];
  }
}

void processBuffer(uint16_t x[BUFLEN_SZ], uint16_t y[BUFLEN_SZ]) {
    int i;
    for (i=0; i<N; i++) {
        samples[i].real = xlaudio_adc14_to_f32(x[i]);
        samples[i].imag = 0.0f;
    }
    dft();
    for (i=0; i<N; i++) {
        y[i] = xlaudio_f32_to_dac14(samples[i].real*samples[i].real +
                                    samples[i].imag*samples[i].imag);
    }
}

int main() {
  inittwiddle();

  xlaudio_init_dma (FS_8000_HZ,
                    XLAUDIO_J1_2_IN,
                    BUFLEN_64,
                    processBuffer);

  int c = xlaudio_measurePerfBuffer(processBuffer);
  printf("Cycles %d\n", c);

  xlaudio_run();
}
