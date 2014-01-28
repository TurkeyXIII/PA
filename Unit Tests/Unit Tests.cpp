// Unit Tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "palib.h"
#include "StubHitbox.h"

#include "Hitbox.h"


TEST_CASE("reference counter")
{
	Ptr<IObject> pObject1 = new IObject();

	CHECK(pObject1->GetCount() == 1);

	IObject* pObject2 = pObject1;
	CHECK(pObject2->GetCount() == 1);

	IObject* pObject3 = new IObject();
	CHECK(pObject3->GetCount() == 0);


	pObject1 = pObject3;
//	CHECK(pObject2->GetCount() == 0); //Object2 should actually have been deleted by now.

	CHECK(pObject1->GetCount() == 1);

	{
		Ptr<IObject> pLocalObject = pObject1;

		CHECK(pObject1->GetCount() == 2);
	}

	CHECK(pObject1->GetCount() == 1);
	pObject1 = NULL;
	CHECK(!pObject1);
	
}

TEST_CASE("PAVECTOR arithmetic")
{
	PAVECTOR unit(1.0f, 1.0f, 1.0f);
	PAVECTOR x(5.0f, 0.0f, 0.0f);
	PAVECTOR y(0.0f, 5.0f, 0.0f);

	CHECK(unit == PAVECTOR(1.0f, 1.0f, 1.0f));
	CHECK(unit != PAVECTOR(1.01f, 1.0f, 1.0f));

	CHECK((unit + x) == PAVECTOR(6.0f, 1.0f, 1.0f));
	CHECK((y - unit) == PAVECTOR(-1.0f, 4.0f, -1.0f));
}

