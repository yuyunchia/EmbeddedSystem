/*
 * Copyright (C) 2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MOTION_NN_H__
#define __MOTION_NN_H__


#include "motion_nn_weights.h"
#include "arm_nnfunctions.h"
#include "arm_math.h"

/* Network Structure 

  10x25 input features
    |
   IP1 : Innerproduct (weights: 24x64)
    |
   IP2 : Innerproduct (weights: 64x16)
    |
   IP3 : Innerproduct (weights: 16x4)
    |
   4 outputs

*/




#define IN_DIM 24
#define OUT_DIM 4
#define IP1_OUT_DIM 64
#define IP2_OUT_DIM 16
#define IP1_WT_DIM (IP1_OUT_DIM*IN_DIM)
#define IP2_WT_DIM (IP2_OUT_DIM*IP1_OUT_DIM)
#define IP3_WT_DIM (OUT_DIM*IP2_OUT_DIM)
#define SCRATCH_BUFFER_SIZE (2*(IN_DIM+3*IP1_OUT_DIM))

class MOTION_NN {

  public:
    MOTION_NN();
    ~MOTION_NN();
    void run_nn(q15_t* in_data, q15_t* out_data);
  
    // int get_num_frames();
    // int get_frame_len();
    // int get_frame_shift();
    int get_num_out_classes();
    // int get_in_dec_bits();

  protected:
    // int frame_len;
    // int frame_shift;

    // int num_frames;
    int num_out_classes;
    // int in_dec_bits;

  private:
    q15_t* scratch_pad;
    q15_t* ip1_out;
    q15_t* ip2_out;
    q15_t* ip3_out;
    q15_t* vec_buffer;
    static q15_t const ip1_wt[IP1_WT_DIM];
    static q15_t const ip1_bias[IP1_OUT_DIM];
    static q15_t const ip2_wt[IP2_WT_DIM];
    static q15_t const ip2_bias[IP2_OUT_DIM];
    static q15_t const ip3_wt[IP3_WT_DIM];
    static q15_t const ip3_bias[OUT_DIM];

};

#endif
