// Mike Schäkermann und Andreas Wallinger

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

#include "Field.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ViergewinntApp : public AppBasic {

private:
	Field m_field;
public:
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();
};

void ViergewinntApp::setup()
{
	m_field.init();

	m_field.insert(0,1);
	m_field.insert(0,0);
	m_field.insert(2,1);
	m_field.insert(0,0);
	m_field.insert(0,1);
	m_field.insert(4,1);
	m_field.insert(0,1);
}

void ViergewinntApp::mouseDown( MouseEvent event )
{
	m_field.print();
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
	m_field.draw();
}

CINDER_APP_BASIC( ViergewinntApp, RendererGl )
