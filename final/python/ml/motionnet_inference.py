import sys
sys.path.append('./inference')

from layer import *

class Motionnet_Inference():
    def __init__(self):
        self.relu = ReLU()

        self.layer1 = Linear_self(24,64)
        self.layer2 = Linear_self(64,16)
        self.layer3 = Linear_self(16, 4)
        
        

        self.body = [
            self.layer1, self.layer2, self.layer3
        ]


    def forward(self, x):

        y1 = self.layer1.forward(x)
        y1 = self.relu.forward(y1)
        y2 = self.layer2.forward(y1)
        y2 = self.relu.forward(y2)
        y3 = self.layer3.forward(y2)
        

        
        return y3

    
    def quantize_forward(self, x):

        y1 = self.layer1.forward(x)
       

        min_value, max_value = np.min(y1), np.max(y1)
        int_bits = int(np.ceil(np.log2(max(abs(min_value), abs(max_value)))))
        dec_bits = 15 - int_bits
        y1 = np.round(y1 * (2 ** dec_bits))
        y1 = y1/(2**dec_bits)


        y1 = self.relu.forward(y1)


        y2 = self.layer2.forward(y1)


        min_value, max_value = np.min(y2), np.max(y2)
        int_bits = int(np.ceil(np.log2(max(abs(min_value), abs(max_value)))))
        dec_bits = 15 - int_bits
        y2 = np.round(y2 * (2 ** dec_bits))
        y2 = y2/(2**dec_bits)


        y2 = self.relu.forward(y2)




        y3 = self.layer3.forward(y2)


        min_value, max_value = np.min(y3), np.max(y3)
        int_bits = int(np.ceil(np.log2(max(abs(min_value), abs(max_value)))))
        dec_bits = 15 - int_bits
        y3 = np.round(y3 * (2 ** dec_bits))
        y3 = y3/(2**dec_bits)
        

        
        return y3



    def load_weights(self, weights):
        s = 0
        for layer in self.body:
            s += layer.load(weights[s:])        





