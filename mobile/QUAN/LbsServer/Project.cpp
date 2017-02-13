#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Project.h"

const double PI = 3.14159265353846;

CProject::CProject()
{
	_bandwidth 	= 3.0;//缺省使用3度带投影
}


void CProject::Gauss_BL2xy_inBand(double b, double l, int & x, int& y, int & bandno)
{
	//计算带号和中央子午线 L0
    int Sign = l  >0 ? 1 : -1;

	bandno =  (int)((l + Sign*(_bandwidth/2 - 10e-14))/_bandwidth);

	double _L0   = bandno * _bandwidth;
	_L0   = _L0 * PI / 180;
	//end

	//计算该点在分带投影x，y坐标
	double X, N, t, t2, p, p2, eta2;   
	double sinB, cosB;   
	//save current B&l    
	double B = b* PI / 180.0;   
	double L = l* PI / 180.0;       
	   
	X = EllipCoeff::A1 * B * 180.0 / PI + EllipCoeff::A2 * sin(2 * B) +    
		EllipCoeff::A3 * sin(4 * B) + EllipCoeff::A4 * sin(6 * B) +    
		EllipCoeff::A5 * sin(8 * B);   
	
	sinB = sin(B);   
	cosB = cos(B);   
	t = tan(B);   
	t2 = t * t;   
	
	N = EllipCoeff::a / sqrt(1 - EllipCoeff::e2 * sinB * sinB);   
	p = cosB * (L - _L0); 
	p2 = p * p;   
	eta2 = cosB * cosB * EllipCoeff::e2 / (1 - EllipCoeff::e2);   
	double _x = X + N * t * ((0.5 + ((5 - t2 + 9 * eta2 + 4 * eta2 * eta2) / 24.0 +    
		(61 - 58 * t2 + t2 * t2) * p2 / 720.0) * p2) * p2);   
	double _y = N * p * ( 1 + p2 * ( (1 - t2 + eta2) / 6.0 + p2 * ( 5 - 18 * t2 + t2 * t2    
		+ 14 * eta2 - 58 * eta2 * t2 ) / 120.0));   
	//_y += 500000;
	
	//修正一下
	x = (int)(_x);
	y = (int)(_y);


	//cout << "bandno "  << bandno << "x = " << x  << " y = " << y  << endl;
}

//b,l为ddd格式，及用小数度来表示分秒。
void CProject::Gauss_BL2xy(double b, double l, int& x, int& y)
{
	int band_x = 0, band_y = 0;
	int bandno = 0;
	Gauss_BL2xy_inBand(b, l, band_x, band_y, bandno);

	
	//计算偏移
	int tmpx = 0, tmpy = 0;
	int tmpno = 0;
	Gauss_BL2xy_inBand(b, _bandwidth/2.0 - 10e-14, tmpx, tmpy, tmpno);

	x = band_x + 10002000;
	y = bandno*tmpy*2 + band_y + 20004000;

	//cout << "x = " << x << " y = " << y << endl;
}

