//
// 2009 jesssoft
//
#include "scene.h"

Scene::Scene(void)
{
	primitive_count_	= 0;
	primitive_		= nullptr;
}

Scene::~Scene(void)
{
	if (primitive_) {
		for (int i = 0; i < primitive_count_; i++)
			delete primitive_[i];

		delete[] primitive_;
		primitive_ = nullptr;
		primitive_count_ = 0;
	}
}

void
Scene::AddBox(Vector3 &pos, Vector3 &size)
{
	Vertex *v[8];
	// Here : Not completed!
	v[0] = new Vertex(Vector3(pos.x, pos.y, pos.z), 0, 0);
	v[1] = new Vertex(Vector3(pos.x + size.x, pos.y, pos.z), 0, 0 );
	v[2] = new Vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z), 0, 0);
	v[3] = new Vertex(Vector3(pos.x, pos.y + size.y, pos.z), 0, 0 );
	v[4] = new Vertex(Vector3(pos.x, pos.y, pos.z + size.z), 0, 0 );
	v[5] = new Vertex(Vector3(pos.x + size.x, pos.y, pos.z + size.z), 0, 0);
	v[6] = new Vertex(Vector3(pos.x + size.x, pos.y + size.y,
	    pos.z + size.z) , 0, 0);
	v[7] = new Vertex(Vector3(pos.x, pos.y + size.y, pos.z + size.z), 0, 0);
	// front plane
	primitive_[primitive_count_++] = new Triangle(v[0], v[1], v[3]);
	primitive_[primitive_count_++] = new Triangle(v[1], v[2], v[3]);
	// back plane
	primitive_[primitive_count_++] = new Triangle(v[5], v[4], v[7]);
	primitive_[primitive_count_++] = new Triangle(v[5], v[7], v[6]);
	// left plane
	primitive_[primitive_count_++] = new Triangle(v[4], v[0], v[3]);
	primitive_[primitive_count_++] = new Triangle(v[4], v[3], v[7]);
	// right plane
	primitive_[primitive_count_++] = new Triangle(v[1], v[5], v[2]);
	primitive_[primitive_count_++] = new Triangle(v[5], v[6], v[2]);
	// top plane
	primitive_[primitive_count_++] = new Triangle(v[4], v[5], v[1]);
	primitive_[primitive_count_++] = new Triangle(v[4], v[1], v[0]);
	// bottom plane
	primitive_[primitive_count_++] = new Triangle(v[6], v[7], v[2]);
	primitive_[primitive_count_++] = new Triangle(v[7], v[3], v[2]);
}

void
Scene::AddPlane(Vector3 &p1, Vector3 &p2, Vector3 &p3, Vector3 &p4)
{
	Vertex *v[4];
	// Here : Not completed!
	v[0] = new Vertex(p1, 0, 0);
	v[1] = new Vertex(p2, 0, 0);
	v[2] = new Vertex(p3, 0, 0);
	v[3] = new Vertex(p4, 0, 0);

	primitive_[primitive_count_++] = new Triangle(v[0], v[1], v[3]);
	primitive_[primitive_count_++] = new Triangle(v[1], v[2], v[3]);
}

void
Scene::Init(void)
{
	primitive_ = new Primitive*[25000];

	//
	// These primitives should be loaded from a file later
	//
	
	//
	// ground plane
	//
	primitive_[primitive_count_] = new PlanePrim(Vector3(0, 1, 0), 3.0f);
	primitive_[primitive_count_]->SetName("plane");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(.8f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(.8f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(1.f, 1.f, 1.f));
	// primitive_[primitive_count_]->GetMaterial()->SetDiffuseRefl(
	// 0.05f);
	// primitive_[primitive_count_]->GetMaterial()->SetTexture(
	// new Texture("textures/wood.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	    new Texture("textures/Fieldstone.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetNTexture(
	    new Texture("textures/FieldstoneBumpDOT3.tga", true));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.1f, 0.1f);
	primitive_count_++;

	//
	// A ceiling
	//
	primitive_[primitive_count_] = new PlanePrim(Vector3(0, -1, 0 ),
	    3.0f/*5.2f*/);
	primitive_[primitive_count_]->SetName("ceiling");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(.8f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(.8f);
//	primitive_[primitive_count_]->GetMaterial()->SetColor( 
	// color_t( 0.8f, 0.8f, 0.8f ) );
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t( 1.f, 1.f, 1.f  ) );
//	primitive_[primitive_count_]->GetMaterial()->SetTexture( 
	// new Texture( "textures/Tile4.tga" ) );
	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	    new Texture("textures/Tile4.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetNTexture(
	    new Texture("textures/Tile4BumpDOT3.tga", true));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.1f, 0.1f);
	primitive_count_++;

	//
	// A Back plane
	//
	primitive_[primitive_count_] = new PlanePrim(Vector3(0, 0, -1), 20.0f);
	primitive_[primitive_count_]->SetName("backplane");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(.8f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(.8f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(.9f, .9f, 0.9f));
//	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	// new Texture( "textures/marble.tga" ) );
	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	    new Texture("textures/White.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetNTexture(
	    new Texture("textures/WhiteBumpDOT3.tga", true));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.1f, 0.1f);
	primitive_count_++;

