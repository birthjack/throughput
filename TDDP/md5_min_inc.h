#ifndef __MD5_MIN_INC_H__
#define __MD5_MIN_INC_H__

#ifdef __cplusplus
extern "C"
{
#endif



/* ����˵��:�ô˷���,ִ�е���ɢ��
* ����˵��: in_buf   :  ����ԭ��
*           out_buf  : �������ĵ�buf,�������
*                      >=16���ֽ�(128λ),��
*						�򷵻�FALSE
* 
* ���ؽ��: ��ȷ 1, ʧ�� 0;
*/
int md5_min_do(const unsigned char *in, int in_len,unsigned char *out, int out_len);







#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __MD5_MIN_INC_H__