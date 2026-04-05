
#include "Game.h"
#include "Engine.h"
#include "MathEngine.h"

#include "Manager_Uber.h"
#include "ShaderObject_Uber.h"
#include "GraphicsObject_Uber.h"
#include "Mesh_Uber.h"
#include "Animation_Uber.h"
#include "GameObject_Uber.h"

#include "GameObjectFactory.h"
#include "CollisionShape_Uber.h"
#include "Octree.h"

#include "Random.h"
#include "Colors.h"

using namespace Azul;

#define TEST_OCTREE_ENTITY  1750

#define TEST_COLLISION		0
#define TEST_OCTREE_BSPHERE 1
#define TEST_OCTREE_AABB    1
#define TEST_OCTREE_OBB     1
#define TEST_OCTREE_RAYCAST	1

Game::Game(const char * const _pName, int _width, int _height) : Engine(_pName, _width, _height)
{
	TimeManager::Create();
}
Game::~Game() {}

bool Game::LoadContent()
{
	#pragma region > Managers (Singletons)
	{
		ShaderObjectNodeMan ::Create();
		MeshNodeMan         ::Create();
		TextureObjectNodeMan::Create();
		ImageMan			::Create();
		FontNodeMan			::Create();
		GameObjectMan       ::Create();
		SkeletonMan			::Create();
		SequenceNodeMan     ::Create();
		CameraNodeMan       ::Create();
		InputMan            ::Create();
		FogMan				::Create(Colors::Black, 30.0f, 60.0f);
	}
	#pragma endregion

	#pragma region > Meshes
	{
		MeshNodeMan::Add(Mesh::Name::CUBE,	 new MeshProto("PrimitiveCube.m.proto.azul"));
		MeshNodeMan::Add(Mesh::Name::AABB,	 new MeshLine ());
		MeshNodeMan::Add(Mesh::Name::SPHERE, new MeshProto("PrimitiveSphere.m.proto.azul"));
		MeshNodeMan::Add(Mesh::Name::SPRITE, new MeshProto("PrimitiveSprite.m.proto.azul"));
		MeshNodeMan::Add(Mesh::Name::VECTOR, new MeshProto("PrimitiveVector.m.proto.azul"));
	}
	#pragma endregion

	#pragma region > Shaders
	{
		ShaderObjectNodeMan::Add(new ShaderObject_ConstColor(ShaderObject::Name::ConstColor));
		ShaderObjectNodeMan::Add(new ShaderObject_FogWireframe(ShaderObject::Name::FogWireframe));
		
		ShaderObjectNodeMan::Add(new ShaderObject_ColorByVertex(ShaderObject::Name::ColorByVertex));
		ShaderObjectNodeMan::Add(new ShaderObject_FlatTexture(ShaderObject::Name::FlatTexture));
		ShaderObjectNodeMan::Add(new ShaderObject_LightTexture(ShaderObject::Name::LightTexture));

		ShaderObjectNodeMan::Add(new ShaderObject_FogLightTexture(ShaderObject::Name::FogLightTexture));
		ShaderObjectNodeMan::Add(new ShaderObject_FogDeformLightTexture(ShaderObject::Name::FogDeformLightTexture));

		ShaderObjectNodeMan::Add(new ShaderObject_Sprite(ShaderObject::Name::Sprite));

		ShaderObjectNodeMan::Add(new ShaderObject_SkinFlatTexture(ShaderObject::Name::SkinFlatTexture));
		ShaderObjectNodeMan::Add(new ShaderObject_SkinLightTexture(ShaderObject::Name::SkinLightTexture));

		ShaderObjectNodeMan::Add(new ShaderObject_BasicCompute(ShaderObject::Name::BasicCompute));
		ShaderObjectNodeMan::Add(new ShaderObject_Mixer(ShaderObject::Name::MixerCompute));
		ShaderObjectNodeMan::Add(new ShaderObject_World(ShaderObject::Name::WorldCompute));
	}
	#pragma endregion

	#pragma region > Textures
	{
		TextureObjectNodeMan::Add(TextureObject::Name::NULL_TEXTURE, new TextureObject());

		TextureObjectNodeMan::Add(TextureObject::Name::WHITE,		 new TextureObject("white.t.proto.azul"));

		TextureObjectNodeMan::Add(TextureObject::Name::FONT_143PT,	 new TextureObject("font.t.proto.azul", TextureObject::FilterMode::MinMagMipPoint));
	}
	#pragma endregion

	#pragma region > Images
	{
		// Glyph needs some data, doesn't really matter what
		ImageMan::Add(Image::Name::GLYPH, TextureObject::Name::FONT_143PT, Rect(1.0f, 0.0f, 94.0f, 143.0f));
	}
	#pragma endregion

	#pragma region > Font
	{
		FontNodeMan::Add(Font::Name::FONT_143PT, new Font("font.xml.proto.azul", TextureObject::Name::FONT_143PT));
	}
	#pragma endregion
	
	#pragma region > Camera
	{
		#pragma region > Camera > Forward
		{
			CameraPerspective * pCamera = new CameraPerspective();
			pCamera->setViewport(0, 0, this->windowWidth, this->windowHeight);
			pCamera->setPerspective(1.0f, 100000.0f, 55.0f, float(pCamera->getScreenWidth()) / float(pCamera->getScreenHeight()));
			pCamera->setOrientAndPosition(Vec3(0.0f, 1.0,   0.0f),
										  Vec3(0.0f, 0.0,   0.0f),
										  Vec3(0.0f, 0.0, -40.0f));
			
			CameraNodeMan::Add(Camera::Name::FORWARD, pCamera);
			CameraNodeMan::SetActiveCamera(Camera::Name::FORWARD, Camera::Type::PERSPECTIVE);
		}
		#pragma endregion

		#pragma region > Camera > Orthographic
		{
			CameraOrthographic * pCamera = new CameraOrthographic();;
			pCamera->setViewport(0, 0, this->windowWidth, this->windowHeight);
			pCamera->setOrthographic((float)-pCamera->getScreenWidth()  * 0.5f, 
									 (float) pCamera->getScreenWidth()  * 0.5f, 
									 (float)-pCamera->getScreenHeight() * 0.5f,
									 (float) pCamera->getScreenHeight() * 0.5f,
									 1.0f,
									 1000.0f);
			pCamera->setOrientAndPosition(Vec3(0.0f, 1.0f,  0.0f),
										  Vec3(0.0f, 0.0f, -1.0f),
										  Vec3(0.0f, 0.0f,  2.0f));

			CameraNodeMan::Add(Camera::Name::CANVAS, pCamera);
			CameraNodeMan::SetActiveCamera(Camera::Name::CANVAS, Camera::Type::ORTHOGRAPHIC);
		}
		#pragma endregion

		CameraNodeMan::Update();
	}
	#pragma endregion

	#pragma region > Game Objects
	{
		// Create World Gizmo
		GameObjectMan::Add(GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::VECTOR, Colors::Red,	 new Ray(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f))));
		GameObjectMan::Add(GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::VECTOR, Colors::Green, new Ray(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f))));
		GameObjectMan::Add(GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::VECTOR, Colors::Blue,	 new Ray(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f))));
	}
	#pragma endregion
	
	#pragma region > Game Objects - Collision Test
	{
		#if (TEST_COLLISION)
		{
			GameObjectCollidable * ptoAABB_WH = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::White, new AABB(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f)));
			GameObjectMan::Add(ptoAABB_WH);

			GameObjectCollidable * ptoAABB_CY = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Cyan, new AABB(Vec3(-2.5f, -2.5f, -1.5f), Vec3(-2.0f, -2.0f, -0.5f)));
			GameObjectMan::Add(ptoAABB_CY);

			GameObjectCollidable * ptoAABB_CO = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Coral, new AABB(Vec3(1.5f, 1.5f, -2.5f), Vec3(2.5f, 2.5f, -1.5f)));
			GameObjectMan::Add(ptoAABB_CO);

			GameObjectCollidable * ptoBSphere = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::SPHERE, Colors::Teal, new BSphere(Vec3(1.5f, 1.5f, 0.5f), 1.0f));
			GameObjectMan::Add(ptoBSphere);

			GameObjectCollidable * ptoOBB_RED = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Red, new OBB(Rot(), Vec3(0.0f, 1.01f, 0.0f), Vec3(2.0f, 0.5f, 0.5f)));
			GameObjectMan::Add(ptoOBB_RED);

			GameObjectCollidable * ptoOBB_BLU = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Blue, new OBB(Rot(Rot1::X, 45.0f * Trig::DegToRad), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.5f, 0.5f, 0.5f)));
			GameObjectMan::Add(ptoOBB_BLU);

			GameObjectCollidable * ptoOBB_GRE = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Green, new OBB(Rot(Rot1::X, 45.0f * Trig::DegToRad), Vec3(0.0f, -1.0f, 1.0f), Vec3(0.5f, 0.5f, 0.5f)));
			GameObjectMan::Add(ptoOBB_GRE);

			GameObjectCollidable * ptoOBB_MAG = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Magenta, new OBB(Rot(Rot3::XYZ, 45.0f * Trig::DegToRad, 30.0f * Trig::DegToRad, 15.0f * Trig::DegToRad), Vec3(0.0f, 1.0f, 1.0f), Vec3(0.75f, 0.75f, 0.75f)));
			GameObjectMan::Add(ptoOBB_MAG);

			GameObjectCollidable * ptoPlane = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::White, new Plane(Vec3(0.0f, -0.4f, 0.0f), Vec3(0.0f, 1.0f, -0.8f)));
			GameObjectMan::Add(ptoPlane);

			GameObjectCollidable * ptoOBB_HON = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Honeydew, new OBB(Rot(Rot1::X, 45.0f * Trig::DegToRad), Vec3(8.0f, 4.0f, 0.0f), Vec3(1.25f, 1.25f, 1.25f)));
			GameObjectMan::Add(ptoOBB_HON);

			GameObjectCollidable * ptoRay_LIM = GameObjectFactory::Create(ShaderObject::Name::ConstColor, Mesh::Name::VECTOR, Colors::LimeGreen, new Ray(Vec3(0.0f, 0.0f, -10.0f), Vec3(0.0f, 0.0f, 1.0f)));
			GameObjectMan::Add(ptoRay_LIM);

			GameObjectCollidable * ptoRay_RED = GameObjectFactory::Create(ShaderObject::Name::ConstColor, Mesh::Name::VECTOR, Colors::Red, new Ray(Vec3(6.0f, 3.0f, -10.0f), Vec3(0.2f, 0.15f, 1.0f)));
			GameObjectMan::Add(ptoRay_RED);

			// Print intersection results in comparison to expectations
			Trace::out("OBB_RED --> OBB_BLU : %d == 1\n", ptoOBB_RED->Intersect(ptoOBB_BLU));
			Trace::out("OBB_BLU --> OBB_RED : %d == 1\n", ptoOBB_BLU->Intersect(ptoOBB_RED));
			Trace::out("OBB_RED --> OBB_GRE : %d == 0\n", ptoOBB_RED->Intersect(ptoOBB_GRE));
			Trace::out("OBB_BLU --> OBB_GRE : %d == 0\n", ptoOBB_BLU->Intersect(ptoOBB_GRE));
			Trace::out("OBB_RED --> OBB_MAG : %d == 1\n", ptoOBB_RED->Intersect(ptoOBB_MAG));
			Trace::out("OBB_BLU --> OBB_MAG : %d == 1\n", ptoOBB_BLU->Intersect(ptoOBB_MAG));
			Trace::out("OBB_GRE --> OBB_MAG : %d == 0\n", ptoOBB_GRE->Intersect(ptoOBB_MAG));
			
			Trace::out("   AABB --> OBB_BLU : %d == 1\n", ptoAABB_WH->Intersect(ptoOBB_BLU));
			Trace::out("   AABB --> OBB_RED : %d == 0\n", ptoAABB_WH->Intersect(ptoOBB_RED));
			Trace::out("   AABB --> OBB_GRE : %d == 0\n", ptoAABB_WH->Intersect(ptoOBB_GRE));
			Trace::out("   AABB --> OBB_MAG : %d == 1\n", ptoAABB_WH->Intersect(ptoOBB_MAG));
			
			Trace::out("BSphere --> OBB_BLU : %d == 0\n", ptoBSphere->Intersect(ptoOBB_BLU));
			Trace::out("BSphere --> OBB_RED : %d == 1\n", ptoBSphere->Intersect(ptoOBB_RED));
			Trace::out("BSphere --> OBB_GRE : %d == 0\n", ptoBSphere->Intersect(ptoOBB_GRE));
			Trace::out("BSphere --> OBB_MAG : %d == 1\n", ptoBSphere->Intersect(ptoOBB_MAG));

			Trace::out("  Plane --> AABB_WH : %d == 1\n", ptoPlane->Intersect(ptoAABB_WH));
			Trace::out("  Plane --> AABB_CY : %d == 1\n", ptoPlane->Intersect(ptoAABB_CY));
			Trace::out("  Plane --> AABB_CO : %d == 0\n", ptoPlane->Intersect(ptoAABB_CO));
			Trace::out("  Plane --> OBB_BLU : %d == 0\n", ptoPlane->Intersect(ptoOBB_BLU));
			Trace::out("  Plane --> OBB_RED : %d == 0\n", ptoPlane->Intersect(ptoOBB_RED));
			Trace::out("  Plane --> OBB_GRE : %d == 1\n", ptoPlane->Intersect(ptoOBB_GRE));
			Trace::out("  Plane --> OBB_MAG : %d == 1\n", ptoPlane->Intersect(ptoOBB_MAG));
			Trace::out("  Plane --> OBB_HON : %d == 0\n", ptoPlane->Intersect(ptoOBB_HON));

			Trace::out("RAY_LIM --> OBB_HON : %d == 0\n", ptoRay_LIM->Intersect(ptoOBB_HON));
			Trace::out("RAY_LIM --> OBB_GRE : %d == 0\n", ptoRay_LIM->Intersect(ptoOBB_GRE));
			Trace::out("RAY_LIM --> OBB_MAG : %d == 1\n", ptoRay_LIM->Intersect(ptoOBB_MAG));
			Trace::out("RAY_RED --> OBB_HON : %d == 1\n", ptoRay_RED->Intersect(ptoOBB_HON));
			Trace::out("RAY_RED --> OBB_GRE : %d == 0\n", ptoRay_RED->Intersect(ptoOBB_GRE));
			Trace::out("RAY_RED --> OBB_MAG : %d == 0\n", ptoRay_RED->Intersect(ptoOBB_MAG));
		}
		#endif
	}
	#pragma endregion

	#pragma region > Game Objects - Octree Demo
	{
		// Create the Octree (bounds, max data before splitting, max depth)
		const AABB octantBounds(Vec3(-10.0f, -10.0f, -10.0f), Vec3(10.0f, 10.0f, 10.0f));
		Octree octA(octantBounds, 10u, 4u);

		const unsigned int entityCount  = TEST_OCTREE_ENTITY;
		unsigned int currentEntityCount = 0u;

		#if (TEST_OCTREE_BSPHERE)
		{
			// Make some entities and add them to the octree
			const float radius = 0.4f;
			for (unsigned int i = 0u; i < entityCount; i++)
			{
				float x = RandomRange(10.0f, -10.0f);
				float y = RandomRange(10.0f, -10.0f);
				float z = RandomRange(10.0f, -10.0f);

				// Bias it a little to show differently sized octants
				x = std::min(x + RandomRange(0.0f, 5.0f), 10.0f);
				y = std::min(y + RandomRange(0.0f, 5.0f), 10.0f);
				z = std::min(z + RandomRange(0.0f, 5.0f), 10.0f);

				GameObjectCollidable * ptoBSphere = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::SPHERE, Colors::Gray, new BSphere(Vec3(x, y, z), radius));
				GameObjectMan::Add(ptoBSphere);
				octA.Add(ptoBSphere);
			}

			currentEntityCount += entityCount;
		}
		#endif
		#if (TEST_OCTREE_AABB)
		{
			//We can use whatever CollisionShape, so we can add AABBs to the mix if we want (thank you polymorphism)
			for (unsigned int i = 0u; i < entityCount; i++)
			{
				float x = RandomRange(10.0f, -10.0f);
				float y = RandomRange(10.0f, -10.0f);
				float z = RandomRange(10.0f, -10.0f);
			
				// Bias it a little to show differently sized octants
				x = std::min(x + RandomRange(0.0f, 5.0f), 10.0f);
				y = std::min(y + RandomRange(0.0f, 5.0f), 10.0f);
				z = std::min(z + RandomRange(0.0f, 5.0f), 10.0f);
			
				float bx = RandomRange(0.1f, 0.3f);
				float by = RandomRange(0.1f, 0.3f);
				float bz = RandomRange(0.1f, 0.3f);
			
				GameObjectCollidable * ptoBSphere = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Gray, new AABB(Vec3(x - bx, y - by, z - bz), Vec3(x + bx, y + by, z + bz)));
				GameObjectMan::Add(ptoBSphere);
				octA.Add(ptoBSphere);
			}

			currentEntityCount += entityCount;
		}
		#endif
		#if (TEST_OCTREE_OBB)
		{
			//We can use whatever CollisionShape, so we can add AABBs to the mix if we want (thank you polymorphism)
			for (unsigned int i = 0u; i < entityCount; i++)
			{
				float x = RandomRange(10.0f, -10.0f);
				float y = RandomRange(10.0f, -10.0f);
				float z = RandomRange(10.0f, -10.0f);
			
				// Bias it a little to show differently sized octants
				x = std::min(x + RandomRange(0.0f, 5.0f), 10.0f);
				y = std::min(y + RandomRange(0.0f, 5.0f), 10.0f);
				z = std::min(z + RandomRange(0.0f, 5.0f), 10.0f);
			
				float rx = RandomRange(0.0f, 360.0f) * Trig::DegToRad;
				float ry = RandomRange(0.0f, 360.0f) * Trig::DegToRad;
				float rz = RandomRange(0.0f, 360.0f) * Trig::DegToRad;

				float sx = RandomRange(0.2f, 0.4f);
				float sy = RandomRange(0.2f, 0.4f);
				float sz = RandomRange(0.2f, 0.4f);
			
				GameObjectCollidable * ptoBSphere = GameObjectFactory::Create(ShaderObject::Name::FogLightTexture, Mesh::Name::CUBE, Colors::Gray, new OBB(Rot(Rot3::XYZ, rx, ry, rz), Vec3(x, y, z), Vec3(sx, sy, sz)));
				GameObjectMan::Add(ptoBSphere);
				octA.Add(ptoBSphere);
			}

			currentEntityCount += entityCount;
		}
		#endif
		#if (TEST_OCTREE_RAYCAST)
		{
			{
				Ray raycast = Ray(Vec3(0.0f, 0.0f, -12.0f), Vec3(1.0f, 1.0f, 5.0f));

				Trace::out("Hit Result (Green):\n");
				GameObjectCollidable * ptoRay_A = GameObjectFactory::Create(ShaderObject::Name::FogWireframe, Mesh::Name::VECTOR, Colors::LimeGreen, new Ray(raycast));
				GameObjectMan::Add(ptoRay_A);

				HitResult result = octA.Raycast(raycast);
				if (result)
				{
					// Debug information for raycast hit result
					Trace::out("\tLocation: (%4.8f, %4.8f, %4.8f)\n\tLength: %4.8f\n", result.GetHitPos().x(), result.GetHitPos().y(), result.GetHitPos().z(), result.GetLength());
					GameObjectCollidable * ptoResult = GameObjectFactory::Create(ShaderObject::Name::ConstColor, Mesh::Name::SPHERE, Colors::LimeGreen, new BSphere(result.GetHitPos(), 0.1f));
					GameObjectMan::Add(ptoResult);
				}

				// Sanity check to prove that fewer intersection checks than number of entities in octree are being done (AKA the entire point of this data structure)
				Trace::out("\tIntersection Checks: %u/%u\n\n", octA.GetIterationCount(), currentEntityCount);
			}
			{
				Ray raycast = Ray(Vec3(-6.0f, -10.0f, -12.0f), Vec3(-0.2f, 0.15f, 1.0f));

				Trace::out("Hit Result (Cyan):\n");
				GameObjectCollidable * ptoRay_A = GameObjectFactory::Create(ShaderObject::Name::FogWireframe, Mesh::Name::VECTOR, Colors::Cyan, new Ray(raycast));
				GameObjectMan::Add(ptoRay_A);

				HitResult result = octA.Raycast(raycast);
				if (result)
				{
					// Debug information for raycast hit result
					Trace::out("\tLocation: (%4.8f, %4.8f, %4.8f)\n\tLength: %4.8f\n", result.GetHitPos().x(), result.GetHitPos().y(), result.GetHitPos().z(), result.GetLength());
					GameObjectCollidable * ptoResult = GameObjectFactory::Create(ShaderObject::Name::ConstColor, Mesh::Name::SPHERE, Colors::Cyan, new BSphere(result.GetHitPos(), 0.1f));
					GameObjectMan::Add(ptoResult);
				}

				// Sanity check to prove that fewer intersection checks than number of entities in octree are being done (AKA the entire point of this data structure)
				Trace::out("\tIntersection Checks: %u/%u\n\n", octA.GetIterationCount(), currentEntityCount);
			}
		}
		#endif
	}
	#pragma endregion

	return true;
}

void Game::Update(float _deltaTime)
{
	TimeManager::Toc();

	InputMan::Update();

	GameObjectMan::Update(TimeManager::GetGlobalTime());
	CameraNodeMan::Update();
}

void Game::Render()
{
	this->SetDefaultTargetMode();

	GameObjectMan::Draw();
}

void Game::UnloadContent()
{
	#pragma region > Managers (Singletons)

	InputMan            ::Destroy();
	CameraNodeMan       ::Destroy();
	SequenceNodeMan     ::Destroy();
	SkeletonMan			::Destroy();
	GameObjectMan       ::Destroy();
	FontNodeMan			::Destroy();
	ImageMan			::Destroy();
	TextureObjectNodeMan::Destroy();
	MeshNodeMan         ::Destroy();
	ShaderObjectNodeMan ::Destroy();
	DirectXDeviceMan    ::Destroy();
	TimeManager			::Destroy();
	FogMan				::Destroy();

	#pragma endregion
}

void Game::ClearDepthStencilBuffer()
{	
	constexpr float clearDepth = 1.0f;
	constexpr uint8_t clearStencil = 0;

	this->mStateRenderTargetView.Clear(FogMan::GetColorV4());
	this->mDepthStencilView.Clear(clearDepth, clearStencil);
}