void CProject::Gauss_xy2BL(int x, int y, double& b, double& l)
{
	double sinB, cosB, t, t2, N ,ng2, Mf, yN;
    double preB0, B0;
    double delta;

    y -= 20004000;
	x -= 10002000;

    B0 = x / EllipCoeff::A1;
    do
	{
		preB0 = B0;
		B0 = B0 * PI / 180.0;
		B0 = (x - (EllipCoeff::A2 * sin(2 * B0) + EllipCoeff::A3 * sin(4 * B0) + EllipCoeff::A4 * sin(6 * B0))) / EllipCoeff::A1;
		delta = fabs(B0 - preB0);
	}while(delta > 0.000000001);


	int Y = y;
    double Bf = B0 * PI / 180.0;

	if( fabs( fabs(Bf) - 90.0 ) < 10e-14)
	{
		//在两极地方,经度随便给
		b = B0;
		l = 0;
		return;
	}

	//计算带号和中央子午线
	int tmpx, tmpy, bandno;
	Gauss_BL2xy_inBand(B0, _bandwidth/2.0 - 10e-14, tmpx, tmpy, bandno);

	bandno = ((Y>0)?1:-1)*( abs(Y/tmpy) + 1) /2;
	Y = Y - bandno*2*tmpy;
	double L0 = bandno*_bandwidth;

	sinB = sin(Bf);
    cosB = cos(Bf);
    t = tan(Bf);
    t2 = t * t;
    N = EllipCoeff::a / sqrt(1 - EllipCoeff::e2 * sinB * sinB);
    ng2 = cosB * cosB * EllipCoeff::e12;             //ng2 = cosB * cosB * e2 / (1 - e2);
	Mf = N/(1+ng2);
    yN = Y / N;

	b = Bf-(t*Y*yN)/Mf*(0.5-(5+ 3*t2 + ng2 -9*ng2*t2)*yN*yN/24.0 + (61 + 90*t2 + 45*t2*t2)* yN * yN * yN * yN/720.0); 
	l = yN*( 1- (1+2*t2 + ng2)*yN*yN/6.0 + (5+28*t2 + 24 * t2 * t2 + 6 * ng2 + 8 * ng2 * t2)* yN * yN * yN * yN/120.0 )/cosB;

	b = b*180.0/PI;
	l = L0 + l*180.0/PI;

	//cout << "b = " << b << " l = " << l << endl;
}

//http://www.movable-type.co.uk/scripts/latlong-vincenty.html
int  CProject::Gauss_Distance(double b1, double l1, double b2, double l2)
{
	int MAXITERS = 20;
	// Convert lat/long to radians
	double lat1 = b1 * PI / 180;
	double lon1 = l1 * PI / 180;
	double lat2 = b2 * PI / 180;
	double lon2 = l2 * PI / 180;	

	double a = 6378137.0; // WGS84 major axis
	double b = 6356752.3142; // WGS84 semi-major axis
	double f = (a - b) / a;
	double aSqMinusBSqOverBSq = (a * a - b * b) / (b * b);
	
	double L = lon2 - lon1;
	double A = 0.0;
	double U1 = atan((1.0 - f) * tan(lat1));
	double U2 = atan((1.0 - f) * tan(lat2));
	
	double cosU1 = cos(U1);
	double cosU2 = cos(U2);
	double sinU1 = sin(U1);
	double sinU2 = sin(U2);
	double cosU1cosU2 = cosU1 * cosU2;
	double sinU1sinU2 = sinU1 * sinU2;
	
	double sigma = 0.0;
	double deltaSigma = 0.0;
	double cosSqAlpha = 0.0;
	double cos2SM = 0.0;
	double cosSigma = 0.0;
	double sinSigma = 0.0;
	double cosLambda = 0.0;
	double sinLambda = 0.0;

	double lambda = L; // initial guess

	for (int iter = 0; iter < MAXITERS; iter++) 
	{
		double lambdaOrig = lambda;
		cosLambda = cos(lambda);
		sinLambda = sin(lambda);
		double t1 = cosU2 * sinLambda;
		double t2 = cosU1 * sinU2 - sinU1 * cosU2 * cosLambda;
		double sinSqSigma = t1 * t1 + t2 * t2; // (14)
		sinSigma = sqrt(sinSqSigma);
		cosSigma = sinU1sinU2 + cosU1cosU2 * cosLambda; // (15)
		sigma = atan2(sinSigma, cosSigma); // (16)
		double sinAlpha = (sinSigma == 0) ? 0.0 :cosU1cosU2 * sinLambda / sinSigma; // (17)
		cosSqAlpha = 1.0 - sinAlpha * sinAlpha;
		cos2SM = (cosSqAlpha == 0) ? 0.0 :cosSigma - 2.0 * sinU1sinU2 / cosSqAlpha; // (18)

		double uSquared = cosSqAlpha * aSqMinusBSqOverBSq; // defn
		A = 1 + (uSquared / 16384.0) * // (3)
			(4096.0 + uSquared *
			 (-768 + uSquared * (320.0 - 175.0 * uSquared)));
		double B = (uSquared / 1024.0) * // (4)
			(256.0 + uSquared *
			 (-128.0 + uSquared * (74.0 - 47.0 * uSquared)));
		double C = (f / 16.0) *
			cosSqAlpha *
			(4.0 + f * (4.0 - 3.0 * cosSqAlpha)); // (10)
		double cos2SMSq = cos2SM * cos2SM;
		deltaSigma = B * sinSigma * // (6)
			(cos2SM + (B / 4.0) *
			 (cosSigma * (-1.0 + 2.0 * cos2SMSq) -
			  (B / 6.0) * cos2SM *
			  (-3.0 + 4.0 * sinSigma * sinSigma) *
			  (-3.0 + 4.0 * cos2SMSq)));

		lambda = L +
			(1.0 - C) * f * sinAlpha *
			(sigma + C * sinSigma *
			 (cos2SM + C * cosSigma *
			  (-1.0 + 2.0 * cos2SM * cos2SM))); // (11)

		double delta = (lambda - lambdaOrig) / lambda;
		if (fabs(delta) < 1.0e-12) 
		{
			break;
		}
	}

	return (int) (b * A * (sigma - deltaSigma));
}

