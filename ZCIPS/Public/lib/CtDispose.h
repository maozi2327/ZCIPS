#ifndef __CTDISPOSE_H
#define __CTDISPOSE_H

#ifdef CTDISPOSE_EXPORTS
#define CTDISPOSE_API __declspec(dllexport)
#else
#define CTDISPOSE_API __declspec(dllimport)
#endif

//**************************************************************************************
//		该动态库适用于V2.1版本数据
//		__Designed by Liurong
//		Data: 2007.10.9
//**************************************************************************************

//**************************************************************************************
//定义16位校正操作使能字
#define	EN_BKGTUNE				0x0001		//允许背景校正
#define	EN_AIRTUNE				0x0002		//允许空气校正
#define	EN_SENSITIVE			0x0004		//允许灵敏度校正
#define	EN_RANGE				0x0008		//允许动态范围校正
#define	EN_CROSSTALK			0x0010		//允许串扰校正
#define	EN_PULSEDOSE			0x0020		//允许单脉冲剂量校正
#define	EN_HARDEN				0x0040		//允许硬化校正
#define	EN_ABNORMALDETECTOR 	0x0080		//允许处理异常探测器
#define EN_MOVETWOPIXELS        0x0100      //允许使用多扫的CT分度来进行一致性校正
#define	EN_SAVEEXTORG			0x8000		//允许保存处理后的原始数据
 

//**************************************************************************************
//定义函数返回代码
#define	SUCCESS					0			//成功
#define	ERR_NO_FILE				1			//文件不存在
#define	ERR_IN_FILEHEAD	    	2			//文件头错误(尺寸不匹配)
#define	ERR_NO_DATA				3			//文件无数据
#define	ERR_DATA				4			//数据错误
#define	ERR_UNFINISH			5			//数据处理失败(未完成)
#define	ERR_OPEN_FILE			6			//打开新文件错误
#define	ERR_WRITE_FILE			7			//写文件错误
#define	ERR_DATA_SORT			8			//排序错(数据不足)
#define	ERR_IN_MEMORY			9			//内存分配错误

#define	ERR_OPEN_BKG_FILE		10			//本底数据文件不存在
#define	ERR_BKG_DATA			11			//本底数据文件错误
#define	ERR_OPEN_AIR_FILE		12			//空气系数文件不存在
#define	ERR_AIR_DATA			13			//空气系数文件错误
#define	ERR_SCAN_MODE			14			//无效扫描方式
#define	ERR_INTERP_TUNE_fI	    15		   	//插值校正失败,第一个插值点无数据
#define	ERR_INTERP_TUNE_lI	    16		    //插值校正失败,最后一个插值点无数据
#define	ERR_INTERP_TUNE_0AVE	17			//插值校正失败,探测器数据均值出现0
#define	ERR_IN_FILEFORMAT		18			//文件版本或格式错误
#define	ERR_IN_COLLIMATE		19			//准直孔代号错误

#define	ERR_IN_RANGE_TUNE		20			//动态范围校正失败(扫描方式不匹配，未校正)
#define	ERR_IN_BIAS				21			//动态范围校正失败(校正探测器数据方差太大或均值太小)
#define	ERR_IN_MEAN				22			//剂量校正失败(校正探测器数据均值太小)
#define	ERR_UPDATE_BKG			23			//本底数据错误,未更新
#define	ERR_OPEN_HD_FILE		24			//硬化校正系数文件不存在
#define	ERR_HD_DATA				25			//硬化校正系数数据文件错误
#define	ERR_IN_DATAFORMAT		26			//数据格式错误
#define	ERR_IN_FdNORMAL	    	27			//分度规范化

#define	ERR_IN_SCANMODE	    	41			//不支持该扫描方式(旋转中心确定)
#define	ERR_IN_SCANRANGE	    42			//不支持该扫描范围(旋转中心确定)
#define ERR_IN_MOVETWOPIXELS    43          //数据不全不能进行新的一致性校正
/*
CtEncrypt.h定义的错误代码
#define	SUCCESS					0			//成功
#define	ERR_OPEN_FILE			6			//打开新文件错误
#define	ERR_INPUT_PARAMETER	    30			//函数输入参数错误
#define	ERR_INVALID_FILENAME	31			//无效文件名
#define	ERR_INVALID_VERSION	    32			//文件头版本无效
#define	ERR_DATA_IN_FILE		33			//文件数据错误
#define	ERR_NOCALL_FSEQUENCE	34			//未按顺序调用函数
#define	ERR_INVALID_LIST		35			//无效链表指针
#define	ERR_INVALID_MEMORY	    36			//申请内存失败
#define	ERR_NOENOUGH_DATA		37			//数据不足
*/
const char dSzResult[][128] =
{
	"成功",									// 0
	"文件不存在",
	"文件头错误(尺寸不匹配)",
	"文件无数据",
	"数据错误",
	"数据处理失败(未完成)",
	"打开新文件错误",
	"写文件错误",
	"排序错(数据不足)",
	"内存分配错误",

	"本底数据文件不存在",									// 10
	"本底数据文件错误",
	"空气系数文件不存在",
	"空气系数文件错误",
	"无效扫描方式",
	"插值校正失败,第一个插值点无数据",
	"插值校正失败,最后一个插值点无数据",
	"插值校正失败,探测器数据均值出现0",
	"文件格式错误",
	"准直孔代号错误",

	"动态范围校正失败(扫描方式不匹配，未校正)",				// 20
	"动态范围校正失败(校正探测器数据方差太大或均值太小)",
	"剂量校正失败(校正探测器数据均值太小)",
	"本底数据错误,未更新",
	"硬化校正系数文件不存在",
	"硬化校正系数数据文件错误",								// 26
	"分度规范化",											// 27
	"",
	"",
	"",

	"函数输入参数错误",										// 30
	"无效文件名",											// 32
	"文件头版本无效",										// 33  
	"文件数据错误",											// 34
	"未按顺序调用函数",										// 35
	"无效链表指针"											// 36
	"申请内存失败",
	"数据不足",
	"",
	"",

	"",														// 40
	"不支持该扫描方式",										// 41
	"不支持该扫描范围",										// 42
	"分度出现错误",                                         // 43
	"函数入参错误"                                          // 44
};





