
// MFCApplication2View.h : CMFCApplication2View ��Ľӿ�
//

#pragma once


class CMFCApplication2View : public CView
{
protected: // �������л�����
	CMFCApplication2View();
	DECLARE_DYNCREATE(CMFCApplication2View)

// ����
public:
	CMFCApplication2Doc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMFCApplication2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

public:
public:
	afx_msg void OnHazeRemove();
	afx_msg void OnHistEqual();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnBoxblur();
	afx_msg void OnEnhanceSharp();
	afx_msg void OnCannyEdgedetection();
	afx_msg void OnHistShow();
	void OnVideoShow();
//	afx_msg void OnPicReset();
	afx_msg void OnHoughCha();
	afx_msg void OnThresholdOtsu();
	afx_msg void OnThresholdFixed();
	afx_msg void OnDftShow( );
};

#ifndef _DEBUG  // MFCApplication2View.cpp �еĵ��԰汾
inline CMFCApplication2Doc* CMFCApplication2View::GetDocument() const
   { return reinterpret_cast<CMFCApplication2Doc*>(m_pDocument); }
#endif

