#pragma once
#include <KamataEngine.h>
#include "Parrticle.h"

class GameScene {

public:
	GameScene() {}
	~GameScene() { 
		delete modelParticle_;
		for (Parrticle* particle : particles_) {
			delete particle;
		}
		particles_.clear();
	}

	void Initialize();

	void Update();

	void Draw();

	void ParticleBorn(KamataEngine::Vector3 position);

private:
	KamataEngine::Model* modelParticle_ = nullptr;
	KamataEngine::Camera* camera_;
	std::list<Parrticle*> particles_;
};
