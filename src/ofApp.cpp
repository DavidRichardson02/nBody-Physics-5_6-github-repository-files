
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(120);
	ofSetBackgroundColor(0);
	ofSetCircleResolution(100);
	ofEnableAntiAliasing();
	ofSetEscapeQuitsApp(false);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();



	numBodies = 10000;
	bodies = new Body[numBodies];


	for (size_t i = 0; i < numBodies; i++)
	{
		bodies[i] = Body(Vec3D(ofRandom(-1500, 1500), ofRandom(-1500, 1500), ofRandom(-1500, 1500)), Vec3D(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1)), 1);
	}

	rootNodeBounds = { bodies, numBodies };
	for (int i = 0; i < numBodies; i++)
	{
		bodies[i].bodyKey = computeMortonKey(bodies[i].position, rootNodeBounds.size);
	}
	RadixSortBodies(bodies, numBodies);
	buildLinearHashedOctreeInPlace(LHTree, bodies, numBodies, rootNodeBounds);


	panningVelocity = ofVec3f(0, 0, 0);
	zoomVelocity = 0.0f;
	canvas.set(-5 * rootNodeBounds.size, -5 * rootNodeBounds.size, 10 * rootNodeBounds.size, 10 * rootNodeBounds.size);
	maxZoomScale = (canvas.width / ofGetWidth()) - zoomBuffer;
	minZoomScale = (ofGetWidth() / canvas.width) + zoomBuffer;
}

