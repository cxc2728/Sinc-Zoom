#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <iomanip>
#include <istream>
#include <math.h>

using namespace std;

void OnFourierTransform(char imageFilename[], int rcxres, int rcyres);
void OnSinc_Zoom(char imageFileName[], int rcxres, int rcyres, double samplingRate);

// declare a class by the name 'Sinc_Int_2023'
class Sinc_Int_2023 {

	// the point is to assign to 'n1' and 'n2' 
	// the correct values from the command console
	int n1; // matrix size x (number of pixels along the x direction)
	int n2; // matrix size y (number of pixels along the y direction)

// declare the class methods
// and embed the methods body
// in the class specification
public:

	// declare a method that returns the number
	// of pixels of the image along the x direction
	int getNofPixelsX(void) { return this->n1; };

	// declare a method that returns the number
	// of pixels of the image along the y direction
	int getNofPixelsY(void) { return this->n2; };

	// declare a method that sets the number
	// of pixels of the image along the x direction
	void setNofPixelsX(int x) { this->n1 = x; };

	// declare a method that sets the number 
	// of pixels of the image along the y direction
	void setNofPixelsY(int y) { this->n2 = y; };

public:
	// declare a structure ' data ' that defines the
	// pointers to pointers to the image
	struct data {

		double** Signal; // declare the pointer to pointer to the matrix (image) 

		double** Edge_X;

		double** Edge_Y;

		double** Region_XY;

	}*pointer; // pointer to the element of the structure 'data'
			   // the pointer points to the memory address of the
			   // pointers to pointers 

public:

	// constructor of the class 'Sinc_Int_2023'
	Sinc_Int_2023(int x, int y) : n1(x), n2(y) { pointer = 0; };

	// declare the prototype of the 
	// function 'allocateData'
	// the function belongs to the 
	// sets of methods of the class 'Sinc_Int_2023'
	void allocateData();

	// declare the prototype of the 
	// function 'save'
	// the function belongs to the 
	// sets of methods of the class 'Sinc_Int_2023'
	void save();

	// destructor of the class 'Sinc_Int_2023'
	~Sinc_Int_2023() { }

};

void Sinc_Int_2023::allocateData() { // allocate data


	// (1) allocate struct 'data' (begin)
	// define a pointer by the name 'pointer'
	// and assign to it the instance of the
	// structure 'data' (the instance is a
	// memory address
	pointer = new data;

	// assign to the pointer to a pointer 'Signal' (pointer->Signal)
	// the instance of the memory address of the pointer to pointer
	// *[this->n2]. Practically define the memory address of the 
	// roSinc of the matrix containing the image 'Signal'.
	pointer->Signal = new double* [this->n2];

	for (int v = 0; v < this->n2; v++) { // (1)
	// at each iteration of the for loop
	// assign to the pointer 'Signal[v]' the instance 
	// of the memory address of the pointer [this-n1].
	// Practically define the memory address of the 
	// columns of the matrices containing the image:
	// 'Signal.

		pointer->Signal[v] = new double[this->n1];


	} // (1) allocate struct 'data' (end)


  // (2) initialize (begin)
	for (int v = 0; v < this->n2; v++) { // (a)

		for (int f = 0; f < this->n1; f++) { // (b)

		// at each iteration of the two for loops
		// initializes the value of the pixel of
		// the image to zero. This is done for the
		// image 'Signal'.
			pointer->Signal[v][f] = (double)0.0;

		} //(b)

	} //(a)
   // (2) initialize (end)

} // allocate data


void Sinc_Int_2023::save() { // saveImages

	// declare a pointer to file
	// to be used to read the image
	FILE* savedata;
	// declare a string which contains
	// the file name of the image
	char outputFile[128];

	// assign the name string "Signal.img"
	// to the string 'outputFile'
	sprintf(outputFile, "%s", "Signal.img");

	// open the image file in write-binary mode
	if ((savedata = fopen(outputFile, "wb")) == NULL)
	{
		// alert the user of possible failure in opening the file
		std::cout << "Cannot open output file, Now Exit..." << endl;
		exit(0);

	}
	else { // (save)


		for (int v = 0; v < this->n2; v++) { // (a)

			for (int f = 0; f < this->n1; f++)

				// at each iteration of the for loop saves the pixel
				// value contained at the memory address: ' &pointer->Signal[v][f] '
				fwrite(&pointer->Signal[v][f], sizeof(double), 1, savedata);

		} // (a)

		// close the file afer saving
		fclose(savedata);

	} // (save)

} // saveImages

