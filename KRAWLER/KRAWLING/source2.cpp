#include "stdafx.h"
#include <iostream> 
#include <vector>
#include <SFML\Graphics.hpp>
#include <future>
#include <chrono>

using namespace std;
using namespace sf;
using namespace std::chrono;

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
		: Component(pEntity, "transform"), m_bUpdateTransform(true), m_origin(0.0f, 0.0f), m_trans(0.0f, 0.0f), m_scale(1.0f, 1.0f), m_rotation(0.0f)
	{
		reconstructTransform();
	}
	~TransformComponent()
	{

	}

	__forceinline void setPosition(const Vector2f& pos)
	{
		m_trans = pos;
		m_bUpdateTransform = true;
	}

	__forceinline void setPosition(float x, float y)
	{
		m_trans.x = x;
		m_trans.y = y;
		m_bUpdateTransform = true;
	}

	__forceinline void setRotation(float angleInDeg)
	{
		m_rotation = angleInDeg;
		if (m_rotation > 360.0f)
		{
			m_rotation = fmod(m_rotation, 360.0f);
		}
		m_bUpdateTransform = true;
	}

	__forceinline void setOrigin(const Vector2f& origin)
	{
		m_origin = origin;
		m_bUpdateTransform = true;
	}

	Vector2f getPosition() const
	{
		const float* pMatrix = !m_pParent ? m_transform.getMatrix() : m_combined.getMatrix();

		return Vector2f(pMatrix[12], pMatrix[13]);
	}

	float getRoation() const
	{
		const float rot = !m_pParent ? acos(m_transform.getMatrix()[0]) : acos(m_transform.getMatrix()[0]);
		return rot * 180 / 3.141592654f;
	}

	__forceinline void move(const Vector2f& trans)
	{
		//m_transform.translate(trans);
		setPosition(m_trans + trans);
	}

	__forceinline void rotate(float angleInDegrees)
	{
		setRotation(m_rotation + angleInDegrees);
	}

	virtual void tick() override;

	const Transform& getTransform() const;
	void setParent(Entity* pEntity) { m_pParent = pEntity; }
private:

	__forceinline void reconstructTransform();
	Entity* m_pParent = nullptr;

	sf::Transform m_transform;
	sf::Transform m_combined;

	float m_rotation;
	Vector2f m_origin;
	Vector2f m_scale;
	Vector2f m_trans;

	bool m_bUpdateTransform;
};

class SpriteComponent : public Component, public Drawable
{
public:

	SpriteComponent(Entity* pEntity) : Component(pEntity, "sprite"), m_pTexture(nullptr), m_pTransform(nullptr) {}
	SpriteComponent(Entity* pEntity, Vector2f size) : Component(pEntity, "sprite"), m_pTexture(nullptr), m_pTransform(nullptr), m_size(size) {}
	~SpriteComponent() = default;

	// set texture
	// set texture rect 

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
	Vector2f m_size;
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

	void applyForce(const Vector2f& force) { m_force += force; }
	void setMass(float mass) { m_mass = mass; }

private:

	Vector2f m_velocity;
	Vector2f m_force;
	float m_mass = 10.0f;
};

class Entity
{
public:

	Entity()
	{
		m_components.push_back(new TransformComponent(this));
	}

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

