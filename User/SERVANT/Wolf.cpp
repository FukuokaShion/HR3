#include"Wolf.h"

Wolf::Wolf() {
}

Wolf::~Wolf() {
	delete bodyObj_;
	delete bodyModel_;
}

void Wolf::Initialize() {
	bodyModel_ = Model::LoadFromOBJ("wolf");
	bodyObj_ = Object3d::Create();
	bodyObj_->SetModel(bodyModel_);
	bodyObj_->wtf.position = defaultPos;
	isAttack = false;
}

//‹ß‹——£
void Wolf::ShortRange() {

	float ShortSpeed = 2.0f;
	len = enemylen;
	enemylen *= ShortSpeed;
	enemylen.y = 0;

	if (coll.CircleCollision(enemyWtf->position, bodyObj_->wtf.position, 2.0f, 2.0f)) {
		Hit();
	}


	if (coll.CircleCollisionXZ(bodyObj_->wtf.position, enemyWtf->position, 2.0f, 2.0f)) {
		isAttack = false;
		bodyObj_->wtf.Initialize();
		bodyObj_->wtf.position = defaultPos;
	}
	else
	{
		bodyObj_->wtf.position += enemylen;
	}
}

//—­‚ß‹ß‹——£
void Wolf::ChargeShortRange() {
	float ShortSpeed = 2.0f;
	len = enemylen;
	enemylen *= ShortSpeed;
	enemylen.y = 0;

	continueAttack--;

	if (coll.CircleCollision(bodyObj_->wtf.position, enemyWtf->position, 2.0f, 2.0f)) {
		if (continueAttack <= 0) {
			Hit();

		}

	}
	if (coll.CircleCollisionXZ(bodyObj_->wtf.position, enemyWtf->position, 2.0f, 2.0f)) {
		if (continueAttack <= 0) {
			continueAttack = 10;
			hitCount++;
		}
	}
	else
	{
		bodyObj_->wtf.position += enemylen;
	}
	if (hitCount >= 3) {
		isAttack = false;
		bodyObj_->wtf.Initialize();
		bodyObj_->wtf.position = defaultPos;
		hitCount = 0;
		continueAttack = 0;
	}

}

//‰“‹——£
void Wolf::LongRange() {

	float ShortSpeed = 5.0f;

	if (longTime == 0) {
		len = enemylen;
		len *= ShortSpeed;
		len.y = 0;
	}

	continueAttack--;
	longTime++;


	if (coll.CircleCollision(bodyObj_->wtf.position, enemyWtf->position, 3.0f, 3.0f)) {
		if (continueAttack <= 0) {
			Hit();
			continueAttack = 20;
		}
	}
	bodyObj_->wtf.position += len;

	if (longTime >= 20) {
		isAttack = false;
		bodyObj_->wtf.Initialize();
		bodyObj_->wtf.position = defaultPos;
		hitCount = 0;
		continueAttack = 0;
		longTime = 0;
	}
}

//—­‚ß‰“‹——£
void Wolf::ChargeLongRange() {
	float ShortSpeed = 5.0f;
	ShortSpeed += 5.0f;
	if (longTime == 0) {

		len = enemylen;
		len *= ShortSpeed;
		len.y = 0;
	}


	if (coll.CircleCollisionXZ(bodyObj_->wtf.position, enemyWtf->position, 2.0f, 2.0f)) {
		if (continueAttack <= 0) {
			hitCount++;
		}
	}
	if (coll.CircleCollision(bodyObj_->wtf.position, enemyWtf->position, 2.0f, 2.0f)) {
		if (continueAttack <= 0) {
			Hit();
			continueAttack = 1;

		}
	}

	bodyObj_->wtf.position += len;
	longTime++;

	if (longTime >= 20) {
		continueAttack = 0;
		ShortSpeed = 1.0f;
		longTime = 0;
		if (hitCount >= 3) {
			isAttack = false;
			bodyObj_->wtf.Initialize();
			bodyObj_->wtf.position = defaultPos;
			hitCount = 0;
			continueAttack = 0;
			longTime = 0;
		}
	}




}

void Wolf::Attack(int attackNmb, Vector3 playerPos)
{

	coolTIme--;
	if (coolTIme <= 0) {
		if (isAttack == false) {
			isAttack = true;
			attackNmb_ = attackNmb;
			coolTIme = 30;
			Vector3 popPos = playerPos;
			bodyObj_->wtf.position = popPos;
		}
	}
}

void Wolf::Move() {
	//if (isAttack) {
	bodyObj_->Update();
	//}
	//else {
	//	bodyObj_->Update(playerWtf);
	//}
}

void Wolf::Update(Transform* enemyTransform) {
	enemyWtf = enemyTransform;

	enemylen = enemyWtf->position - bodyObj_->wtf.position;
	enemylen.nomalize();

	if (isAttack == true) {
		if (attackNmb_ == 1) {
			ShortRange();
		}
		else if (attackNmb_ == 2) {
			ChargeShortRange();
		}
		else if (attackNmb_ == 3) {
			LongRange();
		}
		else if (attackNmb_ == 4) {
			ChargeLongRange();
		}
	}

	Move();
}


void Wolf::Draw() {
	if (isAttack) {
		bodyObj_->Draw();
	}
}

//Œü‚¢‚Ä‚é•ûŒü‚Évelocity‚ð•ÏŠ·‚·‚é
Vector3 Wolf::bVelocity(Vector3& velocity, Transform& worldTransform)
{
	Vector3 result = { 0,0,0 };

	//“àÏ
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

void Wolf::Hit()
{

}

bool Wolf::CheckAttack2Enemy(Vector3 enemyPos, float& damage) {
	if (isAttack == true) {
		if (attackNmb_ == 1) {
			if (coll.CircleCollisionXZ(bodyObj_->wtf.position, enemyPos, 2.0f, 2.0f)) {
				damage = 5;
				return true;
			}
		}
		else if (attackNmb_ == 2) {
			if (coll.CircleCollisionXZ(bodyObj_->wtf.position, enemyPos, 2.0f, 2.0f)) {
				damage = 10;
				return true;
			}
		}
		else if (attackNmb_ == 3) {
			if (coll.CircleCollisionXZ(bodyObj_->wtf.position, enemyPos, 2.0f, 2.0f)) {
				damage = 3;
				return true;
			}
		}
		else if (attackNmb_ == 4) {
			if (coll.CircleCollisionXZ(bodyObj_->wtf.position, enemyPos, 2.0f, 2.0f)) {
				damage = 7;
				return true;
			}
		}
	}
	return false;
}