#include "ChineseFanBasic.h"

GLuint  vao;
GLuint  vbo[2];
GLuint  ebo;
GLuint v, f;
GLboolean show_line = false;
GLboolean folded = false;
GLboolean vertex_update = false;
GLboolean rotate_model = false;
GLboolean scale_model = false;
GLboolean multiple_model = false;


char* ReadFile(const char* filename);
GLuint initShaders(char* v_shader, char* f_shader);
void init();
void display(void);

int main(int argc, char** argv);

GLint matrix_loc;
mat4 model_matrix(1.0f);
mat4 translation_matrix(1.0f);
mat4 scale_matrix(1.0f);
mat4 rotation_matrix(1.0f);

GLfloat angle = 0.0;
GLfloat angleOne = 0.0;


/************************************************/

char* ReadFile(const char* filename) {

	FILE* infile;
#ifdef WIN32
	fopen_s(&infile, filename, "rb");
#else
	infile = fopen(filename, "rb");
#endif


	if (!infile) {
		printf("Unable to open file %s\n", filename);
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	char* source = (char*)malloc(len + 1);
	fread(source, 1, len, infile);
	fclose(infile);
	source[len] = 0;
	return (source);

}

/*************************************************************/

GLuint initShaders(const char* v_shader, const char* f_shader) {

	GLuint p = glCreateProgram();

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	const char * vs = ReadFile(v_shader);
	const char * fs = ReadFile(f_shader);

	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);

	free((char*)vs);
	free((char*)fs);

	glCompileShader(v);

	GLint compiled;

	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)malloc(len + 1);
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {

		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)malloc(len + 1);
		glGetShaderInfoLog(f, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);
	GLint linked;

	glGetProgramiv(p, GL_LINK_STATUS, &linked);

	if (!linked) {

		GLsizei len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)malloc(len + 1);
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
	}

	glUseProgram(p);
	return p;
}

/*******************************************************/
void init() {

	GLuint program = initShaders("chineseFan.vs", "chineseFan.fs");

	matrix_loc = glGetUniformLocation(program, "model_transformation");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(2, vbo);  // you need to have two buffer objects if color is added
	
	// For the 1st attribute - vertex positions

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(0);

	//Color
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(vertices));
	
	glEnableVertexAttribArray(1);
}
/*******************************************************/
void Keyboard(unsigned char key, int x, int y) {

	switch (key) {

	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'l': case 'L':
		show_line = !show_line;
		break;
	case 'f': case 'F':
		folded = !folded;
		break;
	case 'u': case 'U':
		vertex_update = !vertex_update;
		if (vertex_update) glutSetWindowTitle("Chinese Fan Updated");
		else glutSetWindowTitle("Chinese Fan");
		break;
	case 'r': case 'R':
		rotate_model = !rotate_model;
		break;
	case 's': case 'S':
		scale_model = !scale_model;
		break;
	case 'm': case 'M':
		multiple_model = !multiple_model;
		break;
	}
	glutPostRedisplay();
}

/**********************************************************/

void display(void) {

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glPointSize(20.0);

	glLineWidth(5.0);
	if (show_line)
		glPolygonMode(GL_FRONT, GL_LINE);
	else
		glPolygonMode(GL_FRONT, GL_FILL);
	
	if (scale_model) {
		scale_matrix = scale(mat4(1.0f), vec3(0.25, 0.25, 1.0));
		glPointSize((GLfloat)10.0);
	}
	else {
		scale_matrix = mat4(1.0f);
		((GLfloat)20.0);
	}

	if (rotate_model) {
		rotation_matrix = rotate(mat4(1.0f), radians(angle), vec3(0.0, 0.0, 1.0));
	}

	model_matrix = rotation_matrix * scale_matrix;

	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	
	glBindVertexArray(vao); 

	glDrawArrays(GL_TRIANGLES, 0, 24);
	glDrawArrays(GL_POINTS, 0, 24);

	if (multiple_model && scale_model) {
		translation_matrix = translate(mat4(1.0f), vec3(0.6 * cos(radians(angleOne + 0.0)), 0.6 * sin(radians(0.0 + angleOne)), 0.0));
		model_matrix = translation_matrix * rotation_matrix * scale_matrix;
		glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_POINTS, 0, 24);

		translation_matrix = translate(mat4(1.0f), vec3(0.6 * cos(radians(angleOne + 60.0)), 0.6 * sin(radians(angleOne + 60.0)), 0.0));
		model_matrix = translation_matrix * rotation_matrix * scale_matrix;
		glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_POINTS, 0, 24);

		translation_matrix = translate(mat4(1.0f), vec3(0.6 * cos(radians(angleOne + 120.0)), 0.6 * sin(radians(angleOne + 120.0)), 0.0));
		model_matrix = translation_matrix * rotation_matrix * scale_matrix;
		glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_POINTS, 0, 24);

		translation_matrix = translate(mat4(1.0f), vec3(0.6 * cos(radians(angleOne + 180.0)), 0.6 * sin(radians(angleOne + 180.0)), 0.0));
		model_matrix = translation_matrix * rotation_matrix * scale_matrix;
		glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_POINTS, 0, 24);

		translation_matrix = translate(mat4(1.0f), vec3(0.6 * cos(radians(angleOne + 240.0)), 0.6 * sin(radians(angleOne + 240.0)), 0.0));
		model_matrix = translation_matrix * rotation_matrix * scale_matrix;
		glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_POINTS, 0, 24);

		translation_matrix = translate(mat4(1.0f), vec3(0.6 * cos(radians(angleOne + 300.0)), 0.6 * sin(radians(angleOne + 300.0)), 0.0));
		model_matrix = translation_matrix * rotation_matrix * scale_matrix;
		glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_POINTS, 0, 24);
	}

	// add codes on updating vertices
	if (vertex_update) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_update), vertices_update);
	}
	else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	}
	
	glFlush();

}

/*******************************************************/

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Chinese fan");

	if (glewInit()) {
		printf("%s\n", "Unable to initialize GLEW ...");
	}

	init();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(display);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(500, Rotate, 1);
	glutTimerFunc(100, Rotate, 2);
	glutMainLoop();

	return 0;

}

/*******************************************************/

void Rotate(int n) {
	switch (n) {
	case 1:
		if (rotate_model) {
			angle += 5.0f;
		}
		vertex_update = !vertex_update;
		glutPostRedisplay();
		glutTimerFunc(500, Rotate, 1);
		break;
	case 2:
		if (rotate_model) {
			angleOne += 5.0f;
		}
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 2);
		break;
	}
}