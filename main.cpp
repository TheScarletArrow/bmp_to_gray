//#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <string>
#include <windows.h>


using namespace std;

#define BYTE  unsigned char

#define DWORD unsigned long
#define WORD unsigned short

//this is how BITMAPFILEHEADER structure looks like
/*struct BITMAPFILEHEADER {
	WORD bfType;
	DWORD bfSize;
	WORD bfReverse1;
	WORD bfReverse2;
	DWORD bfOffbits;
};*/

//just technical info, no need of this
void print(BITMAPFILEHEADER header) {
	cout << "bfSize" << dec << header.bfSize << endl;
	cout << "bfOffBits" << dec << header.bfOffBits << endl;
}

// BITMAPINFOHEADER
/*struct BITMAPINFOHEADER {
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD biPlanes;
	WORD bibitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
};*/

// base information about bmp
void print(BITMAPINFOHEADER header)
{
	cout << "Size: " << dec << header.biSize << endl;
	cout << "Width: " << header.biWidth << endl;
	cout << "Height: " << header.biHeight << endl;
	cout << "BitCount: " << header.biBitCount << endl;
	cout << "BitCompresion: " << header.biCompression << endl;
	cout << "ImageSize: " << header.biSizeImage << endl;
	cout << "BitX: " << header.biXPelsPerMeter << endl;

}

class ImageMatrix {
public:
	int height;
	int width;
	int rowsize;
	BYTE* term;

	ImageMatrix()
	{
		height = 0;
		width = 0;
		rowsize=0;
		term=0;
	}

	ImageMatrix(int height, int width)
	{
		this->height = height;
		this->width = width;
		rowsize = (3 * width + 3) / 4 * 4;
		term = new BYTE[height * rowsize];
	}

	bool load(char* file)
	{
		BITMAPFILEHEADER h;
		BITMAPINFOHEADER hInfo;
		ifstream InputFile;
		InputFile.open(file, ios::binary);
		InputFile.seekg(0, InputFile.end);
		cout << "File Size: " << InputFile.tellg() << " byte(s)" << endl;
		InputFile.seekg(0, InputFile.beg);
		InputFile.read((char*)& h, sizeof(h));
		InputFile.read((char*)& hInfo, sizeof(hInfo));
		print(h);
		print(hInfo);
		if (h.bfType != 0x4d42
			|| hInfo.biClrUsed != 0
			|| hInfo.biBitCount != 24
			|| hInfo.biCompression != 0
			|| hInfo.biPlanes != 1)
		{
			cout << "ERROR";
			InputFile.close();
			return "err";
		}

		width = hInfo.biWidth;
		height = hInfo.biHeight;
		cout << "Reading from " << file << endl;
		*this = ImageMatrix(height, width);
		InputFile.read((char*)term, height * rowsize);
		InputFile.close();
		return true;
	}

	bool Save(char* file)
	{
		BITMAPFILEHEADER h = {
			0x4d42,
			static_cast<DWORD>(54L + rowsize*height),
			0,
			0,
			54
		};
		BITMAPINFOHEADER hInfo = {
			sizeof(BITMAPINFOHEADER),
			width,
			height,
			1, //plans
			24, //bit per pixel
			0, // compression
			static_cast<DWORD>(rowsize*height), //Image Matrix Size
			3780, //bits per meter x
			3780, //bits per meter y
			0, //number of used colours
			0
		};
		cout << "Writing to " << file;
		ofstream OutputFile;
		OutputFile.open(file, ios::binary);
		OutputFile.write((char*)& h, sizeof(h));
		OutputFile.write((char*)& hInfo, sizeof(hInfo));
		OutputFile.write((char*)term, rowsize * height);
		OutputFile.close();
		cout << endl;
		return true;
	}
};

ImageMatrix ToGray(ImageMatrix m)
{
	ImageMatrix mm = ImageMatrix(m.height, m.width);
	for (int y = 0; y < m.height; y++) {
		for (int xx = 0; xx < m.rowsize; xx += 3)
		{

			BYTE B = m.term[y * m.rowsize + xx];
			BYTE G = m.term[y * m.rowsize + xx + 1];
			BYTE R = m.term[y * m.rowsize + xx + 2];
			BYTE gray = (BYTE)(0.2989 * R + 0.6 * G + 0.1 * B + 0.5);
			mm.term[y * m.rowsize + xx] = mm.term[y * m.rowsize + xx + 1] = mm.term[y * m.rowsize + xx + 2] = gray;
		}
	}
	return mm;
}

ImageMatrix Inverse(ImageMatrix m)
{
    ImageMatrix mm = ImageMatrix(m.height, m.width);
    for (int y=0;y<m.height;y++)
        for (int xx=0;xx<m.rowsize;xx+=3)
        {
            BYTE B = m.term[y * m.rowsize + xx];
            BYTE G = m.term[y * m.rowsize + xx + 1];
            BYTE R = m.term[y * m.rowsize + xx + 2];

            B= (BYTE)(255- (B));
            G=(BYTE)(255-(G));
            R=(BYTE)(255-(R));


            mm.term[y * m.rowsize + xx] = B;
            mm.term[y * m.rowsize + xx + 1] = G;
            mm.term[y * m.rowsize + xx + 2] = R;

        }
    return mm;
}

int main(int argc, char* argv[])
{
	cout << sizeof(BITMAPFILEHEADER) << endl;
	cout << sizeof(BITMAPINFOHEADER) << endl;
	ImageMatrix m, mm1, mm2;
    char inputimage[]="image.bmp";

	char out1[] = "imageout.bmp";
	char out2[] = "omageout2.bmp";
	m.load(inputimage);

	mm1 = ToGray(m);
	mm1.Save(out1);

	mm2 = Inverse(m);
	mm2.Save(out2);

	return 0;

}

