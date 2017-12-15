#include "stdafx.h"
#include <iostream> 
#include <vector>
#include <SFML\Graphics.hpp>

using namespace std;
using namespace sf;

const static float deltaTime = 0.016;

class Entity;
class Component
{
public:

	Component(Entity* pEntity, const string& tag)
		: m_compTag(tag), m_pEntity(pEntity)
	{
	}

	~Component()
	{
	}

	virtual bool init() { return true; }
	virtual void tick() {}
	virtual void fixedTick() {}
	virtual void reset() {}
	virtual void cleanup() {}

	const std::string& getTag() const { return m_compTag; };

protected:
	Entity* const getEntity() { return m_pEntity; }
private:
	std::string m_compTag;
	Entity* m_pEntity = nullptr;
};

class TransformComponent : public Component
{
public:

	TransformComponent(Entity* pEntity)
		: Component(pEntity, "transform")
	{

	}
	~TransformComponent()
	{

	}

	__forceinline void setPosition(const Vector2f& pos)
	{
		const Vector2f point = m_transform.transformPoint(Vector2f(0.0f, 0.0f));
		m_transform.translate(-point).translate(pos);
	}

	__forceinline void setPosition(float x, float y)
	{
		const Vector2f point = m_transform.transformPoint(Vector2f(0.0f, 0.0f));
		m_transform.translate(-point).translate(x, y);
	}

	__forceinline Vector2f getPosition()
	{
		return m_transform.transformPoint(Vector2f(0.0f, 0.0f));
	}

	__forceinline void translate(const Vector2f& trans)
	{
		m_transform.translate(trans);
	}

	__forceinline void rotate(float angleInDegrees)
	{
		m_transform.rotate(angleInDegrees);
	}

	const Transform& getTransform() const { return m_transform; }

private:
	sf::Transform m_transform;
};

class SpriteComponent : public Component, public Drawable
{
public:

	SpriteComponent(Entity* pEntity) : Component(pEntity, "sprite"), m_pTexture(nullptr), m_pTransform(nullptr) {}
	~SpriteComponent() = default;

	virtual bool init() override;
	void tick()
	{

	}

	virtual void draw(RenderTarget& rRenderTarget, RenderStates renderStates) const override
	{
		renderStates.transform *= *m_pTransform;
		renderStates.texture = m_pTexture;
		rRenderTarget.draw(m_vertArray, renderStates);
	}

private:

	VertexArray m_vertArray;
	const Transform* m_pTransform;
	Texture* m_pTexture;
};

class PhysComponent : public Component
{
public:

	PhysComponent(Entity* pEntity) : Component(pEntity, "physics") {}
	~PhysComponent() = default;

	virtual void tick();

	void applyForce(const Vector2f& force);

private:
	Vector2f m_velocity;
	Vector2f m_force;
	float m_mass = 10.0f;
};

class Entity
{
public:

	Entity() = default;
	~Entity()
	{
		for (auto& comp : m_components)
		{
			delete comp;
		}

		m_components.clear();
	};

	void addComponent(Component* pEntity) { m_components.push_back(pEntity); }
	void removeComponent();

	template<typename TComponent = Component>
	TComponent* getComponentByTag(const string& tag)
	{
		auto findResult = std::find_if(m_components.begin(), m_components.end(), [&tag](Component* comp)
		{
			if (dynamic_cast<TComponent*> (comp) == nullptr)
			{
				return false;
			}

			return comp->getTag() == tag;
		});
		if (findResult == m_components.end())
		{
			return nullptr;
		}
		return dynamic_cast<TComponent*> (*findResult);
	}

	bool init()
	{
		for (Component* pComponent : m_components)
		{
			pComponent->init();
		}
		return true;
	}

	void tick()
	{
		for (Component* pComponent : m_components)
		{
			pComponent->tick();
		}
	}


private:

	vector<Component*> m_components;

};

//#define CREATE_GAME_OBJECT(size, pEntity)  
//#define CREATE_DYNAMIC_GAME_OBJECT(size, mass, density, pEntity)

int main(int argc, char* argv[])
{
#define numberofentities 1000
	srand(time(NULL));
	Entity entityArray[numberofentities];

	for (int i = 0; i < numberofentities; ++i)
	{
		Entity& entity = entityArray[i];

		SpriteComponent* pSpriteComp = new SpriteComponent(&entity);

		entity.addComponent(new TransformComponent(&entity));
		entity.addComponent(pSpriteComp);
		entity.addComponent(new PhysComponent(&entity));
	}

	//auto result = entity.getComponentByTag<SpriteComponent>("sprite");

	RenderWindow renderWindow;

	renderWindow.create(VideoMode(960u, 540u), "Entity Component System");
	renderWindow.setVerticalSyncEnabled(true);
	for (Entity& e : entityArray)
	{
		e.init();
		int xPos = rand() % 900;
		int yPos = rand() % 500;
		(e.getComponentByTag<TransformComponent>("transform"))->setPosition(xPos, yPos);
	}
	while (renderWindow.isOpen())
	{
		Event evnt;
		while (renderWindow.pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
			{
				renderWindow.close();
			}
		}

		renderWindow.clear();
		for (Entity& e : entityArray)
		{
			e.tick();
			SpriteComponent* sc = e.getComponentByTag<SpriteComponent>("sprite");
			renderWindow.draw(*sc);
		}
		renderWindow.display();
	}
	system("pause");
	return 0;
}

bool SpriteComponent::init()
{
	const float size = 10.0f;
	m_vertArray.setPrimitiveType(Quads);
	m_vertArray.resize(4);
	m_vertArray[0].position = Vector2f(0.0f, 0.0f);
	m_vertArray[1].position = Vector2f(size, 0.0f);
	m_vertArray[2].position = Vector2f(size, size);
	m_vertArray[3].position = Vector2f(0.0f, size);
	TransformComponent* pTransform = getEntity()->getComponentByTag<TransformComponent>("transform");
	m_pTransform = &pTransform->getTransform();
	return true;
}

void PhysComponent::tick()
{
	TransformComponent* pTransformComponent;
	pTransformComponent = getEntity()->getComponentByTag<TransformComponent>("transform");
	//pTransformComponent->translate(Vector2f(210, 150) * deltaTime);
	//pTransformComponent->rotate(45.0f * deltaTime);
	m_velocity += (Vector2f(0.0f, 9.81f) * deltaTime);
	pTransformComponent->translate(m_velocity * deltaTime);
}