	template<typename TComponent = Component >
	TComponent* getComponentByTag(const string& tag)
	{
		auto findResult = std::find_if(m_components.begin(), m_components.end(),
			[&tag](Component* comp)
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

	const string& getTag() const { return m_tag; }
	void setTag(const string& tag) { m_tag = tag; }
private:
	string m_tag;
	vector<Component*> m_components;

};

#define CREATE_GAME_OBJECT(size, pEntity)														\
					pEntity->addComponent(new SpriteComponent(pEntity, size));					\
					pEntity->addComponent(new PhysComponent(pEntity));							\

#define NUMBER_OF_ENTITIES 5

int main(int argc, char* argv[])
{
	srand(time(NULL));
	Entity entityArray[NUMBER_OF_ENTITIES];

	for (int i = 0; i < NUMBER_OF_ENTITIES; ++i)
	{
		Entity* pEntity = &entityArray[i];

		CREATE_GAME_OBJECT(Vector2f(10, 10), pEntity);
	}
	entityArray[0].setTag("Bobby");
	entityArray[1].setTag("Mary");
	//entityArray[1].getComponentByTag<TransformComponent>("transform")->move(Vector2f(10.0f, 10.0f));
	entityArray[1].getComponentByTag<TransformComponent>("transform")->setParent(&entityArray[0]);
	RenderWindow renderWindow;

	renderWindow.create(VideoMode(960u, 540u), "Entity Component System");
	renderWindow.setVerticalSyncEnabled(true);

	auto t1 = high_resolution_clock::now();
	int idx = 0;

	for (Entity& e : entityArray)
	{
		e.init();
		PhysComponent* pPhys = e.getComponentByTag<PhysComponent>("physics");
		pPhys->applyForce(Vector2f(0.0f, -40000));
	}
	auto t2 = high_resolution_clock::now();

	auto result = duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "Execution time " << (result.count() / NUMBER_OF_ENTITIES) << " ms " << endl;


	for (Entity& e : entityArray)
	{
		if (e.getTag() == "Mary")
			continue;
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
			if (evnt.type == Event::KeyPressed)
			{
				if (evnt.key.code == Keyboard::Escape)
				{
					renderWindow.close();
				}
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
	//system("pause");
	return 0;
}

bool SpriteComponent::init()
{
	const float size = 10.0f;
	m_vertArray.setPrimitiveType(Quads);
	m_vertArray.resize(4);

	m_vertArray[0].position = Vector2f(0.0f, 0.0f);
	m_vertArray[1].position = Vector2f(m_size.x, 0.0f);
	m_vertArray[2].position = Vector2f(m_size.x, m_size.y);
	m_vertArray[3].position = Vector2f(0.0f, m_size.y);

	const Color randCol(rand() % 256, rand() % 256, rand() % 256, 255);

	m_vertArray[0].color = randCol;
	m_vertArray[1].color = randCol;
	m_vertArray[2].color = randCol;
	m_vertArray[3].color = randCol;

	TransformComponent* pTransform = getEntity()->getComponentByTag<TransformComponent>("transform");
	m_pTransform = &pTransform->getTransform();
	return true;
}

void PhysComponent::tick()
{
	TransformComponent* pTransformComponent;
	pTransformComponent = getEntity()->getComponentByTag<TransformComponent>("transform");

	if (getEntity()->getTag() != "Bobby")
	{
		if (getEntity()->getTag() == "Mary")
		{
			if (Keyboard::isKeyPressed(Keyboard::Right))
				pTransformComponent->rotate(40.0f * deltaTime);
			if (Keyboard::isKeyPressed(Keyboard::Left))
				pTransformComponent->rotate(-40.0f* deltaTime);

		}
		return;
	}

	pTransformComponent->setOrigin(Vector2f(5.0f, 5.0f));

	m_velocity += (Vector2f(0.0f, 9.81f) * deltaTime);
	m_velocity += ((1.0f / m_mass) *  m_force * deltaTime) / 2.0f;

	m_force = Vector2f(0.0f, 0.0f);

	pTransformComponent->rotate(-40.0f* deltaTime);

	pTransformComponent->move(m_velocity * deltaTime);
}

void TransformComponent::tick()
{
	if (m_bUpdateTransform)
	{
		reconstructTransform();
		m_bUpdateTransform = false;
	}
	if (m_pParent)
	{
		m_combined = m_pParent->getComponentByTag<TransformComponent>("transform")->getTransform() * m_transform;
		if (getEntity()->getTag() == "Mary")
		{
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				move(Vector2f(10.0f * 0.016f, 0.0f));
			}
		}
	}
	std::cout << getEntity()->getTag() << " = " << " ROT(" << getRoation() << ") " << getPosition().x << " : " << getPosition().y << std::endl;
}
const Transform & TransformComponent::getTransform() const
{
	if (!m_pParent)
		return m_transform;
	return m_combined;
}
void TransformComponent::reconstructTransform()
{
	const float angle = -m_rotation * 3.141592654f / 180.f;
	const float cosine = static_cast<float>(std::cos(angle));
	const float sine = static_cast<float>(std::sin(angle));
	const float sxc = m_scale.x * cosine;
	const float syc = m_scale.y * cosine;
	const float sxs = m_scale.x * sine;
	const float sys = m_scale.y * sine;
	const float tx = -m_origin.x * sxc - m_origin.y * sys + m_trans.x;
	const float ty = m_origin.x * sxs - m_origin.y * syc + m_trans.y;

	m_transform = Transform(sxc, sys, tx,
		-sxs, syc, ty,
		0.f, 0.f, 1.f);
}
