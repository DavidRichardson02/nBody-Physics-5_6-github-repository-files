#include "HashedNode.h"




OctantBounds::OctantBounds() :center(0.0, 0.0, 0.0), size(0.0) {}
OctantBounds::OctantBounds(Vec3D _center, double _size) :center(_center), size(_size) {}
OctantBounds::OctantBounds(const OctantBounds& other) : size(other.size), center(other.center) {}
OctantBounds& OctantBounds::operator=(const OctantBounds& other)
{
    if (this != &other)
    {
        size = other.size;
        center = other.center;
    }
    return(*this);
}
OctantBounds::OctantBounds(Body* nBodies, size_t numBodies)
{
    center = { 0,0,0 };  size = 0.0;
    for (size_t i = 0; i < numBodies; i++)
    {
        center += nBodies[i].position;
    }
    center /= numBodies;
    Vec3D absPosition;
    double tempSize = size;
    for (size_t i = 0; i < numBodies; i++)
    {
        absPosition.x = fabs(nBodies[i].position.x);
        absPosition.y = fabs(nBodies[i].position.y);
        absPosition.z = fabs(nBodies[i].position.z);
        if ((absPosition.x - center.x) > tempSize) { tempSize = absPosition.x - center.x; }
        if ((absPosition.y - center.y) > tempSize) { tempSize = absPosition.y - center.y; }
        if ((absPosition.z - center.z) > tempSize) { tempSize = absPosition.z - center.z; }
    }
    size = tempSize * 2;
}

OctantBounds::OctantBounds(Body* localBodies, size_t start, size_t end)
{
    center = { 0,0,0 };  size = 0.0;
    for (size_t i = start; i < end; i++)
    {
        center += localBodies[i].position;
    }
    center /= (end - start);

    Vec3D absPosition;
    double tempSize = size;
    for (size_t i = start; i < end; i++)
    {
        absPosition.x = (localBodies[i].position.x);
        absPosition.y = (localBodies[i].position.y);
        absPosition.z = (localBodies[i].position.z);
        if ((absPosition.x - center.x) > tempSize) { tempSize = absPosition.x - center.x; }
        if ((absPosition.y - center.y) > tempSize) { tempSize = absPosition.y - center.y; }
        if ((absPosition.z - center.z) > tempSize) { tempSize = absPosition.z - center.z; }
    }
    size = tempSize * 2;

}




HOTNode::HOTNode() : nodeBounds(), baryCenter({ 0.0, 0.0, 0.0 }), mass(0), N(0), childByte(0), nodeKey()
{
    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}
HOTNode::HOTNode(const HOTNode& other) : nodeBounds(other.nodeBounds), baryCenter(other.baryCenter), mass(other.mass), N(other.N), nodeKey(other.nodeKey), childByte(other.childByte)
{
    quadrupoleMoment[0] = other.quadrupoleMoment[0];
    quadrupoleMoment[1] = other.quadrupoleMoment[1];
    quadrupoleMoment[2] = other.quadrupoleMoment[2];
    quadrupoleMoment[3] = other.quadrupoleMoment[3];
    quadrupoleMoment[4] = other.quadrupoleMoment[4];
    quadrupoleMoment[5] = other.quadrupoleMoment[5];
}
HOTNode& HOTNode::operator=(const HOTNode& other)
{
    if (this != &other)
    {
        baryCenter = other.baryCenter;
        nodeBounds = other.nodeBounds;
        nodeKey = other.nodeKey;
        childByte = other.childByte;
        mass = other.mass;
        N = other.N;

        quadrupoleMoment[0] = other.quadrupoleMoment[0];
        quadrupoleMoment[1] = other.quadrupoleMoment[1];
        quadrupoleMoment[2] = other.quadrupoleMoment[2];
        quadrupoleMoment[3] = other.quadrupoleMoment[3];
        quadrupoleMoment[4] = other.quadrupoleMoment[4];
        quadrupoleMoment[5] = other.quadrupoleMoment[5];
    }
    return(*this);
}
HOTNode::HOTNode(HOTNode* parentNode, OctantEnum targetOctant, const Vec3D& _baryCenter, const double& _mass)
{
    spatialKey childKey = GetChildKey(parentNode->nodeKey, targetOctant);
    nodeKey = childKey;
    nodeBounds.size = parentNode->nodeBounds.size * 0.5;
    Vec3D octantDir = DetermineOctantDirection(targetOctant) * 0.5;

    // Compute the center of the new child node
    nodeBounds.center = parentNode->nodeBounds.center + octantDir * (nodeBounds.size);

    baryCenter = _baryCenter;
    mass = _mass;
    N = 1;
    childByte = 0;


    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}


