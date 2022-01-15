
import numpy as np

##from .npconv2d import conv2d

class Layer:
    name = 'layer'
    def __init__(self, name):
        self.name = name

    def forward(self, x): pass

    def backward(self, grad_y): pass

    def para(self): return None

    def load(self, buf): return 0

    def __call__(self, x):
        return self.forward(x)

class Dense_self(Layer):
    name = 'dense_self'
    def __init__(self, c, n):
        self.K = np.zeros((c, n), dtype=np.float32)
        self.bias = np.zeros(n, dtype=np.float32)

    def para(self): return self.K.shape

    def forward(self, x):
        y = x.dot(self.K)
        y += self.bias.reshape((1, -1))
        return y

    def load(self, buf):
        sk, sb = self.K.size, self.bias.size
        self.K.ravel()[:] = buf[:sk]
        self.bias.ravel()[:] = buf[sk:sk+sb]
        return sk + sb


class Conv2d_self(Layer):
    name = 'conv2d_self'
    """
    c: in_channels
    n: out_channels
    w: kernel_size
    g: groups
    s: stride
    d: dilation
    """
    def __init__(self, c, n, kernel_size=(1,1), padding = "VALID",g=1, strides=(1,1), d=1):
        self.n, self.c = n, c
        self.g, self.d = g, d
        self.kernel_size = kernel_size
        self.strides = strides
        self.padding = padding

        self.real_n = int(self.n/self.g)
        
        self.K = np.zeros((kernel_size[0], kernel_size[1], c, n), dtype=np.float32)
        # self.K = np.zeros((n, c, kernel_size[0], kernel_size[1]), dtype=np.float32)
        # self.bias = np.zeros(n, dtype=np.float32)
    # def para(self): 
    #     return self.n, self.c, self.s, self.d  
    
    def forward(self, x):
        out = conv2d(x, self.K,pad=self.padding , stride=self.strides)
        #out = conv_self(x, self.K, self.g, (self.strides[0], self.strides[1]), (self.d, self.d))
        # out += self.bias.reshape((1, -1, 1, 1))
        return out 
    def load(self, buf):
        sk = self.K.size
        self.K.ravel()[:] = buf[:sk]
        # self.bias.ravel()[:] = buf[sk:sk+sb]
        return sk



class DepthwiseConv2d_self(Layer):
    name = 'depthwiseconv2d_self'
    """
    c: in_channels
    n: out_channels
    w: kernel_size
    g: groups
    s: stride
    d: dilation
    """
    def __init__(self, c, kernel_size=(1,1), padding = "VALID", strides=(1,1), d=1):
        self.c = c
        self.d = d
        self.kernel_size = kernel_size
        self.strides = strides
        self.padding = padding

        self.K_list = np.zeros(  (kernel_size[0], kernel_size[1], c, 1, 1), dtype=np.float32)

       

    # def para(self): 
    #     return self.n, self.c, self.s, self.d  
    
    def forward(self, x):
        Depthinput = x[:,:,:,0]
        Depthinput= np.expand_dims(Depthinput,axis = 3)
        out = conv2d(Depthinput, self.K_list[:,:,0,:,:] ,pad=self.padding, stride=self.strides)
        for channelindex in range(1,self.c):
            Depthinput = x[:,:,:,channelindex]
            Depthinput= np.expand_dims(Depthinput,axis = 3)
            Depthout = conv2d(Depthinput, self.K_list[:,:,channelindex,:,:],pad=self.padding, stride=self.strides)
            out= np.concatenate((out,Depthout),axis = 3)
        

        return out
    
    
    def load(self, buf):
        sk = self.K_list.size
        self.K_list.ravel()[:] = buf[:sk]
        # self.bias.ravel()[:] = buf[sk:sk+sb]
        return sk



class ReLU(Layer):
    name = 'relu'
    def __init__(self):pass

    def forward(self, x):
        return (x > 0) * x