void CProject::UTM_BL2xy_inBand(double b, double l, int& x, int& y, int & bandno )
{
  double k0 = 0.9996;

  double L0 = -180;

  //保证经度在 -180.00 .. 179.9 之间
  double L = (l+180)-int((l+180)/360)*360-180 -10e-14; 
  bandno = int((L + 180)/6) + 1; //带号，UTM只有6度分带
  double B = b;

  //这里不是做标准地图，为了方便计算，让每个投影带等宽，就不特殊处理
  /*
  if ( B >= 56.0 && B < 64.0 && L >= 3.0 && L < 12.0 )
  {
	  bandno = 32;
  }


  // UTM投影有些特别的区，需要特别处理
  if( B >= 72.0 && B < 84.0 )
  {
    if(  L >= 0.0  && L <  9.0 )     bandno = 31;
    else if( L >= 9.0  && L < 21.0 ) bandno = 33;
    else if( L >= 21.0 && L < 33.0 ) bandno = 35;
    else if( L >= 33.0 && L < 42.0 ) bandno = 37;
  }
  */

  L0 = (bandno - 1)*6 - 180 + 3;  //+3 puts origin in middle of zone

  //转成弧度
  L0 = L0 * PI/180.0;
  B  = B  * PI/180.0;
  L  = L  * PI/180.0; 

  double sinB = sin(B);   
  double cosB = cos(B);   
  double t = tan(B);   
  double t2 = t * t;   

  double N = EllipCoeff::a / sqrt(1 - EllipCoeff::e2 * sinB * sinB);   
  double p = cosB * (L - L0); 
  double p2 = p * p;   
  double eta2 = cosB * cosB * EllipCoeff::e12 ;   

  double X = EllipCoeff::a*((1- EllipCoeff::e2/4 - 3*EllipCoeff::e2*EllipCoeff::e2/64- 5*EllipCoeff::e2*EllipCoeff::e2*EllipCoeff::e2/256)*B
	  - (3*EllipCoeff::e2/8+ 3*EllipCoeff::e2*EllipCoeff::e2/32+ 45*EllipCoeff::e2*EllipCoeff::e2*EllipCoeff::e2/1024)*sin(2*B)
	  + (15*EllipCoeff::e2*EllipCoeff::e2/256 + 45*EllipCoeff::e2*EllipCoeff::e2*EllipCoeff::e2/1024)*sin(4*B) 
	  - (35*EllipCoeff::e2*EllipCoeff::e2*EllipCoeff::e2/3072)*sin(6*B));

  y = (int)(k0*N*(p+(1-t2+eta2)*p*p2/6 + (5-18*t2+t2*t2+72*eta2-58*EllipCoeff::e12)*p2*p2*p/120));

  x = (int)(k0*(X + N*t*(p2/2+(5-t2+9*eta2+4*eta2*eta2)*p2*p2/24 + (61-58*t2+t2*t2+600*eta2-330*EllipCoeff::e12)*p2*p2*p2/720)));


  //cout << "bandno "  << bandno << "x = " << x  << " y = " << y  << endl;
}

