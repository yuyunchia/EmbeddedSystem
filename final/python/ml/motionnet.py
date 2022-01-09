import torch
from torch import nn

class MotionNet(nn.Module):
    def __init__(self, input_dim: int = 24, output_dim: int = 4):
        super().__init__()
        self.mlp = nn.Sequential(
            nn.Linear(input_dim, 64),
            nn.Dropout(0.3),
            nn.ReLU(inplace=True),
            nn.Linear(64, 16),
            nn.Dropout(0.3),
            nn.ReLU(inplace=True),
            nn.Linear(16, output_dim),
            nn.Softmax()
        )

    def forward(self, x):
        return self.mlp(x)