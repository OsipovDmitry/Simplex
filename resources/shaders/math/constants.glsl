#define INSIDE 1
#define OUTSIDE -1
#define INTERSECT 0
#define IN_FRONT INSIDE
#define IN_BACK OUTSIDE

const float PI = 3.14159265359f;
const float EPS = 0.0001f;
const float FLT_MAX = 3.402823466e+38;

const uint BOUNDING_BOX_POINTS_COUNT = 8u;
const uint BOUNDING_BOX_EDGES_COUNT = 12u;

const uint FRUSTUM_POINTS_COUNT = 8u;
const uint FRUSTUM_EDGES_COUNT = 12u;
const uint FRUSTUM_PLANES_COUNT = 6u;
const uint FRUSTUM_SIDE_PLANES_COUNT = 4u;

const uint[64u] NTH_NUMBER_WITH_EVEN_BITS_COUNT = uint[64u](
    0, 3, 5, 6, 9, 10, 12, 15, 17, 18,
    20, 23, 24, 27, 29, 30, 33, 34, 36,
    39, 40, 43, 45, 46, 48, 51, 53, 54,
    57, 58, 60, 63, 65, 66, 68, 71, 72,
    75, 77, 78, 80, 83, 85, 86, 89, 90,
    92, 95, 96, 99, 101, 102, 105, 106, 108,
    111, 113, 114, 116, 119, 120, 123, 125, 126);

uvec2 boundingBoxEdgeIndices(in uint ID)
{
	uint index = clamp(ID, 0u, BOUNDING_BOX_EDGES_COUNT - 1u);
	return uvec2(
		NTH_NUMBER_WITH_EVEN_BITS_COUNT[index / 3u],
		NTH_NUMBER_WITH_EVEN_BITS_COUNT[index / 3u] ^ (1u << (index % 3u)));
}