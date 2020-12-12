#pragma once
#include "openvr.h"

namespace vr {
	class WrappedIVRCompositor : public IVRCompositor {
	public:
		IVRCompositor *wrapped;

		virtual void SetTrackingSpace( ETrackingUniverseOrigin eOrigin ) { wrapped->SetTrackingSpace( eOrigin ); }

		virtual ETrackingUniverseOrigin GetTrackingSpace() { return wrapped->GetTrackingSpace(); }

		virtual EVRCompositorError WaitGetPoses( VR_ARRAY_COUNT(unRenderPoseArrayCount) TrackedDevicePose_t* pRenderPoseArray, uint32_t unRenderPoseArrayCount,
			VR_ARRAY_COUNT(unGamePoseArrayCount) TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount ) {
			return wrapped->WaitGetPoses(pRenderPoseArray, unRenderPoseArrayCount, pGamePoseArray, unGamePoseArrayCount);
		}

		virtual EVRCompositorError GetLastPoses( VR_ARRAY_COUNT( unRenderPoseArrayCount ) TrackedDevicePose_t* pRenderPoseArray, uint32_t unRenderPoseArrayCount,
			VR_ARRAY_COUNT( unGamePoseArrayCount ) TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount ) {
			return wrapped->GetLastPoses(pRenderPoseArray, unRenderPoseArrayCount, pGamePoseArray, unGamePoseArrayCount);
		}

		virtual EVRCompositorError GetLastPoseForTrackedDeviceIndex( TrackedDeviceIndex_t unDeviceIndex, TrackedDevicePose_t *pOutputPose, TrackedDevicePose_t *pOutputGamePose ) {
			return wrapped->GetLastPoseForTrackedDeviceIndex( unDeviceIndex, pOutputPose, pOutputGamePose );
		}

		virtual EVRCompositorError Submit( EVREye eEye, const Texture_t *pTexture, const VRTextureBounds_t* pBounds = 0, EVRSubmitFlags nSubmitFlags = Submit_Default );

		virtual void ClearLastSubmittedFrame() {
			wrapped->ClearLastSubmittedFrame();
		}

		virtual void PostPresentHandoff() {
			wrapped->PostPresentHandoff();
		}

		virtual bool GetFrameTiming( Compositor_FrameTiming *pTiming, uint32_t unFramesAgo = 0 ) {
			return wrapped->GetFrameTiming( pTiming, unFramesAgo );
		}

		virtual uint32_t GetFrameTimings( Compositor_FrameTiming *pTiming, uint32_t nFrames ) {
			return wrapped->GetFrameTimings( pTiming, nFrames );
		}

		virtual float GetFrameTimeRemaining() { return wrapped->GetFrameTimeRemaining(); }

		virtual void GetCumulativeStats( Compositor_CumulativeStats *pStats, uint32_t nStatsSizeInBytes ) {
			wrapped->GetCumulativeStats( pStats, nStatsSizeInBytes );
		}

		virtual void FadeToColor( float fSeconds, float fRed, float fGreen, float fBlue, float fAlpha, bool bBackground = false ) {
			return wrapped->FadeToColor( fSeconds, fRed, fGreen, fBlue, fAlpha, bBackground );
		}

		virtual HmdColor_t GetCurrentFadeColor( bool bBackground = false ) {
			return wrapped->GetCurrentFadeColor( bBackground );
		}

		virtual void FadeGrid( float fSeconds, bool bFadeIn ) {
			wrapped->FadeGrid( fSeconds, bFadeIn );
		}

		virtual float GetCurrentGridAlpha() {
			return wrapped->GetCurrentGridAlpha();
		}

		virtual EVRCompositorError SetSkyboxOverride( VR_ARRAY_COUNT( unTextureCount ) const Texture_t *pTextures, uint32_t unTextureCount ) {
			return wrapped->SetSkyboxOverride( pTextures, unTextureCount );
		}

		virtual void ClearSkyboxOverride() { wrapped->ClearSkyboxOverride(); }

