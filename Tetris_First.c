/****************************************************************************
* @mainpage Tetris : ��Ʈ���� ���� ����
* @version 1.0
* @date 1985��
* @author �˷����� �������� (���þ�)
* @brief ���� ��Ʈ���� ������ �����Ͽ� ����� ��Ʈ���� ������ �����.
* @detail ��ó : ȥ�� �����ϴ� C/C++ (http://www.soen.kr/)
*            ��.��Ʈ���� http://www.soen.kr/lecture/ccpp/cpp2/14-3-1.htm
*            ��.������ ��� ���� http://www.soen.kr/lecture/ccpp/cpp2/14-3-2.htm
*            ��.������ http://www.soen.kr/lecture/ccpp/cpp2/14-3-3.htm
*            ��.Turboc.h http://soen.kr/lecture/ccpp/cpp1/6-4-4.htm
*****************************************************************************/

#include "turboc.h"

/* ��� */
#define LEFT 75		//��Ű
#define RIGHT 77	//��Ű
#define UP 72		//��Ű
#define DOWN 80		//��Ű   
#define ESC 27
#define BX 5		//�������� �»�� ��ǥ X (�ܺ� ���� �»�� ��ǥ X)
#define BY 1		//�������� �»�� ��ǥ Y (�ܺ� ���� �»�� ��ǥ Y)
#define BW 10		//�������� �� (�ܺκ� ����)
#define BH 20		//�������� ���� (�ܺκ� ����)


/* �Լ� */
void drawScreen();			//������ ���ȭ���� �������� �׸���.
void drawBoard();			//�������� ����ȭ���� �׸���. (�ܺκ��� ���ڿ� ����)
void printBrick(bool Show);	//������ �׸���.
bool moveDown();			//������ �¿�, �Ʒ��� �����δ�.
bool processKey();			//Ű�� ������ �� ������ �����Ѵ�.
int getAround(int x, int y, int b, int r);	//������ �ֺ��� �����Ͽ� ������� �ִ��� üũ�Ѵ�.
void testFull();			//�������� ������ �� á���� üũ�Ѵ�.


/* ����ü */
typedef struct point 
{
	int x, y;
} point;


/* ���� ���� */
//shape[brick][rot][tile], shape[������ȣ][ȸ����ȣ][Ÿ�Ϲ�ȣ]
//������(0,0)���κ����� �����ǥ�� �����ϴ� ���
point shape[][4][4] =
{               
	{ 
		{ {0,0},{1,0},{2,0},{-1,0} }, { {0,0},{0,1},{0,-1},{0,-2} },
 		{ {0,0},{1,0},{2,0},{-1,0} }, { {0,0},{0,1},{0,-1},{0,-2} } 
	},	//0�� ��
	{ 
		{ {0,0},{1,0},{0,1},{1,1} }, { {0,0},{1,0},{0,1},{1,1} },
		{ {0,0},{1,0},{0,1},{1,1} }, { {0,0},{1,0},{0,1},{1,1} }
	},	//1�� ��
	{ 
		{ {0,0},{-1,0},{0,-1},{1,-1} }, { {0,0},{0,1},{-1,0},{-1,-1} },
		{ {0,0},{-1,0},{0,-1},{1,-1} }, { {0,0},{0,1},{-1,0},{-1,-1} }
	},	//2�� ����
	{ 
		{ {0,0},{-1,-1},{0,-1},{1,0} }, { {0,0},{-1,0},{-1,1},{0,-1} },
		{ {0,0},{-1,-1},{0,-1},{1,0} }, { {0,0},{-1,0},{-1,1},{0,-1} }
	},	//3�� ����
	{ 
		{ {0,0},{-1,0},{1,0},{-1,-1} }, { {0,0},{0,-1},{0,1},{-1,1} },
		{ {0,0},{-1,0},{1,0},{1,1} }, { {0,0},{0,-1},{0,1},{1,-1} }
	},	//4�� ����
	{ 
		{ {0,0},{1,0},{-1,0},{1,-1} }, { {0,0},{0,1},{0,-1},{-1,-1} },
		{ {0,0},{1,0},{-1,0},{-1,1} }, { {0,0},{0,-1},{0,1},{1,1} }
	},	//5�� ����
	{ 
		{ {0,0},{-1,0},{1,0},{0,1} }, { {0,0},{0,-1},{0,1},{1,0} },
		{ {0,0},{-1,0},{1,0},{0,-1} }, { {0,0},{-1,0},{0,-1},{0,1} }
	}	//6�� ��
};


enum { EMPTY, BRICK, WALL }; //0-EMPTY-����, 1-BRICK-�ٸ� ����, 2-WALL-��

