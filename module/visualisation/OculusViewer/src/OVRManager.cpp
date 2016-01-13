

#include "OVRManager.h"
#include <algorithm>

OVRManager::OVRManager(){}

void OVRManager::init(){
	riftPresent = true;
   
   	//Init OVR
    ovrResult initResult = ovr_Initialize(nullptr);
	if (!OVR_SUCCESS(initResult)) {
		std::cout << "Failed to Init" << std::endl;
		std::cout << initResult << std::endl;
		riftPresent = false;
	}

	//Init HMD
    ovrResult result = ovr_Create(&session, &luid);
	if (!OVR_SUCCESS(result)) {
		std::cout << "Rift not found!" << std::endl;
		std::cout << result << std::endl;
		riftPresent = false;
		ovr_Shutdown();
	}

    if(riftPresent) {
    	hmdDesc = ovr_GetHmdDesc(session);
	}

	
	bool eyeBufferResult = createEyeBuffers();

	// // Configure the mirror read buffer
	// // Create mirror texture and an FBO used to copy mirror texture to back buffer
	// ovrSizei windowSize = getMirrorResolution();


	// ovrGLTexture mirrorTexture;
	// result = ovr_CreateMirrorTextureGL(session, GL_SRGB8_ALPHA8, windowSize.w, windowSize.h, reinterpret_cast<ovrTexture**>(&mirrorTexture));

	// glGenFramebuffers(1, &mirrorBuffer);
	// glBindFramebuffer(GL_READ_BUFFER, mirrorBuffer);
	// glFramebufferTexture2D(GL_READ_BUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture.OGL.TexId, 0);
	// glFramebufferRenderbuffer(GL_READ_BUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	// glBindFramebuffer(GL_READ_BUFFER, 0);


	EyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
	EyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

	ViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset.y,
					  EyeRenderDesc[1].HmdToEyeViewOffset.y };

	// Turn off vsync to let the compositor do its magic
	wglSwapIntervalEXT(0);

}

ovrSizei OVRManager::getResolution(){
	return hmdDesc.Resolution;
}

ovrSizei OVRManager::getMirrorResolution(){
	//Configure stereo settings
	// Setup Window and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
	return { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };
}

bool OVRManager::createEyeBuffers()
{
	// Configure Stereo settings.
	OVR::Sizei recommenedTex0Size = ovr_GetFovTextureSize(session, ovrEye_Left,
		hmdDesc.DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommenedTex1Size = ovr_GetFovTextureSize(session, ovrEye_Right,
		hmdDesc.DefaultEyeFov[1], 1.0f);

	bufferSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
	bufferSize.h = max(recommenedTex0Size.h, recommenedTex1Size.h);


	if (ovr_CreateSwapTextureSetGL(session, GL_SRGB8_ALPHA8, bufferSize.w, bufferSize.h,
		&pTextureSet) != ovrSuccess)
	{
		std::cout << "OVR SWAP TEXTURE ALLOCATION FAILED" << std::endl;
		return false;
	}

	ovrGLTexture* tex = (ovrGLTexture*)&pTextureSet->Textures[0];

	eyeBuffer = std::make_unique<GL::Framebuffer>(bufferSize.w, bufferSize.h, &tex->OGL.TexId);
	
	return true;
}

void OVRManager::setRenderTarget(GL::Context& gl, OVRManager::RenderTarget target)
{
	if (target == MIRROR) {

	}
	else {
		gl.BindFramebuffer(*eyeBuffer);
		glViewport(bufferSize.w * int(target) / 2, 0, bufferSize.w / 2, bufferSize.h);
	}
}

std::vector<EyePose> OVRManager::getCurrentPoses(){
	std::vector<EyePose> eyePoses;
	if(riftPresent){
		//Struct to emit
       	
       	// Get eye poses, feeding in correct IPD offset
		ViewOffset[0] = EyeRenderDesc[0].HmdToEyeViewOffset;
		ViewOffset[1] = EyeRenderDesc[1].HmdToEyeViewOffset;

        ovrPosef                  EyeRenderPose[2];

        double           ftiming = ovr_GetPredictedDisplayTime(session, 0);
        // Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
        double           sensorSampleTime = ovr_GetTimeInSeconds();
        ovrTrackingState hmdState = ovr_GetTrackingState(session, ftiming, ovrTrue);
        ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);


		for (int eye = 0; eye < 2; ++eye)
		{
			// Get view and projection matrices
			//TODO: revise once working
			OVR::Matrix4f finalRollPitchYaw = OVR::Matrix4f(EyeRenderPose[eye].Orientation);
			OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
			OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
			OVR::Vector3f eyePos = EyeRenderPose[eye].Position;

			OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(eyePos, eyePos + finalForward, finalUp);
			OVR::Matrix4f proj = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 100.0f, ovrProjection_RightHanded);

			// float n = 0.2;
			// float f = 100;
			// auto fov = hmdDesc.DefaultEyeFov[eye];
			// float l = -n * fov.LeftTan;
			// float r = n * fov.RightTan;
			// float t = n * fov.UpTan;
			// float b = - n * fov.DownTan;


			// GL::Mat4 glproj(2 * n / (r - l), 0, (r + l) / (r - l), 0,
			// 				0, 2 * n / (t - b), (t + b) / (t - b), 0,
			// 				0, 0, -(f + n) / (f - n), -2 * f*n / (f - n),
			// 				0, 0, -1, 0);
			// glproj = glproj.Transpose();

			GL::Mat4 glview;
			memcpy(&(glview.m), &(view.M), 16 * sizeof(float));
			GL::Mat4 glproj;
			memcpy(&(glproj.m), &(proj.M), 16 * sizeof(float));
			eyePoses.push_back({ glview.Transpose(), glproj.Transpose()});
		}
			
	} else {
		std::cout << "RIFT NOT CONNECTED!!!" << std::endl;
	}
	return eyePoses;
}

bool OVRManager::renderToRift(){
	// Create eye layer.
	ovrLayerEyeFov eyeLayer;
	eyeLayer.Header.Type = ovrLayerType_EyeFov;
	eyeLayer.Header.Flags = 0;
	for (int eye = 0; eye < 2; eye++)
	{
		eyeLayer.ColorTexture[eye] = pTextureSet;
		//TODO: fix viewport
		eyeLayer.Viewport[eye] = EyeRenderDesc[eye].DistortedViewport;
		eyeLayer.Fov[eye] = EyeRenderDesc[eye].Fov;
		eyeLayer.RenderPose[eye] = EyeRenderPose[eye];
	}

	// The list of layers.
	ovrLayerHeader *layerList = &eyeLayer.Header;

	// Set up positional data.
	ovrViewScaleDesc viewScaleDesc;
	viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
	viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
	viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

	ovrResult result = ovr_SubmitFrame(session, 0, &viewScaleDesc, &layerList, 1);
	return result == ovrSuccess;
}
   
OVRManager::~OVRManager(){
    ovr_Destroy(session);
	ovr_Shutdown();
	delete pTextureSet;
} 