class ReLU_self(Layer):
    name = 'relu_self'
    def __init__(self,max_value):
        self.max_value = max_value
        pass

    def forward(self, x):
        y = (x > 0) * x
        z = y - self.max_value
        z = (z < 0) * z
        z = z + self.max_value
        return z

class Flatten(Layer):
    name = 'flatten'
    def __init__(self):pass

    def forward(self, x):
        return x.reshape((1, -1))

class Sigmoid(Layer):
    name = 'sigmoid'
    def __init__(self):pass

    def forward(self, x):
        return 1/(1 + np.exp(-x))

class Softmax(Layer):
    name = 'softmax'
    def __init__(self, axis=-1):
        self.axis = axis

    def forward(self, x):
        eX = np.exp((x.T - np.max(x, axis=self.axis)).T)
        return (eX.T / eX.sum(axis=self.axis)).T


class GlobalAveragePool_self(Layer):
    name = 'gap_self'
    def __init__(self): pass    

    def forward(self, x):
        return x.mean(axis=(1, 2))

class Concatenate(Layer):
    name = 'concat'
    def __init__(self): pass

    def forward(self, x):
        return np.concatenate(x, axis=1)

class Add(Layer):
    name = 'add'
    def __init__(self): pass

    def forward(self, x):
        return sum(x)
        
class BatchNorm_self(Layer):
    name = 'batchnorm_self'
    def __init__(self, c):
        self.c = c
        # self.k = np.zeros(c, dtype=np.float32)
        self.b = np.zeros(c, dtype=np.float32)
        self.m = np.zeros(c, dtype=np.float32)
        self.v = np.zeros(c, dtype=np.float32)
    
    def forward(self, x):
        x = (x - self.m.reshape(1, 1, 1, self.c))
        # x = (x - self.m.reshape(1, -1, 1, 1))
        #x /= np.sqrt(self.v.reshape(1, 1, 1, self.c) + 0.001)
        x /= np.sqrt(self.v.reshape(1, 1, 1, self.c) + 0.001)


        #x /= np.sqrt(self.v.reshape(1, 1, 1, self.c) )
        #x /= np.sqrt(self.v.reshape(1, 1, 1, self.c) )
        # x *= self.k.reshape(1, -1, 1, 1) 

        x += self.b.reshape(1, 1, 1, self.c)
        return x

    def load(self, buf):
        c = self.c
        # self.k[:] = buf[0*c:1*c]
        self.b[:] = buf[0*c:1*c]
        self.m[:] = buf[1*c:2*c]
        self.v[:] = buf[2*c:3*c]
        return self.c * 3

        
class Linear_self(Layer):
    '''
    c: in_channels
    n: out_channels
    w: weight >> (n,c)

    '''
    name = 'linear_self'
    def __init__(self, c,n):
        self.c = c
        self.n = n
        self.w = np.zeros((n,c), dtype=np.float32) 
        self.b = np.zeros(n, dtype=np.float32)
    
    def forward(self, x):

        x = np.dot(self.w.reshape(self.n,self.c), x )
        x += self.b
       

        ##x += self.b.reshape(1, 1, 1, self.c)
        return x

    def load(self, buf):
        w = self.c * self.n
        # self.k[:] = buf[0*c:1*c]
        self.w.ravel()[:] = buf[ 0 : w]
        self.b.ravel()[:] = buf[ w : w+self.n]
        return w + self.n

# layer_map = {'dense':Dense, 'conv':Conv2d, 'relu':ReLU, 'batchnorm':BatchNorm,
#     'flatten':Flatten, 'sigmoid':Sigmoid, 'softmax': Softmax, 'maxpool': Maxpool, 'avgpool': Avgpool, 
#     'upsample':UpSample, 'concat':Concatenate, 'add':Add, 'gap':GlobalAveragePool}

if __name__ == "__main__":
    pass
