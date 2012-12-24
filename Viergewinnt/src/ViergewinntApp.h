// Mike Schäkermann und Andreas Wallinger

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "NetworkManager.h"
#include "Stage.h"
#include "StageFindOpponent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ViergewinntApp : public AppBasic {
public:
	void setup();
	void mouseDown(MouseEvent event);
	void keyDown(KeyEvent event);
	void update();
	void draw();
	void setStage(Stage * newStage);
	NetworkManager & getNetworkManager();

private:
	NetworkManager networkManager;
	Stage * stage;
};

void ViergewinntApp::setup()
{
	networkManager.listen();
	
	stage = nullptr;
	setStage(new StageFindOpponent(networkManager));
}

void ViergewinntApp::mouseDown(MouseEvent event)
{
	stage->mouseDown(event);
}

void ViergewinntApp::keyDown(KeyEvent event)
{
	stage->keyDown(event);
}

void ViergewinntApp::update()
{
	stage->update();

	if (stage->isCancelled())
	{
		setStage(new StageFindOpponent(networkManager));
	}
	else if (stage->isComplete())
	{
		setStage(stage->getNextStage());
	}
}

void ViergewinntApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	stage->draw();
}

void ViergewinntApp::setStage(Stage * newStage)
{
	if (newStage != nullptr && newStage != stage)
	{
		networkManager.subscribeExclusively(*newStage);

		if (stage != nullptr)
		{
			delete stage;
		}

		stage = newStage;
	}
}

NetworkManager & ViergewinntApp::getNetworkManager()
{
	return networkManager;
}

CINDER_APP_BASIC( ViergewinntApp, RendererGl )
