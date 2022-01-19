from io import FileIO

from numpy.core.fromnumeric import transpose
import torch
import numpy as np
import pandas as pd

from motionnet import MotionNet


from motionnet_inference import Motionnet_Inference


config = {
    "model_dir": "../../models/motion_87.ckpt"
}

def write_params(params: torch.TensorType, file: FileIO, name: str):
    p = params.data.numpy()
    min_value, max_value = np.min(p), np.max(p)
    print("Min: %f, max: %f" % (min_value, max_value))
    int_bits = int(np.ceil(np.log2(max(abs(min_value), abs(max_value)))))
    dec_bits = 15 - int_bits
    print("Int bits: %d, dec bits: %d" % (int_bits, dec_bits))

    p = np.round(p * (2 ** dec_bits))
    
    file.write("#define " + name + " {")

    ## transposed_weights = np.transpose(p)  // need not transpose

    p.tofile(file, sep=", ", format="%d")
    file.write("}\n")


   


def get_params():
    model = MotionNet()
    model.load_state_dict(torch.load(config["model_dir"], map_location="cpu"))
    
    layer_name = ["LINEAR_1_WT", "LINEAR_1_BIAS", "LINEAR_2_WT", "LINEAR_2_BIAS", "LINEAR_3_WT", "LINEAR_3_BIAS"]

   
    with open("../../cpp/weight.h", "w+") as file:
        for i, params in enumerate(model.parameters()):
            #print(params.shape)
            write_params(params, file, layer_name[i])




