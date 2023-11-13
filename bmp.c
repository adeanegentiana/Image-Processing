// Marcu Gentiana Adeane - 313CB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bmp_header.h"
#define EXTENSION_SIZE strlen(".bmp")

typedef struct {
	int sumB;
	int sumG;
	int sumR;
	int nrElem;
} task5_struct;

void freeMatrix(void **matrix, int lines) {
	int i;
	for (i = 0; i < lines; i++) {
		free(matrix[i]);
	}
	free(matrix);
}

pixel **createBitmap(int height, int width) {
	int i;
	pixel **bitmap = calloc(height, sizeof(pixel *));
	if (bitmap == NULL) {
		exit(1);
	}
	for (i = 0; i < height; i++) {
		bitmap[i] = calloc(width, sizeof(pixel));
		if (bitmap[i] == NULL) {
		exit(1);
	}
	}
	return bitmap;
}

void readImage(char *input_file_name, bmp_fileheader *fileHeader,
	bmp_infoheader *infoHeader,	pixel ***bitmap) {
	FILE *fp = fopen(input_file_name, "rb");
	if (!fp) {
		fprintf(stderr, "Error opening file %s!\n", input_file_name);
		exit(EXIT_FAILURE);
	}

	fread(fileHeader, sizeof(bmp_fileheader), 1, fp);
	fread(infoHeader, sizeof(bmp_infoheader), 1, fp);
	fseek(fp, fileHeader->imageDataOffset, SEEK_SET);

	int i;
	*bitmap = createBitmap(infoHeader->height, infoHeader->width);

	// ignoring padding bytes
	int paddingBytes = (FOUR - ((infoHeader->width * THREE) % FOUR)) % FOUR;
	for (i = 0; i < infoHeader->height; i++) {
		fread((*bitmap)[i], sizeof(pixel), infoHeader->width, fp);
		fseek(fp, paddingBytes, SEEK_CUR);
	}

	fclose(fp);
}

void printImage(char *fileName, bmp_fileheader fileHeader,
	bmp_infoheader infoHeader, pixel **bitmap) {
	int zero = 0;
	int paddingBytes = (FOUR - ((infoHeader.width * THREE) % FOUR)) % FOUR;

	FILE *fp = fopen(fileName, "wb");

	fwrite(&fileHeader, sizeof(bmp_fileheader), 1, fp);
	fwrite(&infoHeader, sizeof(bmp_infoheader), 1, fp);
	fwrite(&zero, sizeof(char), fileHeader.imageDataOffset
		- sizeof(bmp_infoheader) - sizeof(bmp_fileheader), fp);

	int i;
	// initialising padding bytes to 0
	for (i = 0; i < infoHeader.height; i++) {
		fwrite(bitmap[i], sizeof(pixel), infoHeader.width, fp);
		fwrite(&zero, sizeof(char), paddingBytes, fp);
	}

	fclose(fp);
}

pixel **copyBitmap(pixel **bitmap, int height, int width) {
	pixel **bitmapCopy = createBitmap(height, width);

	int i, j;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			bitmapCopy[i][j] = bitmap[i][j];
		}
	}

	return bitmapCopy;
}

char *changeFileName(char *fileName, char *taskName) {
	// [name].bmp -> [name]_asked_name.bmp
	char *outputName = calloc(THIRTY + strlen(taskName) + 1, sizeof(char));
	strncpy(outputName, fileName, strlen(fileName) - EXTENSION_SIZE);
	strcat(outputName, taskName);
	strcat(outputName, ".bmp");
	return outputName;
}

void task1(char *fileName, bmp_fileheader fileHeader,
	bmp_infoheader infoHeader, pixel **bitmap) {
	pixel **bitmapCopy = copyBitmap(bitmap, infoHeader.height,
		infoHeader.width);

	int i, j;
	for (i = 0; i < infoHeader.height; i++) {
		for (j = 0; j < infoHeader.width; j++) {
			unsigned char average = ((B(bitmap[i][j]) + G(bitmap[i][j])
				+ R(bitmap[i][j])) / THREE); // pixels average
			B(bitmapCopy[i][j]) = average;
			G(bitmapCopy[i][j]) = average;
			R(bitmapCopy[i][j]) = average;
		}
	}

	char *outputName = changeFileName(fileName, "_black_white");
	printImage(outputName, fileHeader, infoHeader, bitmapCopy);
	free(outputName);
	freeMatrix((void **) bitmapCopy, infoHeader.height);
}

