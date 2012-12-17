#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ViergewinntApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void ViergewinntApp::setup()
{
}

void ViergewinntApp::mouseDown( MouseEvent event )
{
}

void ViergewinntApp::update()
{
}

void ViergewinntApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_BASIC( ViergewinntApp, RendererGl )
