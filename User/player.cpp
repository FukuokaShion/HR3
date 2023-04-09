#include"player.h"

Player::Player() {

}

Player::~Player() {
	delete bodyObj_;
	delete bodyModel_;
	delete wolf_;
	delete particleManager;


	delete debugObj_;
	delete debugModel_;
}

void Player::Initialize(Input* input) {
	input_ = input;

	//プレイヤー設定
	bodyModel_ = Model::LoadFromOBJ("as2");
	bodyObj_ = Object3d::Create();
	bodyObj_->SetModel(bodyModel_);

	//パーティクル生成
	particleManager = ParticleManager::Create();
	particleManager->LoadTexture("doge.png");
	particleManager->Update();


	//バディ
	wolf_ = new Wolf();
	wolf_->Initialize();
	wolf_->SetPlayerWtf(&bodyObj_->wtf);
	enemyPos_ = new Transform;
	enemyPos_->Initialize();

	//デバッグ用
	debugModel_ = Model::LoadFromOBJ("boll");
	debugObj_ = Object3d::Create();
	debugObj_->SetModel(debugModel_);

	Reset();
}

void Player::Reset() {
	bodyObj_->wtf.Initialize();
	bodyObj_->wtf.position = { 0,-3,8 };
	hp = defaultHp;
	isAction = 0;
	isLive = true;

	//弱攻撃
	lightAttackLPos = { 0,0,3 };
	//何回めの連撃か
	lightAttackCount = 0;
	//攻撃の当たり判定の有無
	isLightAttack = false;
	lightAttackTimer = 60;


	//弱攻撃
	heavyAttackLPos = { 0,0,5 };
	//何回めの連撃か
	heavyAttackCount = 0;
	//攻撃の当たり判定の有無
	isHeavyAttack = false;
	heavyAttackTimer = 60;


	//回避設定
	dodgeTimer = dodgeLimit;
	isDodge = false;
}

void Player::Attack() {
	if (isAction == 0) {
		//バディ指示
		if (input_->PushKey(DIK_LSHIFT) || input_->ButtonInput(LT)) {
			if (input_->PushKey(DIK_1) || input_->ButtonInput(B)) {
				//近距離
				wolf_->Attack(1, GetWorldPosition());

			}
			else if (input_->PushKey(DIK_2) || input_->ButtonInput(A)) {
				//遠距離
				wolf_->Attack(2, GetWorldPosition());

			}
			else if (input_->PushKey(DIK_3) || input_->ButtonInput(Y)) {
				//溜め近距離
				wolf_->Attack(3, GetWorldPosition());

			}
			else if (input_->PushKey(DIK_4) || input_->ButtonInput(X)) {
				//溜め遠距離
				wolf_->Attack(4, GetWorldPosition());
			}
		}
		//本体攻撃入力
		else {
			//弱攻撃
			if (input_->PushKey(DIK_4) || input_->ButtonInput(X)) {
				isAction = 1;
				lightAttackCount = 0;
				lightAttackTimer = lightAttackLimit[0];
			}
			//強攻撃
			if (input_->PushKey(DIK_1) || input_->ButtonInput(Y)) {
				isAction = 2;
				heavyAttackCount = 0;
				heavyAttackTimer = heavyAttackLimit[0];
			}
			//回避
			if (input_->PushKey(DIK_3) || input_->ButtonInput(B)) {
				if (input_->LeftStickInput()) {
					isAction = 3;
					isDodge = true;
					dodgeTimer = dodgeLimit;
					Vector2 stickVec = input_->GetLeftStickVec();
					dodgeMoveVec = { stickVec.x,0,stickVec.y };
					dodgeMoveVecNomal = dodgeMoveVec.nomalize();
				}
			}
			//null
			if (input_->TriggerKey(DIK_2) || input_->ButtonInput(A)) {
			}
		}
	}

	//攻撃更新
	//弱攻撃
	else if (isAction == 1) {
		LightAttack();
	}
	//強攻撃
	else if (isAction == 2) {
		HeavyAttack();
	}
	//回避
	else if(isAction==3){
		Dodge();
	}
}

