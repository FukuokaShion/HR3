#include "GameScene.h"


/// <summary>
	/// コンストクラタ
	/// </summary>
GameScene::GameScene() {
}

/// <summary>
/// デストラクタ
/// </summary>
GameScene::~GameScene() {
	delete spriteCommon;
	delete camera;
	delete player_;

	delete camOp;
	delete particleOp;
	delete bloodParticle;

	delete floor;
	delete skydome;
}

/// <summary>
/// 初期化
/// </summary>
void GameScene::Initialize(DirectXCommon* dxCommon, Input* input) {
	// nullチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	//スプライト共通部分の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

	// カメラ生成
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	camWtf.Initialize();
	camWtf.position = { 0.0f, 3.0f, -8.0f };

	targetWtf.Initialize();
	targetWtf.position = { 0.0f,0.0f,targetDistance };

	ParticleManager::SetCamera(camera);
	Object3d::SetCamera(camera);

	floorMD = Model::LoadFromOBJ("floor");
	floor = Object3d::Create();
	floor->SetModel(floorMD);
	floor->wtf.position = (Vector3{ 0, -10, 0 });

	skydomeMD = Model::LoadFromOBJ("skydome");
	skydome = Object3d::Create();
	skydome->SetModel(skydomeMD);
	skydome->wtf.scale = (Vector3{ 1000, 1000, 1000 });

	//プレイヤー
	player_ = new Player();
	player_->Initialize(input);

	//UI
	camOp = new Sprite();
	camOp->Initialize(spriteCommon);
	camOp->SetPozition({ 0,0 });
	camOp->SetSize({ 1280.0f, 720.0f });
;
	particleOp = new Sprite();
	particleOp->Initialize(spriteCommon);
	particleOp->SetPozition({ 0,0 });
	particleOp->SetSize({ 1280.0f, 720.0f });

	spriteCommon->LoadTexture(0, "camOp.png");
	camOp->SetTextureIndex(0);
	spriteCommon->LoadTexture(1, "particleOp.png");
	particleOp->SetTextureIndex(1);
	Reset();

	//パーティクル生成
	bloodParticle = ParticleManager::Create();
	bloodParticle->LoadTexture("blod.png");
	bloodParticle->Update();

}