//**************************************************************************************
//功能：	数据校正处理
//入口：
//			par					---	参数文件路径
//			head				---	文件头结构指针(未加密)
//			firstItem			---	数据链表首指针(未加密)
//			destinationFile		---	处理后数据全路径文件名(.pcd or .pdd or .mcd)
//			destinationFile2	---	处理后II代合成数据全路径文件名(.pcd)
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//说明：	.pcd				---	校正后的CT数据文件(已加密)
//			.pdd				---	校正后的DR数据文件(已加密)
//			.mcd				---	校正后的2代CT数据中间文件(已加密)
//			原文件头和链表内容无变化
//*****************************************************************************************
extern "C" CTDISPOSE_API int dataDisposeA(char *par, void *head, void **firstItem, char *destinationFile, char *destinationFile2);


//**************************************************************************************
//功能：数据校正处理
//入口：
//			par					---	参数文件路径
//			head				---	文件头结构指针
//			firstItem			---	数据链表首指针
//			destinationFile		---	处理后数据全路径文件名(.pcd or .pdd)
//			destinationFile2	---	处理后II代合成数据全路径文件名(.pcd)
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//*****************************************************************************************
extern "C" CTDISPOSE_API int dataDispose(char *par, void *head, void *firstItem, char *destinationFile, char *destinationFile2);

//*****************************************************************************************
//功能：	生成背景校正数据
//入口：
//			par					---	参数文件路径
//			head				---	文件头结构指针
//			firstItem			---	数据链表首指针
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//*****************************************************************************************
extern "C" CTDISPOSE_API int createBkgData(char *par, void *head, void *firstItem);

//*****************************************************************************************
//功能：	生成空气校正数据
//入口：
//			par					---	参数文件路径
//			head				---	文件头结构指针
//			firstItem			---	数据链表首指针
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//*****************************************************************************************
extern "C" CTDISPOSE_API int createAirRatio(char *par, void *head, void *firstItem);

//**************************************************************************************
//					以下函数用于对原始数据文件进行处理
//**************************************************************************************

//*****************************************************************************************
//功能：数据校正处理
//入口：
//			par					---	参数文件路径
//			src					---	原始文件全路径文件名
//			des					---	目标文件路径
//			operate				--- 操作使能字
//          EnergyDNum          --- 剂量校正用探测器数(此参数无效, 从配置文件CtDispose.ini中读入)
//			CrossAlf1			--- 串扰系数1(此参数无效, 从配置文件CtDispose.ini中读入)
//			CrossAlf2			--- 串扰系数2(此参数无效, 从配置文件CtDispose.ini中读入)
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//说明：	该函数处理已加密的V2.1数据文件
//*****************************************************************************************
extern "C" CTDISPOSE_API int fDataDispose(char *par, char *src, char *des, WORD operate,WORD EnergyDNum,float CrossAlf1,float CrossAlf2);

//*****************************************************************************************
//功能：数据校正处理
//入口：
//			par					---	参数文件路径
//			src					---	原始文件全路径文件名
//			des					---	目标文件路径
//			desFileName			--- 目标文件全路径文件名(用于返回处理后的目标文件)
//			operate				--- 操作使能字
//          EnergyDNum          --- 剂量校正用探测器数(此参数无效, 从配置文件CtDispose.ini中读入)
//			CrossAlf1			--- 串扰系数1(此参数无效, 从配置文件中CtDispose.ini读入)
//			CrossAlf2			--- 串扰系数2(此参数无效, 从配置文件中CtDispose.ini读入)
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//说明：	该函数处理已加密的V2.1数据文件
//*****************************************************************************************
extern "C" CTDISPOSE_API int  _fDataDispose(char *par, char *src, char *des, char *desFileName, WORD operate,WORD EnergyDNum,float CrossAlf1,float CrossAlf2);

