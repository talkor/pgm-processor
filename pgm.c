/**
 *  Advanced Programming in C
 *  Final Project
 *
 *  pgm.c
 *
 *  @author Daniel Cohen
 *  @author Tal Koren
 *  @version 1.0 20/06/2017
 */

#include "pgm.h"

void printMenu() {
    
    printf("\nPlease choose one of the options:\n\n");
         
    printf("1. Read an image file in PGM format\n");
    printf("2. Find all segments\n");
    printf("3. Color the segments\n");
    printf("4. Save the colored image in a compressed format\n");
    printf("5. Compress and save the original image in a compressed format\n");
    printf("6. Convert a compressed image to PGM format\n");
    printf("7. Exit\n");
    printf("\nPlease choose an option: ");     
}

void initChecker(grayImage *img) {

	int row, col;

	//Create a 2D table containing zeroes
    checker = (int**) malloc(sizeof(int*) * img->rows);
    checkMalloc(checker);

	for(row = 0; row < img->rows; row++) {
    	checker[row] = (int*) malloc(img->cols * sizeof(int));
        checkMalloc(checker[row]);

        for(col = 0; col < img->cols; col++){
        	checker[row][col] = 0;
        }
    }
}

grayImage *readPGM(char *fname) {
    
    grayImage *image = NULL;
    FILE *file;
    int ch;
    unsigned short row,col;
    unsigned short grayLevel;
    
    file = fopen(fname, "r");
    checkFileOpen(file);
    
    //Get rows and cols
    fscanf(file, "P2\n%hd%hd", &col,&row);
    
    //Malloc for the image
    image = (grayImage*) malloc(sizeof(grayImage));
    checkMalloc(image);
   	image->pixels = (unsigned char**)malloc(row * sizeof(unsigned char*));
    checkMalloc(image->pixels);
    
   	image->rows = row;
   	image->cols = col;

    //Assign rows and cols into global variables for future use (Color segments)
    globalRows = row;
    globalCols = col;

    //Skip the max gray level
    fscanf(file, "%hd", &grayLevel);

    //Get the pixels
    for(row = 0; row < image->rows; row++) {
        image->pixels[row] = (unsigned char*) malloc(col * sizeof(unsigned char));
        checkMalloc(image->pixels[row]);
        for(col = 0; col < image->cols; col++){
        	fscanf(file, "%d", &ch);
            if (ch != ' ' && ch != '\n')
                image->pixels [row][col] = ch;
            else
                col--;
        }
    }

    //Init the checker - a 2D table for keeping track of the segments
	initChecker(image);

    fclose(file);
    return image;
}

void printPGM(grayImage * image) {

    unsigned short row = image->rows;
    unsigned short col = image->cols;

    for(row = 0; row < image->rows; row++){
        for(col = 0; col < image->cols; col++){
            printf("%3d ", image->pixels[row][col]);
        }
        printf("\n");
    }
}

Segment findSingleSegment(grayImage *img, imgPos start, unsigned char threshold) {
    
    Segment segment;
    int row,col;
    
    segment.root = (treeNode*) malloc(sizeof(treeNode));
    checkMalloc(segment.root);
    
    //Assign the start value
    segment.root->position[0] = start[0];
    segment.root->position[1] = start[1];
    
    //Update checker with 1 for 'start'
    checker[start[0]][start[1]] = 1;
    
    //Helper function
    findSegmentHelper(img, segment.root, threshold, start);
    
    return segment;
}

void findSegmentHelper (grayImage *img, treeNode *pixel, unsigned char threshold, imgPos start) {
    
    int row,col;
    int startRow, startCol, endRow, endCol;
    treeNode *tr;
    treeNodeListCell *lst;
    treeNodeListCell *prevList = NULL;
    
    //Find the start and end positions to find the neighbors
    startRow = pixel->position[0] - 1;
    startCol = pixel->position[1] - 1;
    endRow = pixel->position[0] + 1;
    endCol = pixel->position[1] + 1;
    
    //Find the neighbors recursively
    for (row = startRow; row <= endRow; row++)
        for (col = startCol; col <= endCol; col++)
            if (row != pixel->position[0] || col != pixel->position[1])
                if (row >= 0 && row <= img->rows - 1 && col >= 0 && col <= img->cols - 1)
                    if (checker[row][col] == 0) {
                        if (row != start[0] || col != start[1])
                            if ((abs(img->pixels[row][col] - img->pixels[start[0]][start[1]]) <= threshold)){
                                
                                //Update checker with 1
                                checker[row][col] = 1;
                                
                                lst = (treeNodeListCell*) malloc(sizeof(treeNodeListCell));
                                checkMalloc(lst);
                                
                                tr = createNewTreeNode(row, col);
                                
                                //Update the list values
                                lst -> node = tr;
                                lst -> next = NULL;
                                
                                findSegmentHelper(img, lst->node, threshold, start);
                                
                                //Add a neighbor to an existing list or create a new one
                                if (prevList != NULL) {
                                    prevList->next = lst;
                                    prevList = lst;
                                } else {
                                    pixel->next_possible_positions = lst;
                                    prevList = lst;
                                }
                            }
                    }
}

