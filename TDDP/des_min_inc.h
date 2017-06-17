#ifndef  __DES_MIN_INC_H__
#define  __DES_MIN_INC_H__
 

#define DES_ENCRYPT	1
#define DES_DECRYPT	0

#ifdef  __cplusplus
extern "C" {
#endif

/* ����˵��:�ô˷���,ִ�к�des ����,�õ�����,��������
   ����˵��: in   :  ����ʱ,����ԭ��,����ʱ,��������
            in_len  :in ����,
                     ������ԭ��ʱ: ���������Ϊ���ⳤ��,
                     ��des �ڲ�,����64bitΪ��λ����,
					 ������9byte,����ܺ�ĳ���Ϊ16byte,
					 ������7byte,����ܺ�ĳ���Ϊ8byte,

                     ���������ĳ���ʱ: �����������ԭ�ļ���
					 ʱ���صĳ���

            out  :   �������Ļ������ĵ�buf 

            out_len: �������Ļ������ĵ�buf ����,
			         ����: out_len >= ((ԭ�ĳ��� +7)-(ԭ�ĳ��� +7)%8
					 ��: in_len = 9, out_len ����>=16 ,���򷵻� 0;
                     ����: out_len >= ���ĳ���

            key  :   8byte �������봮,��Ϊ��������ܵ�����,des�㷨��,
			         ����������õ�����һ������

            enc  :   DES_ENCRYPT  ����, DES_DECRYPT ����;���궨��
   ���ؽ��: int �ɹ�:���ؽ��ܻ��Ǽ��ܺ�Ĵ���0���������, ʧ�� ���� 0;
 */
int des_min_do(const unsigned char *in,int in_len, unsigned char *out, int out_len, const unsigned char *key, int enc);

#ifdef  __cplusplus
}
#endif

#endif