		virtual void CompositorBringToFront() { wrapped->CompositorBringToFront(); }

		virtual void CompositorGoToBack() {  wrapped->CompositorGoToBack(); }

		virtual void CompositorQuit() {  wrapped->CompositorQuit(); }
		
		virtual bool IsFullscreen() { return wrapped->IsFullscreen(); }

		virtual uint32_t GetCurrentSceneFocusProcess() { return wrapped->GetCurrentSceneFocusProcess(); }

		virtual uint32_t GetLastFrameRenderer() { return wrapped->GetLastFrameRenderer(); }

		virtual bool CanRenderScene() { return wrapped->CanRenderScene(); }

		virtual void ShowMirrorWindow() {  wrapped->ShowMirrorWindow(); }

		virtual void HideMirrorWindow() {  wrapped->HideMirrorWindow(); }

		virtual bool IsMirrorWindowVisible() { return wrapped->IsMirrorWindowVisible(); }

		virtual void CompositorDumpImages() {  wrapped->CompositorDumpImages(); }

		virtual bool ShouldAppRenderWithLowResources() { return wrapped->ShouldAppRenderWithLowResources(); }

		virtual void ForceInterleavedReprojectionOn( bool bOverride ) { wrapped->ForceInterleavedReprojectionOn( bOverride ); }

		virtual void ForceReconnectProcess() { wrapped->ForceReconnectProcess(); }

		virtual void SuspendRendering( bool bSuspend ) { wrapped->SuspendRendering( bSuspend ); }

		virtual vr::EVRCompositorError GetMirrorTextureD3D11( vr::EVREye eEye, void *pD3D11DeviceOrResource, void **ppD3D11ShaderResourceView ) {
			return wrapped->GetMirrorTextureD3D11( eEye, pD3D11DeviceOrResource, ppD3D11ShaderResourceView );
		}

		virtual void ReleaseMirrorTextureD3D11( void *pD3D11ShaderResourceView ) { wrapped->ReleaseMirrorTextureD3D11( pD3D11ShaderResourceView ); }

		virtual vr::EVRCompositorError GetMirrorTextureGL( vr::EVREye eEye, vr::glUInt_t *pglTextureId, vr::glSharedTextureHandle_t *pglSharedTextureHandle ) {
			return wrapped->GetMirrorTextureGL( eEye, pglTextureId, pglSharedTextureHandle );
		}

		virtual bool ReleaseSharedGLTexture( vr::glUInt_t glTextureId, vr::glSharedTextureHandle_t glSharedTextureHandle ) {
			return wrapped->ReleaseSharedGLTexture( glTextureId, glSharedTextureHandle );
		}

		virtual void LockGLSharedTextureForAccess( vr::glSharedTextureHandle_t glSharedTextureHandle ) {
			wrapped->LockGLSharedTextureForAccess( glSharedTextureHandle );
		}

		virtual void UnlockGLSharedTextureForAccess( vr::glSharedTextureHandle_t glSharedTextureHandle ) {
			wrapped->UnlockGLSharedTextureForAccess( glSharedTextureHandle );
		}

		virtual uint32_t GetVulkanInstanceExtensionsRequired( VR_OUT_STRING() char *pchValue, uint32_t unBufferSize ) {
			return wrapped->GetVulkanInstanceExtensionsRequired( pchValue, unBufferSize );
		}

		virtual uint32_t GetVulkanDeviceExtensionsRequired( VkPhysicalDevice_T *pPhysicalDevice, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize ) {
			return wrapped->GetVulkanDeviceExtensionsRequired( pPhysicalDevice, pchValue, unBufferSize );
		}

		virtual void SetExplicitTimingMode( EVRCompositorTimingMode eTimingMode ) {
			wrapped->SetExplicitTimingMode( eTimingMode );
		}

		virtual EVRCompositorError SubmitExplicitTimingData() { return wrapped->SubmitExplicitTimingData(); }

		struct CASRenderResources;
		CASRenderResources *casResources = nullptr;

		~WrappedIVRCompositor();
	};
}