treeNode *createNewTreeNode(int x, int y) {
    
    treeNode *res;
    
    res = (treeNode*) malloc(sizeof(treeNode));
    checkMalloc(res);
    
    res->position[0] = x;
    res->position[1] = y;
    
    res->next_possible_positions = NULL;
    
    return res;
}

int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments) {
    
    Segment seg;
    imgPos pos = {5,3}; //Random position
    int row, col;
    int numOfSegments = 0;
    int i, k;
    
    imgPosList *arr;
    
    arr = (imgPosList*)malloc(10 * sizeof(imgPosList));
    checkMalloc(arr);
    
    //While there are pixels with no sigments
    while(!isFullChecker()) { 
        
        //Call func to update checker table and find lonely segment position
        seg = findSingleSegment(img, pos, threshold);
        
        //Create a list in each element in array
        arr[numOfSegments].head = arr[numOfSegments].tail = NULL;
      
        for(row = 0; row < img->rows; row++){      
        	//Set row
            pos[0] = row;

            for(col = 0; col < img->cols; col++){
                //Set col
                pos[1] = col;
                
                if(checker[row][col] == 1){ //Means that this position is part of the segment
                    
                    //Insert cell to list
                    insertDataToEndList(&arr[numOfSegments], row, col);

					//Assign 2 to every pixel that already joined a segment
                    checker[row][col] = 2;
                }    
            }
        }

        numOfSegments++;
        
        for(i = 0; i < img->rows; i++)
            for(k = 0; k < img->cols; k++){
                if(checker[i][k] == 0){
                    pos[0] = i;
                    pos[1] = k;
                }
            }

        freeTree(&seg);
    }
    
    *segments = arr;
    return numOfSegments;
}

BOOL isFullChecker(){
    
    int row, col;

    for(row = 0; row < globalRows; row++)
        for(col = 0; col < globalCols; col++) 
            if(checker[row][col] == 0) 
               return FALSE;
            
    return TRUE;
}

imgPosCell *createNewCell(int row, int col) {
    
    imgPosCell *res;
    
    res = (imgPosCell*) malloc(sizeof(imgPosCell));
    checkMalloc(res);

    res->position[0] = row;
    res->position[1] = col;
    
    res->next = NULL;
    res->prev = NULL;
    
    return res;
}

BOOL isEmptyList(imgPosList *lst){
    
    if (lst->head == NULL)
        return TRUE;

    return FALSE;
}

void insertNodeToEndList(imgPosList *lst, imgPosCell *tail) {
    
    if (isEmptyList(lst) == TRUE){
        lst->head = lst->tail = tail;
        tail->next = tail->prev = NULL;
    } else {
        tail->next = NULL;
        tail->prev = lst->tail;
        lst->tail->next = tail;
        lst->tail = tail;
    }
}

void insertDataToEndList(imgPosList *lst, int row, int col) {
    
    imgPosCell *newTail;

    newTail = createNewCell(row, col);
    insertNodeToEndList(lst, newTail);
}

grayImage *colorSegments(imgPosList *segments, unsigned int size) {
    
    grayImage *newImage = NULL;
    unsigned short row,col;
    int grayLevel;
    int i;
    imgPosCell *curr;

    //Find the number of rows and cols
    row = globalRows; 
    col = globalCols;
    
    //Malloc for image->pixels
    newImage = (grayImage*) malloc(sizeof(grayImage));
    checkMalloc(newImage);

   	newImage->pixels = (unsigned char**)malloc(row * sizeof(unsigned char*));
    checkMalloc(newImage->pixels);
    
    for (i = 0; i < row; i++) {
    	newImage->pixels[i] = (unsigned char*)malloc(col * sizeof(unsigned char));
        checkMalloc(newImage->pixels[i]);
    }
    
    //Assign rows and cols
    newImage->rows = row;
   	newImage->cols = col;
    
    //Update new levels of gray using the list of segments
    for (i = 0; i < size; i++) {
        grayLevel = (int) ((i * 255) / (size - 1));        
        curr = segments[i].head;
        
        while (curr != NULL) {
        	newImage->pixels[curr->position[0]][curr->position[1]] = grayLevel;
        	curr = curr->next;
        }
    }
    
    return newImage;
}

