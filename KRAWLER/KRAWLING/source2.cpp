#include "stdafx.h"
#include <SFML\Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

class QTree
{
public:
	QTree(int level, const FloatRect& bounds)
	{
		m_nodes = new QTree*[4]{ nullptr };
		m_bounds = bounds;
		m_level = level;
	}

	~QTree()
	{
		for (int i = 0; i < 4; ++i)
		{
			delete m_nodes[i];
		}
		delete[] m_nodes;
	}

	void clear()
	{
		m_objects.clear();
		for (int i = 0; i < 4; ++i)
		{
			m_nodes[i]->clear();
			m_nodes[i] = nullptr;
		}
	}

	void insert(const FloatRect& bounds)
	{
		if (m_nodes[0])
		{
			int index = getIndex(bounds);

			if (index != -1)
			{
				m_nodes[index]->insert(bounds);
			}
		}

		m_objects.push_back(bounds);
		if (m_objects.size() > MAX_OBJ && m_level < MAX_LEVELS)
		{
			if (m_nodes[0] == nullptr)
			{
				split();
			}
			int i = 0;
			while (i < (signed)m_objects.size())
			{
				const int idx = getIndex(m_objects[i]);
				if (idx != -1)
				{
					auto cpy = m_objects[i];
					auto it = std::find(m_objects.begin(), m_objects.end(), m_objects[i]);
					m_nodes[idx]->insert(cpy);
					m_objects.erase(it);
				}
				else
				{
					++i;
				}
			}
		}
	}

	std::vector<FloatRect> retrieve(std::vector<FloatRect>& rObj, const FloatRect& bounds)
	{
		int idx = getIndex(bounds);
		if (idx != -1 && m_nodes[0])
		{
			m_nodes[idx]->retrieve(rObj, bounds);
		}

		for (auto& obj : m_objects)
		{
			rObj.push_back(obj);
		}
		return rObj;
	}

private:

	void split()
	{
		const int subWidth = (int)m_bounds.width / 2.0f;
		const int subHeight = (int)m_bounds.height / 2.0f;
		const int x = (int)m_bounds.left;
		const int y = (int)m_bounds.top;

		m_nodes[0] = new QTree(m_level + 1, FloatRect(x + subWidth, y, subWidth, subHeight));
		m_nodes[1] = new QTree(m_level + 1, FloatRect(x, y, subWidth, subHeight));
		m_nodes[2] = new QTree(m_level + 1, FloatRect(x, y + subHeight, subWidth, subHeight));
		m_nodes[3] = new QTree(m_level + 1, FloatRect(x + subWidth, y + subHeight, subWidth, subHeight));
	}

	int getIndex(const FloatRect& bounds)
	{
		int index = -1;

		const float vMidpoint = m_bounds.left + m_bounds.width / 2.0f;
		const float hMidpoint = m_bounds.top + m_bounds.height / 2.0f;

		const bool topQuadrant = (bounds.top < hMidpoint) && (bounds.top + bounds.height < hMidpoint);
		const bool botQuadrant = (bounds.top > hMidpoint);

		if (bounds.left < vMidpoint && bounds.left + bounds.width < vMidpoint)
		{
			if (topQuadrant)
			{
				index = 1;
			}
			else if (botQuadrant)
			{
				index = 2;
			}
		}
		else if (bounds.left > vMidpoint)
		{
			if (topQuadrant)
			{
				index = 0;
			}
			else if (botQuadrant)
			{
				index = 3;
			}
		}
		return index;
	}

	int MAX_OBJ = 10;
	int MAX_LEVELS = 5;

	int m_level;
	FloatRect m_bounds;
	std::vector <FloatRect> m_objects;
	QTree** m_nodes;


};

int main(void)
{
	srand(time(NULL));
	QTree tree(0, FloatRect(0, 0, 1000, 1000));
	for (int i = 0; i < 10000; ++i)
	{
		tree.insert(FloatRect(rand() % 1000, rand() % 1000, 32, 32));
	}
	std::vector<FloatRect> tr;
	tree.retrieve(tr, FloatRect(10, 10, 32, 32));
	return 0;
}