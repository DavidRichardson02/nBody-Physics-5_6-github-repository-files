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



/*
In my c++ hashed octree class below, while I am constructing the tree by inserting bodies within the 'insertBody' function I frequently create child nodes and then insert them into the hash map of nodes by calling my 'insertHOTNode' function on the child node. This process if working as intended, however, I am confused as to why I am unable to delete these child nodes after inserting them into the hashmap of octree nodes, 'nodes', without creating some error. I was under the impression that both the nodes themselves and the keys to the nodes are stored in the hash map of nodes: 'std::unordered_map nodes'. Please review my c++ code for this below and provide an explanation for why this is.

 Here is my c++ code:


#pragma once
#include "MortonKeys.h"
#include "Body.h"
#include "HashedNode.h"




class HashedOctree
{
public:

	HashedOctree();
	HashedOctree(HOTNode* rootNode);
	HashedOctree(const OctantBounds& rootBounds);
	~HashedOctree();





	HOTNode* getParentNode(const HOTNode* childNode);
	HOTNode* getChildNode(const HOTNode* parentNode);
	HOTNode* lookUpNode(const spatialKey code);




	void insertHOTNode(HOTNode* & node);
	void insertBody(HOTNode*& node, const Body& body);
	void pushHOTNodeBodyToChild(HOTNode* node);


	// Hash map to nodes with Morton code as key
	std::unordered_map<spatialKey, HOTNode*> nodes;

};



static inline void buildHashedOctreeInPlace(HashedOctree& HTree, Body*&bodies, const size_t& numBodies, OctantBounds& domainBounds)
{




HTree.clear();
HTree = HashedOctree(domainBounds);


HOTNode* rootNode =HTree.lookUpNode(ROOT_KEY);
for (size_t i = 0; i < numBodies; i++)
{
	HTree.insertBody(rootNode, bodies[i].position, bodies[i].mass);
}

}


inline int BarnesHutHOTMAC(HOTNode* node, Vec3D bodyPosition, double theta)
{
	// node's diamater / distance-to-bodyPosition < theta
	Vec3D distance = node->baryCenter - bodyPosition;

	double distSquared = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;

	return(4.0 * node->nodeBounds.size * node->nodeBounds.size < distSquared * theta * theta);
}


inline void ComputeHOTOctreeForce(LinearHashedOctree& LHTree, Body*& bodies, Vec3D*& bodiesAccelerations, const size_t& numBodies, HOTNode**& walkList, HOTNode**& interactList, double thetaMAC)
{
	omp_set_num_threads(NUM_THREADS);
	long interactionListLength = 0;



#pragma omp parallel
	{
		int id = omp_get_thread_num(); // current thread
		int numThreads = omp_get_num_threads(); // Get the number of threads inside parallel section


		// Calculate start and end indices based on id and numThreads
		int bodiesPerThread = numBodies / numThreads;
		int remainderBodies = numBodies % numThreads;//(int)numBodies - (numThreads * bodiesPerThread);



		// Thread-local copies of walkList and interactList.

	// Thread assignment: Distribute extra bodies to the first 'remainderBodies' threads.
		size_t start = id * bodiesPerThread;
		size_t end = start + bodiesPerThread;

		if (id < remainderBodies)
		{
			start += id;  // One extra body for this thread.
			end += (id + 1);  // End index will be one more.
		}
		else
		{
			start += remainderBodies; // All extra bodies distributed before this thread.
			end += remainderBodies; // Same here.
		}



		// Thread-local copies of walkList and interactList.
		HOTNode** localWalkList = new HOTNode * [end - start];  // Set your maximum size
		HOTNode** localInteractList = new HOTNode * [end - start];  // Set your maximum size



#pragma omp for
		for (int i = start; i < end; i++)
		{

			interactionListLength = TraverseHOTInteractionList(LHTree, bodies[i].position, thetaMAC, localWalkList, localInteractList);//walkList, interactList);
			ComputeHOTForceInteractionList(bodies[i].position, bodies[i].mass, bodiesAccelerations[i], localInteractList, interactionListLength);

		}
		// Free thread-local storage
		delete[] localWalkList;
		delete[] localInteractList;
	}
}

inline void ComputeHOTForceInteractionList(Vec3D& bodyPosition, double& bodyMass, Vec3D& acceleration, HOTNode**& interactList, long listLength)
{
	acceleration = { 0,0,0 };

	HOTNode* node;
	double dx = 0, dy = 0, dz = 0, D1 = 0, D2 = 0;
	double qx = 0, qy = 0, qz = 0;


	for (int i = 0; i < listLength; i++)
	{

		node = interactList[i];
		//convert r to c.o.m. referece frame of the node.
		dx = node->baryCenter.x - bodyPosition.x;
		dy = node->baryCenter.y - bodyPosition.y;
		dz = node->baryCenter.z - bodyPosition.z;


		// m*r / |r|^3 : normal monopole part/direct interaction
		D2 = dx * dx + dy * dy + dz * dz;
		D2 += SOFTENING * SOFTENING;

		D1 = 1.0 / sqrt(D2); //hopefully compiler catches the one over sqrt
		D2 = 1.0 / D2;
		D1 = D1 * D2; // 1/D3

		//m*r / |r|^3
		acceleration.x += node->mass * dx * D1;
		acceleration.y += node->mass * dy * D1;
		acceleration.z += node->mass * dz * D1;


		if (node->N > 1)//just did monopole so now quadrupole approximate
		{

			//Q.r / |r|^5; recall quadMom is only the upper triangle of symmetric tensor
			qx = node->quadrupoleMoment[0] * dx + node->quadrupoleMoment[1] * dy + node->quadrupoleMoment[2] * dz;
			qy = node->quadrupoleMoment[1] * dx + node->quadrupoleMoment[3] * dy + node->quadrupoleMoment[4] * dz;
			qz = node->quadrupoleMoment[2] * dx + node->quadrupoleMoment[4] * dy + node->quadrupoleMoment[5] * dz;

			D1 *= D2; // 1/D5 now
			acceleration.x -= qx * D1;
			acceleration.y -= qy * D1;
			acceleration.z -= qz * D1;



			//5*r.Q.r*r / 2*|r|^7
			qx = dx * qx + dy * qy + dz * qz;
			qx *= 2.5;
			D1 *= D2; // 1/D7 now


			acceleration.x += qx * dx * D1;
			acceleration.y += qx * dy * D1;
			acceleration.z += qx * dz * D1;
		}
	}

}




//intended to generate the interaction list for a given bodyPosition
//The list will include either leaf nodes (individual bodies) or internal nodes (groups of bodies)
//that satisfy the MAC (multipole acceptance criterion) for approximation.
static inline long TraverseHOTInteractionList(LinearHashedOctree& LHTree, Vec3D& bodyPosition, double theta, HOTNode**& walkList, HOTNode**& interactList)
{
	if (LHTree.nodes.empty())
	{
		return 0;
	}
	HOTNode* node;
	HOTNode* child;
	walkList[0] = LHTree.lookUpNode(ROOT_KEY);
	long walkIdx = 1;
	long intIdx = 0;
	while (walkIdx > 0)
	{
		node = walkList[--walkIdx];
		if (node->childByte != 0)
		{
			for (int i = 0; i < 8; ++i)
			{
				if (!HasOctChild(node->childByte, static_cast<OctantEnum>(i))) 
				{
					continue;
				}
				child = LHTree.lookUpNode(GetChildKey(node->nodeKey, static_cast<OctantEnum>(i)));
				if (BarnesHutHOTMAC(child, bodyPosition, theta))
				{
					interactList[intIdx++] = child;
				}
				else
				{
					walkList[walkIdx++] = child;
				}
			}
		}
		else if (node->baryCenter != bodyPosition)
		{
			interactList[intIdx++] = node;
		}
	}
	return intIdx;
}


#include "HashedOctree.h"





HashedOctree::HashedOctree()
{

}


HashedOctree::HashedOctree(HOTNode* rootNode)
{
	rootNode->nodeKey = ROOT_KEY;

	nodes.emplace(rootNode->nodeKey, rootNode);

}


HashedOctree::HashedOctree(const OctantBounds& rootBounds)
{
	HOTNode* rootNode = new HOTNode(rootBounds, ROOT_KEY);;

	nodes.emplace(rootNode->nodeKey, rootNode);

}




HashedOctree::~HashedOctree()
{
	for (auto& pair : nodes)
	{
		delete pair.second;
	}

	nodes.clear();
}



HOTNode* HashedOctree::getParentNode(const HOTNode* childNode)
{
	const spatialKey parentNode = childNode->nodeKey >> 3;
	return lookUpNode(parentNode);
}


HOTNode* HashedOctree::getChildNode(const HOTNode* parentNode)
{
	const spatialKey childNode = parentNode->nodeKey << 3;
	return lookUpNode(childNode);
}

HOTNode* HashedOctree::lookUpNode(const spatialKey code)
{
	const auto iter = nodes.find(code);
	return (iter == nodes.end() ? nullptr : iter->second);
}


void HashedOctree::insertHOTNode(HOTNode*& node)
{
	if (nodes[node->nodeKey])
	{
		nodes.erase(node->nodeKey);
		delete node;

	}
	nodes[node->nodeKey] = node; //add node to hashmap
}



void HashedOctree::insertBody(HOTNode*& node, const Vec3D bodyPosition, const double bodyMass)
{

	assert(!nodes.empty());
	spatialKey currentKey;
	OctantEnum targetOctant;



	if (node == nullptr)
	{
		currentKey = ROOT_KEY;
		node = lookUpNode(currentKey);
	}

	else
	{
		currentKey = node->nodeKey;
	}


	// Traverse the tree to find the appropriate insertion point for the body.
	while (node != nullptr)
	{
		// The target Node is empty
		if (node->N == 0)
		{
			node->mass = bodyMass;
			node->baryCenter = bodyPosition;
			node->N = 1;
			node->childByte = 0;

			node = nullptr;
		}

		//The target Node has Multiple Bodies
		else if (node->N > 1)
		{

			targetOctant = DetermineOctant(node->nodeBounds.center, bodyPosition);
			node->N = node->N + 1;


			if (HasOctChild(node->childByte, targetOctant))
			{

				spatialKey childKey = GetChildKey(currentKey, targetOctant);
				currentKey = childKey;


				node = lookUpNode(currentKey);
			}


			else
			{



				HOTNode* childNode = new HOTNode(node, targetOctant, bodyPosition, bodyMass);


				SetOctChild(node->childByte, targetOctant);


				insertHOTNode(childNode);


				node = nullptr;
			}
		}

		//The target Node has a Single Body    
		else if (node->N == 1)
		{
			// Push the existing body down to a child node, making space for a new body.
			pushHOTNodeBodyToChild(node);


			targetOctant = DetermineOctant(node->nodeBounds.center, bodyPosition);

			node->N = node->N + 1;

			if (HasOctChild(node->childByte, targetOctant))
			{

				spatialKey childKey = GetChildKey(currentKey, targetOctant);
				currentKey = childKey;


				node = lookUpNode(currentKey);
			}
			else
			{

				HOTNode* childNode = new HOTNode(node, targetOctant, bodyPosition, bodyMass);


				SetOctChild(node->childByte, targetOctant);


				insertHOTNode(childNode);


				node = nullptr;
			}
		}
	}

	pruneEmptyNodes(node);
}



void HashedOctree::pushHOTNodeBodyToChild(HOTNode* node)
{
	if (node == nullptr || node->N != 1)
	{
		return;
	}

	OctantEnum targetOctant = GetOctantFromKey(node->nodeKey);

	HOTNode* childNode = new HOTNode(node, targetOctant, node->baryCenter, node->mass);
	SetOctChild(node->childByte, targetOctant);
	insertHOTNode(childNode);
}



*/