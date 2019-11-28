import os
import cv2
from color_histogram import generate_color_histogram 
from local_binary_pattern import generate_lbp_histogram
import mimetypes
import json

"""
    {
       'files': [
          {
             'path': ..., 
             'histogram': {
                 'rgb': ..., 
                 'lbp': ...
             }
          },                    
          {
             'path': ..., 
             'histogram': {
                 'rgb': ..., 
                 'lbp': ...
             }
          },
          ...
       ]
    }
"""


def extract_histograms(source_path, destination_path):
    image_histograms = create_histograms_for_all_images(source_path)
    with open(source_path + "/histograms.json") as json_file:
        json.dump(image_histograms, json_file)

def create_histograms_for_all_images(folder_path):
    image_histograms = {'images': [] }
    image_paths = get_all_file_paths(folder_path)
    
    for i in range(len(image_paths)):
        histograms = create_histograms(image_paths[i])
        
        image_histograms['images'].append({
            'path': image_paths[i],
            'histograms': histograms
        })
    
    return image_histograms    
        
def get_all_file_paths(folder_path):
    file_paths = []
    
    for root, directories, files in os.walk(folder_path):
        for file in files:
            if is_file_image(file):
                file_paths.append(root + "/" + file)
    
    return file_paths

def is_file_image(file):
    mimetype = mimetypes.guess_type(file)[0]
    
    if mimetype is None:
        return False
    
    return "image" in mimetype

def create_histograms(file_path):
    image, row, column = read_image(file_path)
    
    rgb_histogram = generate_color_histogram(image, row, column)
    lbp_histogram = generate_lbp_histogram(image, row, column)
    
    return { 'rgb':rgb_histogram, 'lbp': lbp_histogram }
    
def read_image(file_path):
    image = cv2.imread(file_path, cv2.IMREAD_COLOR)
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    return image, image.shape[0], image.shape[1]
