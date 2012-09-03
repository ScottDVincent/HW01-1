#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

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
	static const int appWidth = 800;
	static const int appHeight = 600;
	static const int textureSize = 1024;
	void rectangle(uint8_t* pixels, int startx, int starty, int width, int height, Color8u fill);
	void gradient(uint8_t* pixels, Color8u c1, Color8u c2);
	void tint(uint8_t* pixels, Color8u color);
	void circle(uint8_t* pixels, int centerx, int centery, int radius, int thickness, Color8u color);
	void blur(uint8_t* pixels);
};

void HW01App::prepareSettings(Settings* settings){
	(*settings).setWindowSize(appWidth, appHeight);
	(*settings).setResizable(false);
}

void HW01App::rectangle(uint8_t* pixels, int startx, int starty, int width, int height, Color8u fill){
	int x;
	int y;

	for (y = starty; y < (starty + width); y++){
		for(x = startx; x < (startx + width); x++){
			pixels[3*(x + y * textureSize)] = fill.r;
			pixels[3*(x + y * textureSize) + 1] = fill.g;
			pixels[3*(x + y * textureSize) + 2] = fill.b;
		}
	}
}

void HW01App::circle(uint8_t* pixels, int centerx, int centery, int radius, int thickness, Color8u color){
	if (radius <= 0) 
		return;
	for(int y = centery - radius; y <= centery + radius; y++){
		for(int x = centerx - radius; x <= centerx + radius; x++){
			int dist = (int)sqrt((double)((x-centerx)*(x-centerx) + (y-centery)*(y-centery)));
			if (dist <= radius && dist >= radius-thickness){
				pixels[3*(x + y * textureSize)] = color.r;
				pixels[3*(x + y * textureSize) + 1] = color.g;
				pixels[3*(x + y * textureSize) + 2] = color.b;
			}
		}
	}
}
	
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

void HW01App::tint(uint8_t* pixels, Color8u color){
	for(int y = 0; y < appHeight ; y++){
		for(int x = 0; x < appWidth; x++){
			pixels[3*(x + y*textureSize)] += color.r;
			pixels[3*(x + y*textureSize)+1] += color.g;
			pixels[3*(x + y*textureSize)+2] += color.b;
		}
	}
}

void HW01App::blur(uint8_t* pixels){
	Surface cloneSurface = Surface(textureSize, textureSize, false);
	uint8_t* dataArray = (cloneSurface).getData();
	dataArray = pixels;

}

void HW01App::setup(){
	mySurface_ = new Surface(textureSize, textureSize, false);

}

void HW01App::mouseDown( MouseEvent event )
{
}

void HW01App::update()
{
	uint8_t* dataArray = (*mySurface_).getData();

	gradient(dataArray, Color8u(255, 0, 0), Color8u(0, 0, 255));
	tint(dataArray, Color8u(0, 255, 0));
	rectangle(dataArray, 50, 100, 50, 100, Color8u(255, 0, 0));
	circle(dataArray, 400, 357, 78, 25, Color8u(0, 0, 0));
	blur(dataArray);
}

void HW01App::draw()
{
	gl::draw(*mySurface_);
}

CINDER_APP_BASIC( HW01App, RendererGl )
	