#pragma once
static class MatCImageConv
{
public:
	MatCImageConv();
	~MatCImageConv();
	static void Mat_to_CImage(Mat& mat, CImage& cimage);
	static void CImage_to_Mat(CImage& cimage, Mat& mat);
};

