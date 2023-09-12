
#pragma once
#include "Containers.h"
#include "SequenceContainers.h"
#include "MortonKeys.h"
#include "ObjectPool.h"
#include "Body.h"
#include "ofMain.h"


/* GLOBAL CONSTANTS & ENUMERATIONS */
static const int  DEFAULT_HASHED_OCTREE = 14; // Default capacity for the hashed octree
static const size_t CHILD_OCTANTS = 8; // Number of child octants in an octree node
static const spatialKey ROOT_KEY = 1; // Root key for the tree


// OctantEnum: Defines the octant ordering in a 3D coordinate space based on Morton ordering (x > y > z).
enum OctantEnum
{
	Oct_0 = 0x0, //---
	Oct_1 = 0x1, //--+
	Oct_2 = 0x2, //-+-
	Oct_3 = 0x3, //-++
	Oct_4 = 0x4, //+--
	Oct_5 = 0x5, //+-+
	Oct_6 = 0x6, //++-
	Oct_7 = 0x7, //+++
};
// Lookup table for octant directions.
static const double OctantDir[] =
{
	-1.0, -1.0, -1.0,
	-1.0, -1.0,  1.0,
	-1.0,  1.0, -1.0,
	-1.0,  1.0,  1.0,
	 1.0, -1.0, -1.0,
	 1.0, -1.0,  1.0,
	 1.0,  1.0, -1.0,
	 1.0,  1.0,  1.0
};




// Helper Functions: These functions aid in manipulating and determining octant-specific properties.
static inline bool HasOctChild(uint8_t childByte, OctantEnum octant) // Checks if a specific octant is marked as having a child node. If non-zero, this octant has a child.
{
	return (childByte & (1 << octant)) != 0;
}
static inline void SetOctChild(uint8_t& childByte, OctantEnum octant) //set a specific bit in the childByte representing the existence of a child in the given octant, sets the corresponding bit to 1.
{
	childByte |= (1 << octant);
}
static inline void UnsetOctChild(uint8_t& childByte, OctantEnum octant)// unset a specific bit in the childByte, indicating the removal of a child in the given octant, sets the corresponding bit to 0
{
	childByte &= ~(1 << octant);
}
static inline OctantEnum DetermineOctant(const Vec3D& center, const Vec3D& bodyPosition) // Determines the octant for a body based on its position and the center of the current node.
{
	unsigned posX = (bodyPosition.x > center.x);
	unsigned posY = (bodyPosition.y > center.y);
	unsigned posZ = (bodyPosition.z > center.z);
	return static_cast<OctantEnum>((posX << 2) | (posY << 1) | posZ);
}


static inline Vec3D DetermineOctantCenter(const Vec3D& parentCenter, const double& parentSize, OctantEnum octant) // Retrieves the direction for a given octant based on the lookup table.
{
	double halfSize = parentSize * 0.5;
	Vec3D octantCenter;
	octantCenter.x = parentCenter.x + (halfSize * OctantDir[3 * octant]);
	octantCenter.y = parentCenter.y + (halfSize * OctantDir[3 * octant + 1]);
	octantCenter.z = parentCenter.z + (halfSize * OctantDir[3 * octant + 2]);
	return(octantCenter);
}


static inline Vec3D DetermineOctantDirection(OctantEnum octant) // Retrieves the direction for a given octant based on the lookup table.
{
	Vec3D octantDir;
	octantDir.x = OctantDir[3 * octant];
	octantDir.y = OctantDir[3 * octant + 1];
	octantDir.z = OctantDir[3 * octant + 2];
	return(octantDir);
}
static inline spatialKey GetParentKey(const spatialKey childKey) // shifts the childKey three places to the right to get the parent key
{
	const spatialKey parentKey = childKey >> 3;
	return(parentKey);
}
static inline spatialKey GetChildKey(const spatialKey parentKey, OctantEnum octant) //generates the key for a child node by shifting the parentKey three places to the left and adding the octant index
{
	const spatialKey childKey = ((parentKey) << 3 | (octant));
	return(childKey);
}
static inline OctantEnum GetOctantFromKey(spatialKey key) //Retrieves the octant enumeration from a key by taking the last three bits of the key. This is done by taking the key modulo 8, i.e., a bitwise-AND with 7, which gives the last three bits of the key.
{
	OctantEnum octant = static_cast<OctantEnum>(key & static_cast <spatialKey>(7));
	return(octant);
}