void saveCompressed(char *file_name, grayImage *image, unsigned char max_gray_level) {
    
    FILE *file;
    int row, col;
    BYTE *bytes;
    unsigned char *pixels;
    int sizeOfBytes, numOfBits;
    int i, j, k = 0;

    //Create array of pixels to store the pixels of the picture in a row (flatten the 2D array)
    pixels = (unsigned char*) malloc((image->rows * image->cols) * sizeof(unsigned char));
    checkMalloc(pixels);
    
    //Store the values inside the pixels array with the new gray level
    for(row = 0; row < image->rows; row++){
        for(col = 0; col < image->cols; col++){
            *(pixels + (row * image->cols) + col) = (int) (image->pixels[row][col]) * (max_gray_level)/(255);
        }
    }

    //Create the new bin file
    file = fopen(file_name, "wb");
    checkFileOpen(file);

    //Insert row&col to file
    fwrite(&image->rows, sizeof(unsigned short), 1, file);
    fwrite(&image->cols, sizeof(unsigned short), 1, file);
    
    //Insert max grey level to file
    fwrite(&max_gray_level, sizeof(BYTE), 1, file);

    //Count the number of bits of the max_gray_level
    numOfBits = countNumberOfBits(max_gray_level);
   
    //Counts number of bytes required for the compressed pixels
    sizeOfBytes = (image->rows * image->cols * numOfBits) / BYTE_SIZE;

    //Compress the bytes
    bytes = compressBytes(pixels, numOfBits, image->cols * image->rows, sizeOfBytes);

    //Write the bytes to the binary file
    for (i = 0; i < sizeOfBytes; i++) { 
        fwrite(&bytes[i], sizeof(BYTE), 1, file);
    }

    free(pixels);
    free(bytes);

    fclose(file);    
}

int countNumberOfBits(unsigned char num) {

    int counter = 0;
    int i;

    while (num != 0) {        
        num = num >> 1;
        counter++;
    }

    return counter;
}

BYTE *compressBytes(unsigned char *pixels, int numOfBits, int arrSize, int sizeOfBytes) {
    
    int i,j;
    BYTE pixelsMask;
    int bytesBitCounter, pixelsBitCounter;
    BYTE *bytes;

    bytes = (BYTE*) malloc((sizeOfBytes) * sizeof(BYTE));
    checkMalloc(bytes);

    //Init the array
    for (i = 0; i < sizeOfBytes; i++) { 
        bytes[i] = 0;
    }

    i = 0; // cell number in the pixels array
    j = 0; // cell number in the bytes array

    pixelsBitCounter = numOfBits - 1;
    bytesBitCounter = BYTE_SIZE - 1;
        
    while (i < arrSize) {

        pixelsMask = 1 << pixelsBitCounter; //Create a mask for this bit
        pixelsMask = pixels[i] & pixelsMask;// Grab the bit

        if (pixelsBitCounter < bytesBitCounter)
            pixelsMask <<= bytesBitCounter - pixelsBitCounter;
        else
            pixelsMask >>= pixelsBitCounter - bytesBitCounter;

        //Take a bit from pixels and put inside bytes array
        bytes[j] = bytes[j] | (pixelsMask);
        
        pixelsBitCounter--;
        bytesBitCounter--;

        if (pixelsBitCounter < 0) {
            pixelsBitCounter = numOfBits - 1;

            //Move to the next cell of pixels
            i++;
        }

        if (bytesBitCounter <  0) {
            bytesBitCounter = BYTE_SIZE - 1;
            
            //Move to the next cell of bytes
            j++;
        }  
    }

    return bytes;
}

