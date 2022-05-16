#include "Global.h"

void View_Camera_FlyMode(ViewContext* viewCtx, InputContext* inputCtx) {
	Camera* cam = viewCtx->currentCamera;
	
	if (viewCtx->cameraControl) {
		if (inputCtx->key[KEY_LEFT_SHIFT].hold) {
			Math_DelSmoothStepToF(&viewCtx->flyMode.speed, 4.0f, 0.25f, 1.00f, 0.00001f);
		} else if (inputCtx->key[KEY_SPACE].hold) {
			Math_DelSmoothStepToF(&viewCtx->flyMode.speed, 16.0f, 0.25f, 1.00f, 0.00001f);
		} else {
			Math_DelSmoothStepToF(&viewCtx->flyMode.speed, 0.5f, 0.25f, 1.00f, 0.00001f);
		}
		
		if (inputCtx->key[KEY_A].hold || inputCtx->key[KEY_D].hold) {
			if (inputCtx->key[KEY_A].hold)
				Math_DelSmoothStepToF(&viewCtx->flyMode.vel.x, viewCtx->flyMode.speed, 0.25f, 1.00f, 0.00001f);
			if (inputCtx->key[KEY_D].hold)
				Math_DelSmoothStepToF(&viewCtx->flyMode.vel.x, -viewCtx->flyMode.speed, 0.25f, 1.00f, 0.00001f);
		} else {
			Math_DelSmoothStepToF(&viewCtx->flyMode.vel.x, 0, 0.25f, 1.00f, 0.00001f);
		}
		
		if (inputCtx->key[KEY_W].hold || inputCtx->key[KEY_S].hold) {
			if (inputCtx->key[KEY_W].hold)
				Math_DelSmoothStepToF(&viewCtx->flyMode.vel.z, viewCtx->flyMode.speed, 0.25f, 1.00f, 0.00001f);
			if (inputCtx->key[KEY_S].hold)
				Math_DelSmoothStepToF(&viewCtx->flyMode.vel.z, -viewCtx->flyMode.speed, 0.25f, 1.00f, 0.00001f);
		} else {
			Math_DelSmoothStepToF(&viewCtx->flyMode.vel.z, 0, 0.25f, 1.00f, 0.00001f);
		}
	} else {
		Math_DelSmoothStepToF(&viewCtx->flyMode.speed, 0.5f, 0.25f, 1.00f, 0.00001f);
		Math_DelSmoothStepToF(&viewCtx->flyMode.vel.x, 0, 0.25f, 1.00f, 0.00001f);
		Math_DelSmoothStepToF(&viewCtx->flyMode.vel.z, 0, 0.25f, 1.00f, 0.00001f);
	}
	
	Vec3f* eye = &cam->eye;
	Vec3f* at = &cam->at;
	
	if (viewCtx->flyMode.vel.z || viewCtx->flyMode.vel.x || (viewCtx->cameraControl && inputCtx->mouse.clickL.hold)) {
		VecSph camSph = {
			.r = Vec_Vec3f_DistXYZ(eye, at),
			.yaw = Vec_Yaw(at, eye),
			.pitch = Vec_Pitch(at, eye)
		};
		
		if (viewCtx->cameraControl && inputCtx->mouse.clickL.hold) {
			camSph.yaw -= inputCtx->mouse.vel.x * 65;
			camSph.pitch -= inputCtx->mouse.vel.y * 65;
		}
		
		*at = *eye;
		
		Vec_AddVecSphToVec3f(at, &camSph);
	}
	
	if (viewCtx->flyMode.vel.z || viewCtx->flyMode.vel.x) {
		VecSph velSph = {
			.r = viewCtx->flyMode.vel.z * 1000,
			.yaw = Vec_Yaw(at, eye),
			.pitch = Vec_Pitch(at, eye)
		};
		
		Vec_AddVecSphToVec3f(eye, &velSph);
		Vec_AddVecSphToVec3f(at, &velSph);
		
		velSph = (VecSph) {
			.r = viewCtx->flyMode.vel.x * 1000,
			.yaw = Vec_Yaw(at, eye) + 0x3FFF,
			.pitch = 0
		};
		
		Vec_AddVecSphToVec3f(eye, &velSph);
		Vec_AddVecSphToVec3f(at, &velSph);
	}
}

