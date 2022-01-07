import torch
from torch.utils.data import Dataset
import pandas as pd

def map_label(label: str):
    if label == "l":
        return 0
    if label == "r":
        return 1
    if label == "s":
        return 2
    return 3


class MotionSet(Dataset):
    def __init__(self, data_dir, max=600.0) -> None:
        super().__init__()
        self.file = pd.read_csv(data_dir)
        self.max = max

    def __len__(self):
        return len(self.file)

    def __getitem__(self, index):
        data = self.file.iloc[index]["motion"].split()
        label = map_label(self.file.iloc[index]["label"])
        for i in range(len(data)):
            data[i] = float(data[i]) / self.max
        return torch.Tensor(data), label
