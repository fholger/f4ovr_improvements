#include <fstream>
#include <unordered_map>
#include "WrappedIVRCompositor.h"
#define no_init_all deprecated
#include <d3d11.h>
#include <wrl/client.h>
#include "shader_cas_sharpen.h"
#include "shader_cas_upscale.h"
#include <stdint.h>
#define A_CPU 1
#include "cas/ffx_a.h"
#include "cas/ffx_cas.h"

using Microsoft::WRL::ComPtr;

namespace vr {
	std::ostream& log() {
		static std::ofstream logFile ("openvr_log.txt");
		return logFile;
	}

	struct WrappedIVRCompositor::CASRenderResources {
		bool created = false;
		bool enabled = true;
		DWORD lastSwitch = 0;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<ID3D11Texture2D> outputTexture;
		ComPtr<ID3D11ComputeShader> casComputeShader;
		std::unordered_map<ID3D11Texture2D*, ComPtr<ID3D11ShaderResourceView>> inputTextureViews;
		ComPtr<ID3D11UnorderedAccessView> outputTextureView;
		ComPtr<ID3D11Buffer> shaderConstantsBuffer;
		uint32_t outputWidth, outputHeight;
		struct ShaderConstants {
			varAU4(const0);
			varAU4(const1);
		} shaderConstants;

		void Create(ID3D11Texture2D *tex, IVRSystem *system, const Config &config) {
			log() << "Creating resources for CAS\n";
			tex->GetDevice( device.GetAddressOf() );
			device->GetImmediateContext( context.GetAddressOf() );
			D3D11_TEXTURE2D_DESC std;
			tex->GetDesc( &std );

			// create output texture
			outputWidth = std.Width * config.casUpscale;
			outputHeight = std.Height * config.casUpscale;
			log() << "Creating CAS texture of size " << outputWidth << "x" << outputHeight << "\n";
			D3D11_TEXTURE2D_DESC td;
			td.Width = outputWidth;
			td.Height = outputHeight;
			td.MipLevels = 1;
			td.CPUAccessFlags = 0;
			td.Usage = D3D11_USAGE_DEFAULT;
			td.BindFlags = D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
			td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			td.MiscFlags = 0;
			td.SampleDesc.Count = 1;
			td.SampleDesc.Quality = 0;
			td.ArraySize = 1;
			if (FAILED(device->CreateTexture2D( &td, nullptr, outputTexture.GetAddressOf() ))) {
				log() << "Failed to create texture.\n";
				return;
			}
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
			uav.Format = td.Format;
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uav.Texture2D.MipSlice = 0;
			device->CreateUnorderedAccessView( outputTexture.Get(), &uav, outputTextureView.GetAddressOf() );

			// create shader
			bool sharpenOnly = std.Width == td.Width && std.Height == td.Height;
			HRESULT result;
			if (sharpenOnly) {
				log() << "Using CAS in sharpen-only mode\n";
				result = device->CreateComputeShader( g_CASSharpenShader, sizeof(g_CASSharpenShader), nullptr, casComputeShader.GetAddressOf() );
			} else {
				log() << "Using CAS in upscale mode\n";
				result = device->CreateComputeShader( g_CASUpscaleShader, sizeof(g_CASUpscaleShader), nullptr, casComputeShader.GetAddressOf() );
			}
			if (FAILED(result)) {
				log() << "Failed to create compute shader.\n";
				return;
			}

			// create shader constants buffer
			CasSetup( shaderConstants.const0, shaderConstants.const1, config.sharpness, std.Width, std.Height, td.Width, td.Height );
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			bd.StructureByteStride = 0;
			bd.ByteWidth = sizeof(ShaderConstants);
			D3D11_SUBRESOURCE_DATA init;
			init.SysMemPitch = 0;
			init.SysMemSlicePitch = 0;
			init.pSysMem = &shaderConstants;
			if (FAILED(device->CreateBuffer( &bd, &init, shaderConstantsBuffer.GetAddressOf() ))) {
				log() << "Failed to create shader constants buffer.\n";
				return;
			}

			log() << "Resource creation complete\n";
			created = true;
		}

		void Apply(ID3D11Texture2D *tex) {
			if (inputTextureViews.find(tex) == inputTextureViews.end()) {
				log() << "Creating shader resource view for input texture\n";
				// create resource view for input texture
				D3D11_TEXTURE2D_DESC std;
				tex->GetDesc( &std );
				D3D11_SHADER_RESOURCE_VIEW_DESC svd;
				svd.Format = std.Format;
				svd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				svd.Texture2D.MostDetailedMip = 0;
				svd.Texture2D.MipLevels = 1;
				device->CreateShaderResourceView( tex, &svd, inputTextureViews[tex].GetAddressOf() );
			}

			context->OMSetRenderTargets( 0, nullptr, nullptr );
			context->CSSetShader( casComputeShader.Get(), nullptr, 0 );
			UINT uavCount = -1;
			context->CSSetUnorderedAccessViews( 0, 1, outputTextureView.GetAddressOf(), &uavCount );
			context->CSSetConstantBuffers( 0, 1, shaderConstantsBuffer.GetAddressOf() );
			context->CSSetShaderResources( 0, 1, inputTextureViews[tex].GetAddressOf() );
			context->Dispatch( (outputWidth+15)>>4, (outputHeight+15)>>4, 1 );
			context->CSSetShaderResources( 0, 0, nullptr );
			context->CSSetUnorderedAccessViews( 0, 0, nullptr, nullptr );
			context->CSSetConstantBuffers( 0, 0, nullptr );
			context->CSSetShader( nullptr, nullptr, 0 );
		}
	};

	EVRCompositorError WrappedIVRCompositor::Submit( EVREye eEye, const Texture_t *pTexture, const VRTextureBounds_t* pBounds, EVRSubmitFlags nSubmitFlags ) {
		if ( eEye == Eye_Left && pTexture->eType == TextureType_DirectX && config.casEnabled ) {
			ID3D11Texture2D *texture = (ID3D11Texture2D*)pTexture->handle;
			if (casResources == nullptr) {
				casResources = new CASRenderResources;
				casResources->Create( texture, system, config );
				log() << std::flush;
			}
			if (casResources->created) {
				casResources->Apply(texture);
			}
		}

		if (casResources && casResources->created) {
			if (GetTickCount() - casResources->lastSwitch > 5000 && eEye == Eye_Left) {
				casResources->lastSwitch = GetTickCount();
				casResources->enabled = !casResources->enabled;
			}
			if (casResources->enabled || !config.casAlternate) {
				const_cast<Texture_t*>(pTexture)->handle = casResources->outputTexture.Get();
			}
		}

		return wrapped->Submit( eEye, pTexture, pBounds, nSubmitFlags );
	}

	WrappedIVRCompositor::~WrappedIVRCompositor() {
		delete casResources;
	}
}