TEST_CASE("hitbox creation")
{
	CHitbox hitbox;

	SECTION("cube")
	{
		hitbox.AddVert(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		hitbox.AddVert(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		hitbox.AddVert(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		hitbox.AddVert(D3DXVECTOR3(0.0f, 1.0f, 1.0f));
		hitbox.AddVert(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
		hitbox.AddVert(D3DXVECTOR3(1.0f, 0.0f, 1.0f));
		hitbox.AddVert(D3DXVECTOR3(1.0f, 1.0f, 0.0f));
		hitbox.AddVert(D3DXVECTOR3(1.0f, 1.0f, 1.0f));

		// check inside each corner
		CHECK(hitbox.Contains(&PAVECTOR(0.1f, 0.1f, 0.1f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.1f, 0.1f, 0.9f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.1f, 0.9f, 0.1f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.1f, 0.9f, 0.9f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.9f, 0.1f, 0.1f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.9f, 0.1f, 0.9f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.9f, 0.9f, 0.1f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.9f, 0.9f, 0.9f)));

		// and on each face
		CHECK(hitbox.Contains(&PAVECTOR(0.0f, 0.5f, 0.5f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.5f, 0.0f, 0.5f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.5f, 0.5f, 0.0f)));
		CHECK(hitbox.Contains(&PAVECTOR(1.0f, 0.5f, 0.5f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.5f, 1.0f, 0.5f)));
		CHECK(hitbox.Contains(&PAVECTOR(0.5f, 0.5f, 1.0f)));
	}
}

TEST_CASE("body creation")
{
	CPhysicsLogicFactory god;
	Ptr<IPhysicsLogic> pWorld = god.Create();
	
	SECTION("world exists")
	{	
		CHECK(pWorld->GetCount() == 1);
	}

	SECTION("body from def file")
	{
		Ptr<CBodyFactory> bodyFact = new CBodyFactory();
		std::string filename ("rigid4vert.xml");

		SECTION(filename)
		{
			Ptr<IBody> pBody1 = bodyFact->Create(&filename);

			REQUIRE((bool)pBody1);
			CHECK(pBody1->GetCount() == 1);

			IHitbox* pHitbox = (IHitbox*)new StubHitbox();
			pBody1->AddHitbox(pHitbox);
			CHECK(pHitbox->GetCount() == 1);

			//Other requirements of the body object...
			PAVECTOR insideStub(45.0f, 12.5f, 923.0f);
			CHECK(pBody1->Contains(&insideStub));


			PAVECTOR inside1(0.0f, 0.0f, 0.5f);
			PAVECTOR outside1(0.0f, 0.0f, -0.5f);

			CHECK(pBody1->Contains(&inside1));
			CHECK(!pBody1->Contains(&outside1));

			PAVECTOR inside2(-1.0f, -1.0f, 0.0f);
			PAVECTOR outside2(-1.0f, -1.0f, 0.01f);

			CHECK(pBody1->Contains(&inside2));
			CHECK(!pBody1->Contains(&outside2));
		}

		filename = "rigid4vert4vert.xml";
		SECTION(filename)
		{
			Ptr<IBody> pBody2 = bodyFact->Create(&filename);
			REQUIRE((bool)pBody2);

			PAVECTOR inside1(0.0f, 0.0f, 3.1f);
			PAVECTOR outside(0.0f, 0.0f, 5.01f);
			PAVECTOR inside2(0.0f, 0.0f, 2.5f);
			PAVECTOR inside3(0.0f, 0.0f, 5.0f);

			CHECK(pBody2->Contains(&inside1));
			CHECK(pBody2->Contains(&inside2));
			CHECK(pBody2->Contains(&inside3));
			CHECK(!pBody2->Contains(&outside));
		}

		filename = "rigid5vertsimple.xml";
		SECTION(filename)
		{
			Ptr<IBody> pBody3 = bodyFact->Create(&filename);
			REQUIRE((bool)pBody3);

			PAVECTOR outside1(0.0f, -2.0f, 1.9f);
			PAVECTOR inside1(0.0f, -1.99f, 2.0f);
			PAVECTOR inside2(0.05f, 0.8f, 0.05f);
			PAVECTOR inside3(-0.05f, 0.8f, 0.05f);

			CHECK(!pBody3->Contains(&outside1));
			CHECK(pBody3->Contains(&inside1));
			CHECK(pBody3->Contains(&inside2));
			CHECK(pBody3->Contains(&inside3));
		}

		filename = "rigid5vertredundant.xml";
		SECTION(filename)
		{
			Ptr<IBody> pBody4 = bodyFact->Create(&filename);
			REQUIRE((bool)pBody4);

			PAVECTOR outside1(0.0f, 0.0f, 5.1f);
			PAVECTOR inside1(0.0f, 0.0f, 2.5f);
			PAVECTOR inside2(0.0f, 0.0f, 5.0f);

			PAVECTOR inside3(-0.09f, 0.01f, 3.0f);
			PAVECTOR inside4(0.09f, 0.01f, 3.0f);
			PAVECTOR inside5(0.0f, -0.1f, 3.0f);

			PAVECTOR outside2(0.0f, -50.0f, 3.0f);
			PAVECTOR inside6(0.0f, 0.1f, 4.0f);

			CHECK(!pBody4->Contains(&outside1));
			CHECK(pBody4->Contains(&inside1));
			CHECK(pBody4->Contains(&inside2));

			CHECK(pBody4->Contains(&inside3));
			CHECK(pBody4->Contains(&inside4));
			CHECK(pBody4->Contains(&inside5));

			CHECK(!pBody4->Contains(&outside2));
			CHECK(pBody4->Contains(&inside6));
		}
		
		filename = "rigid13vertredundant.xml";
		SECTION(filename)
		{
			Ptr<IBody> pBody5 = bodyFact->Create(&filename);
			REQUIRE((bool)pBody5);

			PAVECTOR outside1(1.0f, 0.0f, 5.0f);
			PAVECTOR outside2(-1.0f, 0.0f, 5.0f);
			PAVECTOR outside3(0.0f, 1.0f, 5.0f);
			PAVECTOR outside4(0.0f, -1.0f, 5.0f);

			CHECK(!pBody5->Contains(&outside1));
			CHECK(!pBody5->Contains(&outside2));
			CHECK(!pBody5->Contains(&outside3));
			CHECK(!pBody5->Contains(&outside4));


			PAVECTOR inside1(1.0f, 0.0f, 3.3f);
			PAVECTOR inside2(-1.0f, 0.0f, 3.3f);
			PAVECTOR inside3(0.0f, 1.0f, 3.3f);
			PAVECTOR inside4(0.0f, -1.0f, 3.3f);

			CHECK(pBody5->Contains(&inside1));
			CHECK(pBody5->Contains(&inside2));
			CHECK(pBody5->Contains(&inside3));
			CHECK(pBody5->Contains(&inside4));

		}

		filename = "material.xml";
		SECTION(filename)
		{
			Ptr<IBody> pBody6 = bodyFact->Create(&filename);
			REQUIRE((bool)pBody6);

			CHECK(pBody6->GetDensity() == 10.0f);
			CHECK(pBody6->GetYoungsModulus() == 1.0f);
		}

		filename = "rigid3dscube.xml";
		SECTION(filename)
		{
			Ptr<IBody> pBody7 = bodyFact->Create(&filename);
			REQUIRE((bool)pBody7);

			PAVECTOR inside1(0.9f, 0.9f, 0.9f);
			PAVECTOR inside2(0.9f, 0.9f, -0.9f);
			PAVECTOR inside3(-0.9f, -0.9f, 0.9f);
			PAVECTOR inside4(-0.9f, -0.9f, -0.9f);

			CHECK(pBody7->Contains(&inside1));
			CHECK(pBody7->Contains(&inside2));
			CHECK(pBody7->Contains(&inside3));
			CHECK(pBody7->Contains(&inside4));

			PAVECTOR outside1(0.0f, 0.0f, 1.1f);
			PAVECTOR outside2(0.0f, 0.0f, -1.1f);

			CHECK(!pBody7->Contains(&outside1));
			CHECK(!pBody7->Contains(&outside2));
		}

		filename = "rigid3ds3cube.xml";
		SECTION(filename)
		{
			Ptr<IBody> pBody8 = bodyFact->Create(&filename);
			REQUIRE((bool)pBody8);

			PAVECTOR inside1(2.0f, -1.0, 0.0f);
			PAVECTOR inside2(-1.0f, 0.0f, 0.0f);
			PAVECTOR inside3(1.0f, 2.0f, 2.0f);

			CHECK(pBody8->Contains(&inside1));
			CHECK(pBody8->Contains(&inside2));
			CHECK(pBody8->Contains(&inside3));

			PAVECTOR outside1(0.0f, 0.0f, 0.0f);
			PAVECTOR outside2(1.0f, 1.4f, 2.0f);

			CHECK(!pBody8->Contains(&outside1));
			CHECK(!pBody8->Contains(&outside2));
		}
	}

	SECTION("world owns body")
	{
		Ptr<CBodyFactory> bodyFact = new CBodyFactory();
		Ptr<IBody> pBody = bodyFact->Create(&std::string("rigid5vertsimple.xml"));

		unsigned long bodyCount = pBody->GetCount();

		pWorld->AddBody(pBody);
		CHECK(pBody->GetCount() > bodyCount);
	}
}

TEST_CASE("body position/orientation")
{
	Ptr<CBodyFactory> pBodyFact = new CBodyFactory();
	
	SECTION("body position")
	{
		Ptr<IBody> pBody = pBodyFact->Create(&std::string("rigid3dscube.xml"));
		REQUIRE((bool)pBody);

		PAVECTOR origin(0.0f, 0.0f, 0.0f);

		CHECK(pBody->Contains(&origin));

		PAVECTOR pos1(100.0f, -50.0f, -1.0f);
		pBody->SetPosition(pos1);
		CHECK(!pBody->Contains(&origin));
		CHECK(pBody->Contains(&pos1));

		PAVECTOR pos2(-2000.1f, -150.0f, 9999990.9f);
		pBody->SetPosition(pos2);
		CHECK(!pBody->Contains(&pos1));
		CHECK(pBody->Contains(&pos2));
	}
	SECTION("body orientation")
	{
		Ptr<IBody> pBody = pBodyFact->Create(&std::string("rigid4vert.xml"));
		REQUIRE((bool)pBody);


		PAVECTOR up(0.0f, 0.0f, 1.0f);
		PAVECTOR down(0.0f, 0.0f, -1.0f);
		PAVECTOR forward(0.0f, 1.0f, 0.0f);
		PAVECTOR backward(0.0f, -1.0f, 0.0f);
		PAVECTOR left(-1.0f, 0.0f, 0.0f);
		PAVECTOR right(1.0f, 0.0f, 0.0f);
	
		CHECK(pBody->GetForwardPAVector() == forward);
		CHECK(pBody->GetUpPAVector() == up);

		pBody->AdjustRoll((float)M_PI);
		CHECK(pBody->GetForwardPAVector() == forward);
		CHECK(pBody->GetUpPAVector() == down);

		pBody->AdjustRoll((float)M_PI_2);
		CHECK(pBody->GetForwardPAVector() == forward);
		CHECK(pBody->GetUpPAVector() == left);

		pBody->AdjustRoll((float)M_PI_2 * 3.0f);
		CHECK(pBody->GetForwardPAVector() == forward);
		CHECK(pBody->GetUpPAVector() == down);

		pBody->AdjustPitch((float)M_PI_2);
		CHECK(pBody->GetForwardPAVector() == down);
		CHECK(pBody->GetUpPAVector() == backward);

		pBody->AdjustPitch((float)-M_PI_2);
		CHECK(pBody->GetForwardPAVector() == forward);
		CHECK(pBody->GetUpPAVector() == down);

		pBody->AdjustPitch((float)M_PI);
		CHECK(pBody->GetForwardPAVector() == backward);
		CHECK(pBody->GetUpPAVector() == up);

		pBody->AdjustYaw((float)M_PI_2);
		CHECK(pBody->GetForwardPAVector() == right);
		CHECK(pBody->GetUpPAVector() == up);

		pBody->AdjustRoll((float)M_PI_2);
		CHECK(pBody->GetForwardPAVector() == right);
		CHECK(pBody->GetUpPAVector() == backward);
		CHECK(pBody->Contains(&PAVECTOR(0.0f, -2.5f, 0.0f)));

		pBody->AdjustYaw((float)M_PI_2);
		CHECK(pBody->GetForwardPAVector() == up);
		CHECK(pBody->GetUpPAVector() == backward);

		pBody->AdjustPitch((float)-M_PI_2);
		CHECK(pBody->GetForwardPAVector() == forward);
		CHECK(pBody->GetUpPAVector() == up);

		pBody->AdjustRoll((float)M_PI_4);
		CHECK(pBody->GetForwardPAVector() == forward);
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.7071f, 0.0f, 0.7071f));

		pBody->AdjustYaw((float)M_PI_2);
		CHECK(pBody->GetForwardPAVector() == PAVECTOR(-0.7071f, 0.0f, 0.7071f));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.7071f, 0.0f, 0.7071f));
		CHECK(pBody->Contains(&PAVECTOR(1.767767f, 0.0f, 1.767767f)));
	}

	SECTION("body position and orientation")
	{
		Ptr<IBody> pBody = pBodyFact->Create(&std::string("rigid4vert.xml"));
		REQUIRE((bool)pBody);

		PAVECTOR inside(1.767767f, 0.0f, 1.767767f);
		pBody->AdjustRoll((float)M_PI_4);
		pBody->AdjustYaw((float)M_PI_2);

		CHECK(pBody->Contains(&inside));
		
		PAVECTOR newPos(435.87f, -2734.0f, 532188.76f);

		pBody->SetPosition(newPos);
		CHECK(pBody->Contains(&(inside + newPos)));

	}

}