HOTNode::HOTNode(const Vec3D& _position, const double _size) : nodeBounds(_position, _size), baryCenter({ 0.0, 0.0, 0.0 }), mass(0), N(0), childByte(0), nodeKey(0)
{
    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}
HOTNode::HOTNode(const OctantBounds _nodeBounds) : nodeBounds(_nodeBounds), baryCenter({ 0.0, 0.0, 0.0 }), mass(0), N(0), childByte(0), nodeKey(0)
{
    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}

HOTNode::HOTNode(const OctantBounds _nodeBounds, const spatialKey rootKey) : nodeBounds(_nodeBounds), baryCenter({ 0.0, 0.0, 0.0 }), mass(0), N(0), childByte(0)
{
    nodeKey = ROOT_KEY;
    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}

HOTNode::HOTNode(const double _size, const spatialKey rootKey) : baryCenter({ 0.0, 0.0, 0.0 }), mass(0), N(0), childByte(0)
{
    nodeBounds.size = _size;
    nodeBounds.center = { 0.0, 0.0, 0.0 };
    nodeKey = ROOT_KEY;
    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}

HOTNode::~HOTNode()
{
    baryCenter = { 0.0, 0.0, 0.0 };
    nodeBounds.center = { 0.0, 0.0, 0.0 };
    nodeBounds.size = 0.0;
    nodeKey = 0;
    childByte = 0;
    mass = 0;
    N = 0;

    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}

bool HOTNode::containsBody(const Vec3D& bodyPosition)
{
    // Calculate half size of the node, which is used for bounds checking.
    double halfSize = nodeBounds.size * 0.5;

    // Calculate the min and max bounds for the x, y, and z axes.
    double minX = nodeBounds.center.x - halfSize;
    double maxX = nodeBounds.center.x + halfSize;
    double minY = nodeBounds.center.y - halfSize;
    double maxY = nodeBounds.center.y + halfSize;
    double minZ = nodeBounds.center.z - halfSize;
    double maxZ = nodeBounds.center.z + halfSize;



    return (bodyPosition.x >= (nodeBounds.center.x - halfSize) && bodyPosition.x <= (nodeBounds.center.x + halfSize)) &&
        (bodyPosition.y >= (nodeBounds.center.y - halfSize) && bodyPosition.y <= (nodeBounds.center.y + halfSize)) &&
        (bodyPosition.z >= (nodeBounds.center.z - halfSize) && bodyPosition.z <= (nodeBounds.center.z + halfSize));
}



void HOTNode::reset()
{
    baryCenter = { 0.0, 0.0, 0.0 };
    nodeBounds.center = { 0.0, 0.0, 0.0 };
    nodeBounds.size = 0.0;
    nodeKey = 0;
    childByte = 0;
    mass = 0;
    N = 0;

    quadrupoleMoment[0] = 0.0;
    quadrupoleMoment[1] = 0.0;
    quadrupoleMoment[2] = 0.0;
    quadrupoleMoment[3] = 0.0;
    quadrupoleMoment[4] = 0.0;
    quadrupoleMoment[5] = 0.0;
}

void HOTNode::updateCenterOfMass()
{


}

void HOTNode::initializeNode(const OctantBounds _nodeBounds, const spatialKey rootKey)
{
    nodeBounds = _nodeBounds;
    baryCenter = { 0.0, 0.0, 0.0 };
    mass = 0;
    N = 0;
    childByte = 0;
    nodeKey = rootKey;
}



void HOTNode::parameterizeChildNode(HOTNode* parentNode, const OctantEnum targetOctant, const Vec3D& _baryCenter, const double& _mass)
{
    spatialKey childKey = GetChildKey(parentNode->nodeKey, targetOctant);
    nodeKey = childKey;
    nodeBounds.size = parentNode->nodeBounds.size * 0.5;
    Vec3D octantDir = DetermineOctantDirection(targetOctant) * 0.5;

    // Compute the center of the new child node
    nodeBounds.center = parentNode->nodeBounds.center + octantDir * (nodeBounds.size);

    baryCenter = _baryCenter;
    mass = _mass;
    N = 1;
    childByte = 0;
}




void HOTNode::insertBodyDirectly(spatialKey parentKey, OctantEnum targetOctant, const Vec3D bodyPosition, const double bodyMass)
{
    nodeKey = GetChildKey(parentKey, targetOctant);
    baryCenter = bodyPosition;
    mass = bodyMass;
    N = 1;
    childByte = 0;
}