void Player::OnCollision() {
	if (isInvincible == false) {
		//回避時
		if (isDodge) {

		}
		//通常時
		else {
			hp -= 0;
			isEffFlag = 1;
			isInvincible = true;
			invincibleTimer = invincibleLimit;

		}
	}
}

void Player::Rota() {
	if (isAction == 0) {
		if (input_->LeftStickInput()) {
			Vector2 stickVec = input_->GetLeftStickVec();

			float theta = atan2(stickVec.x, stickVec.y);

			bodyObj_->wtf.rotation.y = theta;
		}
	}
}

void Player::Update(Transform* cam) {
	if (isInvincible) {
		invincibleTimer--;
		if (invincibleTimer < 0) {
			isInvincible=false;
		}
	}

	Rota();
	Attack();

	if (isEffFlag == 1) {
		EffTimer++;
	}
	if (EffTimer <= 10 && EffTimer >= 1) {
		EffUpdate();
	}
	if (EffTimer >= 11) {
		isEffFlag = 0;
		EffTimer = 0;
	}


	bodyObj_->Update(cam);
	wolf_->Update(enemyPos_);

	debugObj_->Update();
}

void Player::Draw() {
	if (isLive) {
		bodyObj_->Draw();
		wolf_->Draw();

		//デバッグ用
		if (isLightAttack) {
			debugObj_->Draw();
		}
		if (isHeavyAttack) {
			debugObj_->Draw();
		}
	}
}

