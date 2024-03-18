#include "config.hpp"

namespace config {
	bool read() {

		std::ifstream f(file_path);

		json data;
		try {
			data = json::parse(f);
		}
		catch (const std::exception& e) {
			save();
		}

		if (data.empty())
			return false;

		if (data["show box"].is_boolean())
			show_box_esp = data["show box"];

		if (data["team esp"].is_boolean())
			team_esp = data["team esp"];

		if (data["render distance"].is_number())
			render_distance = data["render distance"];

		return true;
	}

	void save() {
		json data;
		
		data["show box"] = show_box_esp;
		data["team esp"] = team_esp;
		data["render distance"] = render_distance;

		std::ofstream output(file_path);
		output << std::setw(4) << data << std::endl;
		output.close();
	}
}
