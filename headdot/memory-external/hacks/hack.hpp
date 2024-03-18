#include <thread>
#include <cmath>
#include "reader.hpp"
#include "../classes/render.hpp"
#include "../classes/config.hpp"
#include "../classes/globals.hpp"

namespace hack {

	std::vector<std::pair<std::string, std::string>> boneConnections = {
						{"neck_0", "spine_1"},
						{"spine_1", "spine_2"},
						{"spine_2", "pelvis"},
						{"spine_1", "arm_upper_L"},
						{"arm_upper_L", "arm_lower_L"},
						{"arm_lower_L", "hand_L"},
						{"spine_1", "arm_upper_R"},
						{"arm_upper_R", "arm_lower_R"},
						{"arm_lower_R", "hand_R"},
						{"pelvis", "leg_upper_L"},
						{"leg_upper_L", "leg_lower_L"},
						{"leg_lower_L", "ankle_L"},
						{"pelvis", "leg_upper_R"},
						{"leg_upper_R", "leg_lower_R"},
						{"leg_lower_R", "ankle_R"}
	};

	void loop() {

		int playerIndex = 0;
		uintptr_t list_entry;
		for (auto player = g_game.players.begin(); player < g_game.players.end(); player++) {
			const Vector3 screenPos = g_game.world_to_screen(&player->origin);
			const Vector3 screenHead = g_game.world_to_screen(&player->head);

			if (screenPos.z >= 0.01f) {
				const float height = screenPos.y - screenHead.y;
				const float width = height / 2.4f;

				float distance = g_game.localOrigin.calculate_distance(player->origin);
				int roundedDistance = std::round(distance / 10.f);

				if (config::show_box_esp)
				{
					render::Box(g::hdcBuffer, screenHead.x - width / 2, screenHead.y, width, height, RGB(45, 0, 0), 1);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

