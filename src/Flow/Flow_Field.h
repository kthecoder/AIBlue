#ifndef FLOWFIELD_H
#define FLOWFIELD_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "fast_marching_method.hpp"

using namespace godot;
using namespace std;
namespace fmm = thinks::fast_marching_method;

class FlowField : public Node {
	GDCLASS(FlowField, Node);

protected:
	static void _bind_methods();

private:
	int width;
	int height;
	vector<array<float, 2>> flow_field;

	string vectorToString(const std::vector<std::array<float, 2>> &flow_field);

public:
	FlowField();
	~FlowField();

	void compute_flow(int width, int height, int arrivalX, int arrivalY, const Array &obstacles);

	// @param world_position : The Vector3 of the selected target location for the flow field
	// @param grid_cell_size : The size of a grid square in the Game World
	Vector3 get_move_direction(const Vector3 &world_position, float grid_cell_size) const;
};

#endif