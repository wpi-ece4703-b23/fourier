#include "xlaudio.h"
#include "xlaudio_armdsp.h"
#include <stdio.h>

#include "arm_const_structs.h" // data structure for FFT

#define BUFLEN BUFLEN_64
#define BUFLEN_SZ 64
#define N 64

float32_t samples[2*N];

void perfCheck(uint16_t x[BUFLEN_SZ], uint16_t y[BUFLEN_SZ]) {
    int i;
    for (i=0; i<N; i=i+1) {
        samples[2*i]     = xlaudio_adc14_to_f32(x[i]);
        samples[2*i + 1] = 0.0;
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len64, samples, 0, 1);
    for (i=0; i<N; i=i+1) {
        y[i] = xlaudio_f32_to_dac14(samples[2*i]*samples[2*i] +
                                    samples[2*i+1]*samples[2*i+1]);
    }
}

int main() {
  xlaudio_init_dma (FS_8000_HZ,
                           XLAUDIO_J1_2_IN,
                           BUFLEN_64,
                           perfCheck);

  int c = xlaudio_measurePerfBuffer(perfCheck);
  printf("Cycles %d\n", c);

  arm_cfft_f32(&arm_cfft_sR_f32_len64, samples, 0, 1);

  xlaudio_run();

}
