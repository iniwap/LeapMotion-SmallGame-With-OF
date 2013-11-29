#include "LeapListener.h"

ofEvent<ofVec3f> LeapListener::GestureEvent = ofEvent<ofVec3f>(); 
ofEvent<ofPoint> LeapListener::FingerMoveEvent = ofEvent<ofPoint>();
ofEvent<ofPoint> LeapListener::FingerTouchEvent = ofEvent<ofPoint>();

void LeapListener::onInit(const Controller& controller) {
	std::cout << "Initialized" << std::endl;
}

void LeapListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
}

void LeapListener::onDisconnect(const Controller& controller) {
	//Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void LeapListener::onExit(const Controller& controller) {
	std::cout << "Exited" << std::endl;
}

void LeapListener::onFrame(const Controller& controller) {

	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();
	//std::cout << "Frame id: " << frame.id()
	//	<< ", timestamp: " << frame.timestamp()
	//	<< ", hands: " << frame.hands().count()
	//	<< ", fingers: " << frame.fingers().count()
	//	<< ", tools: " << frame.tools().count()
	//	<< ", gestures: " << frame.gestures().count() << std::endl;

	if (!frame.hands().empty()) {
		// Get the first hand
		const Hand hand = frame.hands()[0];

		// Check if the hand has any fingers
		const FingerList fingers = hand.fingers();
		if (!fingers.empty()) {
			//// Calculate the hand's average finger tip position
			//Vector avgPos;
			//for (int i = 0; i < fingers.count(); ++i) {
			//	avgPos += fingers[i].tipPosition();
			//}
			//avgPos /= (float)fingers.count();
			//std::cout << "Hand has " << fingers.count()
			//	<< " fingers, average finger tip position" << avgPos << std::endl;
			//////////////////////////////////////////////////////////////////////////
			ofPoint p = getScreenPosition(frame,fingers[0]);
			ofNotifyEvent(FingerMoveEvent,p);

			if(fingers[0].touchDistance()<0){
				ofPoint p1 = getScreenPosition(frame,fingers[0]);
				ofNotifyEvent(FingerTouchEvent,p1);
			}
		}

		//// Get the hand's sphere radius and palm position
		//std::cout << "Hand sphere radius: " << hand.sphereRadius()
		//	<< " mm, palm position: " << hand.palmPosition() << std::endl;

		//// Get the hand's normal vector and direction
		//const Vector normal = hand.palmNormal();
		//const Vector direction = hand.direction();

		//// Calculate the hand's pitch, roll, and yaw angles
		//std::cout << "Hand pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
		//	<< "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
		//	<< "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
	}

	// Get gestures
	const GestureList gestures = frame.gestures();
	for (int g = 0; g < gestures.count(); ++g) {
		Gesture gesture = gestures[g];

		switch (gesture.type()) {
		case Gesture::TYPE_CIRCLE:
			{
				CircleGesture circle = gesture;
				std::string clockwiseness;

				if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
					clockwiseness = "clockwise";
				} else {
					clockwiseness = "counterclockwise";
				}

				// Calculate angle swept since last frame
				float sweptAngle = 0;
				if (circle.state() != Gesture::STATE_START) {
					CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
					sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
				}
				std::cout << "Circle id: " << gesture.id()
					<< ", state: " << gesture.state()
					<< ", progress: " << circle.progress()
					<< ", radius: " << circle.radius()
					<< ", angle " << sweptAngle * RAD_TO_DEG
					<<  ", " << clockwiseness << std::endl;

				//////////////////////////////////////////////////////////////////////////
				ofVec3f gestureData = ofVec3f(circle.center().x,circle.center().y,float(Gesture::TYPE_CIRCLE));
	
				ofNotifyEvent(GestureEvent, gestureData);
				//////////////////////////////////////////////////////////////////////////

				break;
			}
		case Gesture::TYPE_SWIPE:
			{
				SwipeGesture swipe = gesture;
				std::cout << "Swipe id: " << gesture.id()
					<< ", state: " << gesture.state()
					<< ", direction: " << swipe.direction()
					<< ", speed: " << swipe.speed() << std::endl;

				//////////////////////////////////////////////////////////////////////////
				ofVec3f gestureData = ofVec3f(swipe.direction().x,swipe.direction().y,float(Gesture::TYPE_SWIPE));
	
				ofNotifyEvent(GestureEvent, gestureData);
				//////////////////////////////////////////////////////////////////////////


				break;
			}
		case Gesture::TYPE_KEY_TAP:
			{
				KeyTapGesture tap = gesture;
				std::cout << "Key Tap id: " << gesture.id()
					<< ", state: " << gesture.state()
					<< ", position: " << tap.position()
					<< ", direction: " << tap.direction()<< std::endl;
				break;
			}
		case Gesture::TYPE_SCREEN_TAP:
			{
				ScreenTapGesture screentap = gesture;
				std::cout << "Screen Tap id: " << gesture.id()
					<< ", state: " << gesture.state()
					<< ", position: " << screentap.position()
					<< ", direction: " << screentap.direction()<< std::endl;
				break;
			}
		default:
			std::cout << "Unknown gesture type." << std::endl;
			break;
		}
	}

	if (!frame.hands().empty() || !gestures.empty()) {
		std::cout << std::endl;
	}
}

void LeapListener::onFocusGained(const Controller& controller) {
	std::cout << "Focus Gained" << std::endl;
}

void LeapListener::onFocusLost(const Controller& controller) {
	std::cout << "Focus Lost" << std::endl;
}
ofPoint LeapListener::getScreenPosition(Frame frame,Finger finger){

	Vector stabilizedPosition = finger.stabilizedTipPosition();

	InteractionBox iBox = frame.interactionBox();
	Vector normalizedPosition = iBox.normalizePoint(stabilizedPosition);
	float x = normalizedPosition.x * WINDOW_WIDTH;
	float y = WINDOW_HEIGHT - normalizedPosition.y * WINDOW_HEIGHT;

	return ofPoint(x,y);
}