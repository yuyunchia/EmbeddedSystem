import pandas as pd
import numpy as np

config = {
    "data_dir": "../../data/total_data.csv",
    "save_dir": "../../cpp/data.h"
}

def transformation():
    file = pd.read_csv(config["data_dir"])

    left = []
    right = []
    stop = []
    nothing = []
    
    for i in range(len(file)):
        data = file.iloc[i]["motion"].split()
        for j in range(len(data)):
            data[j] = int(data[j])
        label = file.iloc[i]["label"]

        if label == "l":
            left.extend(data)
        elif label == "r":
            right.extend(data)
        elif label == "s":
            stop.extend(data)
        else:
            nothing.extend(data)
    
    left = np.array(left)
    right = np.array(right)
    stop = np.array(stop)
    nothing = np.array(nothing)

    print("left size: %d" % (len(left) / 24))
    print("right size: %d" % (len(right) / 24))
    print("stop size: %d" % (len(stop) / 24))
    print("nothing size: %d" % (len(nothing) / 24))

    with open(config["save_dir"], "w+") as file:
        file.write("#define LEFT {")
        left.tofile(file, sep=", ", format="%d")
        file.write("}\n")
        file.write("#define RIGHT {")
        right.tofile(file, sep=", ", format="%d")
        file.write("}\n")
        file.write("#define STOP {")
        stop.tofile(file, sep=", ", format="%d")
        file.write("}\n")
        file.write("#define NOTHING {")
        nothing.tofile(file, sep=", ", format="%d")
        file.write("}\n")

if __name__ == "__main__":
    transformation()