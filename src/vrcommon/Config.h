#pragma once
#include <fstream>
#include "json/json.h"

namespace vr {
	std::ostream& log();
	
	struct Config {
		bool enableOculusEmulation = true;
		float renderScale = 1.f;
		bool casEnabled = false;
		float sharpness = 1.f;

		static Config Load() {
			Config config;
			try {
				std::ifstream configFile ("fo4_openvr.cfg");
				if (configFile.is_open()) {
					Json::Value root;
					configFile >> root;
					config.enableOculusEmulation = root.get( "enable_oculus_emulation", true ).asBool();
					config.renderScale = root.get("render_scale", 1.0).asFloat();
					Json::Value sharpen = root.get("sharpen", Json::Value());
					config.casEnabled = sharpen.get("enabled", false).asBool();
					config.sharpness = sharpen.get("sharpness", 1.0).asFloat();
				}
			} catch (...) {
				log() << "Could not read config file.\n";
			}
			return config;
		}
	};
}