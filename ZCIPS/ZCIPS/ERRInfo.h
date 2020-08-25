#pragma once

#define SUCCESS    0    // 成功
#define ERR_INPUT  1    // 入参问题
#define ERR_FUNC   2    // 内部函数错误
#define ERR_INER_POINTER    3       // 内部指针错误
#define ERR_INER_PARA       4       // 内部参数错误

#define ERR_NO_CURRIMG_INS  5		// 当前图像实例不存在
#define ERR_NO_DISPLAY      6		// 无图像显示
#define ERR_NOMATCH_INS_DISPLAY 7   // 图像实例的标识与显示图像的标识不匹配
#define ERR_DISPLAY_UPDATE      8   // 图像显示更新失败
#define ERR_NO_CURR_PROCESS     9   // 没有当前图像处理