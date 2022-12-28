




// PCAfaceRecognitionView.cpp : implementation of the CPCAfaceRecognitionView class
//
// Notice that the accuracy is affected by Debug and Release mode.
//
//

	/*** Example showing how to handle cv::reshape() function
	int n1 = 3;
	int n2 = 2;
	Mat V = Mat(n1, n2, CV_32F, 0.0f);  // 3 x 2 matrix
	V.at<float>(0,0) = 11.0; V.at<float>(0,1) = 12.0;
	V.at<float>(1,0) = 21.0; V.at<float>(1,1) = 22.0;
	V.at<float>(2,0) = 31.0; V.at<float>(2,1) = 32.0;
	Mat temp1 = V.reshape(0, 6); // makes 1 x 6 matrix
	Mat temp2 = V.reshape(0, 3); // makes 3-row matrix (3 x 2 matrix)
	cout << V << endl;
	cout << temp1 << endl;
	cout << temp2 << endl;
	***/

	/*** example file writing
	FILE * pFile;
	float buffer[] = { 1.0 , 2.0 , 3.0 , 5.0, 6.0};
	pFile = fopen ( "myfile" , "wb" );  // write in binary format
	fwrite (buffer , 1 , sizeof(buffer) , pFile );
	fclose (pFile);

	pFile = fopen("myfile", "rb");
	fseek(pFile, 0, SEEK_END);
	long sizeFloat = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	float *f = new float[sizeFloat];

	size_t sizeByte = fread(f, sizeof(float), sizeFloat, pFile);
	fclose(pFile);
	cout << "size= " << sizeFloat << "  " << "sizeByte= " << sizeByte << endl;
	int numData = sizeFloat/4;
	for (int i=0; i<numData; i++)
		printf("%f ", f[i]);
	***/

	/*** example file writing with Matrix
	int n1 = 3;
	int n2 = 2;
	Mat V = Mat(n1, n2, CV_32F, 0.0f);  // 3 x 2 matrix
	V.at<float>(0,0) = 11.0; V.at<float>(0,1) = 12.0;
	V.at<float>(1,0) = 21.0; V.at<float>(1,1) = 22.0;
	V.at<float>(2,0) = 31.0; V.at<float>(2,1) = 32.0;
	cout << V << endl;
	for (int i=0; i<n1; i++) {
		for (int j=0; j<n2; j++) {
			printf("%f ", V.at<float>(i,j));
		}
		printf("\n");
	}

	float *buf = new float[n1*n2];
	for (int i=0; i<n1; i++)
		for (int j=0; j<n2; j++)
			buf[i*n2+j] = V.at<float>(i,j);
	printf("\nbuf:\n");
	for (int i=0; i<n1*n2; i++)
		printf("%f ", buf[i]);
	printf("\n");

	FILE * pFile;
	pFile = fopen ( "myfile" , "wb" );  // write in binary format
	fwrite (buf, 1, sizeof(float)*n1*n2, pFile);
	fclose (pFile);

	pFile = fopen("myfile", "rb");
	fseek(pFile, 0, SEEK_END);
	long sizeFloat = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	float *buf2 = new float[sizeFloat];

	size_t sizeByte = fread(buf2, sizeof(float), sizeFloat, pFile);
	fclose(pFile);
	// we can calculate number of float from sizeByte (sizeByte*4=sizeFloat)
	// but we don't know the row and column size from sizeByte
	cout << "size= " << sizeFloat << "  " << "sizeByte= " << sizeByte << endl;

	cout << "buf2: " << endl;
	for (int i=0; i<n1*n2; i++)
		printf("%f ", buf2[i]);
	Mat V2 = Mat(n1, n2, CV_32F, 0.0f);
	printf("\n");
	for (int i=0; i<n1; i++)
		for (int j=0; j<n2; j++)
			V2.at<float>(i,j) = buf2[i*n2+j];

	cout << V2 << endl;

	delete buf, buf2;
	***/

	/*** Example sorting in openCV Matrix
	Mat V = Mat(2, 4, CV_32F, 0.0f);  // 2 x 4 matrix
	V.at<float>(0,0) = 51.0; V.at<float>(0,1) = 12.0; V.at<float>(0,2) = 32.0; V.at<float>(0,3) = 20.0;
	V.at<float>(1,0) = 21.0; V.at<float>(1,1) = 22.0; V.at<float>(1,2) = 12.0; V.at<float>(1,3) = 10.0;
	cout << V << endl;
	Mat m;
	//cv::sort(V, m, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING); // m contains values in sorted order
	cv::sortIdx(V, m, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING); // m contains indices of sorted values
	cout << m << endl;
	***/

