#include "Paracurve.h"

bool Paracurve::sampling(float value, osg::Vec3Array* points, osg::Vec3Array* norms)
{
	//初始化采样系数
	int samplingNumber = 200;//将圆周分为200个扇形
	double radioStep = 0.05;//半径采样步长
	double radio = radioStep;//半径
	double radioMax = 5.0;//最大采样半径
	double zMax = 5.0;//最大采样z值
	double x, y, z;

	//预处理
	std::vector<double> angleSin;//采样角度对应的正弦值
	std::vector<double> angleCos;//采样角度对应的余弦值
	angleSin.reserve(samplingNumber + 1);
	angleCos.reserve(samplingNumber + 1);
	double tmp = 3.14 * 2 / samplingNumber;
	for (int i = 0; i < samplingNumber; ++i)
	{
		angleSin.push_back(sin(i * tmp));
		angleCos.push_back(cos(i * tmp));
	}
	angleSin.push_back(sin(0));
	angleCos.push_back(cos(0));

	//抛物面顶点处采样
	z = radio * radio * value;//固定半径后的z值
	osg::ref_ptr<osg::Vec3Array> vecPre = new osg::Vec3Array;//前一个圆周上的采样点
	osg::ref_ptr<osg::Vec3Array> vecTmp = new osg::Vec3Array;//采样点结果
	vecPre->reserve(samplingNumber + 1);
	vecTmp->reserve(samplingNumber + 1);
	for (int i = 0; i < samplingNumber + 1; ++i)
		vecTmp->push_back(osg::Vec3(radio * angleSin[i], radio * angleCos[i], z));

	vecPre->insert(vecPre->begin(), vecTmp->begin(), vecTmp->end());

	//采样点转换到三角形顶点
	for (int i = 0; i < samplingNumber; ++i)
	{
		points->push_back(vecTmp->at(i));
		points->push_back(vecTmp->at(i + 1));
		points->push_back(osg::Vec3(0, 0, 0));
	}

	//抛物面顶点外的位置采样
	for (radio += radioStep; radio < radioMax; radio += radioStep)
	{
		double z = radio * radio * value;
		if (radio > radioMax || z > zMax || z < -zMax)
			break;

		vecTmp->clear();
		for (int i = 0; i < samplingNumber + 1; ++i)
			vecTmp->push_back(osg::Vec3(radio * angleSin[i], radio * angleCos[i], z));

		//采样点转换到三角形顶点
		for (int i = 0; i < samplingNumber; ++i)
		{
			points->push_back(vecTmp->at(i + 1));
			points->push_back(vecTmp->at(i));
			points->push_back(vecPre->at(i));

			points->push_back(vecPre->at(i));
			points->push_back(vecPre->at(i + 1));
			points->push_back(vecTmp->at(i + 1));
		}

		vecPre->clear();
		vecPre->insert(vecPre->begin(), vecTmp->begin(), vecTmp->end());
	}

	//计算每个三角形的法向量
	int numPoints = points->size();
	norms->reserve(numPoints);
	for (int i = 0; i < numPoints / 3; ++i)
	{
		const auto& ptA = points->at(i * 3);
		const auto& ptB = points->at(i * 3 + 1);
		const auto& ptC = points->at(i * 3 + 2);

		//叉乘 + 单位化
		osg::Vec3 norm = (ptA - ptB).operator^ (ptB - ptC);
		norm.normalize();
		norms->push_back(norm);
		norms->push_back(norm);
		norms->push_back(norm);
	}
	return true;
}