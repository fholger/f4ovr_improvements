#pragma once
#include <fstream>
#include "json/json.h"

namespace vr {
	std::ostream& log();
	
	struct Config {
		bool enableOculusEmulationFix = false;
		float casUpscale = 1.f;
		bool casEnabled = false;
		float sharpness = 1.f;
		float maxColorDelta = 1.f;
		bool casAlternate = false;

		static Config Load() {
			Config config;
			try {
				std::ifstream configFile ("fo4_openvr.cfg");
				if (configFile.is_open()) {
					Json::Value root;
					configFile >> root;
					config.enableOculusEmulationFix = root.get( "enable_oculus_emulation_fix", false ).asBool();
					Json::Value sharpen = root.get("sharpen", Json::Value());
					config.casEnabled = sharpen.get("enabled", false).asBool();
					config.sharpness = sharpen.get("sharpness", 1.0).asFloat();
					if (config.sharpness < 0) config.sharpness = 0;
					if (config.sharpness > 1) config.sharpness = 1;
					config.maxColorDelta = sharpen.get("maxColorDelta", 1.0).asFloat();
					if (config.maxColorDelta < 0) config.maxColorDelta = 0;
					if (config.maxColorDelta > 1) config.maxColorDelta = 1;
					config.casUpscale = sharpen.get("upscale", 1.0).asFloat();
					if (config.casUpscale < 1) config.casUpscale = 1;
					if (config.casUpscale > 2) config.casUpscale = 2;
					config.casAlternate = sharpen.get("alternate", false).asBool();
				}
			} catch (...) {
				log() << "Could not read config file.\n";
			}
			return config;
		}

		static Config Instance() {
			static Config instance = Load();
			return instance;
		}
	};
}