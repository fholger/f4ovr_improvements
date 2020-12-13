#pragma once
#include "openvr.h"
#include "Config.h"
#include <iosfwd>
#include <cmath>

namespace vr {
	std::ostream& log();

	class WrappedIVRSystem : public IVRSystem {
	public:
		IVRSystem *wrapped;
		Config config = Config::Load();

		virtual void GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight ) {
			wrapped->GetRecommendedRenderTargetSize(pnWidth, pnHeight);
			*pnWidth *= std::sqrt(config.renderScale);
			*pnHeight *= std::sqrt(config.renderScale);
			log() << "Render resolution " << *pnWidth << "x" << *pnHeight << " at render scale " << config.renderScale << "\n";
		}

		virtual HmdMatrix44_t GetProjectionMatrix( EVREye eEye, float fNearZ, float fFarZ ) {
			return wrapped->GetProjectionMatrix(eEye, fNearZ, fFarZ);
		}

		virtual void GetProjectionRaw( EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom ) {
			wrapped->GetProjectionRaw(eEye, pfLeft, pfRight, pfTop, pfBottom);
		}

		virtual bool ComputeDistortion( EVREye eEye, float fU, float fV, DistortionCoordinates_t *pDistortionCoordinates ) {
			return wrapped->ComputeDistortion(eEye, fU, fV, pDistortionCoordinates);
		}

		virtual HmdMatrix34_t GetEyeToHeadTransform( EVREye eEye ) {
			return wrapped->GetEyeToHeadTransform(eEye);
		}

		virtual bool GetTimeSinceLastVsync( float *pfSecondsSinceLastVsync, uint64_t *pulFrameCounter ) {
			return wrapped->GetTimeSinceLastVsync(pfSecondsSinceLastVsync, pulFrameCounter);
		}

		virtual int32_t GetD3D9AdapterIndex() {
			return wrapped->GetD3D9AdapterIndex();
		}

		virtual void GetDXGIOutputInfo( int32_t *pnAdapterIndex ) {
			wrapped->GetDXGIOutputInfo(pnAdapterIndex);
		}
		
		virtual void GetOutputDevice( uint64_t *pnDevice, ETextureType textureType, VkInstance_T *pInstance = nullptr ) {
			wrapped->GetOutputDevice(pnDevice, textureType, pInstance);
		}

		virtual bool IsDisplayOnDesktop() {
			return wrapped->IsDisplayOnDesktop();
		}

		virtual bool SetDisplayVisibility( bool bIsVisibleOnDesktop ) {
			return wrapped->SetDisplayVisibility(bIsVisibleOnDesktop);
		}

		virtual void GetDeviceToAbsoluteTrackingPose( ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow, VR_ARRAY_COUNT(unTrackedDevicePoseArrayCount) TrackedDevicePose_t *pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount ) {
			wrapped->GetDeviceToAbsoluteTrackingPose(eOrigin, fPredictedSecondsToPhotonsFromNow, pTrackedDevicePoseArray, unTrackedDevicePoseArrayCount);
		}

		virtual void ResetSeatedZeroPose() {
			wrapped->ResetSeatedZeroPose();
		}

		virtual HmdMatrix34_t GetSeatedZeroPoseToStandingAbsoluteTrackingPose() {
			return wrapped->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
		}

		virtual HmdMatrix34_t GetRawZeroPoseToStandingAbsoluteTrackingPose() {
			return wrapped->GetRawZeroPoseToStandingAbsoluteTrackingPose();
		}

		virtual uint32_t GetSortedTrackedDeviceIndicesOfClass( ETrackedDeviceClass eTrackedDeviceClass, VR_ARRAY_COUNT(unTrackedDeviceIndexArrayCount) vr::TrackedDeviceIndex_t *punTrackedDeviceIndexArray, uint32_t unTrackedDeviceIndexArrayCount, vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex = k_unTrackedDeviceIndex_Hmd ) {
			return wrapped->GetSortedTrackedDeviceIndicesOfClass(eTrackedDeviceClass, punTrackedDeviceIndexArray, unTrackedDeviceIndexArrayCount, unRelativeToTrackedDeviceIndex);
		}

		virtual EDeviceActivityLevel GetTrackedDeviceActivityLevel( vr::TrackedDeviceIndex_t unDeviceId ) {
			return wrapped->GetTrackedDeviceActivityLevel(unDeviceId);
		}

		virtual void ApplyTransform( TrackedDevicePose_t *pOutputPose, const TrackedDevicePose_t *pTrackedDevicePose, const HmdMatrix34_t *pTransform ) {
			wrapped->ApplyTransform(pOutputPose, pTrackedDevicePose, pTransform);
		}

		virtual vr::TrackedDeviceIndex_t GetTrackedDeviceIndexForControllerRole( vr::ETrackedControllerRole unDeviceType ) {
			return wrapped->GetTrackedDeviceIndexForControllerRole(unDeviceType);
		}

		virtual vr::ETrackedControllerRole GetControllerRoleForTrackedDeviceIndex( vr::TrackedDeviceIndex_t unDeviceIndex ) {
			return wrapped->GetControllerRoleForTrackedDeviceIndex(unDeviceIndex);
		}

		virtual ETrackedDeviceClass GetTrackedDeviceClass( vr::TrackedDeviceIndex_t unDeviceIndex ) {
			return wrapped->GetTrackedDeviceClass(unDeviceIndex);
		}

