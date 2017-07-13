//
// 2009 jesssoft
//
#ifndef __SCENE_H__
#define __SCENE_H__

#include "primitive.h"

class Scene {
public:
	Scene(void);
	~Scene(void);

	void			Init(void);
	inline int		GetPrimitiveCount(void)	{ 
		return primitive_count_; 
	};
	inline Primitive *GetPrimitive(int index) { 
	    if (index >= GetPrimitiveCount())
		    return 0;
	    return primitive_[index];
	};

	void		AddBox(Vector3 &pos, Vector3 &size);
	void		AddPlane(Vector3 &p1, Vector3 &p2, Vector3 &p3,
			    Vector3 &p4);
private:
	int		primitive_count_;
	Primitive	**primitive_;
};

#endif

