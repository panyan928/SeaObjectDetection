
// MFCApplication2View.cpp : CMFCApplication2View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCApplication2.h"
#endif

#include "MFCApplication2Doc.h"
#include "MFCApplication2View.h"
#include "MatCImageConv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication2View

IMPLEMENT_DYNCREATE(CMFCApplication2View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication2View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_HAZE_REMOVE, &CMFCApplication2View::OnHazeRemove)
	ON_COMMAND(ID_HIST_EQUAL, &CMFCApplication2View::OnHistEqual)
	ON_COMMAND(ID_BoxBlur, &CMFCApplication2View::OnBoxblur)
	ON_COMMAND(ID_ENHANCE_SHARP, &CMFCApplication2View::OnEnhanceSharp)
	ON_COMMAND(ID_Canny_EdgeDetection, &CMFCApplication2View::OnCannyEdgedetection)
	ON_COMMAND(ID_HIST_SHOW, &CMFCApplication2View::OnHistShow)
//	ON_COMMAND(ID_PIC_RESET, &CMFCApplication2View::OnPicReset)
ON_COMMAND(ID_HOUGH_CHA, &CMFCApplication2View::OnHoughCha)
ON_COMMAND(ID_THRESHOLD_OTSU, &CMFCApplication2View::OnThresholdOtsu)
ON_COMMAND(ID_THRESHOLD_FIXED, &CMFCApplication2View::OnThresholdFixed)
ON_COMMAND(ID_DFT_SHOW, &CMFCApplication2View::OnDftShow)
END_MESSAGE_MAP( )

// CMFCApplication2View 构造/析构

CMFCApplication2View::CMFCApplication2View()
{
	// TODO:  在此处添加构造代码
}

CMFCApplication2View::~CMFCApplication2View()
{

}

BOOL CMFCApplication2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCApplication2View 绘制

void CMFCApplication2View::OnDraw(CDC* pDC)
{
	CMFCApplication2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码

	CRect IRect;
	GetClientRect(&IRect);
	/*GetWindowRect(&WRect);
	GetParentFrame()->GetWindowRect(&FRect);*/
	if (video.isOpened()&&pDoc->img.IsNull())
	{
		Mat video_mat,video_end;
		CImage video_image;
		while (1)
		{
			video >> video_mat;
			MatCImageConv::Mat_to_CImage(video_mat, video_image);
			if (video_mat.empty())break;
			video_image.Draw(pDC->GetSafeHdc(), IRect);
			video_mat.copyTo(video_end);	
		}
		MatCImageConv::Mat_to_CImage(video_end, video_image);
		video_image.Draw(pDC->GetSafeHdc(), IRect);
		//pDoc->img.Draw(pDC->GetSafeHdc(), IRect);
	}
	if (!pDoc->img.IsNull())
	{
		//取窗口的长宽
		pDC->SetStretchBltMode(COLORONCOLOR);
		pDoc->img.Draw(pDC->GetSafeHdc(), IRect);
	
	}
}

//第一次显示窗口时调用,先OnInitialUpdate 再Draw
void CMFCApplication2View::OnInitialUpdate()
{
	// OnInitialUpdate()调用OnUpdate();
	CView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小	
	  	

}


// CMFCApplication2View 打印

BOOL CMFCApplication2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCApplication2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CMFCApplication2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// CMFCApplication2View 诊断