void CProject::UTM_BL2xy(double b, double l, int& x, int& y)
{
	int band_x = 0, band_y = 0;
	int bandno = 0;
	UTM_BL2xy_inBand(b, l, band_x, band_y, bandno);

	
	//计算偏移
	int tmpx = 0, tmpy = 0;
	int tmpno = 0;
	UTM_BL2xy_inBand(b, 6, tmpx, tmpy, tmpno);

	x = band_x + 10002000;
	y = (bandno*2 - 61)*tmpy + band_y + 20004000;

	//cout << "x = " << x << " y = " << y << endl;
}

void CProject::UTM_xy2BL(int x, int y, double& b, double& l)
{
  double k0 = 0.9996;
  double e2 = EllipCoeff::e2;
  double e1 = (1-sqrt(1-e2))/(1+sqrt(1-e2));
 
  double L0;
  
  //先换算为代理坐标
  x = x - 10002000;
 
  double M  = x / k0;
  double mu = M/( EllipCoeff::a*(1-e2/4-3*e2*e2/64-5*e2*e2*e2/256.0));
  double B0 = mu   + (3*e1/2-27*e1*e1*e1/32)*sin(2*mu) + (21*e1*e1/16-55*e1*e1*e1*e1/32)*sin(4*mu) +(151*e1*e1*e1/96)*sin(6*mu);
  double Bf = B0*180.0/PI;

  y = y - 20004000;
  
  //计算纬度为Bf时候的带宽
  int tmpx = 0, tmpy = 0, bandno;
  UTM_BL2xy_inBand(Bf, 6, tmpx, tmpy, bandno);
  bandno = (int)( (y / (tmpy*1.0) + 61 )/2 );
  y = y - (bandno*2 - 61)*tmpy;
  L0 = (bandno - 1)*6 - 180 + 3;

  double sinB = sin(B0);   
  double cosB = cos(B0);   
  double t = tan(B0);   

  double N = EllipCoeff::a/sqrt(1-e2*sinB*sinB);
  double eta2  = EllipCoeff::e12*cosB*cosB;
  double R1 = EllipCoeff::a*(1-e2)/pow(1-e2*sinB*sinB, 1.5);
  double yN = y/(N*k0);

  b = B0 - (N*t/R1)*(yN*yN/2-(5+3*t+10*eta2-4*eta2*eta2-9*EllipCoeff::e12)*yN*yN*yN*yN/24 +(61+90*t+298*eta2+45*t*t-252*EllipCoeff::e12-3*eta2*eta2)*yN*yN*yN*yN*yN*yN/720);
  l = (yN-(1+2*t+eta2)*yN*yN*yN/6+(5-2*eta2+28*t-3*eta2*eta2+8*EllipCoeff::e12+24*t*t) *yN*yN*yN*yN*yN/120)/cosB;

  b = b  *180.0/PI;
  l = L0 + l * 180.0/PI;
  //cout << "b = " << b << " l = " << l << endl;
}



//计算两点之间的距离--无线提供,精准度不是很高，计算方法是是把地球当成一个标准球体
int  CProject::Distance(double b1, double l1, double b2, double l2)
{
	const double EARTH_RADIUS = 6371.1; //km

	// convert to radians
	double lat1 = b1 * PI / 180;
	double lon1 = l1 * PI / 180;
	double lat2 = b2 * PI / 180;
	double lon2 = l2 * PI / 180;	

   return (int)(EARTH_RADIUS * (2 * asin(sqrt(pow(sin((lat1-lat2)/2),2) +
		cos(lat1)*cos(lat2)*pow(sin((lon1-lon2)/2),2)))) * 1000);
}