void task2(char *fileName, bmp_fileheader fileHeader,
	bmp_infoheader infoHeader, pixel **bitmap) {
	pixel **bitmapCopy;

	if (infoHeader.height > infoHeader.width) {
		int dif = infoHeader.height - infoHeader.width; // difference
		int newWidth = infoHeader.height;
		bitmapCopy = createBitmap(infoHeader.height, newWidth);
		int left = dif / TWO;
		pixel white;
		B(white) = FULL;
		G(white) = FULL;
		R(white) = FULL;
		int i, j;
		for (i = 0; i < infoHeader.height; i++) {
			for (j = 0; j < left; j++) {
				bitmapCopy[i][j] = white;
			}
			for (j = left; j < infoHeader.width + left; j++) {
				bitmapCopy[i][j] = bitmap[i][j - left];
			}
			for (j = infoHeader.width + left; j < newWidth; j++) {
				bitmapCopy[i][j] = white;
			}
		}
		infoHeader.width = newWidth;
	} else if (infoHeader.height < infoHeader.width) {
		int dif = infoHeader.width - infoHeader.height;
		int newHeight = infoHeader.width;
		bitmapCopy = createBitmap(newHeight, infoHeader.width);
		int up = dif / TWO + dif % TWO;
		pixel white;
		B(white) = FULL;
		G(white) = FULL;
		R(white) = FULL;
		int i, j;
		for (i = 0; i < up; i++) {
			for (j = 0; j < infoHeader.width; j++){
				bitmapCopy[i][j] = white;
			}
		}
		for (i = up; i < infoHeader.height + up; i++) {
			for (j = 0; j < infoHeader.width; j++){
				bitmapCopy[i][j] = bitmap[i - up][j];
			}
		}
		for (i = infoHeader.height + up ; i < newHeight; i++) {
			for (j = 0; j < infoHeader.width; j++){
				bitmapCopy[i][j] = white;
			}
		}

		infoHeader.height = newHeight;
	} else {
		bitmapCopy = bitmap;
	}

	char *outputName = changeFileName(fileName, "_nocrop");
	printImage(outputName, fileHeader, infoHeader, bitmapCopy);
	free(outputName);
	if (bitmapCopy != bitmap) {
		freeMatrix((void **) bitmapCopy, infoHeader.height);
	}
}

void task3(char *fileName, char *filterFile, bmp_fileheader fileHeader,
	bmp_infoheader infoHeader, pixel **bitmap) {
	pixel **bitmapCopy = createBitmap(infoHeader.height, infoHeader.width);

	FILE *fp = fopen(filterFile, "rt");

	int filterSize;
	fscanf(fp, "%d", &filterSize);
	int **filter = calloc(filterSize, sizeof(int *));
	if (filter == NULL) {
		exit(1);
	}

	int i, j, n, m;

	for (i = 0; i < filterSize; i++) {
		filter[i] = calloc(filterSize, sizeof(int));
	}
	// reading the filter's elements
	for (i = 0; i < filterSize; i++) {
		for (j = 0; j < filterSize; j++) {
			fscanf(fp, "%d", &filter[i][j]);
		}
	}

	fclose(fp);

	for (i = infoHeader.height - 1; i >= 0; i--) {
		for (j = 0; j < infoHeader.width; j++) {
			int blue = 0, green = 0, red = 0;
			for (n = 0; n < filterSize; n++) {
				for (m = 0; m < filterSize; m++) {
					int line = i + filterSize / TWO - n;
					int col = j - filterSize / TWO + m;
					if (line >= 0 && line < infoHeader.height) {
						if (col >= 0 && col < infoHeader.width) {
							// applying filter on picture
							blue += B(bitmap[line][col]) * filter[n][m];
							green += G(bitmap[line][col]) * filter[n][m];
							red += R(bitmap[line][col]) * filter[n][m];
						}
					}
				}
			}
			if (blue < 0) {
				blue = 0;
			} else if (blue > FULL) {
				blue = FULL;
			}
			B(bitmapCopy[i][j]) = blue;

			if (green < 0) {
				green = 0;
			} else if (green > FULL) {
				green = FULL;
			}
			G(bitmapCopy[i][j]) = green;

			if (red < 0) {
				red = 0;
			} else if (red > FULL) {
				red = FULL;
			}
			R(bitmapCopy[i][j]) = red;
		}
	}

	char *outputName = changeFileName(fileName, "_filter");
	printImage(outputName, fileHeader, infoHeader, bitmapCopy);
	free(outputName);
	freeMatrix((void **) filter, filterSize);
	freeMatrix((void **) bitmapCopy, infoHeader.height);
}

