#include <stdio.h>

const unsigned char GS[16] = {
	0x01, 0x0a, 0x04, 0x0c, 0x06, 0x0f, 0x03, 0x09, 0x02, 0x0d, 0x0b, 0x07, 0x05, 0x00, 0x08, 0x0e
};

const char BP[64] = {
0, 17, 34, 51, 48, 1,18, 35, 32, 49, 2, 19, 16, 33, 50, 3,
4, 21, 38, 55, 52, 5, 22, 39, 36, 53, 6 ,23 , 20,37, 54, 7,
8, 25, 42, 59, 56, 9, 26, 43, 40, 57, 10, 27, 24, 41, 58, 11,
12, 29, 46, 63, 60, 13, 30, 47, 44, 61, 14, 31, 28, 45, 62, 15
};

const unsigned char GSinv[16] = {
	0x0d, 0x00, 0x08, 0x06, 0x02, 0x0c, 0x04, 0x0b, 0x0e, 0x07, 0x01, 0x0a, 0x03, 0x09, 0x0f, 0x05
};

const char BPinv[64] = {
0, 5, 10, 15, 16, 21, 26, 31, 32, 37, 42, 47, 48, 53, 58, 63,
12, 1, 6, 11, 28, 17, 22, 27, 44, 33, 38, 43, 60, 49, 54, 59,
8, 13, 2, 7, 24, 29, 18, 23, 40, 45, 34, 39, 56, 61, 50, 55,
4, 9, 14, 3, 20, 25, 30, 19, 36, 41, 46, 35, 52, 57, 62, 51
};

