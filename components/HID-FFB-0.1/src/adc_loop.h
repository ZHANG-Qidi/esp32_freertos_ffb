#ifndef _ADC_LOOP_H_
#define _ADC_LOOP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_READ_INTERVAL (10)

void adc_loop(void);
float adc_output(int index);

#ifdef __cplusplus
}
#endif

#endif