#ifdef _DEBUG
void CMFCApplication2View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication2Doc* CMFCApplication2View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication2Doc)));
	return (CMFCApplication2Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplication2View 消息处理程序

void dehaze_fast(double omega, double win_size);
Mat get_dark_channel(Mat image, double win_size);
Mat get_atmosphere(Mat dark_channel);
Mat get_transimission_estimate(Mat atmosphere, double omega, double win_size);
//Mat get_laplacian();
Mat guided_filter(Mat target, int radius, double eps);
Mat window_sum_filter(Mat image, int r);
Mat cumsum(Mat image, int rc);//图像中像素等于同列元素中该行以上所有像素值相加后与该像素的和
Mat get_radiance(Mat transmission, Mat atmosphere);
void HistShow(Mat img);

void CMFCApplication2View::OnHazeRemove()
{
	// TODO:  在此添加命令处理程序代码
	srcImage.copyTo(dstImage);
	dstImage.convertTo(dstImage, CV_32FC3, 1 / 255.0);
	//源图像缩小为0.1倍
	resize(dstImage, dstImage, Size(0, 0), 0.4, 0.4, CV_INTER_AREA);

	dehaze_fast(0.95, 5);

	dstImage.convertTo(dstImage, CV_8UC3, 255.0);

	CMFCApplication2Doc* pDoc = GetDocument();

	MatCImageConv::Mat_to_CImage(dstImage, pDoc->img);
	
	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	pDoc->UpdateAllViews(NULL);

}

// in default, omega=0.95,win_size=15,lamda=0.0001
void dehaze_fast(double omega, double win_size)
{
	int r = 15;
	double res = 0.001;
	Mat dark_channel = Mat(get_dark_channel(dstImage, win_size));
	Mat atmosphere = get_atmosphere(dark_channel);
	Mat trans_est = get_transimission_estimate(atmosphere, omega, win_size);
	Mat x = guided_filter(trans_est, r, res);
	Mat transmission = x.reshape(1, dstImage.rows);
	dstImage = get_radiance(transmission, atmosphere);

}

Mat get_dark_channel(Mat image, double win_size)
{
	int pad_size = floor(win_size / 2);// 图像外层扩充的大小
	Mat dark_channel = Mat::zeros(image.rows, image.cols, CV_32FC1);
	Mat image_padded = Mat(image);
	copyMakeBorder(image, image_padded, pad_size, pad_size, pad_size, pad_size, BORDER_CONSTANT, Scalar(1, 1, 1));//色彩最大值填充边界
	for (int i = 0; i < image.rows; i++) //i代表行数
	{
		float* min_patch = dark_channel.ptr<float>(i);
		for (int j = 0; j < image.cols; j++) // j代表列数
		{
			Rect rect(j, i, win_size, win_size);
			Mat patch = Mat(image_padded, rect);
			patch = patch.reshape(1, 0);
			double n;
			minMaxLoc(patch, &n);
			min_patch[j] = n;
		}
	}

	return dark_channel;
}

Mat get_atmosphere(Mat dark_channel)
{
	double num_pixels = dstImage.rows*dstImage.cols;
	double num_search_pixels = floor(num_pixels*0.01);
	//matlab中矩阵按列扫描并转换，所以将图像先转置，即竖着扫描像素值
	dark_channel = dark_channel.t();
	dark_channel = dark_channel.reshape(1, num_pixels); //将暗通道图像像素值变为列向量，并降序排列

	Mat dstImage_vec = dstImage.t();
	dstImage_vec = dstImage_vec.reshape(1, num_pixels); //原图像素变为列向量

	cv::sortIdx(dark_channel, dark_channel, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);

	Mat accumulator = Mat::zeros(1, 3, CV_32FC1);//新建矩阵用于像素值叠加
	for (int n = 0; n < num_search_pixels; n++)
	{
		Mat temp(dstImage_vec, Rect(0, *dark_channel.ptr<int>(n), 3, 1));
		accumulator += temp;
	}
	accumulator = accumulator / num_search_pixels;
	return accumulator;
}

Mat get_transimission_estimate(Mat atmosphere, double omega, double win_size)
{
	Mat atmosphere_repeated = atmosphere.reshape(3, 1);
	atmosphere_repeated = repeat(atmosphere_repeated, dstImage.rows, dstImage.cols);

	return (1 - omega*get_dark_channel(dstImage / atmosphere_repeated, win_size));
}

//target变为灰度图像，所以以后调用window_sum_filter中所有的image均为单通道图像
//导向滤波
Mat guided_filter(Mat target, int radius, double eps)
{
	Mat guide;
	cvtColor(dstImage, guide, COLOR_RGB2GRAY);
	Mat avg_denom = window_sum_filter(Mat::ones(guide.rows, guide.cols, CV_32FC1), radius);
	Mat mean_g = window_sum_filter(guide, radius) / avg_denom;
	Mat mean_t = window_sum_filter(target, radius) / avg_denom;
	Mat corr_gg = window_sum_filter(guide.mul(guide), radius) / avg_denom;
	Mat corr_gt = window_sum_filter(guide.mul(target), radius) / avg_denom;
	Mat var_g = corr_gg - mean_g.mul(mean_g);
	Mat cov_gt = corr_gt - mean_g.mul(mean_t);

	Mat a = cov_gt / (var_g + eps);
	Mat b = mean_t - a.mul(mean_g);

	a = window_sum_filter(a, radius) / avg_denom;
	b = window_sum_filter(b, radius) / avg_denom;

	return a.mul(guide) + b;
}

//方框滤波
Mat window_sum_filter(Mat image, int r)
{
	//创建与原图相同大小的图像
	Mat sum_img = Mat::zeros(image.rows, image.cols, CV_32FC1);
	//需进行滤波的图像纵向相加
	Mat im_cum = cumsum(image, 1);
	//r=15,半径
	//算数和图像的15-30行赋值给新图像中0-15行
	for (int i = 0; i<r + 1; i++)
	{
		for (int j = 0; j<image.cols; j++)
		{
			sum_img.at<float>(i, j) = im_cum.at<float>(i + r, j);
		}
	}
	//新图像16行到290行， im_cum 31-306行减去0-275行  高度为(image.rows-2*r-2)
	//sum_img(r+2:image.rows-r, :) = im_cum(2*r+2:image.rows, :) - im_cum(1:image.rows-2*r-1, :);  
	for (int i = r + 1; i<image.rows - r; i++)
	{
		for (int j = 0; j<image.cols; j++)
		{
			sum_img.at<float>(i, j) = im_cum.at<float>(i + r, j) - im_cum.at<float>(i - r - 1, j);
		}
	}
	//sum_img(hei-r+1:hei, :) = repmat(im_cum(hei, :), [r, 1]) - im_cum(hei-2*r:hei-r-1, :);  
	for (int i = image.rows - r; i< image.rows; i++)
	{
		for (int j = 0; j< image.cols; j++)
		{
			sum_img.at<float>(i, j) = im_cum.at<float>(image.rows - 1, j) - im_cum.at<float>(i - r - 1, j);
		}
	}
	im_cum = cumsum(sum_img, 2);
	//sum_img(:, 1:r+1) = im_cum(:, 1+r:2*r+1);  
	for (int i = 0; i<image.rows; i++)
	{
		for (int j = 0; j<r + 1; j++)
		{
			sum_img.at<float>(i, j) = im_cum.at<float>(i, j + r);
		}
	}
	//sum_img(:, r+2:wid-r) = im_cum(:, 2*r+2:wid) - im_cum(:, 1:wid-2*r-1);  
	for (int i = 0; i<image.rows; i++)
	{
		for (int j = r + 1; j<image.cols - r; j++)
		{
			sum_img.at<float>(i, j) = im_cum.at<float>(i, j + r) - im_cum.at<float>(i, j - r - 1);
		}
	}
	//sum_img(:, wid-r+1:wid) = repmat(im_cum(:, wid), [1, r]) - im_cum(:, wid-2*r:wid-r-1);  
	for (int i = 0; i< image.rows; i++)
	{
		for (int j = image.cols - r; j<image.cols; j++)
		{
			sum_img.at<float>(i, j) = im_cum.at<float>(i, image.cols - 1) - im_cum.at<float>(i, j - r - 1);
		}
	}
	return sum_img;
}

Mat cumsum(Mat image, int rc)
{
	Mat im_cum;
	image.copyTo(im_cum);
	if (rc == 1)//纵向求和
	{
		for (int col = 0; col < image.cols; col++)
		{
			for (int row = 1; row < image.rows; row++)//第一行不变，从第二行开始相加
			{
				im_cum.at<float>(row, col) += im_cum.at<float>(row - 1, col);
			}
		}
	}
	if (rc == 2)//横向求和
	{
		for (int col = 1; col < image.cols; col++)
		{
			for (int row = 0; row < image.rows; row++)//第一列不变，从第二列开始相加
			{
				im_cum.at<float>(row, col) += im_cum.at<float>(row, col - 1);
			}
		}
	}

	return im_cum;
}

Mat get_radiance(Mat transmission, Mat atmosphere)
{
	Mat rep_atmosphere = atmosphere.reshape(3, 1);
	rep_atmosphere = repeat(rep_atmosphere, dstImage.rows, dstImage.cols);

	Mat max_transmission = Mat::zeros(transmission.rows, transmission.cols, CV_32FC3);

	/*Mat transmission1 = transmission.clone();
	Mat transmission2 = transmission.clone();
	Mat transmission3 = transmission.clone();*/
	//vector<Mat> max_transmission_vec;
	/*max_transmission_vec.push_back(transmission1);
	max_transmission_vec.push_back(transmission2);
	max_transmission_vec.push_back(transmission3);*/
	//merge函数内存出错，不懂为什么，只能纯复制。。。
	//merge(max_transmission_vec, max_transmission);
	Mat_<Vec3f>::iterator it = max_transmission.begin <Vec3f>();
	Mat_<Vec3f>::iterator itend = max_transmission.end <Vec3f>();
	Mat_<float>::iterator transmission_num = transmission.begin<float>();
	for (; it != itend; it++)
	{
		(*it)[0] = *transmission_num;
		(*it)[1] = *transmission_num;
		(*it)[2] = *transmission_num;
		transmission_num++;

	}

	atmosphere = ((dstImage - rep_atmosphere) / max_transmission) + rep_atmosphere;
	return atmosphere;
}


//直方图均衡化
void CMFCApplication2View::OnHistEqual()// 直方图均衡化
{
	// TODO:  在此添加命令处理程序代码
	srcImage.copyTo(dstImage);
	//转化为灰度图cvtColor(srcImage, dstImage, COLOR_BGR2GRAY);
	vector<Mat> channels;
	Mat imageBlueChannel;
	Mat imageGreenChannel;
	Mat imageRedChannel;
	split(dstImage, channels);
	imageBlueChannel = channels.at(0);
	imageGreenChannel = channels.at(1);
	imageRedChannel = channels.at(2);
	
	equalizeHist(imageBlueChannel, imageBlueChannel);
	equalizeHist(imageGreenChannel, imageGreenChannel);
	equalizeHist(imageRedChannel, imageRedChannel);

	merge(channels, dstImage);

	CMFCApplication2Doc* pDoc = GetDocument();

	MatCImageConv::Mat_to_CImage(dstImage, pDoc->img);
	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	pDoc->UpdateAllViews(NULL);
}


//重绘桌面
void CMFCApplication2View::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO:  在此添加专用代码和/或调用基类
	CRect IRect, WRect, FRect;
	int border_row = 0;
	int border_col = 0;
	CMFCApplication2Doc* pDoc = GetDocument();
	//打开视频时调用
	if (pDoc->img.IsNull() && video.isOpened())
	{
		Mat video_temp;
		video >> video_temp;
		MatCImageConv::Mat_to_CImage(video_temp, pDoc->img);
	}
	if (!pDoc->img.IsNull())
	{
		GetParentFrame()->SetWindowPos(NULL, 0, 0, pDoc->img.GetWidth()+border_row, pDoc->img.GetHeight()+border_col, SWP_NOMOVE);
		
		GetClientRect(&IRect);
		GetWindowRect(&WRect);
		GetParentFrame()->GetWindowRect(&FRect);
		border_row = WRect.left - FRect.left + FRect.right - WRect.right + 4;
		border_col = FRect.bottom - WRect.bottom + WRect.top - FRect.top + 4;

		GetParentFrame()->SetWindowPos(NULL, 0, 0, pDoc->img.GetWidth() + border_row, pDoc->img.GetHeight() + border_col, SWP_NOMOVE);
		//去掉系统的菜单  
		GetParentFrame()->SetMenu(NULL);
		//添加自己的菜单  
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);  //加载自己的菜单  
		GetParentFrame()->SetMenu(&menu);
		menu.Detach();   //该函数是用来将菜单句柄与菜单对象分离	
	}
	//选取视频第一帧图片加载进img只是为了根据img初始化窗口，减少代码量，初始化完后把img销毁
	if (video.isOpened()) pDoc->img.Destroy();
}