class OctantBounds
{
public:
	OctantBounds();
	OctantBounds(Body* nBodies, size_t numBodies); //returns a BoundingBox which has its center at the average position and encloses all bodies
	OctantBounds(Body* localBodies, size_t start, size_t end); //returns a BoundingBox which has its center at the average position and encloses all bodies
	OctantBounds(Vec3D _center, double _size);
	OctantBounds(const OctantBounds& other);
	OctantBounds& operator=(const OctantBounds& other);
	Vec3D center;
	double size;
};




// HOTNode Class: Represents an individual node in the hashed octree structure.
class HOTNode
{
public:
	// Constructors and Destructor
	HOTNode();
	HOTNode(const HOTNode& other);
	HOTNode& operator=(const HOTNode& other);
	HOTNode(HOTNode* parentNode, OctantEnum targetOctant, const Vec3D& _baryCenter, const double& _mass);
	HOTNode(const Vec3D& _position, const double _size);
	HOTNode(const OctantBounds _nodeBounds);
	HOTNode(const OctantBounds _nodeBounds, const spatialKey rootKey);
	HOTNode(const double _size, const spatialKey rootKey);
	~HOTNode();


	bool containsBody(const Vec3D& bodyPosition);


	void reset();
	void updateCenterOfMass();
	void initializeNode(const OctantBounds _nodeBounds, const spatialKey rootKey);
	void insertBodyDirectly(spatialKey parentKey, OctantEnum targetOctant, const Vec3D bodyPosition, const double bodyMass);
	void parameterizeChildNode(HOTNode* parentNode, const OctantEnum targetOctant, const Vec3D& _baryCenter, const double& _mass);

	//Upper triangle of quadrupole moment tensor Q ordered as
	//Qxx, Qxy, Qxz, Qyy, Qyz, Qzz
	double quadrupoleMoment[6];

	Vec3D baryCenter;  	double mass;// the center of mass and total mass of all bodies at or below this node
	long N; //number of bodies at or below this node.
	uint8_t childByte; //a bitfield encoding which children actually exist, each of the 8 bits can represent the existence of one of the 8 children in the octree (where a set bit indicates that the child exists and an unset bit indicates the opposite).
	//private:
		//OctantEnum octant; //compute the octant on the fly
	OctantBounds nodeBounds;
	spatialKey nodeKey; //the spatial key for this node/body
};




















class HOTNodePool
{
private:
	DynamicBufferArray<HOTNode*> nodePool;
	size_t currentIndex;

public:
	HOTNodePool(size_t capacity) : currentIndex(0)
	{
		nodePool.resizeDBA(capacity, nullptr);
		for (size_t i = 0; i < capacity; ++i)
		{
			nodePool[i] = new HOTNode();
		}
	}

	~HOTNodePool()
	{
		for (size_t i = 0; i < nodePool.listData.size; ++i)
		{
			delete nodePool[i];
		}
	}

	HOTNode* acquireNode()
	{
		if (currentIndex >= nodePool.listData.size)
		{
			// No free nodes available in the pool. Return nullptr or handle it differently
			return nullptr;
		}
		return nodePool[currentIndex++];
	}

	void releaseNode(HOTNode* node)
	{
		if (currentIndex == 0)
		{
			// Pool is already fully released. Handle this condition or ignore.
			return;
		}
		node->reset();
		nodePool[--currentIndex] = node;
	}
};



