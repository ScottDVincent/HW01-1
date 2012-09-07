/*@note This project satisfies goals A.1 (rectangle), A.2 (circle), A.4 (gradient),
A.6 (tint), C (draw a picture), D (save the image for easy sharing)
*/

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class HW01App : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);

private:
	Surface* mySurface_; 
	Surface* cloneSurface_;
	int frame_number_;
	boost::posix_time::ptime app_start_time_;

	// Dimensions of Surface and Texture
	static const int appWidth = 800;
	static const int appHeight = 600;
	static const int textureSize = 1024;

	
	/**
	* This method draws a rectangle of width x height as given by the call 
	* pixels: the surface array
	* startx: beginning x coordinate
	* starty: beginning y coordinate
	* width: the width of rectangle
	* height: height of rectangle
	* fill: the (r,g,b) color
	*/
	void rectangle(uint8_t* pixels, int startX, int startY, int width, int height, Color8u fill);


	/**
	* This method draws a gradient across the Surface
	* pixels: the surface array
	* c1: the (r,g,b) color
	* c2: the (r,g,b) color	
	*/
	void gradient(uint8_t* pixels, Color8u c1, Color8u c2);


	/**
	* This method tints the Surface 
	* pixels: the surface array
	* color: the (r,g,b) color used to tint the underlying image
	*/
	void tint(uint8_t* pixels, Color8u color);


	/**
	* This method draws a circle of radius  
	* pixels: the surface array
	* centerX: beginning x coordinate
	* centerY: beginning y coordinate
	* radius: the radius of the circle from the (centerx, centery) coordinate
	* thickness: thickness of circle
	* color: the (r,g,b) color of the circle
	*/
	void circle(uint8_t* pixels, int centerX, int centerY, int radius, int thickness, Color8u color);
	

	/**
	* This method  imposes a convolution filter on the underlying image
	* pixels: the surface array to be convolved
	*/
	void blur(uint8_t* pixels);
};

void HW01App::prepareSettings(Settings* settings){
	(*settings).setWindowSize(appWidth, appHeight);
	(*settings).setResizable(false);
}

// The rectangle method. Displays a rectangle.
void HW01App::rectangle(uint8_t* pixels, int startX, int startY, int width, int height, Color8u fill){

	int x, y;

	// bounds checking
	// I could also write this with a (xBegin, yBegin) and (xEnd, yEnd) so that way I can more easily check for boundry errors
	if ((startX + width <= appWidth) && (startY + height <= appHeight )) {

	// to create the rectangle
			for (y = startY; y < (startY + height); y++){
				for(x = startX; x < (startX + width); x++){
					pixels[3*(x + y * textureSize)] = fill.r;
					pixels[3*(x + y * textureSize) + 1] = fill.g;
					pixels[3*(x + y * textureSize) + 2] = fill.b;
				}
			}

	} //end if
}


// The circle method! I thought it would be fun to make the line thickness variable, so I did.
void HW01App::circle(uint8_t* pixels, int centerX, int centerY, int radius, int thickness, Color8u color){
	if (radius <= 0) 
		return;

	// bounds checking
	if ((centerX + radius <= appWidth) && (centerY + radius <= appHeight )) {

	for(int y = centerY - radius; y <= centerY + radius; y++){
		for(int x = centerX - radius; x <= centerX + radius; x++){
			int dist = (int)sqrt((double)((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY)));
			if (dist <= radius && dist >= radius-thickness){
				pixels[3*(x + y * textureSize)] = color.r;
				pixels[3*(x + y * textureSize) + 1] = color.g;
				pixels[3*(x + y * textureSize) + 2] = color.b;
				} // end loop if
			}
		}

	} // end top if
}
	
// Method for the color gradient.
void HW01App::gradient(uint8_t* pixels, Color8u c1, Color8u c2){
	int y, x;
	Color8u G = Color8u(0,0,0);

	for(y = 0; y < appHeight ; y++){
		G.r = ((c2.r * y) + (c1.r *(appHeight-y)))/appHeight;
		G.g = ((c2.g * y) + (c1.g *(appHeight-y)))/appHeight;
		G.b = ((c2.b * y) + (c1.b *(appHeight-y)))/appHeight;
		for(x = 0; x < appWidth; x++){
			pixels[3*(x + y*textureSize)] = G.r;
			pixels[3*(x + y*textureSize)+1] = G.g;
			pixels[3*(x + y*textureSize)+2] = G.b;
		}
	}
}