void Player::EffUpdate()
{
	//パーティクル範囲
	for (int i = 0; i < 20; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 0.01f;
		Vector3 pos{};
		pos.x += (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y += (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.z += (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos += GetWorldPosition();
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
		particleManager->Add(60, pos, vel, acc, 1.0f, 0.0f);

		particleManager->Update();
	}

}

void Player::EffDraw()
{
	if (isEffFlag == 1) {
		// 3Dオブクジェクトの描画
		particleManager->Draw();
	}
	else{}
}

Vector3 Player::bVelocity(Vector3& velocity,Transform& worldTransform)
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

Vector3 Player::GetWorldPosition()
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分
	worldPos.x = bodyObj_->wtf.matWorld.m[3][0];
	worldPos.y = bodyObj_->wtf.matWorld.m[3][1];
	worldPos.z = bodyObj_->wtf.matWorld.m[3][2];


	return worldPos;
}

bool Player::CheckAttack2Enemy(Vector3 enemyPos, float& damage) {
	//弱攻撃
	if (isAction == 1) {
		//当たり判定が出てるか
		if (isLightAttack) {
			//当たり判定
			if (col.CircleCollisionXZ(lightAttackWPos, enemyPos, 0.5f, 1.0f)) {
				damage = 3;
				return true;
			}
		}
	}

	//強攻撃
	else if (isAction == 2) {
		//当たり判定が出てるか
		if (isHeavyAttack) {
			//当たり判定
			if (col.CircleCollisionXZ(heavyAttackWPos, enemyPos, 1.0f, 1.0f)) {
				damage = 7;
				return true;
			}
		}
	}

	return false;
}

void Player::LightAttack() {
	
	lightAttackTimer--;
	//攻撃の終了
	if (lightAttackTimer <= 0) {
		isAction = 0;
		isLightAttack = false;
	}

	//1撃目
	if (lightAttackCount == 0) {
		//当たり判定の出現
		if (lightAttackTimer <= lightAttackPopTime[0] && lightAttackTimer > 0) {
			isLightAttack = true;
		}

		//当たり判定の移動
		if (isLightAttack) {
			//移動
			lightAttackLPos = { 1.0f,0,3.0f };
			//更新
			lightAttackWPos = lightAttackLPos * bodyObj_->wtf.matWorld;
			debugObj_->wtf.position = lightAttackWPos;
		}

		//次の斬撃入力
		if (input_->ButtonInput(X)) {
			//入力受付時間
			if (lightAttackTimer < lightAttackInput[lightAttackCount] && lightAttackTimer > 0) {
				//次の斬撃設定
				lightAttackCount++;
				lightAttackTimer = lightAttackLimit[lightAttackCount];
				isLightAttack = false;
			}
		}
	}
	//2撃目
	else if (lightAttackCount == 1) {
		//当たり判定の出現
		if (lightAttackTimer <= lightAttackPopTime[1] && lightAttackTimer > 0) {
			isLightAttack = true;
		}

		//当たり判定の移動
		if (isLightAttack) {
			//移動
			lightAttackLPos = { -1.0f,0,3.0f };
			//更新
			lightAttackWPos = lightAttackLPos * bodyObj_->wtf.matWorld;
			debugObj_->wtf.position = lightAttackWPos;
		}

		//次の斬撃入力
		if (input_->ButtonInput(X)) {
			//入力受付時間
			if (lightAttackTimer < lightAttackInput[lightAttackCount] && lightAttackTimer > 0) {
				//次の斬撃設定
				lightAttackCount++;
				lightAttackTimer = lightAttackLimit[lightAttackCount];
				isLightAttack = false;
			}
		}
	}
	//3撃目
	else if (lightAttackCount == 2) {
		//当たり判定の出現
		if (lightAttackTimer <= lightAttackPopTime[2] && lightAttackTimer > 0) {
			isLightAttack = true;
		}

		//当たり判定の移動
		if (isLightAttack) {
			//移動
			lightAttackLPos = { 1.0f,0,3.0f };
			//更新
			lightAttackWPos = lightAttackLPos * bodyObj_->wtf.matWorld;
			debugObj_->wtf.position = lightAttackWPos;
		}

		//次の斬撃入力
		if (input_->ButtonInput(X)) {
			//入力受付時間
			if (lightAttackTimer < lightAttackInput[lightAttackCount] && lightAttackTimer > 0) {
				//次の斬撃設定
				lightAttackCount++;
				lightAttackTimer = lightAttackLimit[lightAttackCount];
				isLightAttack = false;
			}
		}
	}
	//4撃目
	else if (lightAttackCount == 3) {
		//当たり判定の出現
		if (lightAttackTimer <= lightAttackPopTime[3] && lightAttackTimer > 0) {
			isLightAttack = true;
		}

		//当たり判定の移動
		if (isLightAttack) {
			//Lposが相対座標、ここを変える
			lightAttackLPos = { 0,0,4.0f };
			//ワールド座標に変換
			lightAttackWPos = lightAttackLPos * bodyObj_->wtf.matWorld;
			debugObj_->wtf.position = lightAttackWPos;
		}
	}
}

void Player::HeavyAttack() {
	heavyAttackTimer--;
	//攻撃の終了
	if (heavyAttackTimer <= 0) {
		isAction = 0;
		isHeavyAttack = false;
	}

	//1撃目
	if (heavyAttackCount == 0) {
		//当たり判定の出現
		if (heavyAttackTimer <= heavyAttackPopTime[0] && heavyAttackTimer > 0) {
			isHeavyAttack = true;
		}

		//当たり判定の移動
		if (isHeavyAttack) {
			//移動
			heavyAttackLPos = { 0,0,4.0f };
			//更新
			heavyAttackWPos = heavyAttackLPos * bodyObj_->wtf.matWorld;
			debugObj_->wtf.position = heavyAttackWPos;
		}

		//次の斬撃入力
		if (input_->ButtonInput(X)) {
			//入力受付時間
			if (heavyAttackTimer < heavyAttackInput[heavyAttackCount] && heavyAttackTimer > 0) {
				//次の斬撃設定
				heavyAttackCount++;
				heavyAttackTimer = heavyAttackLimit[heavyAttackCount];
				isHeavyAttack = false;
			}
		}
	}
	//2撃目
	else if (heavyAttackCount == 1) {
		//当たり判定の出現
		if (heavyAttackTimer <= heavyAttackPopTime[1] && heavyAttackTimer > 0) {
			isHeavyAttack = true;
		}

		//当たり判定の移動
		if (isHeavyAttack) {
			//移動
			heavyAttackLPos = { 0,0,6.0f };
			//更新
			heavyAttackWPos = heavyAttackLPos * bodyObj_->wtf.matWorld;
			debugObj_->wtf.position = heavyAttackWPos;
		}
	}
}

void Player::Dodge() {
	dodgeTimer--;

	//移動速度変更
	dodgeMoveVec = dodgeMoveVecNomal * (0.4f * pow((30 / dodgeLimit), 2));


	if (dodgeTimer < 0) {
		isAction = 0;
		isDodge = false;
	}

}