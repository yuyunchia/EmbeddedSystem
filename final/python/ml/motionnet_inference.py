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

    def load_weights(self, weights):
        s = 0
        for layer in self.body:
            s += layer.load(weights[s:])        