void View_Camera_OrbitMode(ViewContext* viewCtx, InputContext* inputCtx) {
	Camera* cam = viewCtx->currentCamera;
	VecSph orbitSph = {
		.r = Vec_Vec3f_DistXYZ(&cam->at, &cam->eye),
		.yaw = Vec_Yaw(&cam->eye, &cam->at),
		.pitch = Vec_Pitch(&cam->eye, &cam->at)
	};
	f32 distMult = (orbitSph.r * 0.1);
	
	if (viewCtx->cameraControl) {
		if (inputCtx->mouse.clickMid.hold || inputCtx->mouse.scrollY) {
			if (inputCtx->mouse.clickMid.hold) {
				if (inputCtx->key[KEY_LEFT_SHIFT].hold) {
					VecSph velSph = {
						.r = inputCtx->mouse.vel.y * distMult * 0.01f,
						.yaw = Vec_Yaw(&cam->at, &cam->eye),
						.pitch = Vec_Pitch(&cam->at, &cam->eye) + 0x3FFF
					};
					
					Vec_AddVecSphToVec3f(&cam->eye, &velSph);
					Vec_AddVecSphToVec3f(&cam->at, &velSph);
					
					velSph = (VecSph) {
						.r = inputCtx->mouse.vel.x * distMult * 0.01f,
						.yaw = Vec_Yaw(&cam->at, &cam->eye) + 0x3FFF,
						.pitch = 0
					};
					
					Vec_AddVecSphToVec3f(&cam->eye, &velSph);
					Vec_AddVecSphToVec3f(&cam->at, &velSph);
				} else {
					orbitSph.yaw -= inputCtx->mouse.vel.x * 67;
					orbitSph.pitch += inputCtx->mouse.vel.y * 67;
				}
			}
			if (inputCtx->key[KEY_LEFT_CONTROL].hold) {
				viewCtx->fovyTarget = Clamp(viewCtx->fovyTarget * (1.0 + (inputCtx->mouse.scrollY / 20)), 20, 170);
			} else {
				orbitSph.r = ClampMin(orbitSph.r - (distMult * (inputCtx->mouse.scrollY)), 0.00001f);
				cam->eye = cam->at;
				
				Vec_AddVecSphToVec3f(&cam->eye, &orbitSph);
			}
		}
	}
}

void View_Init(ViewContext* viewCtx, InputContext* inputCtx) {
	Camera* cam;
	
	viewCtx->currentCamera = &viewCtx->camera[0];
	cam = viewCtx->currentCamera;
	
	cam->eye = (Vec3f) { -50.0f * 100, 50.0f * 100, 50.0f * 100 };
	cam->at = (Vec3f) { 0, 0, 0 };
	
	cam->eye = (Vec3f) { 0, 0, 50.0f * 100 };
	cam->at = (Vec3f) { 0, 0, 0 };
	cam->roll = 0;
	
	Matrix_LookAt(&viewCtx->viewMtx, cam->eye, cam->at, cam->roll);
	
	viewCtx->fovyTarget = viewCtx->fovy = 65;
	viewCtx->near = 10.0;
	viewCtx->far = 12800.0;
	viewCtx->scale = 1;
}

void View_Update(ViewContext* viewCtx, InputContext* inputCtx) {
	Camera* cam = viewCtx->currentCamera;
	MtxF model = gMtxFClear;
	
	Math_DelSmoothStepToF(&viewCtx->fovy, viewCtx->fovyTarget, 0.25, 5.25f, 0.00001);
	
	Matrix_Projection(
		&viewCtx->projMtx,
		viewCtx->fovy,
		(f32)viewCtx->projectDim.x / (f32)viewCtx->projectDim.y,
		viewCtx->near,
		viewCtx->far,
		viewCtx->scale
	);
	
	if (inputCtx->mouse.click.press || inputCtx->mouse.scrollY)
		viewCtx->setCamMove = 1;
	
	if (inputCtx->mouse.cursorAction == 0)
		viewCtx->setCamMove = 0;
	
	View_Camera_OrbitMode(viewCtx, inputCtx);
	View_Camera_FlyMode(viewCtx, inputCtx);
	Matrix_LookAt(&viewCtx->viewMtx, cam->eye, cam->at, cam->roll);
	
	Matrix_Scale(1.0, 1.0, 1.0, MTXMODE_NEW);
	Matrix_ToMtxF(&model);
}

void View_SetProjectionDimensions(ViewContext* viewCtx, Vec2s* dim) {
	viewCtx->projectDim = *dim;
}