int main(int argc, char* argv[]) {

	if (argc < 5) {
		std::cout << endl;
		std::cout << "Please type the image file name" << endl;
		std::cout << "Please make sure that the image format is Analyze 'double': 64 bits real" << endl;
		std::cout << "Please enter the following values: " << endl;
		std::cout << "The number of pixels along the X direction (integer)" << endl;
		std::cout << "The number of pixels along the Y direction (integer)" << endl;
		std::cout << "The Sampling Rate (double) in [0.1, 4.0]" << endl;
		std::cout << endl;
		exit(0);
	}

	else { // run the program (begin)

		char imageFileName[128];
		sprintf(imageFileName, "%s", argv[1]);

		int m_rcxres = atoi(argv[2]);
		int m_rcyres = atoi(argv[3]);

		double m_TheSamplingRate = atof(argv[4]);


		if (m_TheSamplingRate > 4.0 || m_TheSamplingRate < 0.1)
		{

		    std::cout << "The Sampling Rate must be (double) in [0.1, 4.0]" << endl;
			exit(0);

		}
		
		/// calculate the zoom factor (begin)
		double zoomValue = 0.0;
		
		// minimum value of the sigmoidal function
		// used to estimate the bandwidth
		double m_Bandwidth = 0.5;

		if ( ((double)2.0 > (double)m_TheSamplingRate) )

			zoomValue = -100.0 * ((double)2.0 * m_Bandwidth - ((double)m_TheSamplingRate/2.0));
		// zoom-in

		else if ( ((double)2.0 < (double)m_TheSamplingRate) )

			zoomValue = -100.0 * ((double)2.0 * m_Bandwidth - ((double)m_TheSamplingRate/2.0));
		// zoom-out

		else zoomValue = (double)zoomValue; // no-zoom
		
		zoomValue /= (double)2.0;

		std::cout << "The Zoom Value is approximately: " << zoomValue << "%" << endl;
		/// calculate the zoom factor (end)
	
		// inform the user of the image size 
		// (number of roSinc and number of columns
		// of the matrix containing the image)
		std::cout << "The number of pixels along the X direction is: " << atoi(argv[2]) << endl;
		std::cout << "The number of pixels along the Y direction is: " << atoi(argv[3]) << endl;
		std::cout << "The Sampling Rate is: " << m_TheSamplingRate << endl;

		// assign to the char string 'outputFile'
		// the value "Sinc_IntF2023.log"
		char outputFile[128] = "Sinc_Int_2023.log";

		// declare a pointer to a file by the
		// name 'savedata'
		FILE* savedata;

		if ((savedata = fopen(outputFile, "w")) == NULL)
		{
			// alert the user of possible failure in opening the file
			std::cout << "Cannot open output file, Now Exit..." << endl;
			exit(0);

		}
		else { // save to log file

			// save into the log file the image size 
			// (number of roSinc and number of columns
			 // of the matrix containing the image)
			fprintf(savedata, "%s%d\n", "The number of pixels along the X direction is: ", m_rcxres);
			fprintf(savedata, "%s%d\n", "The number of pixels along the Y direction is: ", m_rcyres);
			fprintf(savedata, "%s%lf\n", "The Sampling Rate is: ", m_TheSamplingRate);

			fclose(savedata);

		} // save to log file (end)


		// call to the constructor 'Sinc_Int' so to create
		// an object of type 'Sinc_Int'. The data type of 'Sinc_Int'
		// is 'Sinc_Int_2023'
		Sinc_Int_2023 Sinc_Int(m_rcxres, m_rcyres);

		// the object of type 'Sinc_Int' 
		// sends a message (invokes)
		// to the method 'allocateData()'
		Sinc_Int.allocateData();

		/// read image file (begin)
		// declare a file pointer
		FILE* pf;

		// open the file containing the image to
		// process. The image to process is by the
		// name of the string contained by 'imageFileName'
		if ((pf = fopen(imageFileName, "rb")) == NULL)
		{
			// alert the user and save into the log file 
			// the event that the program cannot open the 
			// file containing the image to process
			std::cout << "Cannot open file: " << imageFileName << endl;
			exit(0);

		}
		else { // else

			double number;

			for (int i1 = 0; i1 < m_rcyres; i1++) {// x dim

				for (int i2 = 0; i2 < m_rcxres; i2++) { // y dim

				// at each iteration of the two for loops
				// the program reads the pixel value from the
				// file containing the image and 
					fread(&number, sizeof(double), 1, pf);

					// assigns the pixel value 'number' to the
					// pixel value 'Sinc_Int.pointer->Signal[i1][i2]'
					// where 'Sinc_Int' invokes the pointer to the 
					// image 'Signal' at the matrix location '[i1][i2]'
					Sinc_Int.pointer->Signal[i1][i2] = (double)number;

				} // y dim

			}  // x dim 

			// close the file containg the image to process 	
			fclose(pf);


		} // else 
		/// read image file (end)

		std::cout << "Image data loaded" << endl;

		Sinc_Int.save();

		std::cout << "Now calculating the Direct Fourier transform" << endl;
		OnFourierTransform(imageFileName, m_rcxres, m_rcyres);

		std::cout << "Now Sampling..." << endl;
		OnSinc_Zoom(imageFileName, m_rcxres, m_rcyres, m_TheSamplingRate);

		std::cout << "Now Calculating the Fourier transform of the zoomed image" << endl;
		OnFourierTransform("Sinc_Image.img", m_rcxres, m_rcyres);

		std::cout << "End of Computation..." << endl;
		std::cout << endl;

		// delete the memory address
		// allocated to the images
		// do this by the command 'delete'
		// applied to the object 'Sinc_Int' which
		// invokes the pointer to the data
		// structure 'data' containing the image
		// 'Signal'
		delete Sinc_Int.pointer;

		// the object 'Sinc_Int' invokes the
		// class destructor
		Sinc_Int.~Sinc_Int_2023();

	} // run the program (end)

	return 0;
} // end of main 

