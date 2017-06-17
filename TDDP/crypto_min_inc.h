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

/* ����˵��:�ô˷���,ִ�е���ɢ��
 * ����˵��: in_buf   :  ����ԭ��
 *           out_buf  : �������ĵ�buf,�������
 *                      >=16���ֽ�(128λ),��
 *						�򷵻�FALSE
 * 
 * ���ؽ��: ��ȷ 1, ʧ�� 0;
 */
extern int md5_min_do(const unsigned char *in, int in_len,unsigned char *out, int out_len);

#ifdef  __cplusplus
}
#endif

#endif