#pragma once

#include <osg/Vec3>
#include <osg/Array>
class Paracurve
{
public:
	static bool sampling(float value, osg::Vec3Array* points, osg::Vec3Array* norms);//抛物面的采样函数
};

