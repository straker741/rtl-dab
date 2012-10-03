/*
This file is part of rtl-dab
trl-dab is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with rtl-dab.  If not, see <http://www.gnu.org/licenses/>.


david may 2012
david.may.muc@googlemail.com

*/


#include <stdint.h>
#include <fftw3.h>
#include "dab_read_fifo.h"
#include "dab_sync.h"


#define DEFAULT_BUF_LENGTH (16 * 16384)

typedef struct{
  uint32_t frequency;
  uint8_t input_buffer[DEFAULT_BUF_LENGTH];
  uint16_t input_buffer_len;
  int32_t coarse_timeshift;
  int32_t fine_timeshift;
  uint32_t coarse_freq_shift;
  double fine_freq_shift;
  CircularBuffer fifo;
  int8_t real[196608];
  int8_t imag[196608];
  float filt[196608-2662];
  fftw_complex * dab_frame;
  fftw_complex * prs_ifft;
  fftw_complex * prs_conj_ifft;
  fftw_complex * prs_syms;
  /* raw symbols */
  fftw_complex symbols[76][2048];
}dab_state;


int8_t dab_demod(dab_state *dab);
void dab_demod_init(dab_state *dab);