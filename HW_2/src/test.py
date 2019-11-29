import os
import json

from histograms import extract_histograms

TEST_IMAGES_PATH = "../data/test"
HISTOGRAM_FILE_NAME = "histograms.json"
TRAIN_IMAGES_PATH = "../data/train"

if not os.path.exists(os.path.dirname(os.path.realpath(TEST_IMAGES_PATH) + "/" + HISTOGRAM_FILE_NAME)):
    extract_histograms(TRAIN_IMAGES_PATH, TRAIN_IMAGES_PATH, HISTOGRAM_FILE_NAME)


with open(TEST_IMAGES_PATH + "/" + HISTOGRAM_FILE_NAME) as json_file:
    images = json.load(json_file)['images']
    
len(images)




def calculate_manhattan_distance(histogram_1, histogram_2):
    manhattan_distance = 0
    
    
    