#include "stdafx.h"
	// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
	// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PCAfaceRecognition.h"
#endif

#include <stdio.h>
#include "PCAfaceRecognitionDoc.h"
#include "PCAfaceRecognitionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace cv;

// define filenames to store training results
// these will be read in validataion and test routines
char filenameDim[] = "dimension"; // store row and column of U
char filenameGallery[] = "gallery"; // store gallery image vectors(U_p)
char filenameMean[] = "mean"; // store mean (mu_p)
char filenameGalleryID[] = "galleryID"; // store gallery ID
char filenameVec[] = "eigenvector"; // store eigenvectors
// eigenvector matrix is symmetric in a normal PCA
// but, the row and column of eigenvector matrix are different in the faster PCA solution

int ndim_reduced = 50; // reduced dimension, variable 'c' in the lecture note
int ndim_start = 0;

int GetDirFiles(const char*, vector<string>*);

// CPCAfaceRecognitionView

IMPLEMENT_DYNCREATE(CPCAfaceRecognitionView, CView)

BEGIN_MESSAGE_MAP(CPCAfaceRecognitionView, CView)
	ON_COMMAND(ID_FACERECOGNITION_TRAIN, &CPCAfaceRecognitionView::OnFacerecognitionTrain)
	ON_COMMAND(ID_FACERECOGNITION_TEST, &CPCAfaceRecognitionView::OnFacerecognitionTest)
	ON_COMMAND(ID_TEST2, &CPCAfaceRecognitionView::OnTest2)
	ON_COMMAND(ID_FILE_OPEN, &CPCAfaceRecognitionView::OnFileOpen)
END_MESSAGE_MAP()

// CPCAfaceRecognitionView construction/destruction

CPCAfaceRecognitionView::CPCAfaceRecognitionView()
{
	con.CreateConsole();
	display = 0;
}

CPCAfaceRecognitionView::~CPCAfaceRecognitionView()
{
	con.~ConsoleWnd();
}

BOOL CPCAfaceRecognitionView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}

// CPCAfaceRecognitionView drawing

void CPCAfaceRecognitionView::OnDraw(CDC* pDC)
{
	CPCAfaceRecognitionDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (display) {
		m_probe.Draw(pDC->GetSafeHdc(), 10, 30);  // Display image at x, y coordinates

		for (int i = 0; i < 10; i++) {
			m_match[i].Draw(pDC->GetSafeHdc(), 150 + (42 + 15) * i, 30);

			m_id.Format("%d", m_matchID[i]); // print filtering time
			pDC->TextOut(160 + (42 + 15) * i, 42 + 35, m_id);
		}

		m_id.Format("probe: %d", m_probeID); // print filtering time
		pDC->TextOut(3, 42 + 35, m_id);

		pDC->TextOut(100, 42 + 35, "match: ");

	}
}


// CPCAfaceRecognitionView diagnostics

#ifdef _DEBUG
void CPCAfaceRecognitionView::AssertValid() const
{
	CView::AssertValid();
}

void CPCAfaceRecognitionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPCAfaceRecognitionDoc* CPCAfaceRecognitionView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPCAfaceRecognitionDoc)));
	return (CPCAfaceRecognitionDoc*)m_pDocument;
}
#endif //_DEBUG


// CPCAfaceRecognitionView message handlers

