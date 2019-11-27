import cv2
from color_histogram import generate_color_histogram 
from local_binary_pattern import generate_lbp_histogram

def create_histograms(file_path):
    image, row, column = read_image(file_path)
    
    rgb_histogram = generate_color_histogram(image, row, column)
    lbp_histogram = generate_lbp_histogram(image, row, column)
    
    
    
    
def read_image(file_path):
    image = cv2.imread(file_path, cv2.IMREAD_COLOR)
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    return image, image.shape[0], image.shape[1]