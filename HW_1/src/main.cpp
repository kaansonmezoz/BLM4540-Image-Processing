#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <limits.h>

using namespace cv;

typedef struct pixel Pixel;
typedef struct image Image;
typedef struct arguments Arguments; 
typedef struct cluster Cluster;
typedef struct center Center;
typedef struct label Label;

struct arguments{
    char *imagePath;
    int k;
};

struct pixel {
    uchar *red;
    uchar *green;
    uchar *blue;

    int clusterIndex;
    Label *label;   // pixellerin label'ları aynı olmalı
};

struct image{
    Pixel **pixels;
    int rowCount;
    int columnCount;
};

struct center{
    double red;
    double green;
    double blue;
};

struct cluster{
    int size;
    Center center;
};

struct label{
    Pixel **pixels; // linkli liste olacak; eklemeler başa yapılacak 
    
    struct {
        uchar red;
        uchar gree;
        uchar blue;
    }color; // bunun değerleri en son belli olacak.
};

Arguments getArguments(int, char **);
void validateArgumentCount(int);
Arguments parseArguments(char **);
Mat readImage(char *);
Image *createPixelMatrix(Mat);
Pixel ** extractPixels(Mat);
Pixel **allocatePixelMatrix(int, int);
void segmentImage(Mat, int);
void clusterPixelsByColor(Image *, int);
Cluster **initializeClusters(int);
Cluster *createCluster(int);
void nullCheck(void *);
double calculateEuclideanDistance(Center, Pixel *);
int findClusterForPixel(Cluster **, int, Pixel *);
void labelPixels(Cluster **, int);
void updateClusterCenter(Cluster *, Pixel *);
double calculateMeanColor(double, uchar, int);
int areCentersChangesLessThanThreshold(Cluster **, Center **, int);
double calculateCenterDifference(Center, Center);
void setClusterPreviousCenters(Center **, Cluster **, int);
void updatePixelColors(Image *, Cluster **);

const uchar  MAX_COLOR_VALUE = 255;
const double DELTA_THRESHOLD = 0.000005;

int main(int argc, char *argv[]){
    Arguments args;
    Image *imageMatrix;
    Mat image;


    args = getArguments(argc, argv);   
    image = readImage(args.imagePath);
    imshow("original", image);
    segmentImage(image, args.k); 
    
    waitKey(0);

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
        printf("Argument count should be: 4 not: %d !\n", argc-1);
        printf("Example usage: \n");
        printf("./main.out -f image/path.jpg -k 12\n\n");
        exit(EXIT_FAILURE);
    }
}

Mat readImage(char *imagePath){
    Mat image = imread(imagePath, IMREAD_COLOR);

    if(image.empty()){
        printf("Image can't be read !\n");
        printf("Image path: %s", imagePath);
        exit(EXIT_FAILURE);
    }

    return image;
}

Image *createPixelMatrix(Mat img){
    Image *image = (Image *) malloc(sizeof(Image));
    
    nullCheck(image);

    image->rowCount = img.rows;
    image->columnCount = img.cols;
    image->pixels = extractPixels(img);
    
    return image;
}

void nullCheck(void *variable){
    if(variable == NULL){
        printf("\nVariable is null !");
        printf("\nNot enough memory !");    
        exit(EXIT_FAILURE);
    }
}