// Color tint method
void HW01App::tint(uint8_t* pixels, Color8u color){
	for(int y = 0; y < appHeight ; y++){
		for(int x = 0; x < appWidth; x++){
			pixels[3*(x + y*textureSize)]   += color.r;
			pixels[3*(x + y*textureSize)+1] += color.g;
			pixels[3*(x + y*textureSize)+2] += color.b;
		}
	}
}



void HW01App::blur(uint8_t* pixels){
	static uint8_t work_buffer[3*textureSize*textureSize];

	//copy pixels (the Surface) into the work_buffer pseudo-array
	memcpy(work_buffer, pixels, (3*textureSize*textureSize) );
		

	int x;
	int y;
	int innerX;
	int innerY;
	int offset;
	int offset2;
	int k;
	
	// counters for the running sum of the pixel which is being convolved
	uint8_t total_red  =0;
	uint8_t total_green=0;
	uint8_t total_blue =0;

	// blur kernel
	float kernelA[9] = {1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0};
	// edge kernel
	uint8_t kernelB[9] = {1, 0, 1, 1, -1, 1, 1, 0, 1};

	
	// outer loop to iterate across image
	for(y = 0; y < appHeight - 1; y++){
		for(x = 0; x < appWidth - 1; x++){
			
			//update the offset to the next pixel in RowMajorOrder
			offset = 3*(x + y * appWidth);
			
			//reset the sum to 0 for each new pixel
			total_red = 0;
			total_green = 0;
			total_blue = 0;
			
			//start iterating thru the kernel. the kernel's bounds are (-1,-1) to (1,1)
			 for(innerY = -1; innerY <=  1; innerY++){
				for(innerX = -1; innerX <=  1; innerX++){			
					
					// set the offset for the kernel in relation to the texture, not really necessary
					//offset2 = 3*((x + innerX) + (y + innerY)*textureSize);
					
					// set the index of the kernel. have to +1 to actually start at (0,0) and end at (2,2)
					k = kernelB[innerX + 1 + ((innerY + 1) * 3)];
									
					// sum the multiplication of the kernel value on each color of the pixel
					total_red   += (work_buffer[offset  ] * k);
					total_green += (work_buffer[offset+1] * k);
					total_blue  += (work_buffer[offset+2] * k);

					
				}
			}
			
			// Assign the complete sum to the single pixel to create the new image
			pixels[offset] = total_red;
			pixels[offset + 1] = total_green;
			pixels[offset + 2] = total_blue;
		}
	}
}


void HW01App::setup(){
	frame_number_ = 0;
	mySurface_ = new Surface(textureSize, textureSize, false);
}

void HW01App::mouseDown( MouseEvent event ){
}

void HW01App::update()
{
	uint8_t* dataArray = (*mySurface_).getData();
	
	// Makes a pretty background
	gradient(dataArray, Color8u(0, 0, 255), Color8u(255, 0, 0));
	tint(dataArray, Color8u(0, 255, 0)); 
	
	// Kind of looks like a sun
	circle(dataArray, 650, 100, 75, 75, Color8u(255, 255, 0));

	// These together kind of look like a little stick guy
	rectangle(dataArray, 80, 450, 25, 100, Color8u(100, 12, 100));
	circle(dataArray, 100, 410, 50, 10, Color8u(100, 12, 100));
	circle(dataArray, 125, 395, 5, 5, Color8u(100, 12, 100));

	// call the blur funtion
	blur(dataArray);

	// Saves the first frame of the image
	if(frame_number_ == 0){
		writeImage("sbaker.png", *mySurface_);
		app_start_time_ = boost::posix_time::microsec_clock::local_time();
	}
	
	frame_number_++;
}

void HW01App::draw()
{
	gl::draw(*mySurface_);
}

CINDER_APP_BASIC( HW01App, RendererGl )
	