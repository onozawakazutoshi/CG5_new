#include "Parrticle.h"

using namespace KamataEngine;
using namespace MathUtility;


void Parrticle::Initialize(Model* model, Vector3 position, Vector3 velocity) { 
	assert(model);
	model_ = model;
	worldtransform_.Initialize();
	worldtransform_.translation_ = position;
	worldtransform_.scale_ = {0.2f, 0.2f, 0.2f};

	velocity_ = velocity;
	objectColor_.Initialize();
	color_ = {1, 1, 0, 1};

}

void Parrticle::Update() {
	
	if (isFinished_) {
		return;
	}

	counter_ += 1.0f / 60.0f;

	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	} 
	worldtransform_.translation_ += velocity_;
	//worldtransform_.TransferMatrix();

	worldtransform_.UpdateMatrix();

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);

	objectColor_.SetColor(color_);

	

}

void Parrticle::Draw(Camera& camera) { 
	model_->Draw(worldtransform_, camera,&objectColor_);

}