void task4(char *fileName, char *poolingFile, bmp_fileheader fileHeader,
	bmp_infoheader infoHeader, pixel **bitmap) {
	pixel **bitmapCopy = createBitmap(infoHeader.height, infoHeader.width);

	FILE *fp = fopen(poolingFile, "rt");

	char poolingType; // min / max
	fscanf(fp, "%c", &poolingType);
	int poolingValue;
	fscanf(fp, "%d", &poolingValue);

	fclose(fp);

	int i, j, n, m;

	for (i = infoHeader.height - 1; i >= 0; i--) {
		for (j = 0; j < infoHeader.width; j++) {
			if (poolingType == 'm') {
				B(bitmapCopy[i][j]) = FULL;
				G(bitmapCopy[i][j]) = FULL;
				R(bitmapCopy[i][j]) = FULL;
			}
			for (n = 0; n < poolingValue; n++) {
				for (m = 0; m < poolingValue; m++) {
					int line = i + poolingValue / TWO - n;
					int col = j - poolingValue / TWO + m;
					if ((line >= 0 && line < infoHeader.height)
						&& (col >= 0 && col < infoHeader.width)) {
						if (poolingType == 'm') {
							if (B(bitmapCopy[i][j]) > B(bitmap[line][col])){
								B(bitmapCopy[i][j]) = B(bitmap[line][col]);
							}
							if (G(bitmapCopy[i][j]) > G(bitmap[line][col])){
								G(bitmapCopy[i][j]) = G(bitmap[line][col]);
							}
							if (R(bitmapCopy[i][j]) > R(bitmap[line][col])){
								R(bitmapCopy[i][j]) = R(bitmap[line][col]);
							}
						} else if (poolingType == 'M') {
							if (B(bitmapCopy[i][j]) < B(bitmap[line][col])){
								B(bitmapCopy[i][j]) = B(bitmap[line][col]);
							}
							if (G(bitmapCopy[i][j]) < G(bitmap[line][col])){
								G(bitmapCopy[i][j]) = G(bitmap[line][col]);
							}
							if (R(bitmapCopy[i][j]) < R(bitmap[line][col])){
								R(bitmapCopy[i][j]) = R(bitmap[line][col]);
							}
						}
					} else {
						if (poolingType == 'm') {
							B(bitmapCopy[i][j]) = 0;
							G(bitmapCopy[i][j]) = 0;
							R(bitmapCopy[i][j]) = 0;
						}
					}
				}
			}
		}
	}

	char *outputName = changeFileName(fileName, "_pooling");
	printImage(outputName, fileHeader, infoHeader, bitmapCopy);
	free(outputName);
	freeMatrix((void **) bitmapCopy, infoHeader.height);
}

void explore(pixel **bitmap, int height, int width, int line, int col,
	task5_struct *sums, int **zones, int currentZone,
	int threshold, pixel firstPixel) {
	zones[line][col] = currentZone;
	sums->nrElem++;
	sums->sumB += B(bitmap[line][col]);
	sums->sumG += G(bitmap[line][col]);
	sums->sumR += R(bitmap[line][col]);
	if (line + 1 < height && zones[line + 1][col] == 0) {
		if (abs(B(firstPixel) - B(bitmap[line + 1][col])) +
			abs(G(firstPixel) - G(bitmap[line + 1][col])) +
			abs(R(firstPixel) - R(bitmap[line + 1][col])) <=
			threshold) {
			explore(bitmap, height, width, line + 1, col, sums, zones,
				currentZone, threshold, firstPixel);
		}
	}

	if (line - 1 >= 0 && zones[line - 1][col] == 0) {
		if (abs(B(firstPixel) - B(bitmap[line - 1][col])) +
			abs(G(firstPixel) - G(bitmap[line - 1][col])) +
			abs(R(firstPixel) - R(bitmap[line - 1][col])) <=
			threshold) {
			explore(bitmap, height, width, line - 1, col, sums, zones,
				currentZone, threshold, firstPixel);
		}
	}

	if (col + 1 < width && zones[line][col + 1] == 0) {
		if (abs(B(firstPixel) - B(bitmap[line][col + 1])) +
			abs(G(firstPixel) - G(bitmap[line][col + 1])) +
			abs(R(firstPixel) - R(bitmap[line][col + 1])) <=
			threshold) {
			explore(bitmap, height, width, line, col + 1, sums, zones,
				currentZone, threshold, firstPixel);
		}
	}

	if (col - 1 >= 0 && zones[line][col - 1] == 0) {
		if (abs(B(firstPixel) - B(bitmap[line][col - 1])) +
			abs(G(firstPixel) - G(bitmap[line][col - 1])) +
			abs(R(firstPixel) - R(bitmap[line][col - 1])) <=
			threshold) {
			explore(bitmap, height, width, line, col - 1, sums, zones,
				currentZone, threshold, firstPixel);
		}
	}
}