void CPCAfaceRecognitionView::OnFacerecognitionTrain()
{
	// This shoud contain the Training part
	// Move the part processing Validation data set to OnFacerecognitionTest()
	// Processing Test data set should be performed in OnFacerecognitionTest2()

	CClientDC pDC(this);
	int i, j;
	string inputDir, inputFileName;
	Mat image, imageMean;

	inputDir = "../DBlab9/train1";  // input directory

	vector<string> fileList;
	GetDirFiles(inputDir.c_str(), &fileList);

	Mat temp1, temp2, P;


	image = imread(inputDir + "/" + fileList[0], 1);
	int ncol = image.cols;
	int nrow = image.rows;
	int ndim = ncol * nrow;
	int nGallery = fileList.size();

	P = Mat(ndim, fileList.size(), CV_32F, 0.0f);

	int* galleryID = new int[fileList.size()];

	for (i = 0; i < fileList.size(); i++) {
		//cout << i << endl;
		inputFileName = inputDir + "/" + fileList[i];

		int id = atoi(fileList[i].substr(0, 2).c_str());
		galleryID[i] = id;

		image = imread(inputFileName, 0);
		if (image.empty()) {
			cout << "Couldn't read " << inputFileName << endl;
			return;
		}
		else {
			// reshape() concatenate a matrix row-wise
			// transpose first to perform column-wise concatenation
			temp1 = image.t();
			temp2 = temp1.reshape(0, ndim);
			temp2.copyTo(P(Rect(i, 0, temp2.cols, temp2.rows))); // Rect(left, top, src.cols, src.rows)
		}
	}
	// check # of rows and cols of P
	//cout << P.rows << " " << P.cols << endl;

	Mat mu_p = Mat(ndim, 1, CV_32F, 0.0f);

	float t1;
	for (i = 0; i < ndim; i++) {
		t1 = 0;
		for (j = 0; j < P.cols; j++) {
			t1 += P.at<float>(i, j);
		}
		mu_p.at<float>(i, 0) = t1 / nGallery;
	}
	//for (i=0; i<10; i++) {
	//	printf("%f\n", mu_p.at<float>(i,0));
	//}

	//// Calculate U, by subtracting mean from input data
	/****** this takes much longer time than that of using ROI like the next block
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);
	Mat U2 = Mat(ndim, P.cols, CV_32F, 0.0f);
	for (i=0; i<ndim; i++) {
		for (j=0; j<P.cols; j++) {
			U2.at<float>(i,j) = P.at<float>(i,j) - mu_p.at<float>(i,0);
		}
	}
	QueryPerformanceCounter(&m_end);
	m_result = 1000*(m_end.QuadPart-m_start.QuadPart)/m_frequency.QuadPart;
	printf("time1 = %f\n", m_result);
	******/

	// Comparing to the above block, using ROI (Rect()) is much faster
	Mat U_p = Mat(ndim, P.cols, CV_32F, 0.0f);
	for (i = 0; i < P.cols; i++) {
		Mat roiP(P, Rect(i, 0, 1, ndim));
		roiP -= mu_p;
		roiP.copyTo(U_p(Rect(i, 0, 1, ndim)));
	}


	//// Calculate covariance matrix
	/*****
		Below method was covered in the lecture. But because of the extended execution time, we will
		use a slight variation as shown in the next block.
	*****/
	//QueryPerformanceFrequency(&m_frequency);
	//QueryPerformanceCounter(&m_start);
	//Mat Cov2 = Mat(ndim, ndim, CV_32F, 0.0f);
	//Cov2 = U_p*U_p.t();
	//QueryPerformanceCounter(&m_end);
	//m_result = 1000*(m_end.QuadPart-m_start.QuadPart)/m_frequency.QuadPart;
	//printf("time cov = %f\n", m_result);


	// Below method was not covered in the class. Let's understand this as a faster PCA with "small sample size"
	// Notice that we use U^t*U, instead of U*U^t, then add additional processing on the eigenvectors
	Mat Cov = Mat(nGallery, nGallery, CV_32F, 0.0f);
	Cov = U_p.t() * U_p;
	Mat E, Vtemp, V;
	// Calculate eigenvalue and eigenvector
	// E holds eigenvalues (ndim x 1); V holds eigenvectors (ndim x ndim)
	eigen(Cov, E, Vtemp);
	V = U_p * Vtemp;  // ndim x nGallery matrix


	FILE* pFile;
	pFile = fopen(filenameDim, "wb");  // write in binary format
	float* buf = new float[2];
	buf[0] = ndim; buf[1] = nGallery;
	fwrite(buf, 1, sizeof(float) * 2, pFile);
	fclose(pFile);

	pFile = fopen(filenameGallery, "wb");  // write in binary format
	float* buf2 = new float[ndim * nGallery];
	for (i = 0; i < ndim; i++)
		for (j = 0; j < nGallery; j++)
			buf2[i * nGallery + j] = U_p.at<float>(i, j);
	fwrite(buf2, 1, sizeof(float) * ndim * nGallery, pFile);
	fclose(pFile);

	pFile = fopen(filenameMean, "wb");  // write in binary format
	float* buf3 = new float[ndim];
	for (i = 0; i < ndim; i++)
		buf3[i] = mu_p.at<float>(i, 0);
	fwrite(buf3, 1, sizeof(float) * ndim, pFile);
	fclose(pFile);

	pFile = fopen(filenameGalleryID, "wb");  // write in binary format
	int* buf4 = new int[nGallery];
	for (i = 0; i < nGallery; i++) {
		buf4[i] = galleryID[i];
		//cout << galleryID[i] << endl;
	}
	fwrite(buf4, 1, sizeof(int) * nGallery, pFile);
	fclose(pFile);

	pFile = fopen(filenameVec, "wb");  // write in binary format
	float* buf5 = new float[ndim * nGallery];
	for (i = 0; i < ndim; i++)
		for (j = 0; j < nGallery; j++)
			buf5[i * nGallery + j] = V.at<float>(i, j);
	fwrite(buf5, 1, sizeof(float) * ndim * nGallery, pFile);
	fclose(pFile);

	delete galleryID;
	delete buf, buf2, buf3, buf4, buf5;

	printf("Training is completed...\n");

	display = 0;

	//AfxMessageBox("hello");
}