//	AddPlane(Vector3(-4, 2, 19), Vector3(0, 2, 19), Vector3(0, 0, 19)
	// , Vector3(-4, 0, 19));

	//
	// A left plane
	//
	primitive_[primitive_count_] = new PlanePrim( Vector3(1, 0, 0), 10);
	primitive_[primitive_count_]->SetName("backplane");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(.8f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(.8f);
//	primitive_[primitive_count_]->GetMaterial()->SetColor(
	// color_t( 0.5f, 0.5f, 0.6f ) );
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.9f, 0.9f, 0.9f));
//	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	// new Texture( "textures/marble.tga" ) );
	primitive_[primitive_count_]->GetMaterial()->SetTexture( 
	    new Texture("textures/White.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetNTexture( 
	    new Texture("textures/WhiteBumpDOT3.tga", true));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.1f, 0.1f);
	primitive_count_++;

	//
	// A right plane
	//
	primitive_[primitive_count_] = new PlanePrim(Vector3(-1, 0, 0), 10);
	primitive_[primitive_count_]->SetName("backplane");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.8f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(0.8f);
//	primitive_[primitive_count_]->GetMaterial()->SetColor( 
	// color_t( 0.5f, 0.5f, 0.6f ) );
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.9f, 0.9f, 0.9f));
//	primitive_[primitive_count_]->GetMaterial()->SetTexture( 
	// new Texture( "textures/marble.tga" ) );
	primitive_[primitive_count_]->GetMaterial()->SetTexture( 
	    new Texture("textures/White.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetNTexture( 
	    new Texture("textures/WhiteBumpDOT3.tga", true));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.1f, 0.1f);
	primitive_count_++;

	//
	// big sphere
	//
	/*
	primitive_[primitive_count_] = new Sphere(Vector3(0, -0.5f, 4), 2);
	primitive_[primitive_count_]->SetName("big sphere");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.2f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.7f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.1f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(0.2f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
					color_t( 0.8f, 0.8f, 0.8f ) );
	// primitive_[primitive_count_]->GetMaterial()->SetDiffuseRefl(
					0.3f );
//	primitive_[primitive_count_]->GetMaterial()->SetTexture(
				new Texture( "textures/marble.tga" ) );
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(
					0.8f, 0.8f );
	primitive_[primitive_count_]->GetMaterial()->SetRefrIndex( 1.3f );
	primitive_count_++;
*/
	//
	// A big sphere
	//
	primitive_[primitive_count_] = new Sphere(Vector3(0, 0.0f, 8), 3);
	primitive_[primitive_count_]->SetName("big sphere");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.06f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.9f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.1f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(0.3f);
	primitive_[primitive_count_]->GetMaterial()->SetRefrIndex(1.3f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.8f, 0.8f, .8f));
//	primitive_[primitive_count_]->GetMaterial()->SetDiffuseRefl(0.3f);
	primitive_count_++;

	//
	// Here: A testing triangle 
	//
	/*
	static Vertex vertices[3] = {
		Vertex(Vector3(0, 0, 0), 0, 0),
		Vertex(Vector3(3, 4, 0), 0, 0),
		Vertex(Vector3(6, 0, 0), 0, 0),
	};

	primitive_[primitive_count_] = new Triangle(&vertices[0], &vertices[1],
	    &vertices[2]);
	primitive_[primitive_count_]->SetName("triangle");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.1f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(0.8f);
	primitive_[primitive_count_]->GetMaterial()->SetRefrIndex(1.3f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(.8f, .8f, .8f));
	primitive_count_++;
	*/

	//
	// A big sphere
	//
	primitive_[primitive_count_] = new Sphere(Vector3(3, -2.0f, 5), 1);
	primitive_[primitive_count_]->SetName("big sphere");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.2f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.7f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.1f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(0.2f);
	primitive_[primitive_count_]->GetMaterial()->SetRefrIndex(1.3f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.8f, 0.8f, 0.8f));
	// primitive_[primitive_count_]->GetMaterial()->SetDiffuseRefl(
	// 			0.3f );
