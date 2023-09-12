#include "LinearHashedOctree.h"

LinearHashedOctree::LinearHashedOctree()
{
    nodes.reserve((1 << DEFAULT_HASHED_OCTREE) - 1); //rehash

}

LinearHashedOctree::LinearHashedOctree(const OctantBounds& rootBounds)
{
    // Insert the node into the hashmap.
   //nodes.rehash((1 << DEFAULT_HASHED_OCTREE) - 1);
   //HOTNode* rootNode = new HOTNode(rootBounds, ROOT_KEY);
   //nodes[rootNode->nodeKey] = rootNode;
   //nodes.emplace(rootNode->nodeKey, rootNode);

}


LinearHashedOctree::~LinearHashedOctree()
{
    deleteTree();
    //clear();
}




size_t LinearHashedOctree::GetNodeTreeDepth(const spatialKey nodeKey)
{
    assert(nodeKey);
    size_t depth = 0;
    for (spatialKey locationKey = nodeKey; locationKey != 1; locationKey >>= 3) //shift locationKey right by 3 bits, i.e., moving up one level in the tree until the root node is reached
    {
        depth++;  //increment depth by 1, i.e., moving down one level in the tree
    }
    return depth;
}



HOTNode* LinearHashedOctree::getParentNode(const HOTNode* childNode)
{
    const spatialKey parentNode = childNode->nodeKey >> 3;
    return lookUpNode(parentNode);
}

HOTNode* LinearHashedOctree::getChildNode(const HOTNode* parentNode)
{
    const spatialKey childNode = parentNode->nodeKey << 3;
    return lookUpNode(childNode);
}

HOTNode* LinearHashedOctree::lookUpNode(const spatialKey code)
{
    const auto iter = nodes.find(code);
    return (iter == nodes.end() ? nullptr : iter->second);
}

/**
 * Recursively traverse the octree, visiting all child nodes.
 * This traversal goes through all eight possible child nodes of a given node.
 *
 * @param node: Current node being traversed.
 */
void LinearHashedOctree::traverseTree(HOTNode* node)
{
    for (int i = 0; i < 8; i++) //For all eight possible children
    {
        if (node->childByte & (1 << i))// See if the ith child exist
        {
            const spatialKey locCodeChild = (node->nodeKey << 3) | i; //Compute new Morton key for the child
            auto* child = lookUpNode(locCodeChild);  //Using key, look child up in hash table and recursively visit subtree
            traverseTree(child);
        }
    }
}



void LinearHashedOctree::insertHOTNode(HOTNode*& node)
{
    ///*
    if (nodes.empty()) //if this is the root node
    {
        node->nodeKey = ROOT_KEY;
        nodes[node->nodeKey] = node;
    }
    else //NOT the root node
    {
        if (nodes[node->nodeKey]) //this node already exists
        {
            //deleteNode(node->nodeKey);//delete old node

            nodes.erase(node->nodeKey);   // Remove the node from the map
            delete node;  // Release the node's memory

        }
        nodes[node->nodeKey] = node; //add node to hashmap
    }
    //*/

    //
    /*
    if (nodes[node->nodeKey]) //this node already exists
    {
        nodes.erase(node->nodeKey);   // Remove the node from the map
        delete node;  // Release the node's memory

    }
    nodes[node->nodeKey] = node; //add node to hashmap
    //*/



    //
    /*
    auto result = nodes.emplace(node->nodeKey, node);

    if (!result.second) // this node already exists
    {
        // Overwrite the old node with the new one.
        // Assuming ownership is transferred here, and it's safe to delete the old node.
        delete result.first->second;  // delete old node
        result.first->second = node;  // replace old node with new one
    }
    // Otherwise, the emplace has already added the new node for us

    //*/




}




bool LinearHashedOctree::descendantOf(spatialKey nodeKey, spatialKey parentKey)
{
    if (parentKey == 1)  //only the root node has a key of 1, all nodes are descendants of root node
    {
        return(true);
    }
    if (nodeKey <= parentKey) //if this key is less than the parentKey, then it is an ancestor of the parent key, not a descendant (i.e., it resides higher up in the tree).
    {
        return(false);
    }

    size_t nodeKeyDepth = GetNodeTreeDepth(nodeKey);
    size_t parentKeyDepth = GetNodeTreeDepth(parentKey);


    nodeKey >>= (nodeKeyDepth - parentKeyDepth) * 3; // Bring the nodeKey to the same level as parentKey.
    return (nodeKey == parentKey);
}

