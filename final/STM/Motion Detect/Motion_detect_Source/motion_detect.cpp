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

/*
 * Description: Keyword spotting example code using MFCC feature extraction
 * and neural network. 
 */

#include "motion_detect.h"

MOTION_DETECT::MOTION_DETECT(q15_t* motiondata_buffer)
{
  motion_nn = new MOTION_NN();
  motiondata = motiondata_buffer;
  sliding_window_len = 1;
  init_motion_detect();
}

MOTION_DETECT::~MOTION_DETECT()
{
  delete output;
  delete predictions;
  delete averaged_output;
}

void MOTION_DETECT::init_motion_detect()
{
  
  // num_frames = motion_nn->get_num_frames();
  // frame_len = motion_nn->get_frame_len();
  // frame_shift = motion_nn->get_frame_shift();
  // int mfcc_dec_bits = motion_nn->get_in_dec_bits();
  num_out_classes = motion_nn->get_num_out_classes();
  
  output = new q15_t[num_out_classes];
  averaged_output = new q15_t[num_out_classes];
  predictions = new q15_t[sliding_window_len*num_out_classes];
  // audio_block_size = recording_win*frame_shift;
  // audio_buffer_size = audio_block_size + frame_len - frame_shift;
}


void MOTION_DETECT::classify()
{
  // motion_nn->run_nn(mfcc_buffer, output);
  motion_nn->run_nn(motiondata, output);
  // Softmax
  arm_softmax_q15(output,num_out_classes,output);
}

int MOTION_DETECT::get_top_class(q15_t* prediction)
{
  int max_ind=0;
  int max_val=-32768; // -2^15
  for(int i=0;i<num_out_classes;i++) {
    if(max_val<prediction[i]) {
      max_val = prediction[i];
      max_ind = i;
    }    
  }
  return max_ind;
}

void MOTION_DETECT::average_predictions()
{
  // shift the old predictions left
  arm_copy_q15((q15_t *)(predictions+num_out_classes), (q15_t *)predictions, (sliding_window_len-1)*num_out_classes);
  // add new predictions at the end
  arm_copy_q15((q15_t *)output, (q15_t *)(predictions+(sliding_window_len-1)*num_out_classes), num_out_classes);
  //compute averages
  int sum;
  for(int j=0;j<num_out_classes;j++) {
    sum=0;
    for(int i=0;i<sliding_window_len;i++) 
      sum += predictions[i*num_out_classes+j];
    averaged_output[j] = (q15_t)(sum/sliding_window_len);
  }   
}
  
