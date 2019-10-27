#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

typedef struct pixel Pixel;
typedef struct image Image;
typedef struct arguments Arguments;
typedef struct nodePixel NodePixel; 
typedef struct cluster Cluster;

struct arguments{
    char *imagePath;
    int k;
};

struct pixel {
    uchar red;
    uchar green;
    uchar blue;
};

struct image{
    Pixel **pixels;
    int rowCount;
    int columnCount;
};

struct nodePixel{
    Pixel *pixel;
    NodePixel *next;
};

struct cluster{ // bu cluster'daki elemanların hepsi merkezdeki renk ile ifade edilecek bunu unutmamak lazim !!
    NodePixel *root;
    int size;

    struct center{
        int red;
        int green;
        int blue;
    }center;
};

Arguments getArguments(int, char **);
void validateArgumentCount(int);
Arguments parseArguments(char **);
Image *imageToMatrix(char *);
Image *createPixelMatrix(Mat);
Pixel ** extractPixels(Mat);
Pixel **allocatePixelMatrix(int, int);
void segmentImage(Image *image, int);
Cluster **initializeClusters(int);
Cluster *createCluster(int);
void nullCheck(void *);

const uchar  MAX_COLOR_VALUE = 255;

int main(int argc, char *argv[]){
    Arguments args;
    Image *image;

    args = getArguments(argc, argv);
    image = imageToMatrix(args.imagePath);
    segmentImage(image, args.k);
    
    return 0;
}

Arguments getArguments(int argc, char *argv[]){
    Arguments args;
    validateArgumentCount(argc);
    return parseArguments(argv);
}

Arguments parseArguments(char *argv[]){
    Arguments args;   
       
    if(strcmp(argv[1], "-k") == 0){
        if(strcmp(argv[3], "-f") == 0){
            args.k = atoi(argv[2]);
            args.imagePath = argv[4];
        }
        else{
            printf("Second argument should be -f not %s !\n", argv[3]);
            printf("Example usage: \n");
            printf("./main.out -f image/path.jpg -k 12");
            exit(EXIT_FAILURE);
        }
    }
    else if(strcmp(argv[1], "-f") == 0){
        if(strcmp(argv[3], "-k") == 0){
            args.k = atoi(argv[4]);
            args.imagePath = argv[2];
        }
        else{
            printf("\nSecond argument should be -k not %s !\n", argv[3]);
            printf("Example usage: \n");
            printf("./main.out -k image/path.jpg -f 12");
            exit(EXIT_FAILURE);        
        }    
    }
    else{
        printf("\nFirst argument should be -k or -f not %s !\n", argv[1]);
        printf("Example usage: \n");
        printf("./main.out -k image/path.jpg -f 12");
        exit(EXIT_FAILURE);
    }

    return args;
}

void validateArgumentCount(int argc){
    if(argc != 5){
        printf("Argument count should be: 4 not: %d !", argc-1);
        printf("Example usage: \n");
        printf("./main.out -k image/path.jpg -f 12");
        exit(EXIT_FAILURE);
    }
}

Image *imageToMatrix(char *imagePath){
    Mat image = imread(imagePath, IMREAD_COLOR);

    if(image.empty()){
        printf("Image can't be read !\n");
        printf("Image path: %s", imagePath);
        exit(EXIT_FAILURE);
    }

    //imshow("arnold_schwarzenegger", image);
    waitKey(0);

    return createPixelMatrix(image);
}

Image *createPixelMatrix(Mat img){
    Image *image = (Image *) malloc(sizeof(Image));
    image->rowCount = img.rows;
    image->columnCount = img.cols;
    image->pixels = extractPixels(img);
    
    return image;
}

Pixel **extractPixels(Mat image){
    Pixel **pixels = allocatePixelMatrix(image.rows, image.cols);

    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            // OpenCV stores pixels as in BGR not RGB
            pixels[i][j].red = image.at<Vec3b>(i,j)[2];
            pixels[i][j].green = image.at<Vec3b>(i,j)[1];
            pixels[i][j].blue = image.at<Vec3b>(i,j)[0];
        }
    }

    return pixels;
}

Pixel **allocatePixelMatrix(int height, int width){
    Pixel **pixels = (Pixel **) malloc(height * sizeof(Pixel *));
    nullCheck(pixels);

    for(int i = 0; i < height; i++){
        pixels[i] = (Pixel *) malloc(width * sizeof(Pixel));
        nullCheck(pixels[i]);
    }

    return pixels;
}

void segmentImage(Image *image, int k){
    // k-means algorithm icin fonksiyon cagirilmali
    // ardindan da labelling kısmi yapılmalı
}

void clusterPixelsByColor(Pixel **pixels, int k){
    Cluster **clusters = initializeClusters(k);
}

Cluster **initializeClusters(int k){
    Cluster **clusters = (Cluster **) malloc(sizeof(Cluster *) * k);
    nullCheck(clusters);

    int inc = MAX_COLOR_VALUE / k;
    int value = -inc;

    for(int i = 0; i < k; i++){
        value += inc;
        clusters[i] = createCluster(value);
    }

    return clusters;
}

Cluster *createCluster(int value){
    Cluster *cluster = (Cluster *) malloc(sizeof(Cluster));
    nullCheck(cluster);

    cluster->root = NULL;
    cluster->size = 0;
    cluster->center.red = value;
    cluster->center.green = value;
    cluster->center.blue = value;

    return cluster;   
}

void nullCheck(void *variable){
    if(variable == NULL){
        printf("\nVariable is null !");
        printf("\nNot enough memory !");    
        exit(EXIT_FAILURE);
    }
}