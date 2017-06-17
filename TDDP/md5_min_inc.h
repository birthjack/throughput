#ifndef __MD5_MIN_INC_H__
#define __MD5_MIN_INC_H__

#ifdef __cplusplus
extern "C"
{
#endif



/* 方法说明:用此方法,执行单向散列
* 参数说明: in_buf   :  输入原文
*           out_buf  : 接收密文的buf,这里必须
*                      >=16个字节(128位),否
*						则返回FALSE
* 
* 返回结果: 正确 1, 失败 0;
*/
int md5_min_do(const unsigned char *in, int in_len,unsigned char *out, int out_len);







#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __MD5_MIN_INC_H__