#include "reader.hpp"
#include <thread>
#include <map>
#include <cmath>
#include "../classes/offset.hpp"
#include "../classes/config.hpp"

std::map<std::string, int> boneMap = {
	{"head", 6},
	{"neck_0", 5},
	{"spine_1", 4},
	{"spine_2", 2},
	{"pelvis", 0},
	{"arm_upper_L", 8},
	{"arm_lower_L", 9},
	{"hand_L", 10},
	{"arm_upper_R", 13},
	{"arm_lower_R", 14},
	{"hand_R", 15},
	{"leg_upper_L", 22},
	{"leg_lower_L", 23},
	{"ankle_L", 24},
	{"leg_upper_R", 25},
	{"leg_lower_R", 26},
	{"ankle_R", 27}
};

// CGame
void CGame::init() {
	process = std::make_shared<pProcess>();
	while (!process->AttachProcessHj("cs2.exe"))
		std::this_thread::sleep_for(std::chrono::seconds(1));

	do {
		base_client = process->GetModule("client.dll");
		base_engine = process->GetModule("engine2.dll");
		if (base_client.base == 0 || base_engine.base == 0) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} while (base_client.base == 0 || base_engine.base == 0);

	GetClientRect(process->hwnd_, &game_bounds);
}

void CGame::close() {
	process->Close();
}

void CGame::loop() {
	inGame = false;

	localPlayer = process->read<uintptr_t>(base_client.base + updater::offsets::dwLocalPlayerController);
	if (!localPlayer) return;

	localPlayerPawn = process->read<std::uint32_t>(localPlayer + updater::offsets::m_hPlayerPawn);
	if (!localPlayerPawn) return;

	entity_list = process->read<uintptr_t>(base_client.base + updater::offsets::dwEntityList);

	localList_entry2 = process->read<uintptr_t>(entity_list + 0x8 * ((localPlayerPawn & 0x7FFF) >> 9) + 16);
	localpCSPlayerPawn = process->read<uintptr_t>(localList_entry2 + 120 * (localPlayerPawn & 0x1FF));
	if (!localpCSPlayerPawn) return;

	view_matrix = process->read<view_matrix_t>(base_client.base + updater::offsets::dwViewMatrix);

	localTeam = process->read<int>(localPlayer + updater::offsets::m_iTeamNum);
	localOrigin = process->read<Vector3>(localpCSPlayerPawn + updater::offsets::m_vOldOrigin);

	inGame = true;
	int playerIndex = 0;
	std::vector<CPlayer> list;
	CPlayer player;

	uintptr_t list_entry, list_entry2, playerPawn, playerMoneyServices, clippingWeapon, weaponData, playerNameData;

	while (true) {

		playerIndex++;
		list_entry = process->read<uintptr_t>(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
		if (!list_entry) break;

		player.entity = process->read<uintptr_t>(list_entry + 120 * (playerIndex & 0x1FF));
		if (!player.entity) continue;

		player.team = process->read<int>(player.entity + updater::offsets::m_iTeamNum);
		if (config::team_esp && (player.team == localTeam)) continue;

		playerPawn = process->read<std::uint32_t>(player.entity + updater::offsets::m_hPlayerPawn);

		list_entry2 = process->read<uintptr_t>(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
		if (!list_entry2) continue;

		player.pCSPlayerPawn = process->read<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));
		if (!player.pCSPlayerPawn) continue;

		player.health = process->read<int>(player.pCSPlayerPawn + updater::offsets::m_iHealth);

		if (player.health <= 0 || player.health > 100) continue;

		if (config::team_esp && (player.pCSPlayerPawn == localPlayer)) continue;

		player.gameSceneNode = process->read<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
		player.origin = process->read<Vector3>(player.pCSPlayerPawn + updater::offsets::m_vOldOrigin);
		player.head = { player.origin.x, player.origin.y, player.origin.z + 75.f };

		if (player.origin.x == localOrigin.x && player.origin.y == localOrigin.y && player.origin.z == localOrigin.z)
			continue;

		if (config::render_distance != -1 && (localOrigin - player.origin).length2d() > config::render_distance) continue;
		if (player.origin.x == 0 && player.origin.y == 0) continue;

		list.push_back(player);
	}

	players.clear();
	players.assign(list.begin(), list.end());
}

uintptr_t boneAddress;
Vector3 bonePosition;
int boneIndex;
void CPlayer::ReadHead() {
	boneAddress = boneArray + 6 * 32;
	bonePosition = g_game.process->read<Vector3>(boneAddress);
	bones.bonePositions["head"] = g_game.world_to_screen(&bonePosition);
}

void CPlayer::ReadBones() {
	for (const auto& entry : boneMap) {
		const std::string& boneName = entry.first;
		boneIndex = entry.second;
		boneAddress = boneArray + boneIndex * 32;
		bonePosition = g_game.process->read<Vector3>(boneAddress);
		bones.bonePositions[boneName] = g_game.world_to_screen(&bonePosition);
	}
}

Vector3 CGame::world_to_screen(Vector3* v) {
	float _x = view_matrix[0][0] * v->x + view_matrix[0][1] * v->y + view_matrix[0][2] * v->z + view_matrix[0][3];
	float _y = view_matrix[1][0] * v->x + view_matrix[1][1] * v->y + view_matrix[1][2] * v->z + view_matrix[1][3];

	float w = view_matrix[3][0] * v->x + view_matrix[3][1] * v->y + view_matrix[3][2] * v->z + view_matrix[3][3];

	float inv_w = 1.f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = game_bounds.right * .5f;
	float y = game_bounds.bottom * .5f;

	x += 0.5f * _x * game_bounds.right + 0.5f;
	y -= 0.5f * _y * game_bounds.bottom + 0.5f;

	return { x, y, w };
}