TEST_CASE("Bodies have velocities")
{
	CPhysicsLogicFactory god;
	Ptr<IPhysicsLogic> pWorld = god.Create();
	Ptr<CBodyFactory> bodyFact = new CBodyFactory();

	Ptr<IBody> pBody = bodyFact->Create(&std::string("rigid3dscube.xml"));

	REQUIRE((bool)pBody);

	SECTION("linear velocity")
	{
		PAVECTOR forward(0.0f, 1.0f, 0.0f);

		pBody->SetVelocity(forward);
		CHECK(pBody->GetVelocity() == forward);

		pBody->AdjustVelocity(forward);
		CHECK(pBody->GetVelocity() == PAVECTOR(0.0f, 2.0f, 0.0f));

		CHECK(pBody->GetPosition() == PAVECTOR(0.0f, 0.0f, 0.0f));

		pBody->Update(500);

		CHECK(pBody->GetPosition() == forward);

		pBody->Update(2000);

		CHECK(pBody->GetPosition() == PAVECTOR(0.0f, 4.0f, 0.0f));
		CHECK(pBody->Contains(&PAVECTOR(0.0f, 4.0f, 0.0f)));

	}

	SECTION("angular velocity")
	{
		pBody->AdjustRollVelocity((float)M_PI_4/2.0f);

		pBody->Update(500);

		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.19509f, 0.0f, 0.98079f));

		pBody->Update(16000);

		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.0f, 0.0f, 1.0f));

		pBody->Update(20000);
		CHECK(pBody->GetUpPAVector() == PAVECTOR(1.0f, 0.0f, 0.0f));

		pBody->AdjustRollVelocity((float)-M_PI_4/2.0f);
		pBody->AdjustPitchVelocity((float)M_PI_2);

		pBody->Update(1000);

		CHECK(pBody->GetForwardPAVector() == PAVECTOR(1.0, 0.0f, 0.0f));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.0, -1.0f, 0.0f));

		pBody->Update(22500);

		CHECK(pBody->GetForwardPAVector() == PAVECTOR(-0.7071f, -0.7071f, 0.0f));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(-0.7071f, 0.7071f, 0.0f));

		pBody->Update(23000);
		pBody->AdjustPitchVelocity((float)-M_PI_2);

		pBody->Update(30000);

		CHECK(pBody->GetForwardPAVector() == PAVECTOR(-1.0f, 0.0f, 0.0f));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.0f, 1.0f, 0.0f));

		pBody->AdjustYawVelocity((float)M_PI_4);

		pBody->Update(32500);

		CHECK(pBody->GetForwardPAVector() == PAVECTOR(0.38268f, 0.0f, 0.92388f));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.0f, 1.0f, 0.0f));

		pBody->StopRotation();
		pBody->Update(100000);

		CHECK(pBody->GetForwardPAVector() == PAVECTOR(0.38268f, 0.0f, 0.92388f));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.0f, 1.0f, 0.0f));
	}

	
	SECTION("off-axis angular velocity")
	{
		pBody->AdjustAxisVelocity(PAVECTOR(1.0f, -1.0f, 0.0f), (float)M_PI_4);
		
		pBody->GetForwardPAVector();
		pBody->GetUpPAVector();

		pBody->Update(4000);

		CHECK(pBody->GetForwardPAVector() == PAVECTOR(-1.0, 0.0, 0.0));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.0f, 0.0f, -1.0f));

		pBody->Update(8000);

		CHECK(pBody->GetForwardPAVector() == PAVECTOR(0.0, 1.0, 0.0));
		CHECK(pBody->GetUpPAVector() == PAVECTOR(0.0f, 0.0f, 1.0f));
	}
	
}