void Encryption(unsigned char state[8], unsigned char key[16], unsigned char RC[6], unsigned char Cipher[8])
{
	char i = 0, j = 0;					// for문용
	unsigned char w[16] = { 0, };		// w0부터 w15까지 생성.

	for (i = 0; i < 8; i++)
	{
		char m = 2 * i;
		char n = (2 * i) + 1;
		w[m] = state[i] % 16;			 
		w[n] = state[i] / 16;
	}									//  w[0] 부터 w[15] 까지 4비트씩 넣음. w[0] = d, w[1] = 7 ... 


	// 루프의 시작
	char r = 0;
	for (r = 0; r < 28; r++)
	{

		// SubCells 돌리기
		for (char i = 0; i < 16; i++)
		{
			w[i] = GS[w[i]];
		}				

		unsigned char tmp[64] = { 0, };				// 임시저장용 tmp  
		unsigned char b[64] = { 0, };				// 64비트 평문을 비트단위로 저장


		// w[0] ~ w[15] 를 b[64]에 1비트단위로 나눠주자 (64개의 비트로)
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 4; j++)
			{
				b[4 * i + j] = (w[i] >> j) & 0x01;
			}
		}											//b[0] 부터 b[63]까지 다 채워넣음

		/*
		printf("\n%d : check Subcells \n", r);

		for (char i = 63; i > -1; i--)
		{
			printf("%d ", b[i]);
			if (i % 4 == 0)
				printf(" = %02x \n", b[i + 3] * 8 + b[i + 2] * 4 + b[i + 1] * 2 + b[i + 0]);
		}
		printf("\n\n");
		*/


		//PermBits 돌리기
		for (i = 0; i < 64; i++)
		{
			tmp[i] = b[i];
		}

		for (i = 0; i < 64; i++)
		{
			b[BP[i]] = tmp[i];
		}

		/*
		printf("\n%d : check PermBits\n", r);

		for (char i = 63; i > -1; i--)
		{
			printf("%d ", b[i]);
			if (i % 4 == 0)
				printf(" = %02x \n", b[i + 3] * 8 + b[i + 2] * 4 + b[i + 1] * 2 + b[i + 0]);
		}
		printf("\n\n");
		*/

		/*********************************************************/
		//AddRoundKey

		unsigned char u[16] = { 0, }, v[16] = { 0, };
		tmp[0] = key[0];
		tmp[1] = key[1];
		tmp[2] = key[2];
		tmp[3] = key[3];
		// key 이동 시켜줄때 키를 저장시킬 임시변수

		//key[0] 의 16비트는 v[0] ... v[15]로
		//key[1] 의 16비트는 u[0] ... u[15]로
		for (i = 7; i >= 0; i--)
		{
			v[i] = (tmp[0] >> i) & 0x01;
		}

		for (i = 7; i >= 0; i--)
		{
			v[i + 8] = (tmp[1] >> i) & 0x01;
		}

		for (i = 7; i >= 0; i--)
		{
			u[i] = (tmp[2] >> i) & 0x01;
		}
		for (i = 7; i >= 0; i--)
		{
			u[i + 8] = (tmp[3] >> i) & 0x01;
		}

		/*
		printf("\nCheck RK = U||V \n R = ");
		for (i = 15; i >= 0; i--)
			printf("%d", u[i]);
		printf("\n\n V = ");
		for (i = 15; i >= 0; i--)
			printf("%d", v[i]);
		printf("\n\n");
		*/

		/*이제 라운드키와 b[64]를 XOR해준다*/
		for (i = 0; i < 16; i++)
		{
			b[4 * i + 1] = b[4 * i + 1] ^ u[i];
			b[4 * i] = b[4 * i] ^ v[i];
		}

		//roundconstatnts update 
		tmp[0] = RC[5];
		tmp[1] = RC[4];
		RC[5] = RC[4];
		RC[4] = RC[3];
		RC[3] = RC[2];
		RC[2] = RC[1];
		RC[1] = RC[0];
		RC[0] = tmp[0] ^ tmp[1] ^ 1;

		/*
		printf("Check RoundConstants update\n");
		printf("C = ");
		for (i = 5; i >=0; i--)
			printf("%d", RC[i]);
		printf("\n%d라운드 = %02x", r,RC[0] + RC[1] *2 + RC[2]*4 + RC[3]*8 + RC[4]*16 + RC[5]*32);
		printf("\n\n");
		*/

		//roundconstants 와 xor 해주기
		b[63] = b[63] ^ 1;
		b[23] = b[23] ^ RC[5];
		b[19] = b[19] ^ RC[4];
		b[15] = b[15] ^ RC[3];
		b[11] = b[11] ^ RC[2];
		b[7] = b[7] ^ RC[1];
		b[3] = b[3] ^ RC[0];

		/*
		printf("\n%d : check Add round key\n", r);

		for (char i = 63; i >= 0; i--)
		{
			if (i % 8 == 0)
				printf("%02x ", ((b[i + 7] * 8 + b[i + 6] * 4 + b[i + 5] * 2 + b[i + 4]) << 4) ^ (b[i + 3] * 8 + b[i + 2] * 4 + b[i + 1] * 2 + b[i + 0]));
		}
		printf("\n\n");
		*/

		// roundkey updated 
		tmp[0] = key[0];
		tmp[1] = key[1];
		tmp[2] = key[2];
		tmp[3] = key[3];

		char tmpa = 0, tmpb = 0, tmpc = 0, tmpd = 0;
		tmpa = key[1] & 0x0f;
		tmpb = key[0] & 0x0f;
		tmpc = key[3] & 0x03;
		tmpd = key[2] & 0x03;

		tmp[0] = tmp[0] >> 4;
		tmpa = tmpa << 4;

		tmp[1] = tmp[1] >> 4;
		tmpb = tmpb << 4;

		tmp[2] = tmp[2] >> 2;
		tmpc = tmpc << 6;

		tmp[3] = tmp[3] >> 2;
		tmpd = tmpd << 6;

		for (i = 0; i < 12; i++)
		{
			key[i] = key[i + 4];
		}

		key[12] = tmp[1] + tmpb;
		key[13] = tmp[0] + tmpa;
		key[14] = tmp[2] + tmpc;
		key[15] = tmp[3] + tmpd;

		/*
		printf("%d : Check RoundKey update\n", r);
		for (i = 15; i >= 0; i--)
			{
				printf("%02x \n", key[i]);
			}
		printf("\n\n");
		*/
		/****************************************/

		for (i = 0; i < 16; i++)
		{
			w[i] = b[4 * i] + b[4 * i + 1] * 2 + b[4 * i + 2] * 4 + b[4 * i + 3] * 8;
		}


		/*
		for (i = 15; i >= 0; i--)
		{
			printf("w[%d] = %02x\n", i, w[i]);
		}
		printf("\n\n");
		*/
	}											//28round 종료


	printf("\n-------------------------------------------\nRESULT\n");
	
	for (i = 0; i < 8; i++)
	{
		Cipher[i] = w[2 * i] + (w[2 * i + 1] * 0x10);
	}

	for (i = 7; i >= 0; i--)
	{
		printf("Ciphertext[%d] = 0x%02x\n", i, Cipher[i]);
	}


	
}

