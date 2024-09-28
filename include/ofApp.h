#pragma once

#include "ofMain.h"
#include "C:\ECE-270\of_v0.11.2_vs2017_release\of_v0.11.2_vs2017_release\apps\myApps\nBody Physics 5_6\Containers.h"
#include "C:\ECE-270\of_v0.11.2_vs2017_release\of_v0.11.2_vs2017_release\apps\myApps\nBody Physics 5_6\SequenceContainers.h"
#include "C:\ECE-270\of_v0.11.2_vs2017_release\of_v0.11.2_vs2017_release\apps\myApps\nBody Physics 5_6\MortonKeys.h"
#include "C:\ECE-270\of_v0.11.2_vs2017_release\of_v0.11.2_vs2017_release\apps\myApps\nBody Physics 5_6\ObjectPool.h"
#include "C:\ECE-270\of_v0.11.2_vs2017_release\of_v0.11.2_vs2017_release\apps\myApps\nBody Physics 5_6\Body.h"
#include "C:\ECE-270\of_v0.11.2_vs2017_release\of_v0.11.2_vs2017_release\apps\myApps\nBody Physics 5_6\HashedNode.h"
#include "C:\ECE-270\of_v0.11.2_vs2017_release\of_v0.11.2_vs2017_release\apps\myApps\nBody Physics 5_6\LinearHashedOctree.h"



class ofApp : public ofBaseApp
{
	size_t numBodies;
	Body* bodies;
	OctantBounds rootNodeBounds;
	LinearHashedOctree LHTree;

	double theta = 1;
	long interactionCount = 0, numInteractions = 0;
	double dt = 0.01;


public:

	/*----------------------   2D plane coordinate system navigation  ----------------------*/
	ofRectangle canvas;
	const float zoomBuffer = 0.1; // 10% buffer
	float maxZoomScale;  //the maximum viewable plane size will be ten times that of the initial plane size
	float minZoomScale;  //the minimal viewable plane size will be one tenth that of the initial plane size
	float zoomScale = 1.0;  //the variable to control the value of each scroll wheel input, i.e, how much each scroll changes the view
	float lastZoomScale = zoomScale;
	float zoomIncrement = 0.01;  //the variable to control the value of each scroll wheel input, i.e, how much each scroll changes the view
	ofVec3f dragStartPos;
	ofVec3f offset;
	float horizontalAngle = 0.0;
	float verticalAngle = 0.0;
	bool isDragging = false;
	bool visualizeTree = false;


	float horizontalAngleAtMousePress;
	float verticalAngleAtMousePress;


	ofVec3f panningVelocity;
	float zoomVelocity = 0.0;
	float horizontalRotationVelocity = 0.0;
	float verticalRotationVelocity = 0.0;
	float interfaceInertiaDamping = 0.9;  // adjust this for stronger/weaker inertia

	// Add a member variable to store the offset when the mouse is pressed
	ofVec3f offsetAtMousePress;

	ofVec3f targetOffset;
	bool isFocusing = false;
	float focusSpeed = 0.05;  // Control how fast the focusing happens



	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int x, int y, float scrollX, float scrollY);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

};






/*
*  Compute the acceleration for n bodies at positions r with mass m
 * making use of a hashed octree and a Barnes-Hut MAC for multipole approximation.
 * If a node is internal, the quadrupole moments are included in the potential
 * calculation.
 *


	1. Traverse the tree and count the number of nodes that





*/


*/
