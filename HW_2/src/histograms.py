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


def extract_histograms(source_path, destination_folder, file_name = "histograms.json"):
    image_histograms = create_histograms_for_all_images(source_path)
    
    if not os.path.exists(destination_folder):
        os.makedirs(destination_folder)
    
    with open(destination_folder + "/" + file_name, "w") as json_file:
        json.dump(image_histograms, json_file, indent = 2)

def create_histograms_for_all_images(folder_path):
    image_histograms = {'images': [] }
    image_paths = get_all_file_paths(folder_path)
    
    for i in range(len(image_paths)):
        print(i)
        
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
    
def read_image_rgb(file_path):
    return read_image(file_path, cv2.BGR2RGB)


def read_image_grayscale(file_path):
    return read_image(file_path, cv2.BGR2GRAY)
    
def read_image(file_path, read_flag):
    image = cv2.imread(file_path, cv2.IMREAD_COLOR)
    image = cv2.cvtColor(image, read_flag)
    return image, image.shape[0], image.shape[1]