bool LinearHashedOctree::leafNodeExistsForBody(const Vec3D& bodyPosition)
{
    spatialKey currentKey = ROOT_KEY;
    HOTNode* currentNode = lookUpNode(currentKey);

    while (currentNode != nullptr && currentNode->N > 0)
    {
        OctantEnum targetOctant = DetermineOctant(currentNode->nodeBounds.center, bodyPosition);
        currentKey = GetChildKey(currentKey, targetOctant);
        currentNode = lookUpNode(currentKey);
    }

    return currentNode != nullptr && currentNode->N == 0; // true if it's a leaf node
}

std::vector<OctantEnum> LinearHashedOctree::getPathToLeafNode(const Vec3D& bodyPosition)
{
    std::vector<OctantEnum> path;

    // Start with the root node
    spatialKey currentKey = ROOT_KEY;
    HOTNode* currentNode = lookUpNode(ROOT_KEY);


    // Traverse the tree until we reach a leaf node or an empty node
    while (currentNode) //&& currentNode->N != 0
    {
        OctantEnum octant = DetermineOctant(currentNode->nodeBounds.center, bodyPosition);
        path.push_back(octant);

        currentKey = GetChildKey(currentNode->nodeKey, octant);

        if (nodes.find(currentKey) != nodes.end())
        {
            currentNode = lookUpNode(currentKey);
        }
        else
        {
            break;  // Leaf node or empty node found
        }
    }
    return path;
}















/*
* SEE NBODY PHYSICS 4_7 FOR DOCUMENTATION OF THESE FUNCTIONS
*
*/

void LinearHashedOctree::insertBody(HOTNode*& node, const Vec3D bodyPosition, const double bodyMass)
{
    ///*
    // Ensure the root node is initialized.
    assert(!nodes.empty());
    spatialKey currentKey;
    OctantEnum targetOctant;


    //GENERAL CASE 1
    if (node == nullptr)
    {
        currentKey = ROOT_KEY;
        node = lookUpNode(currentKey);
    }
    //GENERAL CASE 2
    else
    {
        currentKey = node->nodeKey;
    }



    // Traverse the tree to find the appropriate insertion point for the body.
    while (node != nullptr)
    {
        //GENERAL CASE 3: The target Node is empty
        if (node->N == 0)
        {
            node->mass = bodyMass;
            node->baryCenter = bodyPosition;
            node->N = 1;
            node->childByte = 0;

            node = nullptr;  // Reset the node pointer
        }
        else if (node->N > 1)         //GENERAL CASE 4: The target Node has Multiple Bodies
        {
            // Determine the spatial subdivision for the new body.
            targetOctant = DetermineOctant(node->nodeBounds.center, bodyPosition);
            node->N = node->N + 1;
            node->mass = node->mass + bodyMass;
            processNodeInsertion(node, targetOctant, currentKey, bodyPosition, bodyMass);
        }
        else if (node->N == 1)       //GENERAL CASE 5: The target Node has a Single Body    
        {
            pushHOTNodeBodyToChild(node); // Push the existing body down to a child node, making space for a new body.
            targetOctant = DetermineOctant(node->nodeBounds.center, bodyPosition);
            node->N = node->N + 1;
            node->mass = node->mass + bodyMass;
            processNodeInsertion(node, targetOctant, currentKey, bodyPosition, bodyMass);
        }
    }

    //pruneEmptyNodes(node);

    //*/
}

void LinearHashedOctree::pushHOTNodeBodyToChild(HOTNode* node)
{
    if (node == nullptr || node->N != 1)
    {
        return;
    }
    //Since N == 1, com is the position of that one body.
    //OctantEnum targetOctant = GetOctantFromKey(node->nodeKey);
    OctantEnum targetOctant = DetermineOctant(node->nodeBounds.center, node->baryCenter);

    createChildNode(node, targetOctant, node->baryCenter, node->mass);

    //pruneEmptyNodes(node);

    //If the node is empty after pushing body to its child, delete the node
    //if (node->N == 0 && node->childByte == 0)
    //{
    //    nodes.erase(node->nodeKey);   // Remove the node from the map
    //    delete node;  // Release the node's memory
    //}
}

void LinearHashedOctree::processNodeInsertion(HOTNode*& node, OctantEnum& targetOctant, spatialKey& currentKey, const Vec3D bodyPosition, const double bodyMass)
{
    //if (node == nullptr)
    //{
    //    return;
    //}


    if (HasOctChild(node->childByte, targetOctant))
    {
        //updates the current key to that of the child's
        currentKey = GetChildKey(currentKey, targetOctant);

        //fetches the child node from the HOT
        node = lookUpNode(currentKey);
    }
    else
    {
        //create a new child node with the body's information
        createChildNode(node, targetOctant, bodyPosition, bodyMass);

        // Reset the node since we're now at a leaf.
        node = nullptr;
    }

}

