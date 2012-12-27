// Mike Schäkermann und Andreas Wallinger

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "NetworkManager.h"
#include "Stage.h"
#include "StageFindOpponent.h"
#include "cinder/params/Params.h"

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
	NetworkManager			networkManager;
	params::InterfaceGl		parameterMenu;
	Stage *					stage;
};

void ViergewinntApp::setup()
{
	networkManager.listen();
	parameterMenu = params::InterfaceGl("Please enter your ...", Vec2f(200, 150));
	stage = nullptr;

	setStage(new StageFindOpponent(networkManager, parameterMenu));
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

	if (stage->isAborted())
	{
		setStage(new StageFindOpponent(networkManager, parameterMenu, true, stage->wasAbortedByOpponent()));
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