void CPCAfaceRecognitionView::OnFacerecognitionTest()
{

	CClientDC pDC(this);
	FILE* pFile;
	size_t sizeByte;
	int i, j, ndim, nGallery;
	string inputDir, inputFileName;
	Mat image;
	Mat temp1, temp2;

	pFile = fopen(filenameDim, "rb");
	float* buf = new float[2]; // read only number of row and column (2 values)
	sizeByte = fread(buf, sizeof(float), 2, pFile);
	fclose(pFile);
	ndim = (int)buf[0];
	nGallery = (int)buf[1];

	printf("ndim = %d, nGallery = %d\n", ndim, nGallery);

	Mat U_p = Mat(ndim, nGallery, CV_32F, 0.0f);
	Mat mu_p = Mat(ndim, 1, CV_32F, 0.0f);
	int* galleryID = new int[nGallery];
	Mat V = Mat(ndim, nGallery, CV_32F, 0.0f);

	pFile = fopen(filenameGallery, "rb");
	float* buf2 = new float[ndim * nGallery]; // mu_p is ndim*nGallery matrix
	sizeByte = fread(buf2, sizeof(float), ndim * nGallery, pFile);
	fclose(pFile);
	for (i = 0; i < ndim; i++)
		for (j = 0; j < nGallery; j++)
			U_p.at<float>(i, j) = buf2[i * nGallery + j];

	pFile = fopen(filenameMean, "rb");
	float* buf3 = new float[ndim]; // mu_p is ndim*nGallery matrix
	sizeByte = fread(buf3, sizeof(float), ndim, pFile);
	fclose(pFile);
	for (i = 0; i < ndim; i++)
		mu_p.at<float>(i, 0) = buf3[i];

	pFile = fopen(filenameGalleryID, "rb");
	sizeByte = fread(galleryID, sizeof(int), nGallery, pFile);
	fclose(pFile);
	//for (i=0; i<nGallery; i++)
	//	cout << galleryID[i] << endl;

	pFile = fopen(filenameVec, "rb");
	float* buf4 = new float[ndim * nGallery]; // mu_p is ndim*nGallery matrix
	sizeByte = fread(buf4, sizeof(float), ndim * nGallery, pFile);
	fclose(pFile);
	for (i = 0; i < ndim; i++)
		for (j = 0; j < nGallery; j++)
			V.at<float>(i, j) = buf4[i * nGallery + j];


	/*****
		We reduce the dimension of feature vecture by reducing number of eigenvectors.
		First few eigenvectors can also be removed considering that they contain noisy information.
	*****/
	Mat V_reduced(V, Rect(ndim_start, 0, ndim_reduced - ndim_start, ndim));


	////// check eigenvalues and eigenvectors
	//printf("E: \n");
	//for (i=0; i<10; i++) {
	//	printf("%f ", E.at<float>(i,0));
	//}
	//printf("\n");


	Mat Proj_p = V_reduced.t() * U_p;


	inputDir = "../DBlab9/validate1";
	vector<string> validateList;
	GetDirFiles(inputDir.c_str(), &validateList);
	int nValidate = validateList.size();

	Mat Q = Mat(ndim, validateList.size(), CV_32F, 0.0f);

	for (i = 0; i < validateList.size(); i++) {
		inputFileName = inputDir + "/" + validateList[i];

		image = imread(inputFileName, 0);
		if (image.empty()) {
			cout << "Couldn't read " << inputFileName << endl;
			return;
		}
		else {
			// reshape() concatenate a matrix row-wise
			// transpose first to perform column-wise concatenation
			temp1 = image.t();
			temp2 = temp1.reshape(0, ndim);

			temp2.copyTo(Q(Rect(i, 0, temp2.cols, temp2.rows)));
		}
	}

	Mat U_q = Mat(ndim, Q.cols, CV_32F, 0.0f);
	for (i = 0; i < Q.cols; i++) {
		Mat roiQ(Q, Rect(i, 0, 1, ndim));
		roiQ -= mu_p;
		roiQ.copyTo(U_q(Rect(i, 0, 1, ndim)));
	}

	Mat Proj_q = V_reduced.t() * U_q;


	// Calculate distance from each validate image to gallery image
	int k;
	float sum;
	Mat dist(Proj_q.cols, Proj_p.cols, CV_32F, 0.0f);
	for (i = 0; i < Proj_q.cols; i++) {
		///////////////////////////////////////////////////////////////////////
		// TODO 1: Calculate distance from the probe image to gallery images//
		// This method will not run until this part is completed            //
		///////////////////////////////////////////////////////////////////////

		for (j = 0; j < Proj_p.cols; j++) {
			sum = 0;
			for (k = 0; k < Proj_q.rows; k++) {
				sum += pow(Proj_q.at<float>(k, i) - Proj_p.at<float>(k, j), 2);
			}
			dist.at<float>(i, j) = sqrt(sum);
		}
	}

	float nCorrect;
	float acc;
	double v_min, v_max;
	int idx_min[2] = { 255,255 }, idx_max[2] = { 255, 255 };
	nCorrect = 0;
	for (i = 0; i < Proj_q.cols; i++) {
		Mat t4(dist, Rect(0, i, dist.cols, 1));
		cv::minMaxIdx(t4, &v_min, &v_max, idx_min, idx_max);
		//cout << i+1 << ":" << galleryID[idx_min[1]] << " " << endl;

		if (galleryID[idx_min[1]] == i + 1)
			nCorrect++;
	}
	acc = nCorrect / nValidate * 100;
	printf("acc = %3.1f (%%)\n", acc);

	m_cs.Format("Accuracy: %3.1f (%%)", acc); // print filtering time
	pDC.TextOut(0, 10, m_cs);


	delete galleryID;
	delete buf, buf2, buf3, buf4;

	display = 0;

}

