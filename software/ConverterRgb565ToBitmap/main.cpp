#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

//QQVGA SIZE
#define WIDTH		(160)
#define HEIGHT		(120)

#define IMAGESIZE16 ((WIDTH) * (HEIGHT) * 2)
#define	IMAGESIZE24	((IMAGESIZE16 / 2) * 3)

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long LONG;

typedef struct tagBITMAPFILEHEADER { // bmfh  
    WORD    bfType;           // ‘BM’ 
    DWORD   bfSize;           //  
    WORD    bfReserved1;    // ?0 
    WORD    bfReserved2;    // ?0 
    DWORD   bfOffBits;        // RGBA  54 
} BITMAPFILEHEADER; 

typedef struct tagBITMAPINFOHEADER{ // bmih  
    DWORD  biSize;               // (Bitmap Info Header),40   
    LONG   biWidth;               // ? 
    LONG   biHeight;              // ?   
    WORD   biPlanes;             // ??1? 
    WORD   biBitCount;           // ?24(24Bit) 
    DWORD  biCompression;    // ?0() 
    DWORD  biSizeImage;       // ?4? 
    LONG   biXPelsPerMeter;   // /(0)
    LONG   biYPelsPerMeter;   // /(0) 
    DWORD  biClrUsed;           // (0) 
    DWORD  biClrImportant;    // (0) 
} BITMAPINFOHEADER; 


void pixel_rgb565_to_rgb24(const unsigned char *p16,unsigned char *p24)	//16bit24bit
{
	unsigned char R,G,B;
	B=*(unsigned short*)p16 & 0x01F;
	G=(*(unsigned short*)p16 & 0x7E0) >> 5;
	R=(*(unsigned short*)p16 >> 11 ) & 0x1F;
	*(p24+0)=B << 2; 
	*(p24+1)=G << 1;
	*(p24+2)=R << 2;
} 

void image16_to_image24(const unsigned char *image16, unsigned char *image24, int biWidth, int biHeight)
{
	const unsigned char *p16 = image16;
	unsigned char *p24 = image24;
	int i;
	for(i=0; i<biWidth*biHeight; i++, p16+=2, p24+=3)
	{
		pixel_rgb565_to_rgb24(p16, p24);
	}
}

int main(void)
{
/* Compile: 
 via terminal unix: g++ -o converter.exe main.cpp
 Or import to your IDE and compile
*/ 
   
	unsigned char bmp_fileheader[14];	//
	BITMAPINFOHEADER bmp_infoheader;	//

	*(WORD *)bmp_fileheader = 'B' | 'M' << 8;	//bfType 'BM'
	*(DWORD *)(bmp_fileheader + 2) = 14 + 40 + IMAGESIZE24;		//bfSize 
	*(DWORD *)(bmp_fileheader + 6) = 0; //bfReserved1  bfReserved2
	*(DWORD *)(bmp_fileheader + 10) = 54;	//bfOffBits RGBA  54 

	//
	bmp_infoheader.biSize = 40;
	bmp_infoheader.biWidth = WIDTH;						//width
	bmp_infoheader.biHeight = HEIGHT;				    //height
	bmp_infoheader.biPlanes = 1;
	bmp_infoheader.biBitCount = 24;						//
	bmp_infoheader.biCompression = 0;
	bmp_infoheader.biSizeImage = IMAGESIZE24;			//
	bmp_infoheader.biXPelsPerMeter = 0;
	bmp_infoheader.biYPelsPerMeter = 0;
	bmp_infoheader.biClrUsed = 0;
	bmp_infoheader.biClrImportant = 0;

    //printf("HEIGHT = %d\r\n",bmp_infoheader.biHeight);
    //printf("WIDTH = %d\r\n",bmp_infoheader.biWidth);

	unsigned char *imagedata16 = new unsigned char[IMAGESIZE16];
	unsigned char *imagedata24 = new unsigned char[IMAGESIZE24];

	//Open file rgr565 from 0v7670 camera
	FILE *fp_raw = fopen("Image", "rb");
	if(fp_raw){
	  fread(imagedata16, IMAGESIZE16, 1, fp_raw);
	  fclose(fp_raw);
	}else{
	  printf("Error to try read binary file rgb565\n");
	  getch();
	  exit(0);
	}

	//Convert from raw data rgb565 to bitmap structure
	image16_to_image24(imagedata16, imagedata24, WIDTH, HEIGHT);

	//Create the image in bitmap format 
	FILE *file = fopen("Image.bmp", "wb");
	if(file){    
	  fwrite(bmp_fileheader, 14, 1, file);
	  fwrite(&bmp_infoheader, 40, 1, file);
	  fwrite(imagedata24, IMAGESIZE24, 1, file);
	  printf("Image save with success!\n");
	  fclose(file);
	}else {
	  printf("Error to try create Image.bmp on disc\n");
	}
	
	//free memory
	delete []imagedata16;
	delete []imagedata24;
	
	getch();
	return 0;
}
