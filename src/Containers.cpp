#include "Containers.h"
#include <math.h>.


Vec3D::Vec3D() : x(0), y(0), z(0) {}

Vec3D::Vec3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) // using an initializer list to initialize members instead of assignment in the body of the constructor
{}

Vec3D& Vec3D::operator=(const Vec3D& other)
{
    if (this != &other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    return(*this);
}

bool Vec3D::operator!=(const Vec3D& other)
{
    return(x != other.x || y != other.y || z != other.z);
}


Vec3D Vec3D::operator+(const Vec3D& other)
{
    return(Vec3D(x + other.x, y + other.y, z + other.z));
}

Vec3D Vec3D::operator-(const Vec3D& other)
{
    return(Vec3D(x - other.x, y - other.y, z - other.z));
}

Vec3D Vec3D::operator*(const double scalar)
{
    return(Vec3D(x * scalar, y * scalar, z * scalar));
}


void Vec3D::operator+=(const Vec3D& other)
{
    x = x + other.x;
    y = y + other.y;
    z = z + other.z;
}

void Vec3D::operator-=(const Vec3D& other)
{
    x = x - other.x;
    y = y - other.y;
    z = z - other.z;
}

void Vec3D::operator/=(const double scalar)
{
    if (scalar != 0)
    {
        double inverseScalar = 1 / scalar;
        x = x * inverseScalar;
        y = y * inverseScalar;
        z = z * inverseScalar;
    }
}

Vec3D Vec3D::scaleVector(double scalar) const
{
    Vec3D scaledVector = { x * scalar, y * scalar ,z * scalar };
    return(scaledVector);
}

double Vec3D::vectorLength() const
{

    return(sqrt(x * x + y * y + z * z));
}

double Vec3D::vectorSquareLength() const
{
    return(x * x + y * y + z * z);
}

Vec3D Vec3D::vectorNormalize() const
{
    double length = vectorLength();
    if (length != 0)
    {
        double inverseLength = 1 / length;
        Vec3D normalVec = { x * inverseLength , y * inverseLength , z * inverseLength };
        return(normalVec);
    }
    else
    {
        return(*this);
    }
}

double Vec3D::vectorDistance(const Vec3D& other)
{
    return(sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z)));
}

