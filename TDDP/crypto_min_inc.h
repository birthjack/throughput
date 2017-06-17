#ifndef  __CRYPTO_MIN_INC_H__
#define  __CRYPTO_MIN_INC_H__

#include <Windows.h>
// [liguoliang  begin 2013/11/21] 
//#ifndef BOOL
//#define BOOL int
//#endif
// [liguoliang end]

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#define DIGEST_LENGTH 16

#ifdef  __cplusplus
extern "C" {
#endif

int make_md5_digest(unsigned char* digest, const unsigned char* in, int in_len);

BOOL verify_md5_digest(const unsigned char* digest, const unsigned char* in, int in_len);

/* 方法说明:用此方法,执行单向散列
 * 参数说明: in_buf   :  输入原文
 *           out_buf  : 接收密文的buf,这里必须
 *                      >=16个字节(128位),否
 *						则返回FALSE
 * 
 * 返回结果: 正确 1, 失败 0;
 */
extern int md5_min_do(const unsigned char *in, int in_len,unsigned char *out, int out_len);

#ifdef  __cplusplus
}
#endif

#endif