void GameScene::Reset() {
	camWtf.Initialize();
	camWtf.position = { 0.0f, 3.0f, -8.0f };

	targetWtf.Initialize();
	targetWtf.position = { 0.0f,0.0f,targetDistance };

	player_->Reset();
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void GameScene::Update() {
	CamUpdate();
	player_->Update(&camWtf);

	if (input->PushKey(DIK_Q)) {
		isEffFlag = 1;
	}
	else if (input->PushKey(DIK_E)) {
		player_->OnCollision();
	}

	if (isEffFlag == 1) {
		EffTimer++;
	}if (EffTimer <= 10 && EffTimer >= 1) {
		EffUpdate();
	}if (EffTimer >= 11) {
		isEffFlag = 0;
		EffTimer = 0;
	}


	floor->Update();
	skydome->Update();
}

/// <summary>
/// 描画
/// </summary>
void GameScene::Draw() {

	/// <summary>
	/// 3Dオブジェクトの描画
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// <summary>
	//3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	//// 3Dオブクジェクトの描画
	player_->Draw();
	floor->Draw();
	skydome->Draw();
	//3Dオブジェクト描画後処理
	Object3d::PostDraw();


	//// パーティクルの描画

	// パーティクル描画前処理
	ParticleManager::PreDraw(dxCommon->GetCommandList());
	player_->EffDraw();
	EffDraw();
	// パーティクル描画後処理
	ParticleManager::PostDraw();

	camOp->Draw();
	particleOp->Draw();

}

void GameScene::EffUpdate()
{
	//パーティクル範囲
	for (int i = 0; i < 20; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 0.01f;
		Vector3 pos{};
		pos.x += (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y += (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.z += (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		//速度
		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		const float rnd_vel = 0.1f;
		Vector3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		const float rnd_acc = 0.00001f;
		Vector3 acc{};
		acc.x = (float)rand() / RAND_MAX * rnd_acc - rnd_acc / 2.0f;
		acc.y = (float)rand() / RAND_MAX * rnd_acc - rnd_acc / 2.0f;

		//追加
		bloodParticle->Add(60, pos, vel, acc, 1.0f, 0.0f);

		bloodParticle->Update();
	}

}

void GameScene::EffDraw()
{
	if (isEffFlag == 1) {
		// 3Dオブクジェクトの描画
		bloodParticle->Draw();
	}
}


void GameScene::CamMove() {
	if (input->LeftStickInput()) {
		//カメラの移動
		Vector3 eyeVelocity = { 0,0,0 };

		//通常移動
		if (player_->isAction == 0) {
			//入力
			Vector2 stickVec = input->GetLeftStickVec();

			eyeVelocity.x = stickVec.x;
			eyeVelocity.z = stickVec.y;

			eyeVelocity = eyeVelocity.nomalize();

			eyeVelocity *= camMoveSpeed;
		}
		//回避時移動
		else if (player_->isAction == 3) {
			eyeVelocity = player_->GetDodgeMoveVec();

		}

		//移動ベクトルを向いてる方向に合わせる
		eyeVelocity = bVelocity(eyeVelocity, camWtf);

		//更新
		camWtf.position += eyeVelocity;
	}

}

void GameScene::CamRota() {
	//視点移動

	//左右
	Vector3 theta;
	if (input->StickInput(R_LEFT) || input->PushKey(DIK_A)) {
		theta.y = -camRotaSpeed;
	}
	else if (input->StickInput(R_RIGHT) || input->PushKey(DIK_D)) {
		theta.y = camRotaSpeed;
	}
	camWtf.rotation += theta;

	//上下
	if (input->StickInput(R_UP) || input->PushKey(DIK_W)) {
		targetTheta += camRotaSpeed;
	}
	else if (input->StickInput(R_DOWN) || input->PushKey(DIK_S)) {
		targetTheta += -camRotaSpeed;
	}

	//角度制限
	if (targetTheta < -PI / 5 * 2) {//下の制限
		targetTheta = -PI / 5 * 2;
	}
	else if (targetTheta > PI / 3) { //上の制限
		targetTheta = PI / 3;
	}

	//視点は一定の距離
	targetWtf.position.z = cos(targetTheta) * targetDistance;
	targetWtf.position.y = sin(targetTheta) * targetDistance;
}

void GameScene::CamUpdate() {
	CamMove();
	CamRota();

	camWtf.UpdateMat();

	camera->SetEye(camWtf.position);

	targetWtf.UpdateMat();
	targetWtf.matWorld *= camWtf.matWorld;
	//y方向の制限
	if (targetWtf.matWorld.m[3][1] < 0) {
		targetWtf.matWorld.m[3][1] = 0;
	}
	camera->SetTarget({ targetWtf.matWorld.m[3][0],targetWtf.matWorld.m[3][1] ,targetWtf.matWorld.m[3][2] });

	camera->Update();
}

Vector3 GameScene::bVelocity(Vector3& velocity, Transform& worldTransform)
{
	Vector3 result = { 0,0,0 };

	//内積
	result.z = velocity.x * worldTransform.matWorld.m[0][2] +
		velocity.y * worldTransform.matWorld.m[1][2] +
		velocity.z * worldTransform.matWorld.m[2][2];

	result.x = velocity.x * worldTransform.matWorld.m[0][0] +
		velocity.y * worldTransform.matWorld.m[1][0] +
		velocity.z * worldTransform.matWorld.m[2][0];

	result.y = velocity.x * worldTransform.matWorld.m[0][1] +
		velocity.y * worldTransform.matWorld.m[1][1] +
		velocity.z * worldTransform.matWorld.m[2][1];

	return result;
}