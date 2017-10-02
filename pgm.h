/**
 *  Advanced Programming in C
 *  Final Project
 *
 *  pgm.h
 *
 *  @author Daniel Cohen
 *  @author Tal Koren
 *  @version 1.0 20/06/2017
 */

#ifndef __PGM_H
#define __PGM_H

#define _CRT_SECURE_NO_WARNINGS

//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//CONSTS
#define TRUE 1
#define FALSE 0
#define BYTE_SIZE 8
#define MAX_FILE_NAME 50

//STRUCTS
typedef struct _grayImage {
    unsigned short rows, cols;
    unsigned char **pixels;
} grayImage;

typedef unsigned short imgPos[2];

typedef struct _treeNodeListCell treeNodeListCell; //Forward declaration

typedef struct _treeNode {
    imgPos position;
    treeNodeListCell *next_possible_positions;
} treeNode;

typedef struct _treeNodeListCell {
    treeNode * node;
    struct _treeNodeListCell *next;
} treeNodeListCell;

typedef struct _segment {
    treeNode *root;
} Segment;

typedef struct _imgPosCell {
    imgPos position;
    struct _imgPosCell *next, *prev;
} imgPosCell;

typedef struct _imgPosList {
    imgPosCell *head, *tail;
} imgPosList;

typedef int BOOL;

typedef unsigned char BYTE;

//PROTOTYPES

/**
    Prints the menu.

    @param void.
    @return void.
*/
void printMenu();

/**
    Allocates and initializes a 2D array for keeping track of the pixels, mainly for finding the segments.

    @param A gray image.
    @return void.
*/
void initChecker(grayImage *img);

/**
    Reads a PGM picture into the data structure.

    @param PGM picture filename.
    @return grayImage data structure containing the picture's pixels and number of rows and columns.
*/
grayImage *readPGM(char *fname);

/**
    Prints the PGM picture.

    @param A grayImage.
    @return void.
*/
void printPGM(grayImage * image);

/**
    Finds a signle segment inside the picture.

    @param A grayImage, a start pixel position, a threshold.
    @return The tree of the segments containing it's neighbors.
*/
Segment findSingleSegment(grayImage *img, imgPos start, unsigned char threshold);

/**
    Helper function for the fingSingleSegment function.

    @param A grayImage, a pixel, threshold and the original start point given.
    @return void.
*/
void findSegmentHelper(grayImage *img, treeNode *pixel, unsigned char threshold, imgPos start);

/**
    Creates a new tree node for the segments tree.

    @param A position of a pixel, x and y (row and col respectively).
    @return The tree node that has been created.
*/
treeNode *createNewTreeNode(int x, int y);

/**
    Finds all the segments of a picture.

    @param A grayImage, a threshold and an array of segments
    @return The number of segments (also the number of cells in the array)
*/
int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments);

/**
    Checks if the checker is already full.

    @param void.
    @return True or false respectively.
*/
BOOL isFullChecker();

/**
    Creates a new cell for the segments list.

    @param row and col - coordiantes of a pixel.
    @return A cell for the list.
*/
imgPosCell *createNewCell(int row, int col);

/**
    Checks if the list is empty.

    @param A list of imgPos.
    @return True or false respectively.
*/
BOOL isEmptyList(imgPosList *lst);

/**
    Inserts a node to the end of a list.

    @param A list of imgPos and the a pointer to the tail cell.
    @return void.
*/
void insertNodeToEndList(imgPosList *lst, imgPosCell *tail);

/**
    Inserts data of a pixel to the end of the list of imgPos.

    @param  A list of imgPos and the coordinates of the pixel
    @return void.
*/
void insertDataToEndList(imgPosList *lst,int row, int col);

/**
    Colors the picture according to the number of segments.

    @param An array of lists it's size (number of segments).
    @return A gray image data structure after chaging it's colors.
*/
grayImage *colorSegments (imgPosList *segments, unsigned int size);

/**
    Creates a compressed picture and saves it as a binary file

    @param A filename for the binary file, the grayImage, and max gray level for the compression.
    @return void.
*/
void saveCompressed(char *file_name, grayImage *image, unsigned char max_gray_level);

/**
    Counts the number of bits of the max gray level, needed for compression.

    @param The max level of gray.
    @return The number of bits of the max gray level.
*/
int countNumberOfBits(unsigned char num);

/**
    Compresses the bytes of the pixels.

    @param The pixels array, number of bits of the max gray level, size of the array of pixels, and size of the bytes array (after compression).
    @return The array of bytes after compression.
*/
BYTE *compressBytes(unsigned char *pixels, int numOfBits, int arrSize, int sizeOfBytes);

/**
    Converts a compressed picture into PGM format.

    @param Name of binary file containting a compressed picture, and the name for the decompressed PGM picture.
    @return void.
*/
void convertCompressedImageToPGM (char *compressed_file_name, char *pgm_file_name);

/**
    Decompresses the bytes array.

    @param The bytes array, number of bits of the max gray level, size of the array of bytes.
    @return The array of pixels after decompression.
*/
unsigned char *decompressBytes(BYTE *bytes, int numOfBits, int arrSize);

/**
    Finds the max gray level of a picture.

    @param An array of pixels and it's size.
    @return The max gray level of the array of pixels.
*/
int findMaxGrayLevel(unsigned char *pixels, int size);

/**
    Free the checker table.

    @param The number of rows of the checker.
    @return void.
*/
void freeChecker(int rows);

/**
    Frees the image data structure.

    @param A grayImage.
    @return void.
*/
void freeImage(grayImage *image);

/**
    Frees the array of positions.

    @param An array of lists of imgPos and it's size.
    @return void.
*/
void freePosListArr(imgPosList *array, int size);

/**
    Frees a list of imgPos.

    @param A list of imgPos.
    @return void.
*/
void freeList(imgPosList *lst);

/**
    Frees the tree of segment.

    @param A segment tree
    @return void.
*/
void freeTree(Segment * seg);

/**
    Helper function for freeTree

    @param treeNode
    @return void.
*/
void freeTreeHelper(treeNodeListCell *node);

/**
    Checks if malloc was succeeded or not.

    @param A pointer.
    @return void.
*/
void checkMalloc(void *ptr);

/**
    Checks if fopen was succeeded or not.

    @param A pointer to a file.
    @return void.
*/
void checkFileOpen(void *ptr);

//GLOBAL VARIABLES 

//2D table for keeping track when findging the segments
int **checker;

//Number of columns and rows of the picture that has been read into the program
int globalRows;
int globalCols;

#endif