void setAverage(pixel **bitmap, int height, int width, int line, int col,
	char avgB, char avgG, char avgR, int **zones, int currentZone) {
	B(bitmap[line][col]) = avgB;
	G(bitmap[line][col]) = avgG;
	R(bitmap[line][col]) = avgR;
	zones[line][col] = -1;

	if (line + 1 < height && zones[line + 1][col] == currentZone) {
		setAverage(bitmap, height, width, line + 1, col, avgB, avgG, avgR,
			zones, currentZone);
	}

	if (line - 1 >= 0 && zones[line - 1][col] == currentZone) {
		setAverage(bitmap, height, width, line - 1, col, avgB, avgG, avgR,
			zones, currentZone);
	}

	if (col + 1 < width && zones[line][col + 1] == currentZone) {
		setAverage(bitmap, height, width, line, col + 1, avgB, avgG, avgR,
			zones, currentZone);
	}

	if (col - 1 >= 0 && zones[line][col - 1] == currentZone) {
		setAverage(bitmap, height, width, line, col - 1, avgB, avgG, avgR,
			zones, currentZone);
	}
}

void task5(char *fileName, char *clusterFile, bmp_fileheader fileHeader,
	bmp_infoheader infoHeader, pixel **bitmap) {
	pixel **bitmapCopy = copyBitmap(bitmap, infoHeader.height,
		infoHeader.width);

	FILE *fp = fopen(clusterFile, "rt");

	int threshold;
	fscanf(fp, "%d", &threshold);

	fclose(fp);

	int **zones = calloc(infoHeader.height, sizeof(int *));
	if (zones == NULL) {
		exit(1);
	}

	int i, j;

	for (i = 0; i < infoHeader.height; i++) {
		zones[i] = calloc(infoHeader.width, sizeof(int));
	}

	int currentZone = 0;
	for (i = infoHeader.height - 1; i >= 0; i--) {
		for (j = 0; j < infoHeader.width; j++) {
			if (zones[i][j] == 0 ) {
				task5_struct *sums = calloc(1, sizeof(task5_struct));
				if (sums == NULL) {
					exit(1);
				}
				currentZone++;
				explore(bitmap, infoHeader.height, infoHeader.width, i, j,
					sums, zones, currentZone, threshold, bitmap[i][j]);

				unsigned char avgB = sums->sumB / sums->nrElem;
				unsigned char avgG = sums->sumG / sums->nrElem;
				unsigned char avgR = sums->sumR / sums->nrElem;

				setAverage(bitmapCopy, infoHeader.height, infoHeader.width, i,
					j, avgB, avgG, avgR, zones, currentZone);
				free(sums);
			}
		}
	}

	char *outputName = changeFileName(fileName, "_clustered");
	printImage(outputName, fileHeader, infoHeader, bitmapCopy);
	free(outputName);
	freeMatrix((void **) zones, infoHeader.height);
	freeMatrix((void **) bitmapCopy, infoHeader.height);
}

int main() {
	char *inputImageFile = calloc(MAX_NAME_SIZE, sizeof(char));
	if (inputImageFile == NULL) {
		exit(1);
	}
	char *filterFile = calloc(MAX_NAME_SIZE, sizeof(char));
	if (filterFile == NULL) {
		exit(1);
	}
	char *poolingFile = calloc(MAX_NAME_SIZE, sizeof(char));
	if (poolingFile == NULL) {
		exit(1);
	}
	char *clusterFile = calloc(MAX_NAME_SIZE, sizeof(char));
	if (clusterFile == NULL) {
		exit(1);
	}

	FILE *fp = fopen("input.txt", "rt");

	fscanf(fp, "%s", inputImageFile);
	fscanf(fp, "%s", filterFile);
	fscanf(fp, "%s", poolingFile);
	fscanf(fp, "%s", clusterFile);

	fclose(fp);

	bmp_fileheader fileHeader;
	bmp_infoheader infoHeader;
	pixel **bitmap;

	readImage(inputImageFile, &fileHeader, &infoHeader, &bitmap);

	task1(inputImageFile, fileHeader, infoHeader, bitmap);

	task2(inputImageFile, fileHeader, infoHeader, bitmap);

	task3(inputImageFile, filterFile, fileHeader, infoHeader, bitmap);

	task4(inputImageFile, poolingFile, fileHeader, infoHeader, bitmap);

	task5(inputImageFile, clusterFile, fileHeader, infoHeader, bitmap);

	free(inputImageFile);
	free(filterFile);
	free(poolingFile);
	free(clusterFile);
	freeMatrix((void **) bitmap, infoHeader.height);
	return 0;
}