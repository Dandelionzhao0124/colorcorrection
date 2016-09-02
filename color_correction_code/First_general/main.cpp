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

	correcter.initial(folder, transfer, RAlist);

	correcter.Correction(correcter.srcRgbIm, correcter.refRgbIm, sceneFn, folder, srcFn);

	imwrite(folder + "/" + srcFn + "_First_sim.png", correcter.init_sim_corr);
	imwrite(folder + "/" + srcFn + "_First_weight.png", correcter.init_weight_corr);
	cout << "correction done" << endl;

	return 1;
	
}