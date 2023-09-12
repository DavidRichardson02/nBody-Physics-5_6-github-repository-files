/*
 * Body Class: the n-bodies in the n-body simulation
 *
 * Description:
 * This file defines the Body class, which is used to represent 3D bodies in this program's
 * Barnes Hut n-body simulation. They are the objects to be inserted into the hashed octree.
 * Will be computed with random initial conditions, from which a hashed octree that encompasses all bodies
 * will be generated. Because the bodies will be sorted according to their Morton Keys (Morton Ordering),
 * only the number of bodies need to be stored in the hashed octree, which can then be accessed using the linear indexing system
 * that results from the Morton Ordering of bodies
 *
 */
#pragma once
#include "Containers.h"
#include "SequenceContainers.h"
#include "MortonKeys.h"
#include "ofMain.h"






 /**
  *  Body class representing a physical body with position, velocity, and mass properties
  */
class Body
{
public:
    // ------------- Constructors and Assignment operators -------------
    Body();
    Body(Vec3D _position, Vec3D _velocity, double _mass);
    Body(Vec3D _position, Vec3D _velocity, double _mass, const double _size);  //compute bodies based on an inputted position and a defined bounding box
    Body(const Body& other);
    Body& operator=(const Body& other);




    // ------------- Setter and getter for Morton key -------------
    void setBodyKey(spatialKey _bodyKey);
    spatialKey getBodyKey() const;




    // ------------- Debug utility to print the body's details -------------
    void printBodies(string label);




    // ------------- Member variables -------------
    //private:
    Vec3D position;
    Vec3D velocity;
    double mass;

    //private:
    spatialKey bodyKey; // Morton key associated with the body's position
};




// ------------- Helper functions for sorting bodies by their Morton keys using Merge Sort, a divide and conquer algorithm. -------------
static inline void MergeBodies(Body* bodies, spatialKey* keys, size_t left, size_t middle, size_t right); // Utility function to merge two sorted body arrays based on Morton keys
static inline void MergeSortBodies(Body* bodies, spatialKey* keys, size_t left, size_t right); // Recursive utility function to sort bodies based on Morton keys using merge sort
static inline void MergeSortBodiesByMortonKey(const size_t numBodies, Body* bodies); // Main function to sort an array of bodies based on Morton keys


// New function to reorder the bodies using the radix sorted key indices
static inline void ReorderBodiesByIndexes(Body* bodies, size_t* indexes, size_t numBodies);
static inline void RadixSortBodies(Body* bodies, size_t numBodies);


//Helper functions to integrate forces into bodies
static inline void ComputePositionAtHalfTimeStep(double dt, Body*& bodies, size_t numBodies);  // Drift every body once before resetting acceleration
static inline void ComputeVelocityAndPosition(double dt, Body*& bodies, size_t numBodies, Vec3D*& bodiesAccelerations);   //Kick-Drift-Kick Leap-Frog integration scheme



//Helper functions visualize bodies
static inline void VisualizeBodies(Body *&bodies, size_t numBodies);


static inline void MergeBodies(Body* bodies, spatialKey* keys, size_t left, size_t middle, size_t right)
{
    size_t i, j, k;
    size_t size1 = middle - left + 1;
    size_t size2 = right - middle;

    // Create temp arrays
    Body* leftTempBodies = new Body[size1];
    spatialKey* leftTempKeys = new spatialKey[size1];

    Body* rightTempBodies = new Body[size2];
    spatialKey* rightTempKeys = new spatialKey[size2];

    // Copy data to temp arrays
    for (i = 0; i < size1; i++)
    {
        leftTempKeys[i] = keys[left + i];
        leftTempBodies[i] = bodies[left + i];
    }
    for (j = 0; j < size2; j++)
    {
        rightTempKeys[j] = keys[middle + 1 + j];
        rightTempBodies[j] = bodies[middle + 1 + j];
    }

    // Merge the arrays back into keys[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < size1 && j < size2)
    {
        if (leftTempKeys[i] <= rightTempKeys[j])
        {
            keys[k] = leftTempKeys[i];
            bodies[k] = leftTempBodies[i];
            i++;
        }
        else
        {
            keys[k] = rightTempKeys[j];
            bodies[k] = rightTempBodies[j];
            j++;
        }
        k++;
    }
    while (i < size1)  // Copy the remaining elements of leftTemp
    {
        keys[k] = leftTempKeys[i];
        bodies[k] = leftTempBodies[i];
        i++;
        k++;
    }
    while (j < size2)  // Copy the remaining elements of rightTemp
    {
        keys[k] = rightTempKeys[j];
        bodies[k] = rightTempBodies[j];
        j++;
        k++;
    }

    // Clean up
    delete[] leftTempBodies;
    delete[] leftTempKeys;
    delete[] rightTempBodies;
    delete[] rightTempKeys;
}

