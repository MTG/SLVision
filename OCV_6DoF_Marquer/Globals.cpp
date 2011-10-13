#include "Globals.h"
#include "GlobalConfig.h"

IplImage*	Globals::screen						= NULL;
bool		Globals::is_view_enabled			= false;

float		Globals::Font::hscale				= 0.5f;
float		Globals::Font::vscale				= 0.5f;
float		Globals::Font::italicscale			= 0.0f;
int 		Globals::Font::thickness			= 1;
float		Globals::Font::axis_hscale			= 0.8f;
float		Globals::Font::axis_vscale			= 0.8f;
CvFont		Globals::Font::font_info;
CvFont		Globals::Font::font_axis;

int			Globals::width						= 0;
int			Globals::height						= 0;
char		Globals::dim[100];

CvMat* 		Globals::intrinsic					= NULL;
CvMat*		Globals::distortion					= NULL;

void Globals::LoadDefaultDistortionMatrix()
{
	if(intrinsic != NULL)
	{
		cvReleaseMat(&intrinsic);
	}
	intrinsic  = cvCreateMat(3,3,CV_32FC1);
	CV_MAT_ELEM( *intrinsic, float, 0, 0) = 6.57591187e+002f;	CV_MAT_ELEM( *intrinsic, float, 0, 1) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 0, 2) = 3.16504272e+002f;
	CV_MAT_ELEM( *intrinsic, float, 1, 0) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 1, 1) = 6.60952637e+002f;	CV_MAT_ELEM( *intrinsic, float, 1, 2) = 2.27605789e+002f;
	CV_MAT_ELEM( *intrinsic, float, 2, 0) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 2, 1) = 0.f;				CV_MAT_ELEM( *intrinsic, float, 2, 2) = 1.f;

	if(distortion != NULL)
	{
		cvReleaseMat(&distortion);
	}
	distortion = cvCreateMat(4,1,CV_32FC1);
	CV_MAT_ELEM( *distortion, float, 0, 0) = -1.49060376e-002f; 
	CV_MAT_ELEM( *distortion, float, 1, 0) = 2.05916256e-001f; 
	CV_MAT_ELEM( *distortion, float, 2, 0) = -5.76808210e-003f;
	CV_MAT_ELEM( *distortion, float, 3, 0) = -8.43471102e-003f; 

	cvSave(M_PATH_INTRINSIC,intrinsic);
	cvSave(M_PATH_DISTORTION,distortion);
}

void Globals::Font::InitFont()
{

	cvInitFont (&font_info, CV_FONT_HERSHEY_SIMPLEX , hscale, vscale, italicscale, thickness, CV_AA);
	cvInitFont (&font_axis, CV_FONT_HERSHEY_SIMPLEX , axis_hscale, axis_vscale, italicscale, thickness, CV_AA);
}

void Globals::Font::Write(IplImage* dest,const char* text, const CvPoint &position, int font_type, short r, short g, short b)
{
	if(dest != NULL && text != NULL)
	{
		switch(font_type)
		{
			case FONT_AXIS:
				cvPutText(dest, text, position, &font_axis,CV_RGB(r,g,b));
			break;
			default:
			case FONT_HELP:
				cvPutText(dest, text, position, &font_info,CV_RGB(r,g,b));
			break;
		}
	}
}