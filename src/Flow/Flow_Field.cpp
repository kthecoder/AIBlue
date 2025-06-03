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
	//TODO Make a separte function that gets applied, for static obstacles and use this for dynamic obstacles
	vector<bool> is_obstacle(width * height, false);
	for (int i = 0; i < obstacles.size(); ++i) {
		Vector2i pos = obstacles[i];
		is_obstacle[pos.x + pos.y * width] = true;
	}

	// Generate flow field from arrival times
	flow_field.resize(width * height);

	// Compute flow field for each cell
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			int idx = x + y * width;

			if (is_obstacle[idx]) {
				float min_time = 1e6f;
				array<float, 2> best_vector = { 0.0f, 0.0f };

				// Check the four neighbors for the smallest arrival time
				int neighbors[4] = { idx - 1, idx + 1, idx - width, idx + width };

				for (int n = 0; n < 4; n++) {
					if (neighbors[n] < 0 || neighbors[n] >= width * height || is_obstacle[neighbors[n]])
						continue;

					if (arrival_times[neighbors[n]] < min_time) {
						min_time = arrival_times[neighbors[n]];

						// Compute direction to the lowest-time neighbor
						best_vector[0] = (neighbors[n] % width) - x;
						best_vector[1] = ((neighbors[n] / width) - y);
					}
				}

				// Normalize the vector if a valid neighbor was found
				float mag = std::sqrt(best_vector[0] * best_vector[0] + best_vector[1] * best_vector[1]);
				if (mag > 0) {
					flow_field[idx] = { best_vector[0] / mag, best_vector[1] / mag };
				} else {
					flow_field[idx] = { 0.0f, 0.0f }; // No valid direction found
				}
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

	// String godot_string = String(vectorToString().c_str());
	// UtilityFunctions::print("Flow Field Vector: ", godot_string);
}

std::string FlowField::vectorToString() {
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
	if (index < 0 || index > static_cast<int>(flow_field.size())) {
		// UtilityFunctions::push_error("AI Blue Error : Flow Field | Final Flow Field Vector is empty");
		return Vector3();
	}

	// for (size_t i = 0; i < flow_field.size(); ++i) {
	// 	UtilityFunctions::print("Index: ", i, " Value: ", flow_field[i][0], ", ", flow_field[i][1]);
	// }

	// UtilityFunctions::print("World Position: ", world_position);
	// UtilityFunctions::print("Index: ", index);
	// UtilityFunctions::print("Flow Field Max Size: ", flow_field.size());

	// Ref<FileAccess>
	// 		file = FileAccess::open("user://debug_output.txt", FileAccess::WRITE_READ);
	// if (file.is_valid()) {
	// 	file->seek_end(); // Move to end before writing

	// 	// Convert flow_field to a readable string
	// 	String flow_data;
	// 	for (const auto &vector : flow_field) {
	// 		flow_data += String("(") + String::num(vector[0]) + ", " + String::num(vector[1]) + ")\n";
	// 	}

	// 	file->store_string(flow_data); // Write properly formatted data
	// 	file->close();
	// }

	// String godot_string = String(const_cast<FlowField *>(this)->vectorToString().c_str());
	// UtilityFunctions::print("Flow Field Vector: ", godot_string);

	Vector3 move_direction(this->flow_field[index][0], 0.0f, this->flow_field[index][1]);

	if (move_direction.length() > 0.0f) {
		move_direction = move_direction.normalized();
	}

	// UtilityFunctions::print("Final Output: ", move_direction);

	return move_direction;
}

void FlowField::_bind_methods() {
	ClassDB::bind_method(D_METHOD("compute_flow", "width", "height", "arrivalX", "arrivalY", "obstacles"), &FlowField::compute_flow);

	ClassDB::bind_method(D_METHOD("get_move_direction", "world_position", "grid_cell_size"), &FlowField::get_move_direction);
}