void Decryption(unsigned char state[8], unsigned char key[16], unsigned char RC[6], unsigned char Cipher[8])
{

	char i = 0, j = 0;					// for문용
	unsigned char b[64] = { 0, };		//64비트를 1비트단위로 저장시킬 공간
	unsigned char w[16] = { 0, };		//64비트를 4비트단위로 저장시킬 공간

	for (i = 0; i < 8; i++)
	{
		char m = 2 * i;
		char n = (2 * i) + 1;
		w[m] = Cipher[i] % 16;
		w[n] = Cipher[i] / 16;
	}									//64비트를 4비트단위로 쪼갬 

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 4; j++)
		{
			b[4 * i + j] = (w[i] >> j) & 0x01;
		}
	}									//64비트를 1비트단위로 쪼갬

	//루프시작
	char r = 0;
	
		

	for (r = 0; r < 28; r++)
	{
		unsigned char tmp[64] = { 0, };				// 임시저장용 tmp  
		/*****************************************************/
		//AddRoundKeyinv

		unsigned char u[16] = { 0, }, v[16] = { 0, };

		tmp[0] = key[12];
		tmp[1] = key[13];
		tmp[2] = key[14];
		tmp[3] = key[15];
		// key 이동 시켜줄때 키를 저장시킬 임시변수



		char tmpa = 0, tmpb = 0, tmpc = 0, tmpd = 0;
		tmpa = key[12] & 0x0f;
		tmpb = key[13] & 0x0f;
		tmpc = (key[14] >> 6);
		tmpd = (key[15] >> 6);

		tmp[0] = tmp[0] >> 4;
		tmpa = tmpa << 4;

		tmp[1] = tmp[1] >> 4;
		tmpb = tmpb << 4;

		tmp[2] = (tmp[2] & 0x3f) << 2;
		//tmpc = tmpc >> 6;

		tmp[3] = (tmp[3] & 0x3f) << 2;
		//tmpd = tmpd >> 6;

		for (i = 11; i >= 0; i--)
		{
			key[i + 4] = key[i];
		}

		key[0] = tmp[0] + tmpb;
		key[1] = tmp[1] + tmpa;
		key[2] = tmp[2] + tmpd;
		key[3] = tmp[3] + tmpc;

		/*
		printf("check inverse key update \n");
		for (i = 15; i >= 0; i--)
		{
			printf(" %0x ", key[i]);

		}
		printf("\n\n");
		*/


		//key[0] 의 16비트는 v[0] ... v[15]로
		for (i = 7; i >= 0; i--)
		{
			v[i] = (key[0] >> i) & 0x01;
		}

		for (i = 7; i >= 0; i--)
		{
			v[i + 8] = (key[1] >> i) & 0x01;
		}

		//key[1] 의 16비트는 u[0] ... u[15]로
		for (i = 7; i >= 0; i--)
		{
			u[i] = (key[2] >> i) & 0x01;
		}
		for (i = 7; i >= 0; i--)
		{
			u[i + 8] = (key[3] >> i) & 0x01;
		}

		//roundconstantsinv  XOR

		b[63] = b[63] ^ 1;
		b[23] = b[23] ^ RC[5];
		b[19] = b[19] ^ RC[4];
		b[15] = b[15] ^ RC[3];
		b[11] = b[11] ^ RC[2];
		b[7] = b[7] ^ RC[1];
		b[3] = b[3] ^ RC[0];


		//roundconstatntsinv update 
		tmp[0] = RC[0];
		tmp[1] = RC[5];
		RC[0] = RC[1];
		RC[1] = RC[2];
		RC[2] = RC[3];
		RC[3] = RC[4];
		RC[4] = RC[5];
		RC[5] = tmp[0] ^ tmp[1] ^ 1;

		/*
		printf("check RoundConstantsinv update \n");
		for (i = 5; i >= 0; i--)
		{
			printf(" %02x ", RC[i]);

		}
		printf("\n\n");
		*/

		//roundkeyinv XOR
		for (i = 0; i < 16; i++)
		{
			b[4 * i + 1] = b[4 * i + 1] ^ u[i];
			b[4 * i] = b[4 * i] ^ v[i];
		}
		/*
		printf(" %d : check addroundkeyinv\n", r);
		for (i = 0; i < 16; i++)
		{
			w[i] = b[4 * i] + b[4 * i + 1] * 2 + b[4 * i + 2] * 4 + b[4 * i + 3] * 8;
		}
		for (i = 15; i >= 0; i--)
		{
			printf("%02x ", w[i]);
		}
		printf("\n\n");
		*/
		/**********************************************/


		//PermBIts
		for (i = 0; i < 64; i++)
		{
			tmp[i] = b[i];
		}
		for (i = 0; i < 64; i++)
		{
			b[BPinv[i]] = tmp[i];
		}
		for (i = 0; i < 16; i++)
		{
			w[i] = b[4 * i] + b[4 * i + 1] * 2 + b[4 * i + 2] * 4 + b[4 * i + 3] * 8;
		}

		/*
		printf(" %d : check Permbitsinv\n", r);

		for (i = 15; i >= 0; i--)
		{
			printf("%02x ", w[i]);
		}
		printf("\n\n");
		*/

		//SubCellsinv
		for (i = 0; i < 16; i++)
		{
			w[i] = GSinv[w[i]];
		}

		/*
		printf(" %d : Check SubCellsinv\n", r);

		for (i = 15; i >= 0; i--)
		{
			printf("%02x ", w[i]);
		}
		printf("\n\n");
		*/


		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 4; j++)
			{
				b[4 * i + j] = (w[i] >> j) & 0x01;
			}
		}
	}
	//루프의 끝

	for (i = 0; i < 8; i++)
	{
		state[i] = w[2 * i] + (w[2 * i + 1] * 0x10);
	}

}

unsigned char main()
{
	unsigned char state[8] = { 0x7d, 0x8a, 0x9b, 0x7a, 0x72,0xc7, 0x50,0xc4 }; // test vector plain text
	unsigned char key[16] = { 0xe7, 0x44, 0x50, 0xc7, 0xff, 0xf6, 0xf9, 0xa1,
		0x13, 0x27, 0xbc, 0xb6, 0x1e, 0x73, 0x91, 0xbd }; //test vector key
	unsigned char RC[6] = { 0, };	// RoundConstants
	unsigned char Cipher[8] = { 0, };

	printf("Plain text = ");
	for (char i = 7; i >= 0; i--)
	{
		printf("%02x ", state[i]);
	}
	printf("\n\n");

	Encryption(state, key, RC, Cipher);

	Decryption(state, key, RC, Cipher);

	printf("Plain text = ");
	for (char i = 7; i >= 0; i--)
	{
		printf("%02x ", state[i]);
	}
	printf("\n\n");


	return 0;
}