char *arTile[] = { ". ","��","��" }; //����, ����, �� ����� �����ϴ� ���ڿ� �迭, Ÿ�� 1���� 2���ڷ� ��� ǥ��

int board[BW+2][BH+2]; //��ü �������� ����

int nx, ny; //�̵����� ������ �迭���� ���� ��ǥ (ȭ����� ��ǥ BX+nx*2, BY+ny)
int brick; //�̵����� ������ ��ȣ
int rot; //�̵����� ������ ȸ�� ��� ��ȣ


/* main�Լ� */
void main()

{
	int nFrame, nStay; //������ �������� �ӵ�
	int x, y;

	/*���� �ʱ�ȭ*/
	setCursorType(NOCURSOR); //Ŀ�� ����
	srand((unsigned)time(NULL)); //���������� �ʱ�ȭ
	clearScreen(); //ȭ�� ����

	//���� �迭�� �ٱ����� ������ �ʱ�ȭ
	for (x=0 ; x<BW+2 ; x++) 
	{
		for (y=0 ; y<BH+2 ; y++) 
		{
			board[x][y] = ( y==0 || y==BH+1 || x==0 || x==BW+1 ) ? WALL : EMPTY;
		}
	}

	drawScreen(); //ȭ�� ��ü �׸���

	nFrame = 10; //������ �������� �ӵ�����

	while(1)

	{
		/*�� ���� ����*/
		brick = rand() % ( sizeof(shape) / sizeof(shape[0]) ); // 0~6�� ���� �����ϱ�
		nx = BW/2; //������ �ʱ� X��ǥ
		ny = 2; //������ �ʱ� Y��ǥ
		rot = 0; //������ �ʱ� ȸ����� ��ȣ
		printBrick(true); //���� �׸��� �Լ� ȣ��

		/*���� �� ����*/
		if (getAround(nx, ny, brick, rot) != EMPTY) //���� �ֺ��� ������� üũ
			break;
		nStay = nFrame;

		while(1)
		{
			/*��������*/
			if (--nStay == 0) 
			{
				nStay = nFrame;
				if (moveDown()) //�ٴڿ� ������ true
					break;
			}
			/*Ű�Է� ó��*/
			if (processKey()) //�ٸ� ������ ������ true
				break;
			/*�ð� ����*/
			delay(1000/20);  //0.05�ʾ� ���� (1000=1��)
		}
	}
	/*���� �� ó��*/
	clearScreen();
	gotoXY(30, 12); 
	puts("G A M E  O V E R");
	setCursorType(NORMALCURSOR);
	
}


/**
* @brief drawScreen ������ ����� �������� �׸���.
* @detail board[x][y]�� EMPTY(0), BRICK(1), WALL(2)�� ������ �Ϳ� �°�
*         arTile[]�� ���� ". "(0), "��"(1), "��"(2)�� ȭ�鿡 �׸���.
*/
void drawScreen()
{
	int x, y;

	for (x=0 ; x<BW+2 ; x++) 
	{
		for (y=0 ; y<BH+2 ; y++) 
		{
			gotoXY(BX+x*2, BY+y);
			puts(arTile[board[x][y]]);
		}
	}

	gotoXY(50, 3); 
	puts("Tetris Ver 1.0");
	
	gotoXY(50, 5); 
	puts("�¿�:�̵�, ��:ȸ��, �Ʒ�:����");
	
	gotoXY(50, 6); 
	puts("����:���� ����");

}


/**
* @brief drawBoard �������� ���θ� �׸���.
* @detail �ܺκ��� ���ڿ��� ������ �׿� �ִ� ������ �׸���.
*         board[x][y]�� EMPTY(0), BRICK(1), WALL(2)�� ������ �Ϳ� �°�
*         arTile[]�� ���� ". "(0), "��"(1), "��"(2)�� ȭ�鿡 �׸���.
*/
void drawBoard()
{
	int x, y;

	for (x=1 ; x<BW+1 ; x++) 
	{
		for (y=1 ; y<BH+1 ; y++) 
		{
			gotoXY(BX+x*2, BY+y);
			puts(arTile[board[x][y]]);
		}
	}
}


/**
* @brief printBrick ������ �׸���.
* @param bool Show true�̸� ������ �׸���, false�̸� ������ �����Ѵ�.
*/
void printBrick(bool Show)
{
	int i;

	for (i = 0; i<4; i++)
	{
		gotoXY(BX + (shape[brick][rot][i].x + nx) * 2, BY + shape[brick][rot][i].y + ny);
		puts(arTile[Show ? BRICK : EMPTY]);
	}

}


