/**
 * Vec3D Class: provides an abstraction for three-dimensional vectors, encapsulating the vector's components (x, y, z)
 *
 * This class encapsulates a 3D vector's components (x, y, z),  and provides methods
 * to perform basic vector operations and manipulate vector data.
 */
#pragma once




 // Vec3D class representing a 3D Vector 
class Vec3D
{
public:
    // ------------- Constructors -------------
    Vec3D();
    Vec3D(double _x, double _y, double _z);




    // ------------- Comparison operators -------------
    bool operator!=(const Vec3D& other);




    // ------------- Accessors (Return new Vec3D based on current one) -------------
    Vec3D operator+(const Vec3D& other);
    Vec3D operator-(const Vec3D& other);
    Vec3D operator*(const double scalar);




    // ------------- Modifiers (Modify the current Vec3D and return reference) -------------
    Vec3D& operator=(const Vec3D& other);
    void operator+=(const Vec3D& other);
    void operator-=(const Vec3D& other);
    void operator/=(const double scalar);




    // ------------- Vector operations (do not modify the internal state) -------------
    Vec3D scaleVector(double scalar) const;
    double vectorLength() const;
    double vectorSquareLength() const;
    Vec3D vectorNormalize() const;
    double vectorDistance(const Vec3D& other);



    // ------------- Member variables -------------
    //private:
    double x;
    double y;
    double z;
};



