#ifndef VECTOR3_H_
#define VECTOR3_H_
#include <string>

class Vector3{
public:
	int m_iX;
	int m_iY;
	int m_iZ;
	//std::string m_iId;
	char *m_iId;

	Vector3()
	{
		m_iX = 0;
		m_iY = 0;
		m_iZ = 0;
	}

	Vector3(/*std::string id*/char *id, int x, int y, int z)
	{
		m_iX = x;
		m_iY = y;
		m_iZ = z;
		m_iId = id;
	}

	virtual ~Vector3()
	{

	}
};

#endif