/**
* @brief moveDown ������ �� ĭ �� �Ʒ��� ������.
* @return bool �ٸ� ������ ������ true�� ��ȯ�Ѵ�.
*/
bool moveDown()
{
	if (getAround(nx, ny + 1, brick, rot) != EMPTY)
	{
		testFull();
		return true;
	}
	printBrick(false);
	ny++;
	printBrick(true);
	return false;
}


/**
* @brief processKey Ű�Է��� �޾� �׿� �´� ó���� �Ѵ�. 
* @return �̵� ���� ������ �ٸ� ������ ������ true�� ��ȯ�Ѵ�.
*/
bool processKey()
{
	int ch, trot;

	if (kbhit()) //������ Ű�� ������ true�� ��ȯ�Ѵ�.
	{
		ch = getch(); //<conio.h> getch ���� �Է��� ������ ����ߴٰ� ���� ������ �Է¹��ۿ��� 1����Ʈ�� �����´�.
		
		//ȭ��ǥŰ(��Ű��Ű��Ű��Ű)�� 2byte�� ǥ���Ǵ� Ȯ��Ű�ڵ��̴�.
		//Ȯ��Ű�ڵ尡 �ԷµǾ��� ��쿡�� 0 �Ǵ� 0xE0�� ��ȯ�� ��, �ش� Ű ��ĵ�ڵ带 ��ȯ�Ѵ�.
		if (ch==0xE0 || ch==0) 
		{
			ch = getch();

			switch (ch) 
			{
				case LEFT: //��Ű �Է� : ���� �̵�
					if (getAround(nx-1, ny, brick, rot) == EMPTY) 
					{
						printBrick(false);
						nx--;
						printBrick(true);
					}
					break;

				case RIGHT: //��Ű �Է� : ������ �̵�
					if ( getAround(nx+1, ny, brick, rot) == EMPTY ) 
					{
						printBrick(false);
						nx++;
						printBrick(true);
					}
					break;

				case UP: //��Ű �Է� : ȸ��
					trot = (rot==3 ? 0 : rot+1);
					if ( getAround(nx, ny, brick, trot) == EMPTY ) 
					{
						printBrick(false);
						rot = trot;
						printBrick(true);
					}
					break;

				case DOWN: //��Ű �Է� : �Ʒ��� ����
					if ( moveDown() ) 
					{
						return true;
					}
					break;
			}

		}
		else {

			switch (ch) 
			{
				case ' ': //spaceŰ �Է� : �ٴڱ��� ����
					while ( moveDown() == false ) 
					{ 
						; 
					}
					return true;

			}

		}

	}

	return false; 

}


/**
* @brief getAround ������ �ֺ��� ��������, �ٸ� ������ �ִ��� �Ǵ� �������� �Ǵ��Ѵ�.
* @param int x ���� X��ǥ
* @param int y ���� Y��ǥ
* @param int b ���� ��ȣ
* @param int r ������ ȸ�� ��ȣ
* @return int �����̸� EMPTY, �ٸ� ������ ������ BRICK, ���̸� WALL�� ��ȯ�Ѵ�.
*/
int getAround(int x, int y, int b, int r)
{
	int i, k = EMPTY;

	for (i=0 ; i<4 ; i++) 
	{
		k = max(k, board[x + shape[b][r][i].x][y + shape[b][r][i].y]);
	}

	return k;
}


/**
* @brief testFull �������� ������ �� á���� üũ�Ѵ�.
* @detail �̵����� ������ board�迭�� ����Ѵ�.
*         ���� ��� �����̸� ������ �����.
*/
void testFull()
{
	int i, x, y, ty;

	//�̵����� ������ board �迭�� ����Ѵ�.
	for (i=0 ; i<4 ; i++) 
	{
		board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y] = BRICK;
	}

	for (y=1 ; y<BH+1 ; y++) 
	{
		//������ ��� �������� �Ǵ�
		for (x=1; x<BW+1; x++) 
		{
			if (board[x][y] != BRICK) 
				break;
		}
		//������ ��� ������ ���, ������ �� ���ٺ��� ��ĭ�� �Ʒ��� �����Ѵ�.
		if (x == BW+1) 
		{
			for (ty=y ; ty>1 ; ty--) 
			{
				for (x=1 ; x<BW+1 ; x++) 
				{
					board[x][ty] = board[x][ty-1];
				}
			}
			
			drawBoard(); //�� �� ���� �� ������ ����ȭ���� �׸���.
			delay(200); //0.2�� �ð� ����
		}
	}

} 
