# EmbeddedSystem

## How to Compile
`
mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source .\ --source ../Motion_detect --source ../Motion_detect_Source  --source ../CMSIS_5/CMSIS/NN/Include --source ../CMSIS_5/CMSIS/NN/Source --source ../CMSIS_5/CMSIS/DSP/Include --source ../CMSIS_5/CMSIS/DSP/Source --source ../CMSIS_5/CMSIS/Core/Include --profile ../release_O3.json -j 8
`

## How to update to board
add `--flash` after above command

