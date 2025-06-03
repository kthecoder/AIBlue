#include "Flow_Field.h"

FlowField::FlowField() {
}

FlowField::~FlowField() {
}

void FlowField::compute_flow(int set_width, int set_height, int arrivalX, int arrivalY, const Array &obstacles) {
	width = set_width;
	height = set_height;

	array<size_t, 2> grid_size = { static_cast<size_t>(width), static_cast<size_t>(height) };
	array<float, 2> grid_spacing = { 1.f / 16, 1.f / 16 };
	float uniform_speed = 1.f;

	vector<array<int32_t, 2>> boundary_indices = { { arrivalX, arrivalY } }; // Arrival Position
	vector<float> boundary_times = { 0.0f }; // Arrival time at goal is 0

	// Compute arrival times using FMM
	auto arrival_times = fmm::SignedArrivalTime(
			grid_size, boundary_indices, boundary_times,
			fmm::UniformSpeedEikonalSolver<float, 2>(grid_spacing, uniform_speed));

	// Apply Obstacles
	vector<bool> is_obstacle(width * height, false);
	for (int i = 0; i < obstacles.size(); ++i) {
		Vector2i pos = obstacles[i];
		is_obstacle[pos.x + pos.y * width] = true;
	}

	// Generate flow field from arrival times
	flow_field.resize(width * height);

	// Compute flow field for each cell
	for (int x = 1; x < width - 1; ++x) {
		for (int y = 1; y < height - 1; ++y) {
			int idx = x + y * width;

			if (is_obstacle[idx]) {
				flow_field[idx] = { 0.0f, 0.0f };
				continue;
			}

			// Compute Vector gradient
			float dx = arrival_times[idx + 1] - arrival_times[idx - 1];
			float dy = arrival_times[idx + width] - arrival_times[idx - width];
			float magnitude = std::sqrt(dx * dx + dy * dy);

			if (magnitude > 0) {
				flow_field[idx] = { -dx / magnitude, -dy / magnitude };
			} else {
				flow_field[idx] = { 0.0f, 0.0f };
			}
		}
	}

	String godot_string = String(vectorToString(flow_field).c_str());
	UtilityFunctions::print("Flow Field Vector: ", godot_string);
}

std::string FlowField::vectorToString(const std::vector<std::array<float, 2>> &flow_field) {
	std::ostringstream oss;
	oss << "["; // Start with an opening bracket

	for (size_t i = 0; i < flow_field.size(); ++i) {
		oss << "(" << flow_field[i][0] << ", " << flow_field[i][1] << ")";
		if (i < flow_field.size() - 1) {
			oss << ", "; // Add comma between elements
		}
	}

	oss << "]"; // Close the bracket
	return oss.str();
}

Vector3 FlowField::get_move_direction(const Vector3 &world_position, float grid_cell_size) const {
	int gridX = static_cast<int>(world_position.x / grid_cell_size);
	int gridY = static_cast<int>(world_position.z / grid_cell_size);

	int index = gridX + gridY * width;
	if (index < 0 || index >= static_cast<int>(flow_field.size())) {
		UtilityFunctions::push_error("AI Blue Error : Flow Field | Final Flow Field Vector is empty");
		return Vector3();
	}

	const auto &direction_vector = flow_field[index];

	Vector3 move_direction(direction_vector[0], 0.0f, direction_vector[1]);

	if (move_direction.length() > 0.0f) {
		move_direction = move_direction.normalized();
	}

	return move_direction;
}

void FlowField::_bind_methods() {
	ClassDB::bind_method(D_METHOD("compute_flow", "width", "height", "arrivalX", "arrivalY", "obstacles"), &FlowField::compute_flow);

	ClassDB::bind_method(D_METHOD("get_move_direction", "world_position", "grid_cell_size"), &FlowField::get_move_direction);
}