//播放视频
void CMFCApplication2View::OnVideoShow()
{

	//重绘窗口
	OnUpdate(NULL, NULL, NULL);
}




void CMFCApplication2View::OnBoxblur()
{
	// TODO:  在此添加命令处理程序代码
	srcImage.copyTo(dstImage);
	boxFilter(dstImage, dstImage, -1, Size(5, 5));
	CMFCApplication2Doc* pDoc = GetDocument();

	MatCImageConv::Mat_to_CImage(dstImage, pDoc->img);

	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	pDoc->UpdateAllViews(NULL);
}


//锐化处理
void CMFCApplication2View::OnEnhanceSharp()
{
	// TODO:  在此添加命令处理程序代码
	Mat kernel(3, 3, CV_32F, Scalar(-1));
	kernel.at<float>(1, 1) = 8.9;
	filter2D(srcImage, dstImage, srcImage.depth(), kernel);
	CMFCApplication2Doc* pDoc = GetDocument();

	MatCImageConv::Mat_to_CImage(dstImage, pDoc->img);

	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	pDoc->UpdateAllViews(NULL);
}

//Canny算子检测
void CMFCApplication2View::OnCannyEdgedetection()
{
	// TODO:  在此添加命令处理程序代码
	Mat edge;
	dstImage.create(srcImage.size(), srcImage.type());
	cvtColor(srcImage, edge, COLOR_BGR2GRAY);
	blur(edge, edge, Size(3, 3));
	Canny(edge, edge, 3, 200, 3);
	dstImage = Scalar::all(0);
	srcImage.copyTo(dstImage, edge);

	CMFCApplication2Doc* pDoc = GetDocument();
	
	MatCImageConv::Mat_to_CImage(dstImage, pDoc->img);

	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	pDoc->UpdateAllViews(NULL);
}


