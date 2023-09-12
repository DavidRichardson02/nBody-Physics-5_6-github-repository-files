#include "Body.h"


// Default constructor
Body::Body() : position(0.0, 0.0, 0.0), velocity(0.0, 0.0, 0.0), mass(0.0), bodyKey(0) {}

// Overloaded constructors
Body::Body(Vec3D _position, Vec3D _velocity, double _mass) : position(_position), velocity(_velocity), mass(_mass), bodyKey(0) {}
Body::Body(Vec3D _position, Vec3D _velocity, double _mass, const double _size) : position(_position), velocity(_velocity), mass(_mass)
{
    // bodyKey.computeMortonKey(_position, _size);
}

// Copy constructor
Body::Body(const Body& other) : position(other.position), velocity(other.velocity), mass(other.mass), bodyKey(other.bodyKey) {}

// Assignment operator
Body& Body::operator=(const Body& other)
{
    if (this != &other)
    {
        position = other.position;
        velocity = other.velocity;
        mass = other.mass;
        bodyKey = other.bodyKey;
    }
    return(*this);
}

// Setters and getters
void Body::setBodyKey(spatialKey _bodyKey) { bodyKey = _bodyKey; }
spatialKey Body::getBodyKey() const { return(bodyKey); }


// Debug utility implementation
void Body::printBodies(string label)
{
    cout << "\n" << label; cout << "_";
    cout << "	\nPosition: (" << position.x; cout << ", " << position.y; cout << ", " << position.z; cout << ")";
    cout << "	\n     Mass: " << mass;

    //cout << "	\n\nInteger MortonKey: " << mortonKey;
    // Convert the Morton key to binary format and print it
    //std::bitset<64> binary(mortonKey);
    //cout << "\nBinary MortonKey: " << binary;
}
