#include "common.h"
#include "refiner.h"


//输出结果： initial_matches.png; initial_points_view1.png; initial_points_view5E2.png
//outlier_matches.png; refine_matches.png; refine_points_view1.png; refine_points_view5E2.png
//outlier_matches.txt; refine_matches.txt;
int main(int argc, char * argv[])
{
	if(argc != 6)
	{
		cout << "Usage: MatchesRefine.exe folder refIm srcIm mFn" << endl;
		return -1;
	}

	string folder = string(argv[1]);
	string scene = string(argv[2]);
	string refFn = string(argv[3]);
	string srcFn = string(argv[4]);
	string mFn = string(argv[5]);
	folder = folder + "/" + scene;

	string refImFn = folder + "/" + refFn + ".png";
	string srcImFn = folder + "/" + srcFn + ".png";
	string matchFn = folder + "/" + mFn;

	cout << refImFn << endl;
	cout << srcImFn << endl;
	cout << matchFn << endl;

	MatchesRefiner Refiner;
	
	Refiner.Initial(refImFn, srcImFn, matchFn);
	cout << "initial done." << endl;
	
	string ssfn = folder + "/initial_m_sift_" + srcFn + ".png";
	string rsfn = folder + "/initial_m_sift_" + refFn + ".png";
	string msfn = folder + "/initial_matches.png"; 
	Refiner.showKeypoints(INITIAL_REF_PT, rsfn);
	cout << rsfn << endl;
	Refiner.showKeypoints(INITIAL_SRC_PT, ssfn);
	cout << ssfn << endl;
	Refiner.showMatches(INITIAL_MATCH, msfn);
	cout << msfn << endl;
		
	//1. 优化不符合epipolar geometry的match
	Refiner.Refine();

	////Refiner.Remove();
	////2. 利用视差结果对匹配做进一步处理
	//string dfn = folder + "/disp1.png";
	//Mat disp = imread(dfn, CV_LOAD_IMAGE_GRAYSCALE);
	//if(disp.data == nullptr)
	//{
	//	cout << "failed to open " << disp << endl;
	//	return 1;
	//}
	//Refiner.Postprocess(disp);
	//Refiner.Remove();

	
	string refSFn = folder + "/refine_m_sift_" + refFn + ".png";
	string srcSFn = folder + "/refine_m_sift_" + srcFn + ".png";
	Refiner.showKeypoints(REFINE_REF_PT, refSFn);
	Refiner.showKeypoints(REFINE_SRC_PT, srcSFn);

	string matchSFn = folder + "/refine_matches.png"; 
	Refiner.showMatches(REFINE_MATCH, matchSFn);
	matchSFn = folder + "/outlier_matches.png";
	Refiner.showMatches(OUTLIER_MATCH, matchSFn);

	//matchSFn = folder + "/refine_" + mFn;
	matchSFn = folder + "/refine_" + mFn;
	Refiner.saveRefineResults(matchSFn);
	matchSFn = folder + "/outlier_" + mFn;
	Refiner.saveOutlierResults(matchSFn);

	return 1;
}