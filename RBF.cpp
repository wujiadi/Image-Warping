#include "RBF.h"
#include <iostream>
#include <Dense>

using namespace std;
using namespace Eigen;

RBF::RBF()
{

}

RBF::~RBF()
{

}

int RBF::Init(QPolygon &StartPoints, QPolygon &EndPoints)
{
	GetCoeR(StartPoints);
	GetCoeXY(StartPoints, EndPoints);

	return 0;
}

int RBF::DoWrap(QImage &image, QPolygon &StartPoints, QPolygon &EndPoints)
{
	QImage tempimage = image;
	QPoint temppoint;
	QPoint resultpoint;

	MatrixSet.resize(image.height(), image.width());
	MatrixSet.setZero();

	for (int i=0; i<image.width(); i++)
	{
		for (int j=0; j<image.height(); j++)
		{
			image.setPixel(i, j, qRgb(255, 255, 255) );
		}
	}

	for (int i=0; i<image.width(); i++)
	{
		for (int j=0; j<image.height(); j++)
		{
			QRgb color = tempimage.pixel(i, j);
			temppoint.rx() = i;
			temppoint.ry() = j;

			resultpoint = CalculatePixel(temppoint, StartPoints);

			if((resultpoint.x() > 0) && (resultpoint.x() < image.width()) && (resultpoint.y() > 0) && (resultpoint.y() < image.height()))
			{
				image.setPixel(resultpoint.x(), resultpoint.y(), color);
				MatrixSet(resultpoint.y(), resultpoint.x()) = 1;
			}
		}
	}

	return 0;

}

QPoint RBF::CalculatePixel(QPoint const &orgpoint, QPolygon &StartPoints)
{
	QPoint temppoint;
	double tempdata;
	double temppointx;
	double temppointy;

	int i,j,len;

	temppoint.rx() = 0;
	temppoint.ry() = 0;
	temppointx = 0;
	temppointy = 0;

	len = CoeR.size();

	if (len == 0)
	{
		;
	}
	else
	{
		for (i = 0; i < len; i++)
		{
			tempdata = pow(CoeR[i], 2) + pow(GetDistance(StartPoints[i], orgpoint), 2);
			temppointx = temppointx + CoeX[i] * pow(tempdata, 0.5);
			temppointy = temppointy + CoeY[i] * pow(tempdata, 0.5);
		}

		temppoint.rx() = temppointx + orgpoint.x();
		temppoint.ry() = temppointy + orgpoint.y();
	}

	return temppoint;

}


void RBF::GetCoeR(QPolygon &StartPoints)
{
	int i,j,len;
	double minvalue;

	len = StartPoints.size();

	for (i = 0; i < len; i++)
	{
		minvalue = 0xFFFF;

		for (j = 0; j < len; j++)
		{
			if(i != j)
			{
				if (GetDistance(StartPoints[i], StartPoints[j]) < minvalue)
				{
					minvalue = GetDistance(StartPoints[i], StartPoints[j]);
				}
				
			}
		}

		CoeR.push_back(minvalue);

	}

}

//AX = b;
void RBF::GetCoeXY(QPolygon &StartPoints, QPolygon &EndPoints)
{
	int i,j;
	int MatrixSize = StartPoints.size();
	double temp;

	MatrixXd A(MatrixSize,MatrixSize);
	VectorXd bx(MatrixSize);
	VectorXd by(MatrixSize);
	VectorXd X(MatrixSize);
	VectorXd Y(MatrixSize);

	//a[i][j]
	for (i = 0; i < MatrixSize; i++)//row
	{
		for (j = 0; j < MatrixSize; j++)//column
		{
			temp = pow(CoeR[j],2) + pow(GetDistance(StartPoints[i],StartPoints[j]),2);//d^2 + r^2
			temp = pow(temp,0.5);//u = 0.5
			A(i, j) = temp;
		}
	}

	for (i = 0; i < MatrixSize; i++)
	{
		bx[i] = (EndPoints[i].x() - StartPoints[i].x());
		by[i] = (EndPoints[i].y() - StartPoints[i].y());
	}

	X = A.colPivHouseholderQr().solve(bx);

	for (i = 0; i < MatrixSize; i++)
	{
		CoeX.push_back( X[i]);
	}

	Y = A.colPivHouseholderQr().solve(by);

	for (i = 0; i < MatrixSize; i++)
	{
		CoeY.push_back( Y[i]);
	}

	
}