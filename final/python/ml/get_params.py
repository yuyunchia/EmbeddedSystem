from io import FileIO
import torch
import numpy as np

from motionnet import MotionNet

config = {
    "model_dir": "../../models/motion_88.ckpt"
}

def write_params(params: torch.TensorType, file: FileIO, name: str):
    p = params.data.numpy()
    min_value, max_value = np.min(p), np.max(p)
    int_bits = int(np.ceil(np.log2(max(abs(min_value), abs(max_value)))))
    dec_bits = 15 - int_bits

    p = np.round(p * (2 ** dec_bits))
    
    file.write("#define " + name + " {")

    transposed_weights = np.transpose(p)

    transposed_weights.tofile(file, sep=", ", format="%d")
    file.write("}\n")

def get_params():
    model = MotionNet()
    model.load_state_dict(torch.load(config["model_dir"], map_location="cpu"))
    
    layer_name = ["LINEAR_1_WT", "LINEAR_1_BIAS", "LINEAR_2_WT", "LINEAR_2_BIAS", "LINEAR_3_WT", "LINEAR_3_BIAS"]

    with open("../../cpp/weight.h", "w+") as file:
        for i, params in enumerate(model.parameters()):
            print(params.shape)
            write_params(params, file, layer_name[i])
                

if __name__ == "__main__":
    get_params()