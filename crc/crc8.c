//x^8+x^2+^x+1  seed = 0x41
unsigned char  crc_8(unsigned char *const pstr,int length)
{
	unsigned char* p= pstr;
	int len = length;
	unsigned char crc = 0x41;//根据自己实际情况或者手册中初值
	
	unsigned char i = 0;
	while(len--)
	{
		crc ^= *p++;
		for(i = 8 ; i > 0 ; --i)
		{
			if(crc & 0x80)
			{								//判断最高位是不是为1 
				crc =  (crc << 1)^ 0x07; 	// 最高位为1，不需要异或，往左移一位，然后与0x31异或 
			}else
			{
				crc = (crc << 1); 
			} 
		} 
	} 
	return crc;
}