void convertCompressedImageToPGM(char *compressed_file_name, char *pgm_file_name) {
    
    FILE *cFile;
    FILE *pgmFile;
    unsigned short numRows, numCols;
    int maxGray = 0;
    int i, j, k;
    unsigned char *pixels;
    BYTE *bytes;
    int byteCounter = 0;
    long int saver;
    int numOfBits;
    
    //Open bin file
    cFile = fopen(compressed_file_name, "rb");
    checkFileOpen(cFile);

    //Read the rows, cols and max gray from the binary file
    fread(&numRows, sizeof(unsigned short), 1, cFile);
    fread(&numCols, sizeof(unsigned short), 1, cFile);
    fread(&maxGray, sizeof(BYTE), 1, cFile);

    bytes = (BYTE*) malloc(numRows * numCols * sizeof(BYTE));
    checkMalloc(bytes);

    while (!feof(cFile)) {
        fread(&bytes[byteCounter], sizeof(BYTE), 1, cFile);
        byteCounter++;
    }

    byteCounter -= 1; //Minus 1 because we don't need EOF

    bytes = (BYTE*) realloc(bytes, (byteCounter) * sizeof(BYTE)); 

    //Count the number of bits of the max gray value for decompression
    numOfBits = countNumberOfBits(maxGray);

    //Decompress the bytes
    pixels = decompressBytes(bytes, numOfBits, numRows * numCols);
   
    //Return to original gray values
    for(i = 0; i < numRows * numCols; i++) 
        pixels[i] = ceil((float)pixels[i] * (255)/(maxGray));

    //Update max gray level as before compressing
    maxGray = findMaxGrayLevel(pixels, numRows * numCols);

    //Open and write data on pgm file
    pgmFile = fopen(pgm_file_name, "w");
    checkFileOpen(pgmFile);
    
    fprintf(pgmFile, "P2\n"); //First line
    fprintf(pgmFile, "%d ", numCols);
	fprintf(pgmFile, "%d\n", numRows);
    fprintf(pgmFile, "%d\n", maxGray);

    //Insert pixels to the text file
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numCols; j++) {
            fprintf(pgmFile, "%d", *(pixels + (i * numCols) + j));
            if (j != numCols - 1)
                fprintf(pgmFile, " ");
        }
        if (i != numRows - 1)
            fprintf(pgmFile, "\n");
    }
    
    fclose(pgmFile);
    fclose(cFile);

    free(pixels);
    free(bytes);
}

unsigned char *decompressBytes(BYTE *bytes, int numOfBits, int arrSize) {
    
    int i,j;
    BYTE bytesMask;
    int bytesBitCounter, pixelsBitCounter;
    unsigned char *pixels;

    pixels = (unsigned char*) malloc((arrSize) * sizeof(unsigned char));
    checkMalloc(pixels);

    //Init the array
    for (i = 0; i < arrSize; i++) { 
        pixels[i] = 0;
    }

    i = 0; // cell number in the bytes array
    j = 0; // cell number in the pixels array

    bytesBitCounter = BYTE_SIZE - 1;
    pixelsBitCounter = numOfBits - 1;
        
    while (i < arrSize) {

        bytesMask = 1 << bytesBitCounter; //Create a mask for this bit
        bytesMask = bytes[i] & bytesMask;// Grab the bit

        if (bytesBitCounter < pixelsBitCounter)
            bytesMask <<= pixelsBitCounter - bytesBitCounter;
        else
            bytesMask >>= bytesBitCounter - pixelsBitCounter;

        //Take a bit from pixels and put inside bytes array
        pixels[j] = pixels[j] | (bytesMask);
        
        pixelsBitCounter--;
        bytesBitCounter--;

        if (bytesBitCounter < 0) {
            bytesBitCounter = BYTE_SIZE - 1;

            //Move to the next cell of bytes
            i++;
        }

        if (pixelsBitCounter <  0) {
            pixelsBitCounter =  numOfBits- 1;
            
            //Move to the next cell of pixels
            j++;
        }  
    }

    return pixels;
}

int findMaxGrayLevel(unsigned char *pixels, int size) {

	int max = 0;
	int i;

	for (i = 0; i < size; i++)
		if (pixels[i] > max)
			max = pixels[i];
	return max;
}

void freeChecker(int rows) {
    
    int row;
    
    for (row = 0; row < rows; row++)
        free(checker[row]);

    free(checker);
}

void freeImage(grayImage *image){
    
    int row;
    
    for (row = 0; row < image->rows; row++) {
        free(image->pixels[row]);
    }

    free(image->pixels);
    free(image);
}

void freePosListArr(imgPosList *array, int size) {
    
    int i;
    
    for (i = 0; i < size; i++){
        freeList(&array[i]);
    }
    
    free(array);
}

void freeList(imgPosList *lst) {

    imgPosCell *current = lst->head;
    imgPosCell *next;

    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
}

void freeTree(Segment *seg){

    freeTreeHelper(seg->root->next_possible_positions);
    free(seg->root);
}

void freeTreeHelper(treeNodeListCell *lst){
    
    treeNodeListCell* node;
    treeNodeListCell* nodeList;

    if (lst == NULL)
        return;
    else {

        node = lst->next;
        nodeList = lst->node->next_possible_positions;

        free(lst->node);
        free(lst);

        freeTreeHelper(node);
    } 

    if (nodeList == NULL) 
        return;
    else
        freeTreeHelper(nodeList);
}


void checkMalloc(void *ptr) {
    
    if (ptr == NULL) {
        printf("Memomry allocation failed, program will now exit.\n");
        exit(1);
    }
}

void checkFileOpen(void *ptr) {
    
    if (ptr == NULL) {
        printf("Could not open this file, or file does not exist.\n");
        exit(1);
    }
}
