
// MFCApplication2Doc.cpp : CMFCApplication2Doc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCApplication2.h"
#endif

#include "MFCApplication2Doc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCApplication2Doc

IMPLEMENT_DYNCREATE(CMFCApplication2Doc, CDocument)

BEGIN_MESSAGE_MAP(CMFCApplication2Doc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, &CMFCApplication2Doc::OnFileSave)
	ON_COMMAND(ID_PIC_RESET, &CMFCApplication2Doc::OnPicReset)
	ON_COMMAND(ID_FILE_SAVE_AS, &CMFCApplication2Doc::OnFileSaveAs)
END_MESSAGE_MAP()


// CMFCApplication2Doc 构造/析构

CMFCApplication2Doc::CMFCApplication2Doc()
{
	// TODO:  在此添加一次性构造代码

}

CMFCApplication2Doc::~CMFCApplication2Doc()
{
}

BOOL CMFCApplication2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMFCApplication2Doc 序列化

void CMFCApplication2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
		img.Save(m_strPathName);
	}
	else
	{
		// TODO:  在此添加加载代码
		//打开新文件前先将之前打开的视频或图片文件销毁
		video.release();
		if (!img.IsNull()) img.Destroy();
		img.Load(ar.GetFile()->GetFilePath());
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMFCApplication2Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMFCApplication2Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMFCApplication2Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMFCApplication2Doc 诊断

#ifdef _DEBUG
void CMFCApplication2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCApplication2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCApplication2Doc 命令




void CMFCApplication2Doc::OnFileSave()
{
	// TODO:  在此添加命令处理程序代码
	img.Save(m_strPathName);
	SetModifiedFlag(FALSE);
}

void CMFCApplication2Doc::OnPicReset()
{
	// TODO:  在此添加命令处理程序代码
	img.Destroy();
	img.Load(m_strPathName);
	UpdateAllViews(NULL);
	srcImage.copyTo(dstImage);
	SetModifiedFlag(FALSE);
}

void CMFCApplication2Doc::OnFileSaveAs()
{
	// TODO:  在此添加命令处理程序代码
	CString strFilter,lpszFileName;
	strFilter = "JPEG(*.jpg;*.jpeg;*.jpe)|*.jpg; *.jpeg;*.jpe| BMP (*.bmp;*.dib)|*.bmp; *.dib|GIF(*.gif)|*.gif| TIFF(*.tif;*.tiff)|*.tif;*.tiff| PNG(*.png)|*.png| All Files(*.*) | *.* || ";
	lpszFileName = m_strTitle;
	CFileDialog dlg(FALSE, NULL,lpszFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if (IDOK != dlg.DoModal())
		return;
	else
	{
		//OpenDocumentFile(dlg.m_ofn.lpstrFile);
		img.Save(dlg.m_ofn.lpstrFile);
		SetModifiedFlag(FALSE);
	}
}