def run_inference():
    model = MotionNet()
    model.load_state_dict(torch.load(config["model_dir"], map_location="cpu"))

    p_weight = 0

    for i, params in enumerate(model.parameters()):
        ##print(params.shape)
        p = params.data.numpy()

        ## flatten
        if (i == 0):
            p_weight = p.flatten()
        else:
            p_weight = np.concatenate( (p_weight, p.flatten()) )

        ##print(p.size)

        
    net = Motionnet_Inference()



    index_to_label = {
        0: 'left',
        1: 'right',
        2: 'stop',
        3: 'none'
    }

    net = Motionnet_Inference()
   

    net.load_weights(p_weight)

    confusion_matrix = np.zeros( (4,4))
    '''
              left|  right|   stop|nothing|
       left|      |       |       |       |
      right|      |       |       |       |
       stop|      |       |       |       |
    nothing|      |       |       |       |
    '''

    data = pd.read_csv(config["data_dir"])
    data = data.values
    num = len(data)

    la = 0
    ra = 0
    sa = 0
    na = 0

    for i in range(num):
        label = data[i][1] ## 
        inputdata = data[i][0].split(' ')
        inputdata = [float(item) for item in inputdata]
        inputdata = np.array(inputdata)

        predictions = net.forward(inputdata)
        predicted_labels = np.argmax(predictions, axis=0)
        #print(index_to_label[predicted_labels])

        if (label == 'l'):
            la += 1
            if (la > 300):
                continue
            confusion_matrix[0][predicted_labels] += 1
        elif(label == 'r'):
            ra += 1
            if (ra > 300):
                continue
            confusion_matrix[1][predicted_labels] += 1
        elif(label == 's'):
            if (sa > 12):
                continue
            sa += 1
            confusion_matrix[2][predicted_labels] += 1
        elif(label == 'n'):
            if (na > 300):
                continue
            na += 1
            confusion_matrix[3][predicted_labels] += 1
    

    print("result of: ",config["model_dir"])
    print("            left|   right|    stop| nothing|")
    print("   left|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[0][0],confusion_matrix[0][1],confusion_matrix[0][2],confusion_matrix[0][3]) )
    print("  right|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[1][0],confusion_matrix[1][1],confusion_matrix[1][2],confusion_matrix[1][3]) )
    print("   stop|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[2][0],confusion_matrix[2][1],confusion_matrix[2][2],confusion_matrix[2][3]) )
    print("nothing|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[3][0],confusion_matrix[3][1],confusion_matrix[3][2],confusion_matrix[3][3]) )


    model = MotionNet()
    model.load_state_dict(torch.load(config["model_dir"], map_location="cpu"))

    p_weight = 0

    for i, params in enumerate(model.parameters()):
        ##print(params.shape)
        p = params.data.numpy()

        if (i == 0):
            p_weight = p.flatten()
        else:
            p_weight = np.concatenate( (p_weight, p.flatten()) )

        ##print(p.size)

        
    net = Motionnet_Inference()



    index_to_label = {
        0: 'left',
        1: 'right',
        2: 'stop',
        3: 'none'
    }

    net = Motionnet_Inference()
   

    net.load_weights(p_weight)

    confusion_matrix = np.zeros( (4,4))
    '''
              left|  right|   stop|nothing|
       left|      |       |       |       |
      right|      |       |       |       |
       stop|      |       |       |       |
    nothing|      |       |       |       |
    '''

    data = pd.read_csv(config["data_dir"])
    data = data.values
    num = len(data)

    for i in range(num):
        label = data[i][1] ## 
        inputdata = data[i][0].split(' ')
        inputdata = [float(item) for item in inputdata]
        inputdata = np.array(inputdata)

        predictions = net.forward(inputdata)
        predicted_labels = np.argmax(predictions, axis=0)
        #print(index_to_label[predicted_labels])

        if (label == 'l'):
            confusion_matrix[0][predicted_labels] += 1
        elif(label == 'r'):
            confusion_matrix[1][predicted_labels] += 1
        elif(label == 's'):
            confusion_matrix[2][predicted_labels] += 1
        elif(label == 'n'):
            confusion_matrix[3][predicted_labels] += 1
    

    print("result of: ",config["model_dir"])
    print("            left|   right|    stop| nothing|")
    print("   left|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[0][0],confusion_matrix[0][1],confusion_matrix[0][2],confusion_matrix[0][3]) )
    print("  right|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[1][0],confusion_matrix[1][1],confusion_matrix[1][2],confusion_matrix[1][3]) )
    print("   stop|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[2][0],confusion_matrix[2][1],confusion_matrix[2][2],confusion_matrix[2][3]) )
    print("nothing|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[3][0],confusion_matrix[3][1],confusion_matrix[3][2],confusion_matrix[3][3]) )






def run_quantize_inference():
    model = MotionNet()
    model.load_state_dict(torch.load(config["model_dir"], map_location="cpu"))

    p_weight = 0

    for i, params in enumerate(model.parameters()):
        ##print(params.shape)
        p = params.data.numpy()

        ## quantize
        min_value, max_value = np.min(p), np.max(p)
        int_bits = int(np.ceil(np.log2(max(abs(min_value), abs(max_value)))))
        dec_bits = 15 - int_bits
        p = np.round(p * (2 ** dec_bits))
        p = p/(2**dec_bits)
        ## quantize

        ## flatten
        if (i == 0):
            p_weight = p.flatten()
        else:
            p_weight = np.concatenate( (p_weight, p.flatten()) )

        ##print(p.size)

        
    net = Motionnet_Inference()



    index_to_label = {
        0: 'left',
        1: 'right',
        2: 'stop',
        3: 'none'
    }

    net = Motionnet_Inference()
   

    net.load_weights(p_weight)

    confusion_matrix = np.zeros( (4,4))
    '''
              left|  right|   stop|nothing|
       left|      |       |       |       |
      right|      |       |       |       |
       stop|      |       |       |       |
    nothing|      |       |       |       |
    '''

    data = pd.read_csv(config["data_dir"])
    data = data.values
    num = len(data)

    for i in range(num):
        label = data[i][1] ## 
        inputdata = data[i][0].split(' ')
        inputdata = [float(item) for item in inputdata]
        inputdata = np.array(inputdata)

        predictions = net.quantize_forward(inputdata)
        predicted_labels = np.argmax(predictions, axis=0)
        #print(index_to_label[predicted_labels])

        if (label == 'l'):
            confusion_matrix[0][predicted_labels] += 1
        elif(label == 'r'):
            confusion_matrix[1][predicted_labels] += 1
        elif(label == 's'):
            confusion_matrix[2][predicted_labels] += 1
        elif(label == 'n'):
            confusion_matrix[3][predicted_labels] += 1
    

    print("result of: ",config["model_dir"])
    print("            left|   right|    stop| nothing|")
    print("   left|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[0][0],confusion_matrix[0][1],confusion_matrix[0][2],confusion_matrix[0][3]) )
    print("  right|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[1][0],confusion_matrix[1][1],confusion_matrix[1][2],confusion_matrix[1][3]) )
    print("   stop|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[2][0],confusion_matrix[2][1],confusion_matrix[2][2],confusion_matrix[2][3]) )
    print("nothing|    %4d|    %4d|    %4d|    %4d|" % (confusion_matrix[3][0],confusion_matrix[3][1],confusion_matrix[3][2],confusion_matrix[3][3]) )






if __name__ == "__main__":
    #get_params()
    run_inference()
    