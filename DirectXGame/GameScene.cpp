#include "GameScene.h"
#include <random>

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());
std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

using namespace KamataEngine;
using namespace MathUtility;

void GameScene::Initialize() {
	modelParticle_ = Model::CreateSphere(4, 4); 
	camera_ = new Camera;
	camera_->Initialize();
	
	srand((unsigned)time(NULL));
}

void GameScene::Update() { 
	if (rand() % 20 == 0) {
		Vector3 position = {distribution(randomEngine) * 30.0f, distribution(randomEngine) * 20.0f, 0};
		ParticleBorn(position);
	}
	for (Parrticle* particle : particles_) {
		particle->Update();
	}
	particles_.remove_if([](Parrticle* particle) {
		if (particle->IsFinished()) {
			delete particle;
			return true;
		}
		return false;
	});
	

}

void GameScene::Draw() { 
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	for (Parrticle* particle : particles_) {
		particle->Draw(*camera_);
	}
	Model::PostDraw();
	

}

void GameScene::ParticleBorn(Vector3 position) {
	for (int i = 0; i < 150; i++) {
		Parrticle* particle_ = new Parrticle();
		Vector3 position_ = position;

		Vector3 velocity = {distribution(randomEngine), distribution(randomEngine), 0};

		Normalize(velocity);
		velocity *= distribution(randomEngine);
		velocity *= 0.1f;

		particle_->Initialize(modelParticle_, position_, velocity);
		particles_.push_back(particle_);
	}
}
