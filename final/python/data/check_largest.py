import pandas as pd

config = {
    "csv_path": "../../data/total_data.csv"
}

if __name__ == "__main__":
    csv_file = pd.read_csv(config["csv_path"])
    print("Data num: %d" % len(csv_file))
    max = 0
    for i in range(len(csv_file)):
        data = csv_file.iloc[i]["motion"].split()
        for d in data:
            max = max if max > abs(int(d)) else abs(int(d))

    print("Max abs value: %d" % max)
