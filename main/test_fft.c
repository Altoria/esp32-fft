/* FFT Example

   This example runs a few FFTs and measure the timing.

  Author: Robin Scheibler, 2017
   This code is released under MIT license. See the README for more details.§
*/
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "fft.h"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define REP 20
#define NFFT 2048

double start, end;

timer_config_t timer_config = {
  .alarm_en = false,
  .counter_en = true,
  .counter_dir = TIMER_COUNT_UP,
  .divider = 80   /* 1 us per tick */
};

void clock_init()
{
  timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_start(TIMER_GROUP_0, TIMER_0);
}

void fft_test_task()
{
  int k;

  fft_config_t *fft_analysis = fft_init(NFFT, FFT_REAL, FFT_FORWARD, NULL, NULL);
  fft_config_t *fft_synthesis = fft_init(NFFT, FFT_REAL, FFT_BACKWARD, fft_analysis->output, NULL);

  // Fill array with some dummy data
  for (k = 0 ; k < fft_analysis->size ; k++)
    fft_analysis->input[k] = (float)k / (float)fft_analysis->size;

  // Test accuracy
  fft_execute(fft_analysis);
  fft_execute(fft_synthesis);

  int n_errors = 0;
  for (k = 0 ; k < fft_analysis->size ; k++)
    if (abs(fft_analysis->input[k] - fft_synthesis->output[k]) > 1e-5)
    {
      printf("bin=%d input=%.4f output=%.4f\n err=%f", 
          k, fft_analysis->input[k], fft_synthesis->output[k], 
          fabsf(fft_analysis->input[k] - fft_synthesis->output[k]));
      n_errors++;
    }
  if (n_errors == 0)
    printf("Transform seems to work!\n");

  // Now measure execution time
  timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &start);
  for (k = 0 ; k < REP ; k++)
    fft_execute(fft_analysis);
  timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &end);
  printf(" Real FFT size=%d runtime=%f ms\n", NFFT, 1000 * (end - start) / REP);

  timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &start);
  for (k = 0 ; k < REP ; k++)
    fft_execute(fft_synthesis);
  timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &end);
  printf("Real iFFT size=%d runtime=%f ms\n", NFFT, 1000 * (end - start) / REP);
}

void app_main()
{
  clock_init();
  fft_test_task();
  //xTaskCreate(&fft_test_task, "fft_test_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
