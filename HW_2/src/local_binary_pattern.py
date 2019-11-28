
def generate_lbp_histogram(image, row_count, column_count):
    histogram = init_histogram()    
    
    for i in range(row_count):
        for j in range(column_count):
            add_pixel_to_histogram(histogram, image, i, j)
    
    ## Hala uç caseleri dusunmedim
    ## Bunun dışında normalizasyon işlemi yapılması gerekiyor 
    
    return histogram
            

def init_histogram():
    red, green, blue = [0] * 256, [0] * 256, [0] * 256     
    return {'red': red, 'green': green, 'blue': blue }

def get_pixel(image ,row, column):
    red =  image[row][column][0]
    green = image[row][column][1]
    blue = image[row][column][2]
    return {'red': red, 'green': green, 'blue': blue}

def add_pixel_to_histogram(histogram, image, row, column):
    center_pixel = get_pixel(image, row, column)
    neighbours = extract_neighbour_pixels(image, row, column)
    encoding = generate_binary_encoding(neighbours, center_pixel)
    red, green, blue = binary_encoding_to_int(encoding)
    histogram['red'][red] += 1
    histogram['green'][green] += 1
    histogram['blue'][blue] += 1


## Happy path burası uç durumları düşünmek lazım ...
## Yani diyelim ki center pixel ya da center alacak pixel'in sol
## tarafında herhangi bir şey yok o zaman ne yapacağız ?    
    
"""
        
        Bir resmin pixeldekilerinin numaralandirilmasi
        bu numaralandırılma listedeki indisine denk düşer ve aynı
        zamanda ikinin kuvvetine denk düşer
    

            +---+---+---+
            | 0 | 1 | 2 |
            +---+---+---+
            | 3 |   | 4 |
            +---+---+---+
            | 5 | 6 | 7 |
            +---+---+---+
            


"""


def extract_neighbour_pixels(image, center_row, center_column):
    neighbour_pixels = []
    
    left_upper_pixel = get_pixel(image, center_row - 1, center_column - 1)
    center_upper_pixel = get_pixel(image, center_row - 1, center_column)
    right_upper_pixel = get_pixel(image, center_row - 1, center_column + 1)
    
    left_pixel = get_pixel(image, center_row, center_column - 1)
    right_pixel = get_pixel(image, center_row, center_column + 1)
    
    left_lower_pixel = get_pixel(image, center_row + 1, center_column - 1)
    center_lower_pixel = get_pixel(image, center_row + 1, center_column)
    right_lower_pixel = get_pixel(image, center_row + 1, center_column + 1)
    
    neighbour_pixels.append(left_upper_pixel)
    neighbour_pixels.append(center_upper_pixel)
    neighbour_pixels.append(right_upper_pixel)
    neighbour_pixels.append(left_pixel)
    neighbour_pixels.append(right_pixel)
    neighbour_pixels.append(left_lower_pixel)
    neighbour_pixels.append(center_lower_pixel)
    neighbour_pixels.append(right_lower_pixel)
    
    return neighbour_pixels

def generate_binary_encoding(neighbour_pixels, center_pixel):
    red_encoding = ""
    green_encoding = ""
    blue_encoding = ""
    
    for i in range(len(neighbour_pixels)):
        red_binary, green_binary, blue_binary = compare_neighbour_to_center(neighbour_pixels[i], center_pixel)
        
        red_encoding = red_encoding + red_binary
        green_encoding = green_encoding + green_binary
        blue_encoding = blue_encoding + blue_binary
    
    return {'red': red_encoding, 'blue': blue_encoding, 'green': green_encoding}
    
def compare_neighbour_to_center(neighbour, center):
     return "1" if center["red"] >= neighbour["red"]  else "0",
     "1" if center["green"] >= neighbour["green"] else "0",
     "1" if center["blue"] >= neighbour["blue"]   else "0"
           
def binary_encoding_to_int(encoding):
    red_int = 0
    green_int = 0
    blue_int = 0
    
    bit_length = len(encoding['red'])
    
    for i in range(bit_length):
        multiplier = 2 ** i
        red_int += encoding['red'][i] * multiplier
        blue_int += encoding['blue'][i] * multiplier
        green_int += encoding['green'][i] * multiplier
    
    return red_int, green_int, blue_int