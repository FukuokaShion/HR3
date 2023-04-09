#pragma once
#include"Object3d.h"
#include "Input.h"

#include"Wolf.h"

#include "ParticleManager.h"

class Player {
	

public:
	Player();
	~Player();

	void Initialize(Input* input);
	void Reset();
	
	void Update(Transform* cam);
	void Rota();
	
	void Attack();

	void Draw();
	void EffUpdate();
	void EffDraw();

	void OnCollision();

	void SetEnemyPos(Transform* enemyPos) { enemyPos_ = enemyPos; };

	void LightAttack();
	void HeavyAttack();
	void Dodge();

	bool CheckAttack2Enemy(Vector3 enemyPos, float& damage);


	Vector3 bVelocity(Vector3& velocity,Transform& worldTransform);
	
	////ワールド座標を取得
	Vector3 GetWorldPosition();

	float GetHp() { return hp; };
	bool GetIsDodge() { return isDodge; };

	Vector3 GetDodgeMoveVec() { return dodgeMoveVec; };

	/// <summary>
	/// ポジション
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(Vector3 pos) { bodyObj_->wtf.position = pos; };

private:
	const float PI = 3.141592;
	Input* input_ = nullptr;
	Collision col;
	//プレイヤー
	Model* bodyModel_ = nullptr;
	Object3d* bodyObj_ = nullptr;
	const float moveSpeed_ = 0.1f;
	const float rotaSpeed_ = 0.1f;

	//ステータス
	const int defaultHp = 100;
	int hp;
	bool isLive = true;

	//無敵時間
	bool isInvincible;
	const float invincibleLimit = 15;
	float invincibleTimer = invincibleLimit;

	//弱攻撃
	Vector3 lightAttackLPos;
	Vector3 lightAttackWPos;
	//何回めの連撃か
	int lightAttackCount;
	//一回の攻撃全体の時間
	float lightAttackLimit[4] = { 20,20,20,20 };
	float lightAttackTimer;
	//攻撃の当たり判定の有無
	bool isLightAttack;
	//判定が出始める時間
	float lightAttackPopTime[4] = { 15,15,15,15 };
	//次の連撃への入力受付開始時間
	float lightAttackInput[4] = { 10,10,10,0 };


	//強攻撃
	Vector3 heavyAttackLPos;
	Vector3 heavyAttackWPos;
	//何回めの連撃か
	int heavyAttackCount;
	//一回の攻撃全体の時間
	float heavyAttackLimit[2] = { 30,30 };
	float heavyAttackTimer;
	//攻撃の当たり判定の有無
	bool isHeavyAttack;
	//判定が出始める時間
	float heavyAttackPopTime[2] = { 20,20 };
	//次の連撃への入力受付開始時間
	float heavyAttackInput[2] = { 10,0 };


	//回避
	bool isDodge;
	const int dodgeLimit = 20;
	int dodgeTimer;
	Vector3 dodgeMoveVec;
	Vector3 dodgeMoveVecNomal;

	//敵
	Transform* enemyPos_;

	//パーティクルクラスの初期化 
	ParticleManager* particleManager = nullptr;
	//当たった時のエフェクト発生
	int isEffFlag = 0;
	int EffTimer = 0;

	//デバッグ用
	Model* debugModel_ = nullptr;
	Object3d* debugObj_ = nullptr;


public:
	int isAction;
	//バディ
	Wolf* wolf_ = nullptr;
};