import os
import csv

config = {
    "total_data_dir": "../../data/total_data.csv",
    "merge_path": "../../data"
}

if __name__ == "__main__":
    with open(config["total_data_dir"], "w") as csv_file:
        tag = ["motion", "label"]
        writer = csv.DictWriter(csv_file, fieldnames=tag, quoting=csv.QUOTE_NONE)
        writer.writeheader()

        merge_files = os.listdir(config["merge_path"])
        for file in merge_files:
            if ".txt" in file:
                file_path = os.path.join(config["merge_path"], file)
                print("Merging %s" % file_path)
                with open(file_path, "r") as txt_file:
                    lines = txt_file.readlines()
                    for l in lines:
                        _l = l.split()
                        data = _l[: 24]
                        label = _l[-1]
                        if label in "rlsn":
                            writer.writerow({
                                "motion": " ".join(data),
                                "label": label
                            })
                    txt_file.close()
        csv_file.close()