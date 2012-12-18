#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "NetworkManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ViergewinntApp : public AppBasic {
public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

private:
	NetworkManager networkManager;
};

void ViergewinntApp::setup()
{
	networkManager.listen();
}

void ViergewinntApp::mouseDown( MouseEvent event )
{
	networkManager
		.createMessage()
		.add("version", 1)
		.add("clienttype", 0)
		.add("stage", 1)
		.add("clientname", "Mike")
		.makeJSON()
		.broadcast();
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
