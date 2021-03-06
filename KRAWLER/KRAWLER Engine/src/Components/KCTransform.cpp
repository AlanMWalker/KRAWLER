#include "Components\KCTransform.h"
#include "Maths\KMathVector.h"
using namespace Krawler;
using namespace Krawler::Components;

KCTransform::KCTransform(KEntity* pEntity)
	: KComponentBase(pEntity), m_pParentTransform(nullptr), m_bHasParent(false),
	m_rotation(0.0f), m_origin(0.0f, 0.0f), m_scale(1.0f, 1.0f), m_trans(0.0, 0.0f)
{

}

KCTransform::KCTransform(KCTransform & toCopy)
	: KComponentBase(toCopy.getEntity()), m_pParentTransform(toCopy.m_pParentTransform), m_bHasParent(toCopy.m_bHasParent),
	m_rotation(toCopy.m_rotation), m_origin(toCopy.m_origin), m_scale(toCopy.m_scale), m_trans(toCopy.m_trans), m_bUpdateTransform(true)

{
}

void KCTransform::operator=(KCTransform & toCopy)
{
	m_pParentTransform = toCopy.m_pParentTransform;
	m_bHasParent = toCopy.m_bHasParent;

	m_rotation = toCopy.m_rotation;  
	m_origin = toCopy.m_origin;
	m_scale = toCopy.m_scale;
	m_trans = toCopy.m_trans;
	//m_bUpdateTransform = true;
	reconstructTransform();
}

void Krawler::Components::KCTransform::tick()
{
	if (m_bUpdateTransform)
	{
		reconstructTransform();
		m_bUpdateTransform = false;
	}
	if (m_bHasParent)
	{
		//float parentRot = m_pParentTransform->getRotation();
		m_combinedWithParentTransform = m_pParentTransform->getTransform() * m_transform;
		//m_parentedTransform.translate(m_pParentTransform->getOrigin().x*m_pParentTransform->getScale().x, m_pParentTransform->getOrigin().y*m_pParentTransform->getScale().y);
	}

	m_worldRot = getRotation();
	m_worldPos = getPosition();
	m_worldScale = getScale();
}

const sf::Transform & Krawler::Components::KCTransform::getTransform() const
{
	if (m_bHasParent)
	{
		return m_combinedWithParentTransform;
	}
	return m_transform;
}

void KCTransform::setParent(KEntity* pEntity)
{
	KCTransform* pTransform = pEntity->getComponent<KCTransform>();
	KCHECK(pTransform);
	m_bHasParent = true;
	m_pParentTransform = pTransform;
}

void Krawler::Components::KCTransform::setRotation(float angleInDeg)
{
	m_rotation = angleInDeg;
	if (fabs(m_rotation) > 360.0f)
	{
		m_rotation = fmod(m_rotation, 360.0f);
	}
	//m_bUpdateTransform = true;
	reconstructTransform();
}

void Krawler::Components::KCTransform::setOrigin(const Vec2f & origin)
{
	m_origin = origin;
	//m_bUpdateTransform = true;
	reconstructTransform();
}

void Krawler::Components::KCTransform::setOrigin(float x, float y)
{
	setOrigin(Vec2f(x, y));
}

void Krawler::Components::KCTransform::setPosition(const Vec2f & trans)
{
	m_trans = trans;
	//m_bUpdateTransform = true;
	reconstructTransform();
}

void Krawler::Components::KCTransform::setPosition(float dx, float dy)
{
	setPosition(Vec2f(dx, dy));
}

const Vec2f Krawler::Components::KCTransform::getPosition() const
{
	const sf::Transform* const pTransform = !m_bHasParent ? &m_transform : &m_combinedWithParentTransform;
	return pTransform->transformPoint(getOrigin());
}

float Krawler::Components::KCTransform::getRotation() const
{
	if (!m_bHasParent)
	{
		return m_rotation;
	}

	return fmod(m_rotation + m_pParentTransform->getRotation(), 360.0f);
}

const Vec2f & Krawler::Components::KCTransform::getOrigin() const
{
	return m_origin;
}

void Krawler::Components::KCTransform::setScale(const Vec2f & scale)
{
	m_scale = scale;
	//m_bUpdateTransform = true;
	reconstructTransform();
}

void Krawler::Components::KCTransform::setScale(float x, float y)
{
	setScale(Vec2f(x, y));
}

Vec2f Krawler::Components::KCTransform::getScale() const
{
	if (m_bHasParent)
		return Vec2f(m_scale.x* m_pParentTransform->getScale().x, m_scale.y * m_pParentTransform->getScale().y);
	return m_scale;
}

void Krawler::Components::KCTransform::move(float dx, float dy)
{
	move(Vec2f(dx, dy));
}

void Krawler::Components::KCTransform::move(const Vec2f & trans)
{
	setPosition(m_trans + trans);
	//m_bUpdateTransform = true;
	reconstructTransform();
}

void Krawler::Components::KCTransform::rotate(float angleInDeg)
{
	setRotation(m_rotation + angleInDeg);
}

void Krawler::Components::KCTransform::scale(float sx, float sy)
{
	scale(Vec2f(sx, sy));
}

void Krawler::Components::KCTransform::scale(const Vec2f & scale)
{
	setScale(m_scale.x * scale.x, m_scale.y * scale.y);
}

void Krawler::Components::KCTransform::reconstructTransform()
{
	/*
	- Credit to SFML transformable class for how to create a transform component
	*/
	const float angleInRad = Maths::Radians(-m_rotation);
	const float cosAngle = cosf(angleInRad);
	const float sinAngle = sinf(angleInRad);

	const float scaleSineX = m_scale.x * sinAngle, scaleSineY = m_scale.y * sinAngle;
	const float scaleCosX = m_scale.x * cosAngle, scaleCosY = m_scale.y * cosAngle;
	const float transX = -m_origin.x * scaleCosX - m_origin.y * scaleSineY + m_trans.x;
	const float transY = m_origin.x * scaleSineX - m_origin.y * scaleCosY + m_trans.y;

	m_transform = sf::Transform(scaleCosX, scaleSineY, transX, -scaleSineX, scaleCosY, transY,
		0.0f, 0.0f, 1.0f);
}