//--------------------------------------------------------------
void ofApp::update()
{
	rootNodeBounds = { bodies, numBodies };
	for (int i = 0; i < numBodies; i++)
	{
		bodies[i].bodyKey = computeMortonKey(bodies[i].position, rootNodeBounds.size);
	}
	RadixSortBodies(bodies, numBodies);




	///*
	panningVelocity *= interfaceInertiaDamping;
	zoomVelocity *= interfaceInertiaDamping;
	horizontalRotationVelocity *= interfaceInertiaDamping;
	verticalRotationVelocity *= interfaceInertiaDamping;
	offset += panningVelocity * interfaceInertiaDamping;
	zoomScale += zoomVelocity * interfaceInertiaDamping;
	horizontalAngle += horizontalRotationVelocity;
	verticalAngle += verticalRotationVelocity;
	if (isFocusing)
	{
		// Set offset to target object's position, inversely scaled by current zoom factor
		offset = targetOffset / zoomScale;
		offset += (offset + targetOffset) * focusSpeed;
		if ((targetOffset - offset).length() < 1.0f)  // Check if close enough
		{
			isFocusing = false;
		}
	}
	//*/
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(255);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 2), ofGetWidth() - 200, 45);
	ofDrawBitmapString("MAC: " + ofToString(theta, 2), ofGetWidth() - 200, 65);
	ofDrawBitmapString("numBodies: " + ofToString(numBodies, 2), ofGetWidth() - 200, 85);

	///*
	ofPushMatrix();
	ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5); // First, translate to center
	ofRotateDeg(ofRadToDeg(horizontalAngle), 0, 1, 0);   // Apply the rotations from the center of the screen // Rotate around Y
	ofRotateDeg(ofRadToDeg(verticalAngle), 1, 0, 0);  // Rotate around X
	ofScale(zoomScale, zoomScale, zoomScale); 	// Scale
	ofTranslate(-ofGetWidth() * 0.5 + offset.x, -ofGetHeight() * 0.5 + offset.y); 	// Translate back to original position or offset




	ComputePositionAtHalfTimeStep(dt, bodies, numBodies);


	buildLinearHashedOctreeInPlace(LHTree, bodies, numBodies, rootNodeBounds);

	if (visualizeTree)
	{
		LHTree.visualizeTree();
	}

	HOTNode** walkList = new  HOTNode * [numBodies]; // This should be based on the depth of your tree
	HOTNode** interactList = new  HOTNode * [numBodies];


	Vec3D* bodiesAccelerations;
	bodiesAccelerations = new Vec3D[numBodies];


	ComputeHOTOctreeForce(LHTree, bodies, bodiesAccelerations, numBodies, walkList, interactList, theta); //this function computes the accelerations from gravity for all bodies
	ComputeVelocityAndPosition(dt, bodies, numBodies, bodiesAccelerations);



	delete[] walkList;
	delete[] interactList;
	delete[] bodiesAccelerations;
	LHTree.deleteTree();


	VisualizeBodies(bodies, numBodies);

	ofPopMatrix();
	//*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'z') //reset zoom
	{
		zoomScale = 1;
		lastZoomScale = zoomScale;
		offset.set(0, 0);

		panningVelocity.set(0, 0, 0);  // Reset panning velocity
		horizontalRotationVelocity = 0.0f;
		verticalRotationVelocity = 0.0f;
		zoomVelocity = 0.0f;  // Reset zoom velocity
	}

	///*
	if (key == 'f')  // Set the target position
	{
		isFocusing = true;
		targetOffset = ofVec3f(-ofGetWidth() * 0.5 + ofGetMouseX(), -ofGetHeight() * 0.5 + ofGetMouseY(), 0);
		zoomScale = 1.0f;
	}

	if (key == 'v')
	{
		visualizeTree = !visualizeTree;
	}


	if (key == OF_KEY_UP)
	{
		theta = theta + 0.1;
	}
	if (key == OF_KEY_DOWN)
	{
		theta = theta - 0.1;
	}
	//*/

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{


	///*
	ofVec3f mousePos(x, y, 0);  // Z is 0 for basic mouse input
	ofVec3f dragOffset = mousePos - dragStartPos;








	if (button == 0) //left mouse button for panning
	{
		//
		/* -------- DIRECTIONAL VECTOR PANNING, LESS EXPENSIVE, LESS ACCURATE --------
		//*--- 2 of 3 three dimensions of panning stop working if the orientation of the scene is changed
		//*--- updirection has to assume 1 direction is up relative to the other 2, so when this changes it loses it's ability to
		//*--- calculate panning vectors based on the rotation, the viewer's horizontal and vertical angles have changed
		ofVec3f rightDirection(cos(horizontalAngle), 0, -sin(horizontalAngle));   // cross product with up vector (0,1,0)
		ofVec3f upDirection(sin(verticalAngle) * sin(horizontalAngle), cos(verticalAngle), sin(verticalAngle) * cos(horizontalAngle));
		// New depth direction for forward/backward movement based on viewpoint
		ofVec3f depthDirection(sin(horizontalAngle), 0, cos(horizontalAngle));
		// Adjust the offset based on the panning vectors. We're using dragOffset.y for depth.
		offset += rightDirection * dragOffset.x + upDirection * dragOffset.y + depthDirection * dragOffset.y;
		//*/



		///* -------- MATRIX INVERSION PANNING, MORE EXPENSIVE, MORE ACCURATE --------
		//*--- Uses a rotation matrix to transform the 2D drag into the 3D world space by applying the inverse of the current view rotation

		// Calculate 3D drag offset based on current rotation angles
		ofVec3f dragOffset3D(dragOffset.x, dragOffset.y, 0);

		// Create a rotation matrix based on the horizontal and vertical angles
		ofMatrix4x4 rotMatrix;
		rotMatrix.makeIdentityMatrix();
		rotMatrix.rotate(-ofRadToDeg(horizontalAngle), 0, 1, 0);
		rotMatrix.rotate(-ofRadToDeg(verticalAngle), 1, 0, 0);

		// Rotate the 3D drag offset by the inverse of the current rotation matrix
		ofVec3f rotatedOffset = dragOffset3D * rotMatrix.getInverse();

		// Add the rotated offset to the existing offset
		offset += rotatedOffset;
		//*/

	}
	else if (button == 2) //right mouse button for rotating
	{
		// Convert drag movement to spherical coordinates rotation
		float sensitivity = 0.005f;  // Adjust this value for rotation sensitivity 
		horizontalRotationVelocity = dragOffset.x * sensitivity;
		verticalRotationVelocity = -dragOffset.y * sensitivity;

		horizontalAngle += horizontalRotationVelocity;
		verticalAngle += verticalRotationVelocity;
	}

	dragStartPos = mousePos;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	isDragging = true;
	dragStartPos.set(x, y, 0);  // Z is 0 for basic mouse input


	if (button == 0) //left mouse button for panning
	{
		panningVelocity.set(0, 0, 0);  // Reset panning velocity
		offsetAtMousePress = offset;  // Store the offset value when the mouse is pressed
	}
	else if (button == 2) // right mouse button for rotating
	{
		horizontalRotationVelocity = 0.0;
		verticalRotationVelocity = 0.0;

		horizontalAngleAtMousePress = horizontalAngle;
		verticalAngleAtMousePress = verticalAngle;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	isDragging = false;
	if (button == 0)  // Panning
	{
		panningVelocity = (offset - offsetAtMousePress) * 0.1;  // Calculate velocity based on the difference from mouse press to release
	}
	else if (button == 2) // Rotating
	{
		horizontalRotationVelocity = (horizontalAngle - horizontalAngleAtMousePress) * 0.1;  // Calculate rotation velocity based on angle differences
		verticalRotationVelocity = (verticalAngle - verticalAngleAtMousePress) * 0.1;
	}
}




void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
	lastZoomScale = zoomScale;
	zoomVelocity = 0.0;  // Reset zoom velocity
	float scrollAmount = scrollY * zoomIncrement;  //receive amount scrolled, controlled by the arbitrarily defined zoomIncrement
	zoomScale += scrollAmount;   // Update zoomScale

	maxZoomScale = canvas.width / ofGetWidth() - zoomBuffer;
	minZoomScale = ofGetWidth() / canvas.width + zoomBuffer;
	zoomScale = ofClamp(zoomScale, minZoomScale, maxZoomScale);

	offset -= (ofVec3f(x - offset.x, y - offset.y) * (zoomScale - lastZoomScale)) / lastZoomScale;


	zoomVelocity = (zoomScale - lastZoomScale) * 0.1;  // multiplied by a factor for control
}



//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	maxZoomScale = canvas.width / ofGetWidth();
	minZoomScale = ofGetWidth() / canvas.width;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}





