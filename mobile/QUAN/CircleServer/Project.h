#ifndef __PENGYOU_PROJECT_H__
#define __PENGYOU_PROJECT_H__

#include <sys/types.h>


//椭球参数
namespace EllipCoeff
{
	//缺省使用WGS-84的椭球参数
	const double	a   = 6378137;   
	const double	f   = 298.257224; 
	const double	A1  = 111132.9525494;   
	const double	A2  = -16038.50840;   
	const double	A3  = 16.83260;   
	const double	A4  = -0.02198;   
	const double	A5  = 0.00003;   
	
	const double	e2  = 0.0066943799013;   
	const double	e12 = 0.00673949674227; 
};

class CProject
{
public:
	CProject();
public:
	//高斯-克吕格投影 经纬度(单位为度)到直角坐标系的转换(单位为米)
	void Gauss_BL2xy(double b, double l, int& x, int& y);

	//由XY反算经纬度坐标
	void Gauss_xy2BL(int x, int y, double& b, double& l);

    //UTM 投影
	void UTM_BL2xy(double b, double l, int& x, int& y);

	//UTM 投影由XY反算经纬度坐标
	void UTM_xy2BL(int x, int y, double& b, double& l);

	//计算两点之间的距离
	int  Gauss_Distance(double b1, double l1, double b2, double l2);

	int  Distance(double b1, double l1, double b2, double l2);
private:
	//计算带内坐标,高斯-克吕克投影
	void Gauss_BL2xy_inBand(double b, double l, int& x, int& y, int & bandno);

	//UTM 投影
	void UTM_BL2xy_inBand(double b, double l, int& x, int& y, int & bandno );

private:
	double	_bandwidth;		//分带投影，带宽
};

#endif


