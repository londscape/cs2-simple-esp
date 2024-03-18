#pragma once
#include <fstream>
#include "json.hpp"
#include "offset.hpp"

using json = nlohmann::json;

struct RGB {
	int r;
	int g;
	int b;

	// Conversion function from RGB to COLORREF
	operator COLORREF() const {
		return RGB(r, g, b);
	}
};


namespace config {
	const std::string file_path = "config.json";

	extern bool read();
	extern void save();

	inline bool show_box_esp = true;
	inline bool team_esp = false;
	inline float render_distance = -1.f;
}