//直方图显示
void CMFCApplication2View::OnHistShow()
{
	// TODO:  在此添加命令处理程序代码
	cvtColor(srcImage, dstImage, COLOR_BGR2GRAY);
	//dstImage.create(srcImage.size(), srcImage.type());
	//adaptiveThreshold(dstImage, dstImage, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 5);
	
	HistShow(dstImage);

	//CMFCApplication2Doc* pDoc = GetDocument();

	//MatCImageConv::Mat_to_CImage(edge, pDoc->img);

	////设置脏标记
	//pDoc->SetModifiedFlag(TRUE);

	//pDoc->UpdateAllViews(NULL);
}

//显示图片直方图
void HistShow(Mat img)
{
	Mat hist;
	int bins = 256;
	int hist_size[] = { bins };
	float range[] = { 0, 256 };
	const float* ranges[] = { range,range,range };
	int channels[] = { 0 ,1,2};
	calcHist(&img, 1, channels, Mat(), hist, 1, hist_size, ranges, true, false);
	int hist_height = 256;
	double max_val;
	int scale = 2;
	minMaxLoc(hist, 0, &max_val, 0, 0);
	Mat hist_img = Mat::zeros(hist_height, bins*scale, CV_8UC3);

	for (int i = 0; i < bins; i++)
	{
		float bin_val = hist.at<float>(i);
		int intensity = cvRound(bin_val*hist_height / max_val);
		rectangle(hist_img, Point(i*scale, hist_height - 1), Point((i + 1)*scale - 1, hist_height - intensity), CV_RGB(256, 256, 256));
	}

	imshow("Gray Histogram", hist_img);
}


