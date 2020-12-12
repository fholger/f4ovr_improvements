#include <fstream>
#include <unordered_map>
#include "WrappedIVRCompositor.h"
#define no_init_all deprecated
#include <d3d11.h>
#include <wrl/client.h>
#include "shader_tri_compiled.h"
#include "shader_cas_compiled.h"

using Microsoft::WRL::ComPtr;

namespace vr {
	std::ostream& log() {
		static std::ofstream logFile ("openvr_log.txt");
		return logFile;
	}

	struct WrappedIVRCompositor::CASRenderResources {
		bool created = false;
		bool enabled = false;
		DWORD lastSwitch = 0;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<ID3D11RasterizerState> rasterizerState;
		ComPtr<ID3D11Texture2D> renderTexture;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3D11PixelShader> pixelShader;
		std::unordered_map<ID3D11Texture2D*, ComPtr<ID3D11ShaderResourceView>> shaderResourceViews;

		void Create(ID3D11Texture2D *tex) {
			log() << "Creating resources for CAS\n";
			tex->GetDevice( device.GetAddressOf() );
			device->GetImmediateContext( context.GetAddressOf() );

			// create rasterizer state
			D3D11_RASTERIZER_DESC rd;
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.DepthClipEnable = false;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			if (FAILED(device->CreateRasterizerState( &rd, rasterizerState.GetAddressOf() ))) {
				log() << "Failed to create rasterizer state.\n";
				return;
			}

			// create render output texture and render target view
			D3D11_TEXTURE2D_DESC td;
			tex->GetDesc( &td );
			td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			if (FAILED(device->CreateTexture2D( &td, nullptr, renderTexture.GetAddressOf() ))) {
				log() << "Failed to create texture.\n";
				return;
			}
			D3D11_RENDER_TARGET_VIEW_DESC rvd;
			rvd.Format = td.Format;
			rvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rvd.Texture2D.MipSlice = 0;
			if (FAILED(device->CreateRenderTargetView( renderTexture.Get(), &rvd, renderTargetView.GetAddressOf() ))) {
				log() << "Failed to create render target view.\n";
				return;
			}

			// create shaders
			if (FAILED(device->CreateVertexShader( g_VSFullscreenTri, sizeof(g_VSFullscreenTri), nullptr, vertexShader.GetAddressOf() ))) {
				log() << "Failed to create vertex shader.\n";
				return;
			}
			if (FAILED(device->CreatePixelShader( g_PSCas, sizeof(g_PSCas), nullptr, pixelShader.GetAddressOf() ))) {
				log() << "Failed to create pixel shader.\n";
				return;
			}

			log() << "Resource creation complete\n";
			created = true;
		}

		void Apply(ID3D11Texture2D *tex) {
			ComPtr<ID3D11RasterizerState> currentState;
			context->RSGetState( currentState.GetAddressOf() );
			ComPtr<ID3D11Buffer> indexBuffer;
			DXGI_FORMAT indexFormat;
			UINT indexOffset;
			context->IAGetIndexBuffer( indexBuffer.GetAddressOf(), &indexFormat, &indexOffset );
			ComPtr<ID3D11Buffer> vertexBuffer;
			UINT vertexStride, vertexOffset;
			context->IAGetVertexBuffers( 0, 1, vertexBuffer.GetAddressOf(), &vertexStride, &vertexOffset );
			D3D11_TEXTURE2D_DESC td;
			tex->GetDesc( &td );

			// create shader resource view for current texture if needed
			if ( shaderResourceViews.find( tex ) == shaderResourceViews.end() ) {
				log() << "Creating shader view for texture " << tex << "\n";
				D3D11_SHADER_RESOURCE_VIEW_DESC svd;
				svd.Format = td.Format;
				svd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				svd.Texture2D.MostDetailedMip = 0;
				svd.Texture2D.MipLevels = 1;
				device->CreateShaderResourceView( tex, &svd, shaderResourceViews[tex].GetAddressOf() );
			}

			D3D11_VIEWPORT viewport;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = td.Width;
			viewport.Height = td.Height;
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1;
			context->OMSetRenderTargets( 1, renderTargetView.GetAddressOf(), nullptr );
			context->RSSetState( rasterizerState.Get() );
			context->IASetInputLayout( nullptr );
			context->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			context->IASetVertexBuffers( 0, 0, nullptr, nullptr, nullptr );
			context->IASetIndexBuffer( nullptr, DXGI_FORMAT_UNKNOWN, 0 );
			context->VSSetShader( vertexShader.Get(), nullptr, 0 );
			context->PSSetShader( pixelShader.Get(), nullptr, 0 );
			context->PSSetShaderResources( 0, 1, shaderResourceViews[tex].GetAddressOf() );
			context->RSSetViewports( 1, &viewport );
			context->Draw( 3, 0 );

			context->RSSetState( currentState.Get() );
			context->IASetIndexBuffer( indexBuffer.Get(), indexFormat, indexOffset );
			context->IASetVertexBuffers( 0, 1, vertexBuffer.GetAddressOf(), &vertexStride, &vertexOffset );
		}
	};

	EVRCompositorError WrappedIVRCompositor::Submit( EVREye eEye, const Texture_t *pTexture, const VRTextureBounds_t* pBounds, EVRSubmitFlags nSubmitFlags ) {
		if ( eEye == Eye_Left && pTexture->eType == TextureType_DirectX ) {
			ID3D11Texture2D *texture = (ID3D11Texture2D*)pTexture->handle;
			if (casResources == nullptr) {
				casResources = new CASRenderResources;
				casResources->Create( texture );
			}
			if (casResources->created) {
				casResources->Apply( texture );
			}
		}

		if (casResources && casResources->created) {
			if (GetTickCount() - casResources->lastSwitch > 2000) {
				casResources->lastSwitch = GetTickCount();
				casResources->enabled = !casResources->enabled;
			}
			if (casResources->enabled) {
				Texture_t replacedTexture;
				replacedTexture.eType = TextureType_DirectX;
				replacedTexture.eColorSpace = ColorSpace_Linear;
				replacedTexture.handle = casResources->renderTexture.Get();
				return wrapped->Submit( eEye, &replacedTexture, pBounds, nSubmitFlags );
			}
		}

		return wrapped->Submit( eEye, pTexture, pBounds, nSubmitFlags );
	}

	WrappedIVRCompositor::~WrappedIVRCompositor() {
		delete casResources;
	}
}
