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
		bool casAlternate = false;
		bool rdmEnabled = false;
		float rdmInnerRadius = 0.3f;
		float rdmMidRadius = 0.75f;
		float rdmOuterRadius = 0.85f;
		bool rdmAlternate = false;

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
					config.casUpscale = sharpen.get("upscale", 1.0).asFloat();
					if (config.casUpscale < 1) config.casUpscale = 1;
					if (config.casUpscale > 2) config.casUpscale = 2;
					config.casAlternate = sharpen.get("alternate", false).asBool();

					Json::Value rdm = root.get("fixed_foveated", Json::Value());
					config.rdmEnabled = rdm.get("enabled", false).asBool();
					config.rdmInnerRadius = rdm.get("inner_radius", 0.3f).asFloat();
					config.rdmMidRadius = rdm.get("mid_radius", 0.75f).asFloat();
					config.rdmOuterRadius = rdm.get("outer_radius", 0.85f).asFloat();
					config.rdmAlternate = rdm.get("alternate", false).asBool();
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