//霍夫变换
void CMFCApplication2View::OnHoughCha()
{
	// TODO:  在此添加命令处理程序代码
	cvtColor(srcImage, dstImage, COLOR_BGR2GRAY);
	//HoughLines(dstImage,dstImage,)
}

//Otsu阈值分割
void CMFCApplication2View::OnThresholdOtsu()
{
	// TODO:  在此添加命令处理程序代码
	//如果进行多次操作，进行该操作时dstImage仍为上一处理后结果，copyTo后dstImage未改变，所以需先将dstImage释放掉
	dstImage.release();

	Mat GrayImage, SplitImage;// = Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	cvtColor(srcImage, GrayImage, COLOR_BGR2GRAY);
	//阈值分割
	threshold(GrayImage, SplitImage, 100, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//中值滤波，去除椒盐噪声
	medianBlur(SplitImage, SplitImage, 11);
	//Canny边缘
	//Canny(GrayImage, SplitImage, 20, 255);
	//闭运算
	//morphologyEx(SplitImage, SplitImage, MORPH_CLOSE, element);
	//开运算
	//morphologyEx(SplitImage, SplitImage, MORPH_OPEN, element);

	namedWindow("分割图", CV_WINDOW_NORMAL);
	imshow("分割图", SplitImage);
	vector<vector<Point>> contours;
	findContours(SplitImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int max_contour = 0;
	int max_contour_list = 0;
	for (int i = 0; i < contours.size( ); i++)
	{
		int count = contours.at(i).size( );
		if (count>max_contour)
		{
			max_contour = count;
			max_contour_list = i;
		}
	}
	for (int i = 0; i < contours.size( ); i++)
	{
		if (i != max_contour_list) contours.at(i).clear( );
	}

	Mat result(SplitImage.size( ), CV_8U, Scalar(0));
	drawContours(result, contours, -1, Scalar(255), 3);
	//medianBlur(SplitImage, SplitImage, 11);
	//分割结果进行开运算
	//namedWindow("未处理分割图", CV_WINDOW_NORMAL);
	//imshow("未处理分割图", SplitImage);
	//morphologyEx(SplitImage, SplitImage, MORPH_CLOSE, element);
	//morphologyEx(SplitImage, SplitImage, MORPH_OPEN, element);

	result.copyTo(dstImage);

	//更新界面
	CMFCApplication2Doc* pDoc = GetDocument();
	MatCImageConv::Mat_to_CImage(dstImage, pDoc->img);
	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);
	pDoc->UpdateAllViews(NULL);
}

//固定阈值
void CMFCApplication2View::OnThresholdFixed()
{
	// TODO:  在此添加命令处理程序代码
	dstImage.release();
	Mat BinaryImage, SplitImage;// = Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);
	cvtColor(srcImage, BinaryImage, COLOR_BGR2GRAY);
	threshold(BinaryImage, SplitImage, 100, 255, CV_THRESH_BINARY);
	srcImage.copyTo(dstImage, SplitImage);

	CMFCApplication2Doc* pDoc = GetDocument();
	MatCImageConv::Mat_to_CImage(dstImage, pDoc->img);
	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);
	pDoc->UpdateAllViews(NULL);
}


