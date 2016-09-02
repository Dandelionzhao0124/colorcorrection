#include "transfer.h"
#include "correction.h"

int main(int argc, char* argv[])
{
	//H:\RQing_code\data_cc\Art Art view1 view5E2
	if(argc != 5)
	{
		//文件夹路径， 测试数据名称， 参考图像名称， 原图像名称
		//../../data_cc Art view1 view5E2
		cout << "Usage: RegionWisedTransfer.exe folder sceneFn refFn srcFn" << endl; 
		exit(-1);
	}

	string folder  = string(argv[1]);//路径
	string sceneFn = string(argv[2]);//图像名称
	string refFn   = string(argv[3]);//参考图名称
	string srcFn   = string(argv[4]);//目标图名称
	folder = folder + "/" + sceneFn;

	string refrFn = refFn+"-r";//参考图reflectance名称
	string refsFn = refFn+"-s";//参考图shading名称
	string srcrFn = srcFn+"-r";//目标图reflectance名称
	string srcsFn = srcFn+"-s";//目标图shading名称

	RWTransfer  transfer;

	transfer.initial(folder, sceneFn, refFn, srcFn);//初始化图像、视差、可见性、分割结果、匹配结果、区域平均颜色

	transfer.FirstSrcCombine(folder, srcFn);//若某一个ref区域对应了多个src区域，且对应点个数大于1，则要将它们融合，使得每个ref区域都有一个确定的src区域

   	transfer.FuseSrcNopoint(folder, srcFn);//如果src的某区域没有匹配点，则找二阶邻域内跟它颜色和距离最相近的区域融合

	transfer.Matches(folder, srcFn, refFn);//src对应ref,如果存在一个src区域对应多个ref区域，则融合

	transfer.showSameColor(folder, "b");

	transfer.FuseRef(folder, refFn);

	transfer.showSameColor(folder, "a");

	vector<vector<bool>> RAlist;
	transfer.getrefRAList(RAlist);

	CCorrecter correcter;

	correcter.initial(folder, srcsFn, refsFn, srcrFn, refrFn, transfer, RAlist);
	/*correcter.Showlayers(correcter.refrIm, "r", folder, "refrIm");
	correcter.Showlayers(correcter.refsIm, "s", folder, "refsIm");
	correcter.Showlayers(correcter.srcrIm, "r", folder, "srcrIm");
	correcter.Showlayers(correcter.srcsIm, "s", folder, "srcsIm");*/

	string mode;
	mode = "i";
	correcter.Correction(correcter.srcRgbIm, correcter.refRgbIm, sceneFn, folder, srcFn, mode);
	//mode = "r";
	//correcter.Correction(correcter.srcrIm, correcter.refrIm, sceneFn, folder, srcFn, mode);
	//mode = "s";
	//correcter.Correction(correcter.srcsIm, correcter.refsIm, sceneFn, folder, srcFn, mode);

	//string srcSimResultFn = "src_sim_IN";
	//string srcWeightResultFn = "src_weight_IN";

	//
	////correcter.Combine(correcter.reflect_sim_corr, correcter.shading_sim_corr, srcSimResultFn, folder);
	//correcter.Combine(correcter.reflect_weight_corr, correcter.shading_weight_corr, srcWeightResultFn, folder);

	//mode = "l";
	//cout << "l" <<endl;
	//correcter.Correction(correcter.srcinresult, correcter.refRgbIm, sceneFn, folder, srcFn, mode);
	//imwrite(folder + "/last_sim_IN.png", correcter.last_in_sim_corr);
	//imwrite(folder + "/" + srcFn+ "_IN.png", correcter.last_in_weight_corr);
	imwrite(folder + "/src_sim_GE.png", correcter.init_sim_corr);
	imwrite(folder + "/" + srcFn + "_IRW.png", correcter.init_weight_corr);

	return 1;
	
}