TEST_CASE("World updates")
{
	CPhysicsLogicFactory god;
	Ptr<IPhysicsLogic> pWorld = god.Create();
	Ptr<CBodyFactory> bodyFact = new CBodyFactory();

	Ptr<IBody> pBody1 = bodyFact->Create(&std::string("rigid3dscube.xml"));
	REQUIRE((bool)pBody1);
	pWorld->AddBody(pBody1);

	Ptr<IBody> pBody2 = bodyFact->Create(&std::string("rigid3dscube.xml"));
	REQUIRE((bool)pBody2);
	pWorld->AddBody(pBody2);

	pBody1->SetVelocity(PAVECTOR(0.0f, 10.0f, 0.0f));
	pBody2->SetPosition(PAVECTOR(10.0f, 0.0f, 0.0f));
	pBody2->AdjustAxisVelocity(PAVECTOR(1.0f, -1.0f, 0.0f), (float)M_PI_4);

	CHECK(pBody1->GetPosition() == PAVECTOR(0.0f, 0.0f, 0.0f));
	CHECK(pBody2->GetForwardPAVector() == PAVECTOR(0.0f, 1.0f, 0.0f));
	CHECK(pBody2->GetUpPAVector() == PAVECTOR(0.0f, 0.0f, 1.0f));
	
	pWorld->Update(2000);

	CHECK(pBody1->GetPosition() == PAVECTOR(0.0f, 20.0f, 0.0f));
	CHECK(pBody2->GetForwardPAVector() == PAVECTOR(-0.5f, 0.5f, 0.7071f));
	CHECK(pBody2->GetUpPAVector() == PAVECTOR(0.-0.7071f, -0.7071f, 0.0f));
	
	pWorld->Increment(2000);

	CHECK(pBody1->GetPosition() == PAVECTOR(0.0f, 40.0f, 0.0f));
	CHECK(pBody2->GetForwardPAVector() == PAVECTOR(-1.0f, 0.0f, 0.0f));
	CHECK(pBody2->GetUpPAVector() == PAVECTOR(0.0f, 0.0f, -1.0f));
}

/*
TEST_CASE("Applying a force")
{
	Ptr<CBodyFactory> bodyFact = new CBodyFactory();

	Ptr<IBody> pBody1 = bodyFact->Create(&std::string("rigid3dscube.xml"));
	REQUIRE((bool)pBody1);

	Ptr<IForce> pForce;
}
*/