static inline void MergeSortBodies(Body* bodies, spatialKey* keys, size_t left, size_t right)
{
    if (left < right)
    {
        size_t middle = left + (right - left) / 2; // Same as (left+right)/2, but avoids overflow for large left and right

        // Recursively Sort first and second halves until the sub-arrays are small enough to be solved directly
        MergeSortBodies(bodies, keys, left, middle);
        MergeSortBodies(bodies, keys, middle + 1, right);

        //merge the sorted halves
        MergeBodies(bodies, keys, left, middle, right);
    }
}

static inline void MergeSortBodiesByMortonKey(const size_t numBodies, Body* bodies)
{
    if (bodies == nullptr)
    {
        throw std::invalid_argument("bodies is a null pointer");
    }

    // Extract Morton keys
    spatialKey* keys = new spatialKey[numBodies];
    for (size_t i = 0; i < numBodies; ++i)
    {
        keys[i] = bodies[i].bodyKey;
    }
    MergeSortBodies(bodies, keys, 0, numBodies - 1);
    // Clean up
    delete[] keys;
}




// New function to reorder the bodies using the sorted indexes
static inline void ReorderBodiesByIndexes(Body* bodies, size_t* indexes, size_t numBodies)
{
    Body* tempBodies = new Body[numBodies];
    for (size_t i = 0; i < numBodies; ++i)
    {
        tempBodies[i] = bodies[indexes[i]];
    }

    // Copy the sorted bodies back
    for (size_t i = 0; i < numBodies; ++i)
    {
        bodies[i] = tempBodies[i];
    }

    // Clean up
    delete[] tempBodies;
}

static inline void RadixSortBodies(Body* bodies, size_t numBodies)
{
    // We'll extract the MortonKeys to a separate array for sorting
    spatialKey* keys = new spatialKey[numBodies];
    size_t* indexes = new size_t[numBodies];
    for (size_t i = 0; i < numBodies; ++i)
    {
        keys[i] = bodies[i].bodyKey;
        indexes[i] = i;  // Store the original indexes
    }

    // Sort the keys and their original indexes using ThreePassRadixSortMortonKeys
    ThreePassRadixSortMortonKeys(keys, indexes, numBodies);


    // Reorder bodies based on the sorted keys using the indexes
    ReorderBodiesByIndexes(bodies, indexes, numBodies);


    // Clean up
    delete[] keys;
    delete[] indexes;
}

inline void ComputePositionAtHalfTimeStep(double dt, Body*& bodies, size_t numBodies)
{
    for (size_t i = 0; i < numBodies; i++)
    {
        bodies[i].position = bodies[i].position + bodies[i].velocity * (dt / 2);
    }
}

inline void ComputeVelocityAndPosition(double dt, Body*& bodies, size_t numBodies, Vec3D*& bodiesAccelerations)
{
    for (size_t i = 0; i < numBodies; i++)
    {
        //KDK Leap Frog 
        bodies[i].velocity = bodies[i].velocity + bodiesAccelerations[i] * (dt); // Kick
        bodies[i].position = bodies[i].position + bodies[i].velocity * (dt / 2); // Drift   
    }
}

inline void VisualizeBodies(Body*& bodies, size_t numBodies)
{
    ofSetColor(31.875, 223.125, 63.75);
    ofFill();
    for (size_t i = 0; i < numBodies; i++)
    {

        ofDrawSphere(bodies[i].position.x, bodies[i].position.y, bodies[i].position.z, bodies[i].mass * 4);
        //ofSetColor(66, 66, 255);
        //ofDrawBitmapString("bodiesAccelerations: (" + ofToString(bodiesAccelerations[i].x) + ", " + ofToString(bodiesAccelerations[i].y) + ", " + ofToString(bodiesAccelerations[i].z) + ")", bodies[i].position.x, bodies[i].position.y, bodies[i].position.z);
    }
}


/* ---------------- PERFORMANCE BENCHMARRKS ----------------
NOTE: time is recorded in seconds. Time to sort bodies equals:
                (time taken to sort bodies) - (time taken to initialize simulation) = time to sort bodies

benchmarks are tracked for each of the functions that sort morton keys


------------ BENCHMARKS FOR MORTON KEY SORTING OF BODIES ------------


--- MergeSortBodies ---
For 2097152 Bodies
    - Sorting Bodies by MortonKeys Time: (5.43043 - 1.93317) seconds
    - Sorting Bodies by MortonKeys Time: (5.47774 - 1.92111) seconds

For 20971520 Bodies
    - Sorting Bodies by MortonKeys Time: (66.6898 - 13.7062) seconds





--- RadixSortBodies ---
For 2097152 Bodies
    - Sorting Bodies by MortonKeys Time: (2.31742 - 1.90053) seconds
    - Sorting Bodies by MortonKeys Time: (2.3267 - 1.89934) seconds

For 20971520 Bodies
    - Sorting Bodies by MortonKeys Time: (18.7694 - 13.4233) seconds




*/