//	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	// new Texture( "textures/marble.tga" ) );
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.8f, 0.8f);
	primitive_count_++;

	//
	// A big sphere
	//
	primitive_[primitive_count_] = new Sphere(Vector3(-2, -2.0f, 4), 1);
	primitive_[primitive_count_]->SetName("big sphere");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.8f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(0.1f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.8f, 0.8f, 0.8f));
	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	    new Texture("textures/marble.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.8f, 0.8f);
	primitive_count_++;

	//
	// A big sphere
	//
	primitive_[primitive_count_] = new Sphere(Vector3(1, -2.5f, 3), 0.5);
	primitive_[primitive_count_]->SetName("big sphere");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.6f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.5f);
	primitive_[primitive_count_]->GetMaterial()->SetSpecular(0.7f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(1.f, 0.2f, 0.2f));
//	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	// new Texture( "textures/marble.tga" ) );
//	primitive_[primitive_count_]->GetMaterial()->SetUVScale(
	// 0.8f, 0.8f );
	primitive_count_++;

	//
	// A small sphere
	//
	primitive_[primitive_count_] = new Sphere(Vector3(-5, -0.8f, 6), 2);
	primitive_[primitive_count_]->SetName("small sphere");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.2f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.7f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.8f, 0.8f, 1.0f));
	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	    new Texture("textures/marble.tga"));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.8f, 0.8f);
	// primitive_[primitive_count_]->GetMaterial()->SetDiffuseRefl(
	// 0.6f );
	primitive_count_++;

	//
	// A Third sphere
	//
	primitive_[primitive_count_] = new Sphere(Vector3(5, -0.8f, 7), 2);
	primitive_[primitive_count_]->SetName("small sphere");
	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.6f);
	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
	primitive_[primitive_count_]->GetMaterial()->SetRefrIndex(1.3f);
	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(0.4f);
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.8f, 0.8f, .8f));
	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.8f, 0.8f);
	primitive_[primitive_count_]->GetMaterial()->SetTexture(
	    new Texture("textures/marble.tga"));
	primitive_count_++;
	
	//
	// A statue
	//
//	primitive_[primitive_count_] = new Box(Aabb(Vector3(-1.f, -2, 1.f),
//	    Vector3( 2, 2, 2 ) ) );
//	primitive_[primitive_count_]->GetMaterial()->SetReflection(0.0f);
//	primitive_[primitive_count_]->GetMaterial()->SetRefraction(0.0f);
//	primitive_[primitive_count_]->GetMaterial()->SetDiffuse(1.0f);
//	primitive_[primitive_count_]->GetMaterial()->SetColor(
//	    color_t(0.7f, 0.7f, 0.7f));
//	primitive_[primitive_count_]->GetMaterial()->SetTexture(
//	    new Texture("textures/marble.tga"));
//	primitive_[primitive_count_]->GetMaterial()->SetUVScale(0.4f, 0.4f);
//	primitive_count_++;

	//
	// An area light
	//
	/*
	primitive_[primitive_count_] = new Box(Aabb(Vector3(-1, 5, 4),
	    Vector3(2, 0.1f, 2)));
	primitive_[primitive_count_]->SetLightFlag();
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(1.f, 1.f, 1.f));
	primitive_count_++;
	*/

	/*
	primitive_[primitive_count_] = new Box(Aabb(Vector3(-1, 5, -1),
	    Vector3(2, 0.1f, 2)));
	primitive_[primitive_count_]->SetLightFlag();
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(1.f, 1.f, 1.f));
	primitive_count_++;
	*/

	//
	// Test
	//
	primitive_[primitive_count_] = new Sphere(Vector3(0, 0, 1), 0.1f);
	primitive_[primitive_count_]->SetLightFlag();
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(1.0f, 1.0f, 1.0f));
	primitive_count_++;
	/*
	//
	// A light source 1
	//
	primitive_[primitive_count_] = new Sphere(Vector3(4, 3, -4), 0.1f);
	primitive_[primitive_count_]->SetLightFlag();
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(.7f, .7f, .7f));
	primitive_count_++;

	//
	// Another light source 2
	//
	primitive_[primitive_count_] = new Sphere(Vector3(2, 5, -2), 0.1f);
	primitive_[primitive_count_]->SetLightFlag();
	primitive_[primitive_count_]->GetMaterial()->SetColor(
	    color_t(0.7f, 0.7f, 0.7f));
	primitive_count_++;
	*/
}