//*****************************************************************************************
//功能：	删除奇异数据并合并相同采样序号数据，处理后数据保存到文件中
//入口：
//			par					---	参数文件路径
//			src					---	原始文件全路径文件名
//			des					---	目标文件全路径文件名
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//说明：	该函数处理已加密的V2.1以上数据文件
//*****************************************************************************************
extern "C" CTDISPOSE_API int fDelOdd_Comb(char *par, char *src, char *des);

//*****************************************************************************************
//功能：	生成背景校正数据
//入口：
//			par					---	参数文件路径(如：d:\\sys\\para)
//			src					---	原始文件全路径文件名
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//说明：	该函数处理已加密的V2.1数据文件
//*****************************************************************************************
extern "C" CTDISPOSE_API int fCreateBkgData(char *par, char *src);

//*****************************************************************************************
//功能：	生成空气校正数据
//入口：
//			par					---	参数文件路径(如：d:\\sys\\para)
//			src					---	原始文件全路径文件名
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//说明：	该函数处理已加密的V2.1数据文件
//*****************************************************************************************
extern "C" CTDISPOSE_API int fCreateAirRatio(char *par, char *src);

//*****************************************************************************************
//功能：	将文件头和链表中数据加密后保存到文件中
//入口：	
//			des					---	全路径目的文件名
//			h					---	文件头指针(未加密)
//			firstItem			---	链表首指针(未加密)
//			pw					---	加密开关(为1时加密, 为0时不加密)
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//说明：	1. 调用该函数后原文件头中参数无变化.
//			2. 如果pw为1, 保存的是加密后的数据, 原链表中的数据已经加密.
//			3. 如果pw为0, 保存的数据未加密.
//*****************************************************************************************
extern "C" CTDISPOSE_API int saveOrgHeaderAndList(char *des, void *h, void *firstItem, int pw);

//*****************************************************************************************
//功能：	返回数据处理程序版本号
//入口：	无
//出口：	主版本号在main中，次版本号在sub中
//*****************************************************************************************
extern "C" CTDISPOSE_API void dVersion(int *main, int *sub);

//*****************************************************************************************
//功能：对缓冲区中的数据进行校正处理(2007.12.27增加)
//入口：
//			par					---	参数文件路径
//			head				---	文件头结构指针(未加密)
//			buf					---	缓冲区指针(未加密)
//			destinationFile		---	处理后数据全路径文件名(.pcd or .pdd or .mcd)
//			destinationFile2	---	处理后II代合成数据全路径文件名(.pcd)
//			operate				---	操作使能字
//出口：
//			0					---	处理成功	
//			其他				---	错误代码
//*****************************************************************************************
extern "C" CTDISPOSE_API int bufDataDispose(char *par, void *head, void *buf, char *destinationFile, char *destinationFile2, WORD operate);



//测试用*************************************************************
//*****************************************************************************************
//功能：
//			1. 数组排序
//			2. 数据合并
//入口：	待排序数据在数组DataArray[]中, 数组长度为Size,允许数据偏离中值的程度delta
//出口：	合并后的数据
//*****************************************************************************************
extern "C" CTDISPOSE_API DWORD SelectionSortAndCombine(DWORD *DataArray,int Size,float delta);

 /****************************************
* 功能：旋转中心确定实现函数 (2010-07-25)
* 参考文献：Stephen G.AZEVEDO,Calculation of the rotational centers in computed tomography sinograms  
*           IEEE Transaction on NUclear science ,Vol37.NO.4.August,1990                     
* 参数：float *ProjArray   ---投影数组
*       int NumProj        ---投影分度数 
*       int NumRays        ---射线数/每投影
*       float ValofFOV     ---扫描视场
*       float RayAngle     ---射线间夹角
*		int ScanRange      ---扫描范围0：360；1：180；2：其他
* 输入：投影数据
*
* 输出：旋转中心偏移量  
* 说明：该方法对噪声较为敏感，暂时不采用，待加强噪声处理后使用。                          
*****************************************/
 extern "C" CTDISPOSE_API float FindCOR(float *ProjArray, DWORD NumofRays,DWORD NumofViews,float ValofFOV,float RayAngle,float ScanRange);

/****************************************
* 功能：旋转中心确定   (2010-07-25增加) 
*                      
//入口：
//			SrcFile		        --	待处理数据链表指针
//
//出口：
//			旋转中心偏移量 		
//说明：给予边缘定位中心线位置，精度仅可在0.5像素内。					                         
*****************************************/
 extern "C" CTDISPOSE_API float fDetermingCOR(char *SrcFile);

/****************************************
* 功能：探测器最佳分层位自动定位 (2010-09-08)
*
* 算法：移动窗口滤波，差分求一阶到零点，插值确定最佳分层位                     
*
* 参数：float *ProjArray       ---分层数组（不同分层位合并后数）
*       float *LayerPos        ---分层位置
*       float Layerincrement   ---采样层间距  
*       DWORD NumofLayers      ---分层个数 
* 
* 输出：最佳分层位置
*  
* 说明：与非线性最小二乘拟合法相比，该方法算法简单，易实现，速度快，但确定精度低，且与分层间距大小相关。                          
*****************************************/
 extern "C" CTDISPOSE_API float DetermingLayerPos(float *ProjArray, float *LayerPos,  float Layerincrement,DWORD NumofLayers );


#endif
