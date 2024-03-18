#pragma once
#include <Windows.h>
#include <WinINet.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;


namespace updater {
	const std::string file_path = "offsets.json";

	namespace offsets {
		inline std::ptrdiff_t dwLocalPlayerController = 0x1912578;
		inline std::ptrdiff_t dwEntityList = 0x18C2D58;
		inline std::ptrdiff_t dwViewMatrix = 0x19241A0;
		inline std::ptrdiff_t m_vecAbsOrigin = 0xC8;
		inline std::ptrdiff_t m_vOldOrigin = 0x127C;
		inline std::ptrdiff_t m_pGameSceneNode = 0x318;
		inline std::ptrdiff_t m_flFlashOverlayAlpha = 0x14C0;
		inline std::ptrdiff_t m_szName = 0xC20;
		inline std::ptrdiff_t m_iHealth = 0x334;
		inline std::ptrdiff_t m_hPlayerPawn = 0x7E4;
		inline std::ptrdiff_t m_iTeamNum = 0x3CB;
	}
}
