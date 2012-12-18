// Mike Schäkermann und Andreas Wallinger

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "NetworkManager.h"
//#include "Field.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ViergewinntApp : public AppBasic {
public:
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();

private:
	NetworkManager networkManager;
	//Field field;
};

void ViergewinntApp::setup()
{
	networkManager.listen();
	/*
	field.init();

	field.insert(0,1);
	field.insert(0,0);
	field.insert(2,1);
	field.insert(0,0);
	field.insert(0,1);
	field.insert(4,1);
	field.insert(0,1);
	*/
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

	//field.print();
}

void ViergewinntApp::keyDown( KeyEvent event )
{

}


void ViergewinntApp::update()
{
}

void ViergewinntApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
	//field.draw();
}

CINDER_APP_BASIC( ViergewinntApp, RendererGl )