void LinearHashedOctree::pruneEmptyNodes(HOTNode* node)
{
    //if (node == nullptr)//|| node->nodeKey == 0)
    //{
    //    return;
    //}


    // Iterate over all possible child nodes
    for (int i = 0; i < 8; ++i)
    {
        if (node->childByte & (1 << i))
        {
            spatialKey childKey = (node->nodeKey << 3) | i;
            auto* childNode = lookUpNode(childKey);

            // Recursively prune children of this child node
            pruneEmptyNodes(childNode);
        }
    }


    // Check if the current node is empty and has no children
    if (node->N == 0 && node->childByte == 0)
    {
        nodes.erase(node->nodeKey);   // Remove the node from the map
        delete node;  // Release the node's memory
    }
}

void LinearHashedOctree::deleteNode(spatialKey nodeCode)
{
    auto iter = nodes.find(nodeCode);
    if (iter != nodes.end())
    {
        delete iter->second;  // Deallocate the memory for the node
        nodes.erase(iter);    // Remove the entry from the unordered_map
    }
}

void LinearHashedOctree::clear()
{
    nodes.clear();
}

void LinearHashedOctree::deleteTree()
{
    for (auto& node_pair : nodes)
    {
        delete node_pair.second; // Deallocate the memory for the node
    }
    clear(); // Remove the entry from the unordered_map
}

void LinearHashedOctree::createChildNode(HOTNode*& node, OctantEnum& targetOctant, const Vec3D& bodyPosition, const double& bodyMass)
{
    spatialKey childKey = GetChildKey(node->nodeKey, targetOctant);

    HOTNode* childNode = new HOTNode(node, targetOctant, bodyPosition, bodyMass);

    //Update the parent node to reflect the new child's existence.
    SetOctChild(node->childByte, targetOctant);

    insertHOTNode(childNode);
}



void LinearHashedOctree::computeNodeBaryCenters(HOTNode*& node)
{
    //compute c.o.m. first.
    spatialKey childKey;
    HOTNode* childNode;
    double mass;
    for (size_t i = 0; i < 8; i++) //For all eight possible children
    {
        if (node->childByte & (1 << i))
        {
            childKey = (node->nodeKey << 3) | i; //Compute new Morton key for the child
            childNode = lookUpNode(childKey);            //fetches the child node from the HOT
            mass = childNode->mass;
            node->mass += mass;
            node->N += childNode->N;

            node->baryCenter += (childNode->baryCenter * childNode->mass);
        }
        node->baryCenter /= node->mass;
    }

    //compute quadrapole moment contribution
    node->quadrupoleMoment[0] = 0.0;
    node->quadrupoleMoment[1] = 0.0;
    node->quadrupoleMoment[2] = 0.0;
    node->quadrupoleMoment[3] = 0.0;
    node->quadrupoleMoment[4] = 0.0;
    node->quadrupoleMoment[5] = 0.0;

    Vec3D childPosition;
    double xx, yy, zz, d2;


    for (size_t i = 0; i < 8; i++) //For all eight possible children
    {
        if (node->childByte & (1 << i))
        {
            childKey = (node->nodeKey << 3) | i; //Compute new Morton key for the child
            childNode = lookUpNode(childKey);            //fetches the child node from the HOT

            //first, get child's pos in c.o.m. reference frame
            childPosition = childNode->baryCenter - node->baryCenter;

            mass = childNode->mass;

            xx = childPosition.x * childPosition.y; //xy
            node->quadrupoleMoment[1] += 3.0 * mass * xx;

            zz = childPosition.x * childPosition.z; //xz
            node->quadrupoleMoment[2] += 3.0 * mass * zz;


            yy = childPosition.y * childPosition.z; //yz
            node->quadrupoleMoment[4] += 3.0 * mass * yy;



            xx = childPosition.x * childPosition.x;
            zz = childPosition.z * childPosition.z;
            yy = childPosition.y * childPosition.y;

            d2 = xx + yy + zz;
            d2 *= mass;
            mass *= 3.0;


            node->quadrupoleMoment[0] += mass * xx - d2;
            node->quadrupoleMoment[3] += mass * yy - d2;
            node->quadrupoleMoment[5] += mass * zz - d2;


            //directly add child's moments also
            if (childNode->N > 1)
            {
                node->quadrupoleMoment[0] += childNode->quadrupoleMoment[0];
                node->quadrupoleMoment[1] += childNode->quadrupoleMoment[1];
                node->quadrupoleMoment[2] += childNode->quadrupoleMoment[2];
                node->quadrupoleMoment[3] += childNode->quadrupoleMoment[3];
                node->quadrupoleMoment[4] += childNode->quadrupoleMoment[4];
                node->quadrupoleMoment[5] += childNode->quadrupoleMoment[5];
            }
        }



    }

}