void CMFCApplication2View::OnDftShow( )
{
	// TODO: Add your command handler code here
	dstImage.release( );
	//[1] 彩色图像转为灰度图
	cvtColor(srcImage, dstImage, COLOR_BGR2GRAY);
	//【2】将输入图像延扩到最佳的尺寸，边界用0补充
	int m = getOptimalDFTSize(dstImage.rows);
	int n = getOptimalDFTSize(dstImage.cols);
	//将添加的像素初始化为0.
	Mat padded;
	copyMakeBorder(dstImage, padded, 0, m - dstImage.rows, 0, n - dstImage.cols, BORDER_CONSTANT, Scalar::all(0));

	//【3】为傅立叶变换的结果(实部和虚部)分配存储空间。
	//将planes数组组合合并成一个多通道的数组complexI
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size( ), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);

	//【4】进行就地离散傅里叶变换
	dft(complexI, complexI);

	//【5】将复数转换为幅值，即=> log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	split(complexI, planes); // 将多通道数组complexI分离成几个单通道数组，planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude  
	Mat magnitudeImage = planes[0];

	//【6】进行对数尺度(logarithmic scale)缩放
	magnitudeImage += Scalar::all(1);
	log(magnitudeImage, magnitudeImage);//求自然对数

	//【7】剪切和重分布幅度图象限
	//若有奇数行或奇数列，进行频谱裁剪      
	magnitudeImage = magnitudeImage(Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));
	//重新排列傅立叶图像中的象限，使得原点位于图像中心  
	int cx = magnitudeImage.cols / 2;
	int cy = magnitudeImage.rows / 2;
	Mat q0(magnitudeImage, Rect(0, 0, cx, cy));   // ROI区域的左上
	Mat q1(magnitudeImage, Rect(cx, 0, cx, cy));  // ROI区域的右上
	Mat q2(magnitudeImage, Rect(0, cy, cx, cy));  // ROI区域的左下
	Mat q3(magnitudeImage, Rect(cx, cy, cx, cy)); // ROI区域的右下
	//交换象限（左上与右下进行交换）
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	//交换象限（右上与左下进行交换）
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	//【8】归一化，用0到1之间的浮点值将矩阵变换为可视的图像格式
	normalize(magnitudeImage, magnitudeImage, 0, 1, NORM_MINMAX);
	magnitudeImage.convertTo(magnitudeImage, CV_8UC3, 255.0);
	//【9】显示效果图
	CMFCApplication2Doc* pDoc = GetDocument( );
	MatCImageConv::Mat_to_CImage(magnitudeImage, pDoc->img);
	//设置脏标记
	pDoc->SetModifiedFlag(TRUE);
	pDoc->UpdateAllViews(NULL);
}
