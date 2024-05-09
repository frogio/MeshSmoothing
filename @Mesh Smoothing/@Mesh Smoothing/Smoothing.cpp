#include "Model.h"
#include "gl/glut.h"

void glReshape(int w, int h);
void Display();

Model model("obj/buddha.obj");

float _zoom = 15.0f;
unsigned char _btnState[3] = { 0, };
int last_x = 0;
int last_y = 0;

float _rotate_x = 0.0f;
float _rotate_y = 0.001f;

void Mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);	
void keyboad(unsigned char key, int x, int y);

void main(int argc, char* argv[]) {

	printf("Key 1 : Simple Smoothing\n");
	printf("Key 2 : Laplacian Smoothing\n");
	printf("Key 3 : Taubin Smoothing\n");


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(50, 100);		// 윈도우의 초기 생성 위치, 왼쪽 상단
	glutInitWindowSize(800, 600);			// 윈도우 사이즈
	glutCreateWindow("GL_01e");				// 윈도우 생성
	glutReshapeFunc(glReshape);
	glutDisplayFunc(Display);				// 윈도우 안에 그릴 함수 지정
	glutMouseFunc(Mouse);					// 마우스 콜백 함수
	glutKeyboardFunc(keyboad);
	glutMotionFunc(mouseMotion);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();							// 렌더링 루프

}


void glReshape(int w, int h) {
	if (w == 0)
		h = 1;

	glViewport(0, 0, w, h);	// 그려질 스케치북의 사이즈
	glMatrixMode(GL_PROJECTION);
	// 연산 대상이 되는 행렬 선택 (카메라 좌표 행렬)
	glLoadIdentity();
	// 초기화
	gluPerspective(90.0f, (float)w / h, 0.1f, 100.0f);
	// Perspective 행렬 연산, 90도의 fov, 종횡비, near, far값 (카메라 위치)

	glMatrixMode(GL_MODELVIEW);
	// 연산 대상이 되는 행렬 선택 (모델의 좌표 행렬을 변환함.)	

	// OpenGL의 기본 설정
}

void Display() {

	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// 단위행렬을 불러옴, (행렬 연산의 초기값)
	glTranslatef(0.0f, 0.0f, -_zoom);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glRotatef(_rotate_x, 1, 0, 0);
	glRotatef(_rotate_y, 0, 1, 0);

	model.drawModel();
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	glutSwapBuffers();
	// 더블 버퍼링
}

void mouseMotion(int x, int y) {

	int diff_x = x - last_x;
	int diff_y = y - last_y;
	// 마우스를 드래그 할 때 거리값 계산

	if (_btnState[2]) {
		_zoom -= (float)0.05f * diff_x;
	}
	else if (_btnState[0]) {
		_rotate_x += (float)0.05f * diff_y;
		_rotate_y += (float)0.05f * diff_x;
	}

	glutPostRedisplay();
}


void Mouse(int button, int state, int x, int y) {
	last_x = x;
	last_y = y;
	// 마우스를 클릭할 때 최초의 좌표

	switch (button) {
	case GLUT_LEFT_BUTTON:
		printf("left button\n");
		_btnState[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		printf("middle button\n");
		_btnState[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		printf("right button\n");
		_btnState[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	}

	glutPostRedisplay();
	// 변경된 _zoom 값과 _translate된 좌표값으로 Redisplay 실행
}

void keyboad(unsigned char key, int x, int y) {

	switch (key) {
	case '1':
		model.simpleSmoothing();
		break;

	case '2':
		model.laplacianSmoothing(0.01f);
		break;

	case '3':
		model.taubinSmoothing(0.1f, 0.1f);
		break;

	}

	glutPostRedisplay();
}