		virtual bool IsTrackedDeviceConnected( vr::TrackedDeviceIndex_t unDeviceIndex ) {
			return wrapped->IsTrackedDeviceConnected(unDeviceIndex);
		}

		virtual bool GetBoolTrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
			bool result = wrapped->GetBoolTrackedDeviceProperty(unDeviceIndex, prop, pError);
            return result;
		}

		virtual float GetFloatTrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
			return wrapped->GetFloatTrackedDeviceProperty(unDeviceIndex, prop, pError);
		}

		virtual int32_t GetInt32TrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
			int32_t result = wrapped->GetInt32TrackedDeviceProperty(unDeviceIndex, prop, pError);
			if (prop == Prop_Axis0Type_Int32 && config.enableOculusEmulation) {
				// always report the first axis as a Joystick
				result = k_eControllerAxis_Joystick;
			}
			return result;
		}

		virtual uint64_t GetUint64TrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
			return wrapped->GetUint64TrackedDeviceProperty(unDeviceIndex, prop, pError);
		}

		virtual HmdMatrix34_t GetMatrix34TrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
			return wrapped->GetMatrix34TrackedDeviceProperty(unDeviceIndex, prop, pError);
		}

		virtual uint32_t GetArrayTrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, PropertyTypeTag_t propType, void *pBuffer, uint32_t unBufferSize, ETrackedPropertyError *pError = 0L ) {
			return wrapped->GetArrayTrackedDeviceProperty(unDeviceIndex, prop, propType, pBuffer, unBufferSize);
		}

		virtual uint32_t GetStringTrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize, ETrackedPropertyError *pError = 0L ) {
			uint32_t ret = wrapped->GetStringTrackedDeviceProperty(unDeviceIndex, prop, pchValue, unBufferSize, pError);
			if (prop == Prop_TrackingSystemName_String && config.enableOculusEmulation) {
				strncpy(pchValue, "oculus", unBufferSize);
			}
			return ret;
		}

		virtual const char *GetPropErrorNameFromEnum( ETrackedPropertyError error ) {
			return wrapped->GetPropErrorNameFromEnum(error);
		}

		virtual bool PollNextEvent( VREvent_t *pEvent, uint32_t uncbVREvent ) {
			return wrapped->PollNextEvent(pEvent, uncbVREvent);
		}

		virtual bool PollNextEventWithPose( ETrackingUniverseOrigin eOrigin, VREvent_t *pEvent, uint32_t uncbVREvent, vr::TrackedDevicePose_t *pTrackedDevicePose ) {
			return wrapped->PollNextEventWithPose(eOrigin, pEvent, uncbVREvent, pTrackedDevicePose);
		}

		virtual const char *GetEventTypeNameFromEnum( EVREventType eType ) {
			return wrapped->GetEventTypeNameFromEnum(eType);
		}

		virtual HiddenAreaMesh_t GetHiddenAreaMesh( EVREye eEye, EHiddenAreaMeshType type = k_eHiddenAreaMesh_Standard ) {
			return wrapped->GetHiddenAreaMesh(eEye, type);
		}

		virtual bool GetControllerState( vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState, uint32_t unControllerStateSize ) {
			return wrapped->GetControllerState(unControllerDeviceIndex, pControllerState, unControllerStateSize);
		}

		virtual bool GetControllerStateWithPose( ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState, uint32_t unControllerStateSize, TrackedDevicePose_t *pTrackedDevicePose ) {
            return wrapped->GetControllerStateWithPose(eOrigin, unControllerDeviceIndex, pControllerState, unControllerStateSize, pTrackedDevicePose);
		}

		virtual void TriggerHapticPulse( vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec ) {
			wrapped->TriggerHapticPulse(unControllerDeviceIndex, unAxisId, usDurationMicroSec);
		}

		virtual const char *GetButtonIdNameFromEnum( EVRButtonId eButtonId ) {
			return wrapped->GetButtonIdNameFromEnum(eButtonId);
		}

		virtual const char *GetControllerAxisTypeNameFromEnum( EVRControllerAxisType eAxisType ) {
			return wrapped->GetControllerAxisTypeNameFromEnum(eAxisType);
		}

		virtual bool IsInputAvailable() {
			return wrapped->IsInputAvailable();
		}

		virtual bool IsSteamVRDrawingControllers() {
			return wrapped->IsSteamVRDrawingControllers();
		}

		virtual bool ShouldApplicationPause() {
			return wrapped->ShouldApplicationPause();
		}

		virtual bool ShouldApplicationReduceRenderingWork() {
			return wrapped->ShouldApplicationReduceRenderingWork();
		}

		virtual uint32_t DriverDebugRequest( vr::TrackedDeviceIndex_t unDeviceIndex, const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) {
			return wrapped->DriverDebugRequest(unDeviceIndex, pchRequest, pchResponseBuffer, unResponseBufferSize);
		}

		virtual vr::EVRFirmwareError PerformFirmwareUpdate( vr::TrackedDeviceIndex_t unDeviceIndex ) {
			return wrapped->PerformFirmwareUpdate(unDeviceIndex);
		}

		virtual void AcknowledgeQuit_Exiting() {
			wrapped->AcknowledgeQuit_Exiting();
		}

		virtual void AcknowledgeQuit_UserPrompt() {
			wrapped->AcknowledgeQuit_UserPrompt();
		}
	};
}