void LinearHashedOctree::computeTreeBaryCenters(HOTNode*& node)
{
    if (node == nullptr || nodes.empty() || node->N == 1)
    {
        return;
    }


    //   
    /*
    for (auto& pair : nodes)
    {
        //const HOTNode* node = pair.second;
        computeTreeBaryCenters(pair.second);
    }
    //*/


    ///*


    //do a depth-first search
    //recursively update children first.
    spatialKey childKey;
    for (int i = 0; i < 8; i++) //For all eight possible children
    {
        if (node->childByte & (1 << i))// See if the ith child exist
        {
            childKey = (node->nodeKey << 3) | i; //Compute new Morton key for the child
            auto* child = lookUpNode(childKey);  //Using key, look child up in hash table and recursively visit subtree
            computeTreeBaryCenters(child);
        }
    }
    //*/


    computeNodeBaryCenters(node);

}






void LinearHashedOctree::printHashedOctree()
{
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nnodes.bucket_count(): " << nodes.bucket_count();
    cout << "\n\nnodes.size(): " << nodes.size();


    size_t i = 0;
    for (auto& pair : nodes)
    {
        i++;
        const spatialKey& key = pair.first;


        // Print out key

        std::cout << "\n\n\n\n\n\n\n\n\nKey: " << key;        cout << "                  i" << i;

        std::bitset<64> binary(key);
        cout << "\nBinary MortonKey: " << binary;



        if (pair.second != nullptr)
        {
            //OctantEnum octant1 = DetermineOctant(value->nodeBounds.center, value->baryCenter); //determine which octant the new body should be placed in 
            //cout << "\n\nDetermineOctant: " << octant1;

            OctantEnum octant2 = GetOctantFromKey(pair.second->nodeKey);
            cout << "\n\n\n\nGetOctantFromKey: " << octant2;

            cout << "\nvalue->nodeBounds.size: " << pair.second->nodeBounds.size;
            cout << "\nvalue->nodeBounds.center: (" << pair.second->nodeBounds.center.x; cout << ", " << pair.second->nodeBounds.center.y; cout << ", " << pair.second->nodeBounds.center.z; cout << ")";


            cout << "\nvalue->baryCenter: (" << pair.second->baryCenter.x; cout << ", " << pair.second->baryCenter.y; cout << ", " << pair.second->baryCenter.z; cout << ")";
            cout << "\nvalue->mass: " << pair.second->mass;

            cout << "\nvalue->N: " << pair.second->N;
            cout << "\nvalue->childByte: " << std::bitset<8>(pair.second->childByte);

        }
    }
}

void LinearHashedOctree::visualizeTree()
{
    for (auto& pair : nodes)
    {
        visualizeNode(pair.second);
    }
}

void LinearHashedOctree::visualizeNode(const HOTNode* node)
{
    if (node == nullptr)
    {
        return;
    }


    ofNoFill();
    // Get the center and size of the node
    //ofVec3f center(node->nodeBounds.center.x, node->nodeBounds.center.y, node->nodeBounds.center.z);


    // Set the box color
    ofSetColor(255, 255, 255, 63.75);
    //ofSetColor(255, 255, 255, 31.875);

    // Draw the box
    ofDrawBox(node->nodeBounds.center.x, node->nodeBounds.center.y, node->nodeBounds.center.z, node->nodeBounds.size); //uses the passed-in coordinates as the center of the cube


    //ofSetColor(66, 66, 255);
    //ofDrawBitmapString("Mass: " + ofToString(node->mass) + "\nN: " + ofToString(node->N) + "\nQuadrupole Moment[4]: " + ofToString(node->quadrupoleMoment[4]), center);

    //size_t depthLevel = GetNodeTreeDepth(node->nodeKey);
    //ofDrawBitmapString("Mass: " + ofToString(node->mass) + "\nN: " + ofToString(node->N) + "\nKey: " + ofToString(node->nodeKey) + "\nDepth: " + ofToString(depthLevel), center);
}
