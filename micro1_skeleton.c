#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image.h"
#include "stb_image_write.h"


#include <math.h>
#include <stdio.h>
#include <float.h>


void mirror_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void grayScale_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void sobelFiltering_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);

int main()
{

	int height;
	int width;
	int channel;

	char command;

	printf("Take a picture? (y/n)\n");
	scanf("%c", &command);

	if (command == 'n')
		exit(1);


	printf("Cheeze !\r\n");
	system("raspistill -w 640 -h 480 -t 10 -o image.bmp");


	unsigned char* imgIn = stbi_load("image.bmp", &width, &height, &channel, 3);


	unsigned char* imgOut_mirror = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);
	unsigned char* imgOut_grayScale = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);
	unsigned char* imgOut_sobelFiltering = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);
	
	mirror_transform(imgIn, height, width, channel, imgOut_mirror);
	grayScale_transform(imgIn, height, width, channel, imgOut_grayScale);
	sobelFiltering_transform(imgOut_grayScale, height, width, channel, imgOut_sobelFiltering);

	
	stbi_write_bmp("image_mirror.bmp", width, height, channel, imgOut_mirror);
	stbi_write_bmp("image_grayScale.bmp", width, height, channel, imgOut_grayScale);
	stbi_write_bmp("image_sobelFiltering.bmp", width, height, channel, imgOut_sobelFiltering);
	
	stbi_image_free(imgIn);
	free(imgOut_mirror);
	free(imgOut_grayScale);
	free(imgOut_sobelFiltering);


	return 0;
}

void mirror_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	int size = height * width * channel;

	for (int i = 0; i < size; i++) {
		out[i] = in[i];
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width / 2; j++) {
			for (int c = 0; c < channel; c++) {
				unsigned char temp;
				int k = width - j - 1; 
				temp = out[i * width * channel + j * channel + c];
				out[i * width * channel + j * channel + c] = out[i * width * channel + k * channel + c];
				out[i * width * channel + k * channel + c] = temp;
			}
		}
	}
}

void grayScale_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	int size;
	size = height * width * channel;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int sum = 0;
			for (int c = 0; c < channel; c++) {
				sum += in[i * width * channel + j * channel + c];
			}
			unsigned char avg = (unsigned char)(sum / 3);
			for (int c = 0; c < channel; c++) {
				out[i * width * channel + j * channel + c] = avg;
			}
		}
	}
}

void sobelFiltering_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {

	int Xfilter[9] = { -1, 0,1,-2,0,2,-1,0,1};
	int Yfilter[9] = { 1,2,1,0,0,0,-1,-2,-1 };
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			for (int c = 0; c < channel; c++) {
				int cnt = 0;
				int sum_x = 0;
				int sum_y = 0;
				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						sum_x += in[(y + i) * width * channel + (x + j) * channel + c] * Xfilter[cnt];
						sum_y += in[(y + i) * width * channel + (x + j) * channel + c] * Yfilter[cnt];
						cnt++;
					}
				}
				out[y * width * channel + x * channel + c] = (unsigned char)(abs(sum_x)+abs(sum_y));
			}
		}
	}

}
/*
void sobelFiltering_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {

	int xfilter[3][3] = { -1,0,1,-2,0,2,-1,0,1 };
	int yfilter[3][3] = { 1,2,1,0,0,0,-1,-2,-1 };

	int conx = 0;
	int cony = 0;

	unsigned char* zero_padding;

	zero_padding = (unsigned char*)malloc(sizeof(unsigned char) * ((height + 2) * (width + 2) * channel));


	for (int i = 1; i < height + 1; i++) {
		for (int j = 1; j < width + 1; j++) {
			for (int k = 0; k < channel; k++) {
				zero_padding[i * width * channel + j * channel + k] = in[(i - 1) * width * channel + (j - 1) * channel + k];
			}
		}
	}

	for (int j = 0; j < width + 2; j++)
	{
		for (int k = 0; k < channel; k++)
		{
			zero_padding[0 * (width + 2) * channel + j * channel + k] = 0;
			zero_padding[(height + 1) * (width + 2) * channel + j * channel + k] = 0;
		}
	}

	for (int i = 0; i < height + 2; i++)
	{
		for (int k = 0; k < channel; k++)
		{
			zero_padding[i * (width + 2) * channel + 0 * channel + k] = 0;
			zero_padding[i * (width + 2) * channel + (height + 1) * channel + k] = 0;
		}
	}

	for (int i = 0; i < height - 2; i++) {
		for (int j = 0; j < width - 2; j++)
		{
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					conx = conx + (zero_padding[(i + x) * width * channel + (j + y) * channel] * xfilter[x][y]);
					cony = cony + (zero_padding[(i + x) * width * channel + (j + y) * channel] * yfilter[x][y]);
				}
			}
			for (int k = 0; k < channel; k++)
			{
				out[i * width * channel + j * channel + k] = abs(conx) + abs(cony);
			}
			conx = 0;
			cony = 0;
		}
	}

	stbi_write_bmp("padding.bmp", width, height, channel, zero_padding);
}*/