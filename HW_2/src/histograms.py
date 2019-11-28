import os
import cv2
from color_histogram import generate_color_histogram 
from local_binary_pattern import generate_lbp_histogram

#def extract_histograms_from(folder_path):
    

def get_all_file_paths(folder_path):
    file_paths = []
    
    for root, directories, files in os.walk(folder_path):
        for file in files:
            file_paths.append(file)
    
    return file_paths
    
def create_histograms(file_path):
    image, row, column = read_image(file_path)
    
    rgb_histogram = generate_color_histogram(image, row, column)
    lbp_histogram = generate_lbp_histogram(image, row, column)
    
    return {
            'path': file_path, 
            'histogram': { 'rgb':rgb_histogram, 'lbp': lbp_histogram }
    }
    
    
    
def read_image(file_path):
    image = cv2.imread(file_path, cv2.IMREAD_COLOR)
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    return image, image.shape[0], image.shape[1]


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