#ifndef STORAGE_H
#define STORAGE_H

#define FLASHADDR_BASEINFO      		0x0000	//������Ϣ
	#define BASEOFFSET_DEVICEVER		0x00	//�豸�汾��2B��
	#define BASEOFFSET_DEVICEADDR		0x02	//�豸��ַ��2B��
	#define BASEOFFSET_BAUD				0x04	//�����ʣ�2B��
	#define BASEOFFSET_INTERVAL         0x06    //������ʱ��
	#define BASEOFFSET_OUTPUTFORMAT		0x08	//�����ʽ��ʮ���ƻ�ʮ�����ƣ�2B��
	#define BASEOFFSET_OUTPUTMODE		0x0A	//���ģʽ����������ͱ��������2B��
	#define BASEOFFSET_BUZZER			0x0C	//���������ƣ�2B��
	#define BASEOFFSET_RELAY			0x0E	//�̵�������ֵ��2B��
	
#endif