void CPCAfaceRecognitionView::OnTest2()
{
	// read training data and images in DBlab9/test1 and perform face recognition

	display = 0;
}

void CPCAfaceRecognitionView::OnFileOpen()
{

	// Clear ATL images before loading
	m_probe.Destroy();
	for (int i = 0; i < 10; i++)
		m_match[i].Destroy();


	POSITION pos;
	string fileName, tmp;
	int nFiles;
	size_t found;
	FILE* pFile;
	size_t sizeByte;
	int i, j, ndim, nGallery;
	string inputDir, inputFileName;
	Mat image;
	Mat temp1, temp2;

	// Take an input image
	CFileDialog dlg1(TRUE, ".jpg", NULL, OFN_ALLOWMULTISELECT, "*.*||");
	dlg1.m_ofn.nFilterIndex = m_nFilterLoad;
	if (dlg1.DoModal() == IDOK) {
		m_nFilterLoad = dlg1.m_ofn.nFilterIndex;
		pos = dlg1.GetStartPosition();
	}
	else
		return;
	m_nFilterLoad = dlg1.m_ofn.nFilterIndex;
	nFiles = 0;
	while (pos) {
		fileName = dlg1.GetNextPathName(pos);
		nFiles++;
	}
	while (nFiles < 1) {
		if (dlg1.DoModal() == IDOK) {
			pos = dlg1.GetStartPosition();
		}
		else
			return;
		while (pos) {
			fileName = dlg1.GetNextPathName(pos);
			nFiles++;
		}
	}

	printf("%s\n", fileName.c_str());
	int p = fileName.find_last_of("\\");
	m_probeID = atoi(fileName.substr(p + 1, 2).c_str());
	//cout << m_probeID << endl;

	m_probe.Load(fileName.c_str());



	// read trained data
	pFile = fopen(filenameDim, "rb");
	float* buf = new float[2]; // read only number of row and column (2 values)
	sizeByte = fread(buf, sizeof(float), 2, pFile);
	fclose(pFile);
	ndim = (int)buf[0];
	nGallery = (int)buf[1];

	printf("ndim = %d, nGallery = %d\n", ndim, nGallery);

	Mat U_p = Mat(ndim, nGallery, CV_32F, 0.0f);
	Mat mu_p = Mat(ndim, 1, CV_32F, 0.0f);
	int* galleryID = new int[nGallery];
	Mat V = Mat(ndim, nGallery, CV_32F, 0.0f);

	pFile = fopen(filenameGallery, "rb");
	float* buf2 = new float[ndim * nGallery]; // mu_p is ndim*nGallery matrix
	sizeByte = fread(buf2, sizeof(float), ndim * nGallery, pFile);
	fclose(pFile);
	for (i = 0; i < ndim; i++)
		for (j = 0; j < nGallery; j++)
			U_p.at<float>(i, j) = buf2[i * nGallery + j];

	pFile = fopen(filenameMean, "rb");
	float* buf3 = new float[ndim]; // mu_p is ndim*nGallery matrix
	sizeByte = fread(buf3, sizeof(float), ndim, pFile);
	fclose(pFile);
	for (i = 0; i < ndim; i++)
		mu_p.at<float>(i, 0) = buf3[i];

	pFile = fopen(filenameGalleryID, "rb");
	sizeByte = fread(galleryID, sizeof(int), nGallery, pFile);
	fclose(pFile);
	//for (i=0; i<nGallery; i++)
	//	cout << galleryID[i] << endl;

	pFile = fopen(filenameVec, "rb");
	float* buf4 = new float[ndim * nGallery]; // mu_p is ndim*nGallery matrix
	sizeByte = fread(buf4, sizeof(float), ndim * nGallery, pFile);
	fclose(pFile);
	for (i = 0; i < ndim; i++)
		for (j = 0; j < nGallery; j++)
			V.at<float>(i, j) = buf4[i * nGallery + j];


	/*****
		We reduce the dimension of feature vecture by reducing number of eigenvectors.
		First few eigenvectors can also be removed considering that they contain noisy information.
	*****/
	Mat V_reduced(V, Rect(ndim_start, 0, ndim_reduced - ndim_start, ndim));

	Mat Proj_p = V_reduced.t() * U_p;


	Mat Q = Mat(ndim, 1, CV_32F, 0.0f);



	image = imread(fileName, 0);
	if (image.empty()) {
		cout << "Couldn't read " << inputFileName << endl;
		return;
	}
	else {
		// reshape() concatenate a matrix row-wise
		// transpose first to perform column-wise concatenation
		temp1 = image.t();
		temp2 = temp1.reshape(0, ndim);

		temp2.copyTo(Q(Rect(0, 0, temp2.cols, temp2.rows)));
	}


	Mat U_q = Mat(ndim, Q.cols, CV_32F, 0.0f);
	for (i = 0; i < Q.cols; i++) {
		Mat roiQ(Q, Rect(i, 0, 1, ndim));
		roiQ -= mu_p;
		roiQ.copyTo(U_q(Rect(i, 0, 1, ndim)));
	}

	Mat Proj_q = V_reduced.t() * U_q;


	// Calculate distance from the probe image to gallery images
	int k;
	float sum;
	Mat dist(Proj_q.cols, Proj_p.cols, CV_32F, 0.0f);
	for (i = 0; i < Proj_q.cols; i++) {
		///////////////////////////////////////////////////////////////////////
		// TODO 2: Calculate distance from the probe image to gallery images//
		// This method will not run until this part is completed            //
		///////////////////////////////////////////////////////////////////////
		for (j = 0; j < Proj_p.cols; j++) {
			sum = 0;
			for (k = 0; k < Proj_q.rows; k++) {
				sum += pow(Proj_q.at<float>(k, i) - Proj_p.at<float>(k, j), 2);
			}
			dist.at<float>(i, j) = sqrt(sum);
		}
	}


	Mat m(1, nGallery, CV_32F, 0.0f);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// TODO 3: obtain the sorted index of matching result  // m contains indices of sorted values//
	// This method will not run until this part is completed                                     //
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Use "cv::sortIdx" opencv2.4.1.3 code reference link //
	// https://cppsecrets.com/users/20251211111031011151049910497110100114971079711011610455565764103109971051084699111109/C00-OpenCVsortIdx.php

	cv::sortIdx(dist, m, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);

	// load top 10 candidate images
	inputDir = "../DBlab9/train1";
	vector<string> trainList;
	GetDirFiles(inputDir.c_str(), &trainList);
	for (i = 0; i < 10; i++) {
		inputFileName = inputDir + "/" + trainList[m.at<int>(0, i)];
		m_match[i].Load(inputFileName.c_str());
		//cout << "load " << inputFileName << endl;

		m_matchID[i] = atoi(trainList[m.at<int>(0, i)].substr(0, 2).c_str());
	}

	delete galleryID;
	delete buf, buf2, buf3, buf4;

	display = 1;

	Invalidate(); // refresh window

}

int GetDirFiles(const char* path_name, vector<string>* fileList)
{
	WIN32_FIND_DATA FN;
	HANDLE hFind;
	char search_arg[MAX_PATH], new_file_path[MAX_PATH];
	string cs;
	sprintf(search_arg, "%s\\*.*", path_name);
	int i;

	hFind = FindFirstFile((LPCTSTR)search_arg, &FN);
	//cout << (hFind != INVALID_HANDLE_VALUE) << endl << FN.cFileName << endl;
	if (hFind != INVALID_HANDLE_VALUE) {
		i = 0;
		do {
			cs = FN.cFileName;
			if (cs != "." && cs != "..") {
				fileList->push_back(cs);
				//cout << i << "  " << cs << "  " << fileList->at(i) << endl;
				i++;
			}
		} while (FindNextFile(hFind, &FN) != 0);
		if (GetLastError() == ERROR_NO_MORE_FILES)
			FindClose(hFind);
	}

	return i;
}