void OnSinc_Zoom(char imageFileName[], int rcxres, int rcyres, double samplingRate)
{

	int NofXpixels = rcxres;
	int NofYpixels = rcyres;

	int i, j, index;
	double pi = 3.141592;

	double* Sinc = 0;
	double* signal = 0;

	double* edgeX = 0;
	double* edgeY = 0;
	double* nedgeX = 0;
	double* nedgeY = 0;

	double* regionXY = 0;
	double* regionXYnegx = 0;
	double* regionXYnegy = 0;
	double* regionXYnegxy = 0;

	double* spsincx = 0;
	double* spsincy = 0;
	double* snsincx = 0;
	double* snsincy = 0;

	double* TwoDsumsincxy = 0;
	double* TwoDsumsincnegx = 0;
	double* TwoDsumsincnegy = 0;
	double* TwoDsumsincnegxy = 0;

	double* SincXY = 0;

	double* KSpaceM = 0;
	double* BandWidth = 0;

	FILE* logfile;

	char logfilename[128] = "Sinc.log";

	if ((logfile = fopen(logfilename, "w+")) == NULL)
	{

		printf("%s\n %s\n", "Unable to open log File", "Now Exit");

		exit(0);

	}
	else { // allocate memory 


		if ((Sinc = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			exit(0);

		}

		if ((signal = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);

			exit(0);

		}

		if ((edgeX = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);

			exit(0);

		}

		if ((edgeY = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);

			exit(0);

		}

		if ((nedgeX = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);

			exit(0);

		}

		if ((nedgeY = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);

			exit(0);

		}

		if ((regionXY = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);

			exit(0);

		}

		if ((regionXYnegx = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);

			exit(0);

		}

		if ((regionXYnegy = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);

			exit(0);

		}

		if ((regionXYnegxy = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);

			exit(0);

		}

		if ((spsincx = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			exit(0);
		}

		if ((spsincy = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);

			exit(0);
		}

		if ((snsincx = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);

			exit(0);
		}

		if ((snsincy = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);

			exit(0);
		}

		if ((TwoDsumsincxy = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			exit(0);
		}

		if ((TwoDsumsincnegx = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);

			exit(0);
		}


		if ((TwoDsumsincnegy = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);

			exit(0);
		}

		if ((TwoDsumsincnegxy = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);

			exit(0);
		}


		if ((KSpaceM = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			exit(0);

		}

		if ((BandWidth = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Bandwidth data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);

			exit(0);

		}


		if ((SincXY = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Bandwidth data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			exit(0);

		}

	} // allocate memory 

	
	FILE* pf;
	char filename[328];
	double savedata = 0;
	double savedata2 = 0;


	//// initialize pointer
	for (i = 0; i < NofYpixels; i++)
	{
		for (j = 0; j < NofXpixels; j++)
		{

			index = ((j * NofYpixels) + i);

			
			*(Sinc + index) = (double)0.0;


			*(spsincx + index) = (double)0.0;

			*(spsincy + index) = (double)0.0;

			*(snsincx + index) = (double)0.0;

			*(snsincy + index) = (double)0.0;


			*(TwoDsumsincxy + index) = (double)0.0;

			*(TwoDsumsincnegx + index) = (double)0.0;

			*(TwoDsumsincnegy + index) = (double)0.0;

			*(TwoDsumsincnegxy + index) = (double)0.0;


			*(edgeX + index) = (double)0.0;

			*(edgeY + index) = (double)0.0;

			*(nedgeX + index) = (double)0.0;

			*(nedgeY + index) = (double)0.0;


			*(regionXY + index) = (double)0.0;

			*(regionXYnegx + index) = (double) 0.0;

			*(regionXYnegy + index) = (double) 0.0;

			*(regionXYnegxy + index) = (double) 0.0;

			*(KSpaceM + index) = (double)0.0;

			*(BandWidth + index) = (double)0.0;

			*(SincXY + index) = (double)0.0;

		}

	}

		// open the file containing the image to
		// process. The image to process is by the
		// name of the string contained by 'imageFileName'
		if ((pf = fopen(imageFileName, "rb")) == NULL)
		{
			// alert the user and save into the log file 
			// the event that the program cannot open the 
			// file containing the image to process
			std::cout << "Cannot open file: " << imageFileName << endl;

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);
			
			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

			exit(0);

		}
		else { // else

			double number;

			//// initialize pointer
			for (i = 0; i < NofYpixels; i++)
			{
				for (j = 0; j < NofXpixels; j++)
				{

					// at each iteration of the two for loops
					// the program reads the pixel value from the
					// file containing the image and 
					fread(&number, sizeof(double), 1, pf);

					index = ((j * NofYpixels) + i);

					*(signal + index) = (double)number;

				} // y dim

			}  // x dim 

			// close the file containg the image to process 	
			fclose(pf);


		} // else 
		/// read image file (end)


		// open the file containing the image to
	// process. The image to process is by the
	// name of the string contained by 'imageFileName'

	sprintf(filename, "%s%s", "K-SpaceM-", imageFileName);

	if ((pf = fopen(filename, "rb")) == NULL)
	{
		// alert the user and save into the log file 
		// the event that the program cannot open the 
		// file containing the image to process
		std::cout << "Cannot open file: " << filename << endl;

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);
			
			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // else

		double number;

		//// initialize pointer
		for (i = 0; i < NofYpixels; i++)
		{
			for (j = 0; j < NofXpixels; j++)
			{

				// at each iteration of the two for loops
				// the program reads the pixel value from the
				// file containing the image and 
				fread(&number, sizeof(double), 1, pf);

				index = ((j * NofYpixels) + i);

				*(KSpaceM + index) = (double)number;

			} // y dim

		}  // x dim 

		// close the file containg the image to process 	
		fclose(pf);

	} // else 
	/// read image file (end)

	// scale the K-space magnitude (begin)
	// Procedure to scale the K-space Magnitude (begin)
	// scale (begin) 
	double max = *(KSpaceM);
	double min = *(KSpaceM);

	double averageKspace = 0.0;

	/// compute max and min of data (begin)
	for (i = 0; i < rcyres; i++)
	{
		for (j = 0; j < rcxres; j++)
		{

			index = ((j * rcyres) + i);

			if (*(KSpaceM + index) > (double)max)

				max = (double)*(KSpaceM + index);

			if (*(KSpaceM + index) < (double)min)

				min = (double)*(KSpaceM + index);

			averageKspace += (double)*(KSpaceM + index);

		} // y dim

	}  // x dim
	/// compute max and min of data (end)

	averageKspace /= ((double)rcxres * rcyres);

	double sDevKspace = (double)0.0;

	for (i = 0; i < rcyres; i++)
	{
		for (j = 0; j < rcxres; j++)
		{

			index = ((j * rcyres) + i);

			sDevKspace += (double)pow(((double)*(KSpaceM + index) - (double)averageKspace), 2.0);

		} // y dim

	}  // x dim

	sDevKspace = (double)sqrt((double)sDevKspace / ((double)rcxres * rcyres));

	
    // calculate Bandwidth adaptively (begin)
	for (i = 0; i < rcyres; i++)
	{
		for (j = 0; j < rcxres; j++)
		{

			index = ((j * rcyres) + i);

			if (max == min) { *(BandWidth + index) = (double)1.0; }

			else {

			if ( (double)sDevKspace != (double)0.0 )
			{
				
				*(BandWidth + index) =  
					         
				(((double)*(KSpaceM + index) - averageKspace) / sDevKspace) / ((double)max - min);

			
				*(BandWidth + index) =   (double)1.0 /
						               ( (double)1.0 + exp(-(double)*(BandWidth + index) ) );
				
			}
			else { *(BandWidth + index) = (double)1.0; }
		
			
			}
		

		} // y dim

	}  // x dim
	// calculate Bandwidth adaptively (end)

	
	/// calculate the average value of the bandwidth so to be able to calculate the zoom factor (begin)
	// take the values from the user input
	double m_TheSamplingRate = ((double)samplingRate);
	// take the values from the user input

	std::cout << "Input Sampling Rate: " << m_TheSamplingRate << endl;
	
	int t, w;
	int di, dj;
	double dx, dy;
	double dtx, dty;
	double sincx, sincy;
	double toll = 0.000001;


		for (i = 0; i < NofYpixels; i++)
		{ ///calculate Sinc-Space data

			for (j = 0; j < NofXpixels; j++)
			{ ///calculate Sinc-Space data
				 
			      di = ((int)i - NofYpixels / 2);
				  dj = ((int)j - NofXpixels / 2);
				
				  w = ((j * NofYpixels) + i);

			double sum = 0.0;
			double sumedgex = 0.0;
			double sumedgey = 0.0;
			double sumnedgex = 0.0;
			double sumnedgey = 0.0;

			double sumregionxy = 0.0;
			double sumnregionXYnegx = 0.0;
			double sumnregionXYnegy = 0.0;
			double sumnregionXYnegxy = 0.0;

			double sumsincx = 0.0;
			double sumsincy = 0.0;
			double sumnsincx = 0.0;
			double sumnsincy = 0.0;

			double sumsincxy = 0.0;
			double sumsincnegx = 0.0;
			double sumsincnegy = 0.0;
			double sumsincnegxy = 0.0;

			double sumSinc = 0.0;

			for (int s = 0; s < NofYpixels; s++)
			{ ///calculate Sinc-Space data

				for (int p = 0; p < NofXpixels; p++)
				{

					t = ((p * NofYpixels) + s);

				  	dty = ((double)((double)s - NofYpixels / 2) * m_TheSamplingRate * (double)*(BandWidth + t));
					dtx = ((double)((double)p - NofXpixels / 2) * m_TheSamplingRate * (double)*(BandWidth + t));
				
					/// Whittaker-Shannon interpolation formula 2010
					dy = (double)pi * (2.0 * (double)*(BandWidth + t) * di - dty);
					dx = (double)pi * (2.0 * (double)*(BandWidth + t) * dj - dtx);
					/// Whittaker-Shannon interpolation formula 2010

					t = ((p * NofYpixels) + s);

					///// Whittaker-Shannon sampling 2010//////
					if ((double)fabs((double)dx) > toll)

						sincx = (double)sin((double)dx) / ((double)dx);

					else sincx = (double)1.0;

					if ((double)fabs((double)dy) > toll)

						sincy = (double)sin((double)dy) / ((double)dy);

					else sincy = (double)1.0;
					///// Whittaker-Shannon sampling 2010//////


					// Sinc Image (zoomed image)
					sum += (double) *(signal + t) * ((double)sincx * sincy);

					// Sinc Image (Filter)
					sumSinc += ((double)sincx * sincy);


					// 1D sinc functions
					if ( (double)sincx >= (double) 0.0 ) sumsincx += (double)sincx; // Sinc of Rx positive
					if ( (double)sincy >= (double) 0.0 ) sumsincy += (double)sincy; // Sinc of Ry positive

					if ( (double)sincx < (double) 0.0 ) sumnsincx += (double)sincx; // Sinc of Rx negative
					if ( (double)sincy < (double) 0.0 ) sumnsincy += (double)sincy; // Sinc of Ry negative


					// 2D sinc functions
					if ( (double)sincx >= (double) 0.0 && (double)sincy >= (double) 0.0 )
					
					{ sumsincxy += ((double)sincx * sincy); } // Sinc of Rxy positive
					
					else if ( (double)sincx < (double) 0.0 && (double)sincy >= (double) 0.0 )
					
					{ sumsincnegx += ((double)sincx * sincy); } // Sinc of Rxy when Rx negative and Ry positive

					else if ( (double)sincx >= (double) 0.0 && (double)sincy < (double) 0.0 )
					
					{ sumsincnegy += ((double)sincx * sincy); } // Sinc of Rxy when Rx positive and Ry negative

					else if ( (double)sincx < (double) 0.0 && (double)sincy < (double) 0.0 )
					
					{ sumsincnegxy += ((double)sincx * sincy); } // Sinc of Rxy when Rx negative and Ry negative


					// edges
					// Rx image when Rx is positive
					if ( (double)dx >= (double) 0.0 ) sumedgex += (double)dx;

					// Ry image when Ry is positive
					if ( (double)dy >= (double) 0.0 ) sumedgey += (double)dy;

					//Rx image when Rx is negative
					if ( (double)dx < (double) 0.0 ) sumnedgex += (double)dx;

					//Ry image when Ry is negative
					if ( (double)dy < (double) 0.0 ) sumnedgey += (double)dy;



					// regions images
					if ( (double)dx >= (double) 0.0 && (double)dy >= (double) 0.0 )
					
					{ sumregionxy += ((double)dx * dy); } // Rxy when Rx is positive and Ry is positive
					
					else if ( (double)dx < (double) 0.0 && (double)dy >= (double) 0.0 )
					
					{ sumnregionXYnegx += ((double)dx * dy); } // Rxy when Rx is negative and Ry is positive

					else if ( (double)dx >= (double) 0.0 && (double)dy < (double) 0.0 )
					
					{ sumnregionXYnegy += ((double)dx * dy); } // Rxy when Rx is positive and Ry is negative

					else if ( (double)dx < (double) 0.0 && (double)dy < (double) 0.0 )
					
					{ sumnregionXYnegxy += ((double)dx * dy); } // Rxy when Rx is negative and Ry is negative

				}
			}

			        // Sinc Image (zoomed image)
					*(Sinc + w) = (double)sum;

					// Sinc Image (Filter)
					*(SincXY + w) = (double)sumSinc;

					// 1D sinc functions
					// Sinc of Rx positive
					*(spsincx + w) = (double) sumsincx / ((double) NofXpixels * NofYpixels);

					// Sinc of Ry positive
					*(spsincy + w) = (double) sumsincy / ((double) NofXpixels * NofYpixels);

					// Sinc of Rx negative
					*(snsincx + w) = (double) sumnsincx / ((double) NofXpixels * NofYpixels);

					// Sinc of Ry negative
					*(snsincy + w) = (double) sumnsincy / ((double) NofXpixels * NofYpixels);


					// 2D sinc functions
					// Sinc of Rxy positive
					*(TwoDsumsincxy + w) = (double) sumsincxy / ((double) NofXpixels * NofYpixels);

					// Sinc of Rxy when Rx negative and Ry positive
					*(TwoDsumsincnegx + w) = (double) sumsincnegx / ((double) NofXpixels * NofYpixels);

					// Sinc of Rxy when Rx positive and Ry negative
					*(TwoDsumsincnegy + w) = (double) sumsincnegy / ((double) NofXpixels * NofYpixels);

					// Sinc of Rxy when Rx negative and Ry negative
					*(TwoDsumsincnegxy + w) = (double) sumsincnegxy / ((double) NofXpixels * NofYpixels);

				
					// Edges
					// Rx image when Rx is positive
					*(edgeX + w) = (double) sumedgex / ((double) pi * NofXpixels * NofYpixels);

					// Ry image when Ry is positive
					*(edgeY + w) = (double) sumedgey / ((double) pi * NofXpixels * NofYpixels);
					
					// Rx image when Rx is negative
					*(nedgeX + w) = (double) sumnedgex / ((double) pi * NofXpixels * NofYpixels);

					// Ry image when Ry is negative
					*(nedgeY + w) = (double) sumnedgey / ((double) pi * NofXpixels * NofYpixels);


					// regions images
					// Rxy when Rx is positive and Ry is positive
					*(regionXY + w) = (double) sumregionxy / ((double) pi * pi * NofXpixels * NofYpixels);

					// Rxy when Rx is negative and Ry is positive
					*(regionXYnegx + w) = (double) sumnregionXYnegx / ((double) pi * pi * NofXpixels * NofYpixels);

					// Rxy when Rx is positive and Ry is negative
					*(regionXYnegy + w) = (double) sumnregionXYnegy / ((double) pi * pi * NofXpixels * NofYpixels);

					// Rxy when Rx is negative and Ry is negative
					*(regionXYnegxy + w) = (double) sumnregionXYnegxy / ((double) pi * pi * NofXpixels * NofYpixels);


		} ///calculate Sinc-Space data

	} ///calculate Sinc-Space data


	///////////////////////calculate Variance of SincXY image (begin) ////////////////
	double* averageSincX = 0;

	if ((averageSincX = (double*)calloc(NofXpixels, sizeof(double))) == NULL)
	{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Bandwidth data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

			exit(0);

	}

	/// compute average (begin)
	for (j = 0; j < rcxres; j++)
	{
		for (i = 0; i < rcyres; i++)
		{

			index = ((j * rcyres) + i);

			*(averageSincX + j) += (double)*(SincXY + index);

		} // y dim

		*(averageSincX + j) /= (double)rcxres;

	}  // x dim
	/// compute average (end)

	double sDevSincXY_x_Sum = (double)0.0;

	for (j = 0; j < rcxres; j++)
	{

		double sDevSincXY_x = (double)0.0;

		for (i = 0; i < rcyres; i++)
		{

			index = ((j * rcyres) + i);

			sDevSincXY_x += (double)pow(((double)*(SincXY + index) - (double)*(averageSincX + j)), 2.0);

		} // y dim

		sDevSincXY_x = (double)sqrt((double)sDevSincXY_x / ((double)rcxres));

		sDevSincXY_x_Sum += (double)sDevSincXY_x;

	}  // x dim

	free(averageSincX);


	double* averageSincY = 0;

	if ((averageSincY = (double*)calloc(NofYpixels, sizeof(double))) == NULL)
	{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Bandwidth data: Exit");

			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);
			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

			exit(0);

	}

	/// compute average (begin)
	for (i = 0; i < rcyres; i++)
	{
		for (j = 0; j < rcxres; j++)
		{

			index = ((j * rcyres) + i);

			*(averageSincY + i) += (double)*(SincXY + index);

		} // y dim

		*(averageSincY + i) /= (double)rcyres;

	}  // x dim
	/// compute average (end)

	
	double sDevSincXY_y_Sum = (double)0.0;

	for (i = 0; i < rcyres; i++)
	{

		double sDevSincXY_y = (double)0.0;

		for (j = 0; j < rcxres; j++)
		{

			index = ((j * rcyres) + i);

			sDevSincXY_y += (double)pow(((double)*(SincXY + index) - (double)*(averageSincY + i)), 2.0);

		} // y dim

		sDevSincXY_y = (double)sqrt((double)sDevSincXY_y / ((double)rcyres));

		sDevSincXY_y_Sum += (double)sDevSincXY_y;

	}  // x dim

	free(averageSincY);
	
    fprintf(logfile, "%s\t%lf\n", "Sum of Variance along X Lines: ", sDevSincXY_x_Sum);
	fprintf(logfile, "%s\t%lf\n", "Sum of Variance along Y Lines: ", sDevSincXY_y_Sum);
	///////////////////////calculate Variance of SincXY image (end) ////////////////

	////////////////////////////sinc image (zoomed image)////////////////////
	sprintf(filename, "%s", "Sinc_Image.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(Sinc + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc image (zoomed image)////////////////////

	////////////////////////////sinc of Rx positive image //////////////
	sprintf(filename, "%s", "Spsincx.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(spsincx + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc of Rx positive image //////////////

	////////////////////////////sinc of Ry positive image //////////////
	sprintf(filename, "%s", "Spsincy.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(spsincy + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc of Ry positive image //////////////

	////////////////////////////sinc of Rx negative image //////////////
	sprintf(filename, "%s", "Snsincx.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(snsincx + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc of Rx negative image //////////////

	////////////////////////////sinc of Ry negative image //////////////
	sprintf(filename, "%s", "Snsincy.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(snsincy + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc of Ry negative image //////////////

	////////////////////////////sinc of Rxy positive image //////////////
	sprintf(filename, "%s", "TwoDsumsincxy.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(TwoDsumsincxy + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
    ////////////////////////////sinc of Rxy positive image //////////////

	////////////////////////////sinc of Rxy when Rx negative and Ry positive //////////////
	sprintf(filename, "%s", "TwoDsumsincnegx.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(TwoDsumsincnegx + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc of Rxy when Rx negative and Ry positive //////////////

	////////////////////////////sinc of Rxy when Rx positive and Ry negative //////////////
	sprintf(filename, "%s", "TwoDsumsincnegy.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(TwoDsumsincnegy + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc of Rxy when Rx positive and Ry negative //////////////

	////////////////////////////sinc of Rxy when Rx negative and Ry negative //////////////
	sprintf(filename, "%s", "TwoDsumsincnegxy.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Sinc Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Sinc image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(TwoDsumsincnegxy + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data
	////////////////////////////sinc of Rxy when Rx negative and Ry negative //////////////


	//////////////////////////// Rx image when Rx is positive //////////////
	sprintf(filename, "%s", "EdgeX.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Edge Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Edge image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(edgeX + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Edge image Saved");

		fclose(pf);
	} // save data
	//////////////////////////// Rx image when Rx is positive //////////////

	//////////////////////////// Ry image when Ry is positive //////////////
	sprintf(filename, "%s", "EdgeY.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Edge Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Edge image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(edgeY + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Edge image Saved");

		fclose(pf);
	} // save data
	//////////////////////////// Ry image when Ry is positive //////////////

	//////////////////////////// Rx image when Rx is negative //////////////
	sprintf(filename, "%s", "nEdgeX.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Edge Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Edge image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(nedgeX + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Edge image Saved");

		fclose(pf);
	} // save data
	//////////////////////////// Rx image when Rx is negative //////////////

	//////////////////////////// Ry image when Ry is negative //////////////
	sprintf(filename, "%s", "nEdgeY.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Edge Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Edge image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(nedgeY + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Edge image Saved");

		fclose(pf);
	} // save data
	//////////////////////////// Ry image when Ry is negative //////////////

	//////////////////////////// Rxy image when Rx is positive and Ry is positive //////////////////
	sprintf(filename, "%s", "RegionXY.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Region Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Region image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(regionXY + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Region image Saved");

		fclose(pf);
	} // save data
	//////////////////////////// Rxy image when Rx is positive and Ry is positive //////////////////

	//////////////////////////// Rxy image when Rx is negative and Ry is positive //////////////////
	sprintf(filename, "%s", "RegionXYnegx.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Region Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Region image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(regionXYnegx + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Region image Saved");

		fclose(pf);
	} // save data
	//////////////////////////// Rxy image when Rx is negative and Ry is positive //////////////////

	//////////////////////////// Rxy image when Rx is positive and Ry is negative //////////////////
	sprintf(filename, "%s", "RegionXYnegy.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Region Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Region image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(regionXYnegy + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Region image Saved");

		fclose(pf);
	} // save data
    //////////////////////////// Rxy image when Rx is positive and Ry is negative //////////////////
	
	//////////////////////////// Rxy image when Rx is negative and Ry is negative //////////////////
	sprintf(filename, "%s", "RegionXYnegxy.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Region Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Region image");

			// FIFO memory deallocation method
			free(Sinc);
			free(signal);
			free(edgeX);
			free(edgeY);
			free(nedgeX);
			free(nedgeY);
			free(regionXY);
			free(regionXYnegx);
			free(regionXYnegy);
			free(regionXYnegxy);

			free(spsincx);
			free(spsincy);
			free(snsincx);
			free(snsincy);

			free(TwoDsumsincxy);
			free(TwoDsumsincnegx);
			free(TwoDsumsincnegy);
			free(TwoDsumsincnegxy);

			free(KSpaceM);
			free(BandWidth);

			free(SincXY);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(regionXYnegxy + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Region image Saved");

		fclose(pf);
	} // save data
	//////////////////////////// Rxy image when Rx is negative and Ry is negative //////////////////

	sprintf(filename, "%s", "Bandwidth.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Edge Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save Edge image");

	// FIFO memory deallocation method
	free(Sinc);
	free(signal);
	free(edgeX);
	free(edgeY);
	free(nedgeX);
	free(nedgeY);
	free(regionXY);
	free(regionXYnegx);
	free(regionXYnegy);
	free(regionXYnegxy);

	free(spsincx);
	free(spsincy);
	free(snsincx);
	free(snsincy);

	free(TwoDsumsincxy);
	free(TwoDsumsincnegx);
	free(TwoDsumsincnegy);
	free(TwoDsumsincnegxy);
	
	free(KSpaceM);
	free(BandWidth);

	free(SincXY);

	exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(BandWidth + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Bandwidth image Saved");

		fclose(pf);
	} // save data


	sprintf(filename, "%s", "SincXY.img");

	fprintf(logfile, "%s\t%s\n", "Now Saving Edge Image in File: ", filename);

	if ((pf = fopen(filename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save SincXY image");

	// FIFO memory deallocation method
	free(Sinc);
	free(signal);
	free(edgeX);
	free(edgeY);
	free(nedgeX);
	free(nedgeY);
	free(regionXY);
	free(regionXYnegx);
	free(regionXYnegy);
	free(regionXYnegxy);

	free(spsincx);
	free(spsincy);
	free(snsincx);
	free(snsincy);

	free(TwoDsumsincxy);
	free(TwoDsumsincnegx);
	free(TwoDsumsincnegy);
	free(TwoDsumsincnegxy);
	
	free(KSpaceM);
	free(BandWidth);

	free(SincXY);

	exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save TF data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(SincXY + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save Z-Space data

		fprintf(logfile, "%s\n", "Sinc image Saved");

		fclose(pf);
	} // save data


	printf("%s\n", "Sampling Completed");
	fprintf_s(logfile, "%s\n", "Sampling Completed");

	fclose(logfile);

	// FIFO memory deallocation method
	free(Sinc);
	free(signal);
	free(edgeX);
	free(edgeY);
	free(nedgeX);
	free(nedgeY);
	free(regionXY);
	free(regionXYnegx);
	free(regionXYnegy);
	free(regionXYnegxy);

	free(spsincx);
	free(spsincy);
	free(snsincx);
	free(snsincy);

	free(TwoDsumsincxy);
	free(TwoDsumsincnegx);
	free(TwoDsumsincnegy);
	free(TwoDsumsincnegxy);
	
	free(KSpaceM);
	free(BandWidth);

	free(SincXY);

} // end of function

void OnFourierTransform(char imageFilename[], int rcxres, int rcyres)
{

	int NofXpixels = rcxres;
	int NofYpixels = rcyres;

	int i, j, index;
	int dx, dy;
	int ds, dp;
	int k2, k3, w, t;

	double pi = 3.141592;

	double* kSpaceR = 0;
	double* kSpaceI = 0;
	double* Signal = 0;

	FILE* logfile;

	char logfilename[128] = "Fourier-T.log";

	if ((logfile = fopen(logfilename, "w+")) == NULL)
	{

		printf("%s\n %s\n", "Unable to open log File", "Now Exit");

		exit(0);

	}
	else { // allocate memory 


		if ((kSpaceR = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			exit(0);

		}

		if ((kSpaceI = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			// FIFO memory deallocation method
			free(kSpaceR);
			exit(0);

		}

		if ((Signal = (double*)calloc(NofXpixels * NofYpixels, sizeof(double))) == NULL)
		{

			fprintf(logfile, "%s\n", "Not enough memory to allocate Real Image data: Exit");

			// FIFO memory deallocation method
			free(kSpaceR);
			free(kSpaceI);
			exit(0);

		}

	} // allocate memory 

	//// read image data and initialize pointers
	double number = 0.0;

	for (i = 0; i < NofYpixels; i++)
	{
		for (j = 0; j < NofXpixels; j++)
		{

			index = ((j * NofYpixels) + i);

			*(kSpaceR + index) = (double)0.0;

			*(kSpaceI + index) = (double)0.0;

		}

	}

	FILE* pf;
	char SignalFilename[128];
	double readData;

	sprintf(SignalFilename, "%s", imageFilename);

	if ((pf = fopen(SignalFilename, "rb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to read Signal");

		// FIFO memory deallocation method
		free(kSpaceR);
		free(kSpaceI);
		free(Signal);

		exit(0);

	}
	else { // read data


		for (i = 0; i < rcyres; i++)
		{ ///read signal data
			for (j = 0; j < rcxres; j++)
			{

				index = ((j * rcyres) + i);

				fread(&readData, sizeof(double), 1, pf);

				*(Signal + index) = (double)readData;

			}
		} ///read signal data

		fprintf(logfile, "%s\n", "Signal Read in DOUBLE (64bits) format");

		fclose(pf);
	} // save data

	double phase, complexR, complexI;

	///// Fourier Transform //////
	for (i = 0; i < NofYpixels; i++)
	{ ///calculate k-space data

		for (j = 0; j < NofXpixels; j++)
		{


			dx = ((int)i - NofYpixels / 2);
			dy = ((int)j - NofXpixels / 2);

			k2 = ((int)(dy * NofYpixels) + dx);

			w = ((j * NofYpixels) + i);

			for (int s = 0; s < NofYpixels; s++)
			{ ///calculate k-space data 
				for (int p = 0; p < NofXpixels; p++)
				{


					ds = ((int)s - NofYpixels / 2);
					dp = ((int)p - NofXpixels / 2);

					k3 = ((int)(ds * NofXpixels) + dp);

					t = ((p * NofYpixels) + s);

					phase = ((double)(2.0 * pi * k2 * k3) / ((double)NofXpixels * NofYpixels));

					//** nayuki.eigenstate.org/page/how-to-implement-the-discrete-fourier-transform (begin)**/
					complexR = (double)cos((double)phase) + (double)sin((double)phase);

					complexI = -(double)sin((double)phase) + (double)cos((double)phase);
					//** nayuki.eigenstate.org/page/how-to-implement-the-discrete-fourier-transform (end)**/

					*(kSpaceR + w) += (double)*(Signal + t) * (double)complexR;

					*(kSpaceI + w) += (double)*(Signal + t) * (double)complexI;

				}

			}///calculate k-space data 


		}
	} ///calculate k-space data

	///// Fourier Transform //////
	double savedata = 0.0;
	char FTfilename[128];

	sprintf(FTfilename, "%s%s", "K-SpaceR-", imageFilename);

	fprintf(logfile, "%s\t%s\n", "Now Saving K-Space Signal (Real) in File: ", FTfilename);

	if ((pf = fopen(FTfilename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save K-Space Signal");


		// FIFO memory deallocation method
		free(kSpaceR);
		free(kSpaceI);
		free(Signal);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save k-space data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(kSpaceR + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save k-space data

		fprintf(logfile, "%s\n", "K-Space Signal (Real) Saved");

		fclose(pf);
	} // save data



	sprintf(FTfilename, "%s%s", "K-SpaceI-", imageFilename);

	fprintf(logfile, "%s\t%s\n", "Now Saving K-Space Signal (Imaginary) in File: ", FTfilename);

	if ((pf = fopen(FTfilename, "wb+")) == NULL)
	{

		fprintf(logfile, "%s\n", "Cannot open file to save K-Space Signal");

		// FIFO memory deallocation method
		free(kSpaceR);
		free(kSpaceI);
		free(Signal);

		exit(0);

	}
	else { // save data


		for (i = 0; i < NofYpixels; i++)
		{ ///save k-space data
			for (j = 0; j < NofXpixels; j++)
			{

				index = ((j * NofYpixels) + i);

				savedata = (double)*(kSpaceI + index);

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} ///save k-space data

		fprintf(logfile, "%s\n", "K-Space Signal (Imaginary) Saved");

		fclose(pf);

	} // save data

	sprintf(FTfilename, "%s%s", "K-SpaceM-", imageFilename);

	fprintf_s(logfile, "%s\t%s\n", "Now Saving K-Space Magnitude of the Signal in File: ", FTfilename);

	if ((pf = fopen(FTfilename, "wb+")) == NULL)
	{

		fprintf_s(logfile, "%s\n", "Cannot open file to save K-Space Magnitude of the Signal");

		// FIFO memory deallocation method
		free(kSpaceR);
		free(kSpaceI);
		free(Signal);

		exit(0);

	}
	else { // save data

	 // K-Space Magnitude (begin)
		for (int s = 0; s < (int)NofYpixels; s++)
		{
			for (int p = 0; p < (int)NofXpixels; p++)
			{


				index = ((p * NofYpixels) + s);

				savedata = (double)sqrt((double)*(kSpaceR + index) * (double)*(kSpaceR + index) +
					                    (double)*(kSpaceI + index) * (double)*(kSpaceI + index));

				fwrite(&savedata, sizeof(double), 1, pf);

			}
		} // K-Space Magnitude (end)

		fprintf_s(logfile, "%s\n", "K-Space Magnitude of the Signal Saved");

		fclose(pf);
	} // save data


	printf("%s\n", "FT Processing Completed");
	fprintf_s(logfile, "%s\n", "FT Processing Completed");

	fclose(logfile);


	// FIFO memory deallocation method
	free(kSpaceR);
	free(kSpaceI);
	free(Signal);

}