Pixel **extractPixels(Mat image){
    Pixel **pixels = allocatePixelMatrix(image.rows, image.cols);

    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            // OpenCV stores pixels as in BGR not RGB
            pixels[i][j].red = &image.at<Vec3b>(i,j)[2];
            pixels[i][j].green = &image.at<Vec3b>(i,j)[1];
            pixels[i][j].blue = &image.at<Vec3b>(i,j)[0];
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

void segmentImage(Mat image, int k){
    Image *imageMatrix = createPixelMatrix(image);
    clusterPixelsByColor(imageMatrix, k);
    imshow("kMeans", image);

    // ardindan da labelling kısmi yapılmalı
}

void clusterPixelsByColor(Image *image, int k){
    int clusterIndex, i, j;
    Pixel *pixel;

    Center **oldCenters = (Center **) malloc(sizeof(Center *) * k);
    nullCheck(oldCenters);

    for(int i = 0; i < k; i ++){
        oldCenters[i] = (Center *) malloc(sizeof(Center));
        oldCenters[i]->blue = 0;
        oldCenters[i]->red = 0;
        oldCenters[i]->green = 0;
    }

    Cluster **clusters = initializeClusters(k);

    do{
        setClusterPreviousCenters(oldCenters, clusters, k);
        for(i = 0; i < image->rowCount; i++){
            for(j = 0; j < image->columnCount; j++){
                pixel = &(image->pixels[i][j]);
                clusterIndex = findClusterForPixel(clusters, k, pixel);        
                //addPixelToCluster(clusters[clusterIndex], pixel);
                pixel->clusterIndex = clusterIndex;
                updateClusterCenter(clusters[clusterIndex], pixel);
            }
        }
    }while(!areCentersChangesLessThanThreshold(clusters, oldCenters, k));

    updatePixelColors(image, clusters);
    
    //labelPixels(clusters, k);
}

int areCentersChangesLessThanThreshold(Cluster **clusters, Center **oldCenters, int k){
    double delta;
        
    for(int i = 0; i < k; i++){
        delta = calculateCenterDifference(clusters[i]->center, *oldCenters[i]);

        if(delta > DELTA_THRESHOLD){
            return 0;
        }
    }

    return 1;
}

double calculateCenterDifference(Center currentCenter, Center oldCenter){
    double redDifference = currentCenter.red - oldCenter.red;
    double blueDifference = currentCenter.blue - oldCenter.blue;
    double greenDifference = currentCenter.green - oldCenter.green;

    return sqrt((redDifference * redDifference) + 
                (blueDifference * blueDifference) + 
                (greenDifference * greenDifference)
    );
}

void setClusterPreviousCenters(Center **oldCenters, Cluster **clusters, int k){
    for(int i = 0; i < k; i++){
        oldCenters[i]->red = clusters[i]->center.red;
        oldCenters[i]->green = clusters[i]->center.green;
        oldCenters[i]->blue = clusters[i]->center.blue;
        clusters[i]->size = 1;
    }    
}

Cluster **initializeClusters(int k){
    Cluster **clusters = (Cluster **) malloc(sizeof(Cluster *) * k);
    nullCheck(clusters);

    int inc = MAX_COLOR_VALUE / k;
    int value = 0;

    for(int i = 0; i < k; i++){        
        clusters[i] = createCluster(value);
        value += inc;
    }

    return clusters;
}

Cluster *createCluster(int value){
    Cluster *cluster = (Cluster *) malloc(sizeof(Cluster));
    nullCheck(cluster);

    cluster->size = 1;
    cluster->center.red = value;
    cluster->center.green = value;
    cluster->center.blue = value;

    return cluster;   
}

int findClusterForPixel(Cluster **clusters, int k, Pixel *pixel){
    long double distanceToCenter = 0;
    int closestCluster = -1;
    long double minDistance = INT_MAX;

    for(int i = 0; i < k; i++){
        distanceToCenter = calculateEuclideanDistance(clusters[i]->center, pixel);

        if(minDistance >= distanceToCenter){
            closestCluster = i;
            minDistance = distanceToCenter;
        }
    }

    return closestCluster;
}

double calculateEuclideanDistance(Center center, Pixel *pixel){
    double redDistance = center.red - *(pixel->red);
    double greenDistance = center.green - *(pixel->green);
    double blueDistance = center.blue - *(pixel->blue);

    return sqrt((redDistance * redDistance) + 
                (greenDistance * greenDistance) + 
                (blueDistance * blueDistance)
    );
}

void updateClusterCenter(Cluster *cluster, Pixel *pixel){    
    double totalRed = (cluster->center.red * cluster->size);
    double totalGreen = (cluster->center.green * cluster->size);
    double totalBlue = (cluster->center.blue * cluster->size);
    
    cluster->size++;
    cluster->center.red = calculateMeanColor(totalRed, *(pixel->red), cluster->size);
    cluster->center.green = calculateMeanColor(totalGreen, *(pixel->green), cluster->size);
    cluster->center.blue = calculateMeanColor(totalBlue, *(pixel->blue), cluster->size);
}

double calculateMeanColor(double total, uchar newColor, int newSize){
    return (total + newColor) / newSize;
}


void updatePixelColors(Image *image, Cluster **clusters){
    Pixel *pixel;
    int clusterIndex;
    
    for(int i = 0; i < image->rowCount; i++){
        for(int j = 0; j < image->columnCount; j++){
            pixel = &(image->pixels[i][j]);
            clusterIndex = pixel->clusterIndex;
            *(pixel->red) = clusters[clusterIndex]->center.red;
            *(pixel->green) = clusters[clusterIndex]->center.green;
            *(pixel->blue) = clusters[clusterIndex]->center.blue;
        }
    }
}



//TODO: free kısımları var bunları atlamamak lazim