

#include "MortonKeys.h"




/**
 * Interleave the bits of three spatialKey values representing the integer coordinates in 3D space.
 *
 * This function interleaves the bits of xInt, yInt, and zInt to produce a single 63-bit Morton key, which is a linear index for 3D points that preserves spatial locality.
 * The interleaving of bits is achieved through a series of bit-wise manipulations, essentially spreading the bits of each integer across the entire 63-bit range.
 *
 *  @param xInt: X-coordinate integer representation.
 *  @param yInt: Y-coordinate integer representation.
 *  @param zInt: Z-coordinate integer representation.
 *
 *  @return Morton key interleaved from xInt, yInt, and zInt.
 */


 /**
 * Compute the Morton key for a given 3D position and spatial size (_size)..
 *
 * This function first scales and converts the floating-point 3D position
 * values into the integer domain. After this conversion, it calls the `interleaveBits` method to create the Morton key
 * which is stored as the instance variable.
 *
 *  @param _position: 3D position to be encoded into a Morton key.
 *  @param _size: Maximum spatial extent of the positions. This value is used to scale the positions to [0, 1) range.
 */



 /**
 * Takes a Morton key and decodes it back into a 3D position.
 *
 * The function decodeMortonKey is designed to reverse the operation performed by interleaveBits, effectively 'de-interleaving' the bits of the Morton key back into three separate x, y, and z components.
 * These integer values are then scaled back into the floating-point domain to retrieve the original 3D position.
 *
 * A loop is performed over the number of bits used for each dimension(21). For each bit index i, we extract the i-th bit from the Morton key for each dimension using bitwise AND (&) and shifting (<<, >>).
 * This is done by: bitwise ANDing (&) the Morton key with a mask that has a 1 in the position corresponding to the i-th bit of the given dimension and 0s everywhere else.
 * For x, this is (1ull << ((3*i) + 2)) - i, for y, this is (1ull << ((3*i) + 1)) - i, and for z, this is (1ull << (3*i)) - i. This operation will yield a number
 * where only the i-th bit of the dimension is potentially non-zero. Bitwise ORing (|=) the result with the accumulated result for the dimension. This 'keeps' the bits that were already set in the dimension
 * and potentially sets the i-th bit if it was set in the Morton key.
 *
 * It then scales these integer values back into the floating-point domain to retrieve the original 3D position.
 *
 *  @param _mortonKey: Morton key to be decoded.
 *  @param _size: Maximum spatial extent of the positions. This value is used to scale back the decoded position to its original range.
 *
 *  @return Decoded 3D position from the Morton key.
 */
