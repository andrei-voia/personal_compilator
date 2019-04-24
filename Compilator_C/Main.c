#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "Header.h"


//defined variables
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)throw_error("not enough memory");
#define SIZE 100000	
//#define file_name "C:\\Users\\PREDATOREL\\Desktop\\input.txt"
#define file_name "input.txt"


//used to simplify the ID work
enum {
	ID, CT_INT, CT_REAL, CT_CHAR, CT_STRING, COMMA, SEMICOLOMN, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, ADD, 
	SUB, MUL, DOT, AND, OR, NOT, NOTEQ, ASSIGN, EQUAL, LESS, LESSEQ, GREATER, GREATEREQ, DIV, END,
	CONTINUE, BREAK, VOID, CHAR, DOUBLE, INT, ELSE, IF, FOR, WHILE, STRUCT, RETURN
};


//used for printing the specific ID
const char* enum_name[] = { 
	"ID", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "COMMA", "SEMICOLOMN", "LPAR", "RPAR",
	"LBRACKET", "RBRACKET", "LACC", "RACC", "ADD", "SUB", "MUL", "DOT", "AND", "OR", "NOT", "NOTEQ", "ASSIGN", 
	"EQUAL", "LESS", "LESSEQ", "GREATER", "GREATEREQ", "DIV", "END",
	"CONTINUE", "BREAK", "VOID", "CHAR", "DOUBLE", "INT", "ELSE", "IF", "FOR", "WHILE", "STRUCT", "RETURN"
};


//basic token structure which contains the basic knowledge about an element
typedef struct _Token {
	int code;					// code (name ID)
	union {
		char *text;				// used for ID, CT_STRING (dynamically allocated)
		long int int_val;		// used for CT_INT, CT_CHAR
		double double_val;		// used for CT_REAL
	};
	int line;					// the line from the input file
	struct _Token *next;		// next element in the list
}Token;


//global variables
Token *lastToken;
Token *tokens;		//first token pointing to the beginning of the list
int line = 1;
char *point_inbuff;


// this prints an error when required
void throw_error(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);
	exit(-1);
}


// this prints a specific token error when required
void token_error(const Token *tk, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "error in line %d: ", tk->line);
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);
	exit(-1);
}


//add a new token at the end of the list and it saves the code and the line of the token
Token *add_token(int code)
{
	Token *tk;
	SAFEALLOC(tk, Token)
		tk->code = code;
	tk->line = line;
	tk->next = NULL;
	if (lastToken) {
		lastToken->next = tk;
	}
	else {
		tokens = tk;
	}
	lastToken = tk;
	return tk;
}


//print all elements from the list of the tokens, in the order they were added
void print_token_list()
{
	printf("\nTOKEN LIST:\n\n");

	Token *tk;
	tk = tokens;

	//go through the token list
	while (tk != NULL)
	{
		printf("%d %s", tk->line, enum_name[tk->code]);

		if (tk->code == ID || tk->code == CT_STRING)printf(": %s\n", tk->text);
		else if (tk->code == CT_INT)printf(": %d\n", tk->int_val);
		else if (tk->code == CT_REAL)printf(": %g\n", tk->double_val);
		else if (tk->code == CT_CHAR)printf(": %c\n", (char)tk->int_val);
		else printf("\n");

		tk = tk->next;
	}
}


//release all the remaining dynamically allocated memory from the heap
void release_memory()
{
	printf("\n\nRELEASING MEMORY...\n\n");

	Token *tk;
	tk = tokens;

	//go through the token list
	while (tk != NULL)
	{
		//release the pointer string from the current token
		if (tk->code == ID || tk->code == CT_STRING)
		{
			printf("-> %s\n", tk->text);
			free(tk->text);
		}
			
		//go to the next token in the next while iteration
		Token *release_last = tk;
		tk = tk->next;

		//release current token
		free(release_last);
	}

	printf("\nMEMORY RELEASED...\n");
}


//this function removes and replaces special characters with their specific function 
char* remove_special_characters(char *text)
{
	int current_size = strlen(text);
	int calculate_lines = strlen(text);

	//here we calculate the required final theoretical length after the modifications of the required characters
	for (int i = 0; i < current_size; i++)
	{
		if (text[i] == '\\')
			if (text[i + 1] == '\\' || text[i + 1] == '\"' || text[i + 1] == '\'' || text[i + 1] == '0' ||
				text[i + 1] == '?' || text[i + 1] == 'a' || text[i + 1] == 'b' || text[i + 1] == 'f' ||
				text[i + 1] == 'n' || text[i + 1] == 'r' || text[i + 1] == 't' || text[i + 1] == 'v')
			{
				calculate_lines--;
			}
	}
	
	//add +1 for the '\0' ending character 
	calculate_lines++;

	//allocate new string pointer
	char *new_text = malloc(sizeof(char) * calculate_lines);

	int count_current = 0;
	//move the elements into the new string, with the required modifications
	for (int i = 0; i < current_size; i++, count_current++)
	{
		if (text[i] == '\\')
		{
			switch (text[i + 1])
			{
			case '\\':
				new_text[count_current] = '\\';
				break;

			case '\"':
				new_text[count_current] = '\"';
				break;

			case '\'':
				new_text[count_current] = '\'';
				break;

			case '0':
				new_text[count_current] = '\0';
				break;

			case '?':
				new_text[count_current] = '\?';
				break;

			case 'a':
				new_text[count_current] = '\a';
				break;

			case 'b':
				new_text[count_current] = '\b';
				break;

			case 'f':
				new_text[count_current] = '\f';
				break;

			case 'n':
				new_text[count_current] = '\n';
				break;

			case 'r':
				new_text[count_current] = '\r';
				break;

			case 't':
				new_text[count_current] = '\t';
				break;

			case 'v':
				new_text[count_current] = '\v';
				break;

			default: {printf("\nERROR CHANGING TO SPECIAL CHARACTERS...\n"); i--; }
			}
			i++;
		}

		else
		{
			new_text[count_current] = text[i];
		}
	}

	//set the last character as '\0'
	new_text[calculate_lines - 1] = '\0';

	//release the old pointer because we don't need it anymore
	free(text);
	return new_text;
}


//extracts and creates the text between the specified string pointers
char *create_text(char *begin, char *after)
{
	int pointer_size = after - begin + 1;
	char *text = malloc(sizeof(char) * pointer_size);

	strncpy(text, begin, pointer_size - 1);
	text[pointer_size - 1] = '\0';

	return text;
}


//creates a string, eliminating the quotation marks (")
char *create_string(char *begin, char *after)
{
	++begin;
	--after;

	return create_text(begin, after);
}


//creates the char character
int create_char(char *begin)
{
	char val;

	if (begin[1] == '\\')
		switch (begin[2])
		{
		case '\\':
			val = '\\';
			break;

		case '\"':
			val = '\"';
			break;

		case '\'':
			val = '\'';
			break;

		case '0':
			val = '\0';
			break;

		case '?':
			val = '\?';
			break;

		case 'a':
			val = '\a';
			break;

		case 'b':
			val = '\b';
			break;

		case 'f':
			val = '\f';
			break;

		case 'n':
			val = '\n';
			break;

		case 'r':
			val = '\r';
			break;

		case 't':
			val = '\t';
			break;

		case 'v':
			val = '\v';
			break;

		default: printf("\nERROR SAVING THE CHAR CHARACTER...\n");
		}

	else val = begin[1];
	return (int)val;
}
 

//creates an integer (decimal, hexa and octa) from the specified string
int create_int(char *begin, char *after)
{
	char *aux;
	int val;
	//create the text and save it in an auxiliray string
	aux = create_text(begin, after);

	//check if the int is HEXA, OCTA or DECIMAL
	if (aux[0] == '0' && (aux[1] == 'x' || aux[1] == 'X'))
	{
		//printf("HEXA\n");
		aux += 2;
		val = (int)strtol(aux, NULL, 16);
		return val;
	}
	else if (aux[0] == '0' && strlen(aux) > 1)
	{
		//printf("OCTA\n");
		aux++;	//not necessary, just optional
		val = (int)strtol(aux, NULL, 8);
		return val;
	}

	//printf("DECIMAL\n");
	//or we could use val = atoi(aux); for this
	val = (int)strtol(aux, NULL, 10);
	free(aux);

	return val;
}


//creates a double variable from a string pointer
double create_double(char *begin, char *after)
{
	char *aux;
	double val;
	aux = create_text(begin, after);

	val = strtod(aux, NULL);
	free(aux);

	return val;
}


//this method gets the next token from the global string pointer, point_inbuff
int get_next_token()
{
	char current_char;
	int state = 0;

	char *pointer_start;
	int pointer_size;
	Token *token;

	pointer_start = point_inbuff;

	for (;;)
	{
		current_char = *point_inbuff;
		//printf("#%d %c(%d)\n", state, current_char, current_char);

		switch (state)
		{
		case 0: //SAVE THE START OF THE CURRENT ELEMENT
				pointer_start = point_inbuff;

				//CHECK THE BIG STATES ...
				if (current_char == '0') { state = 1; ++point_inbuff; }
				else if (isdigit(current_char) && current_char != '0') { state = 6; ++point_inbuff; }
				else if (isalpha(current_char) || current_char == '_') { state = 13; ++point_inbuff; }
				else if (current_char == '\'') { state = 15; ++point_inbuff; }
				else if (current_char == '\"') { state = 19; ++point_inbuff; }

				//SECOND SIMPLER STATES ...
				else if (current_char == ',') { state = 23; ++point_inbuff; }
				else if (current_char == ';') { state = 24; ++point_inbuff; }
				else if (current_char == '(') { state = 25; ++point_inbuff; }
				else if (current_char == ')') { state = 26; ++point_inbuff; }
				else if (current_char == '[') { state = 27; ++point_inbuff; }
				else if (current_char == ']') { state = 28; ++point_inbuff; }
				else if (current_char == '{') { state = 29; ++point_inbuff; }
				else if (current_char == '}') { state = 30; ++point_inbuff; }
				else if (current_char == '+') { state = 31; ++point_inbuff; }
				else if (current_char == '-') { state = 32; ++point_inbuff; }
				else if (current_char == '*') { state = 33; ++point_inbuff; }
				else if (current_char == '.') { state = 34; ++point_inbuff; }
				else if (current_char == '&') { state = 35; ++point_inbuff; }
				else if (current_char == '|') { state = 37; ++point_inbuff; }
				else if (current_char == '!') { state = 39; ++point_inbuff; }
				else if (current_char == '=') { state = 42; ++point_inbuff; }
				else if (current_char == '<') { state = 45; ++point_inbuff; }
				else if (current_char == '>') { state = 48; ++point_inbuff; }
				else if (current_char == '/') { state = 51; ++point_inbuff; }
				else if (current_char == '\0') { state = 56; ++point_inbuff; }

				//ELIMINATE BLANK SPACES
				else if (current_char == ' ' || current_char == '\r' || current_char == '\t') ++point_inbuff;
				else if (current_char == '\n') { line++;  ++point_inbuff; }
				break;

		case 1: if (current_char == 'x') { state = 2; ++point_inbuff; }
				else state = 4;
				break;

		case 2: if (isalpha(current_char) || isdigit(current_char)) { state = 3; ++point_inbuff; }
				break;

		case 3: if ((current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F') || isdigit(current_char)) { state = 3; ++point_inbuff; }
				else state = 5;
				break;

		case 4: if (current_char >= '0' && current_char <= '7') { state = 4; ++point_inbuff; }
				else if (current_char == '8' || current_char == '9') { state = 6; ++point_inbuff; }
				else if (current_char == '.') { state = 7; ++point_inbuff; }
				else if (current_char == 'e' || current_char == 'E') { state = 9; ++point_inbuff; }
				else state = 5;
				break;

		case 5: token = add_token(CT_INT);
			token->int_val = create_int(pointer_start, point_inbuff);
			return CT_INT;

		case 6: if (isdigit(current_char)) { state = 6; ++point_inbuff; }
				else if (current_char == '.') { state = 7; ++point_inbuff; }
				else if (current_char == 'e' || current_char == 'E') { state = 9; ++point_inbuff; }
				else state = 5;
				break;

		case 7: if (isdigit(current_char)) { state = 8; ++point_inbuff; }
				break;

		case 8: if (isdigit(current_char)) { state = 8; ++point_inbuff; }
				else if (current_char == 'e' || current_char == 'E') { state = 9; ++point_inbuff; }
				else state = 12;
				break;

		case 9: if (current_char == '+' || current_char == '-') { state = 10; ++point_inbuff; }
				else state = 10;
				break;

		case 10: if (isdigit(current_char)) { state = 11; ++point_inbuff; }
				 break;

		case 11: if (isdigit(current_char)) { state = 11; ++point_inbuff; }
				 else state = 12;
				 break;

		case 12: token = add_token(CT_REAL);
			token->double_val = create_double(pointer_start, point_inbuff);
			return CT_REAL;

		case 13: if (isalpha(current_char) || isdigit(current_char) || current_char == '_') { state = 13; ++point_inbuff; }
				 else state = 14;
				 break;

		case 14: pointer_size = point_inbuff - pointer_start;
			//test every single possibility
			if (pointer_size == 8 && !memcmp(pointer_start, "continue", pointer_size))
			{
				token = add_token(CONTINUE);
				token->text = create_text(pointer_start, point_inbuff);
				return CONTINUE;
			}
			else if (pointer_size == 5 && !memcmp(pointer_start, "break", pointer_size))
			{
				token = add_token(BREAK);
				token->text = create_text(pointer_start, point_inbuff);
				return BREAK;
			}
			else if (pointer_size == 4 && !memcmp(pointer_start, "void", pointer_size))
			{
				token = add_token(VOID);
				token->text = create_text(pointer_start, point_inbuff);
				return VOID;
			}
			else if (pointer_size == 4 && !memcmp(pointer_start, "char", pointer_size))
			{
				token = add_token(CHAR);
				token->text = create_text(pointer_start, point_inbuff);
				return CHAR;
			}
			else if (pointer_size == 6 && !memcmp(pointer_start, "double", pointer_size))
			{
				token = add_token(DOUBLE);
				token->text = create_text(pointer_start, point_inbuff);
				return DOUBLE;
			}
			else if (pointer_size == 3 && !memcmp(pointer_start, "int", pointer_size))
			{
				token = add_token(INT);
				token->text = create_text(pointer_start, point_inbuff);
				return INT;
			}
			else if (pointer_size == 4 && !memcmp(pointer_start, "else", pointer_size))
			{
				token = add_token(ELSE);
				token->text = create_text(pointer_start, point_inbuff);
				return ELSE;
			}
			else if (pointer_size == 2 && !memcmp(pointer_start, "if", pointer_size))
			{
				token = add_token(IF);
				token->text = create_text(pointer_start, point_inbuff);
				return IF;
			}
			else if (pointer_size == 3 && !memcmp(pointer_start, "for", pointer_size))
			{
				token = add_token(FOR);
				token->text = create_text(pointer_start, point_inbuff);
				return FOR;
			}
			else if (pointer_size == 5 && !memcmp(pointer_start, "while", pointer_size))
			{
				token = add_token(WHILE);
				token->text = create_text(pointer_start, point_inbuff);
				return WHILE;
			}
			else if (pointer_size == 6 && !memcmp(pointer_start, "struct", pointer_size))
			{
				token = add_token(STRUCT);
				token->text = create_text(pointer_start, point_inbuff);
				return STRUCT;
			}
			else if (pointer_size == 6 && !memcmp(pointer_start, "return", pointer_size))
			{
				token = add_token(RETURN);
				token->text = create_text(pointer_start, point_inbuff);
				return RETURN;
			}

			else
			{
				token = add_token(ID);
				token->text = create_text(pointer_start, point_inbuff);
				return ID;
			}

		case 15: if (current_char == '\\') { state = 16; ++point_inbuff; }
				 else if (current_char != '\'' && current_char != '\\') { state = 17; ++point_inbuff; }
				 break;

		case 16: if (current_char == 'a' || current_char == 'b' || current_char == 'f' || current_char == 'n'
			|| current_char == 'r' || current_char == 't' || current_char == 'v' || current_char == '\''
			|| current_char == '?' || current_char == '\"' || current_char == '\\' || current_char == '0')
		{
			state = 17; ++point_inbuff;
		}
				 break;

		case 17: if (current_char == '\'') { state = 18; ++point_inbuff; }
				 break;

		case 18: token = add_token(CT_CHAR);
			token->int_val = create_char(pointer_start);
			return CT_CHAR;

		case 19: if (current_char == '\\') { state = 20; ++point_inbuff; }
				 else if (current_char != '\"' && current_char != '\\') { state = 21; ++point_inbuff; }
				 else if (current_char == '\"') { state = 22; ++point_inbuff; }
				 break;

		case 20: if (current_char == 'a' || current_char == 'b' || current_char == 'f' || current_char == 'n'
			|| current_char == 'r' || current_char == 't' || current_char == 'v' || current_char == '\''
			|| current_char == '?' || current_char == '\"' || current_char == '\\' || current_char == '0')
		{
			state = 21; ++point_inbuff;
		}
				 break;

		case 21: if (current_char == '\"') { state = 22; ++point_inbuff; }
				 else state = 19;
				 break;

		case 22: token = add_token(CT_STRING);
			token->text = create_string(pointer_start, point_inbuff);
			token->text = remove_special_characters(token->text);
			return CT_STRING;

		case 23: add_token(COMMA);
			return COMMA;

		case 24: add_token(SEMICOLOMN);
			return SEMICOLOMN;

		case 25: add_token(LPAR);
			return LPAR;

		case 26: add_token(RPAR);
			return RPAR;

		case 27: add_token(LBRACKET);
			return LBRACKET;

		case 28: add_token(RBRACKET);
			return RBRACKET;

		case 29: add_token(LACC);
			return LACC;

		case 30: add_token(RACC);
			return RACC;

		case 31: add_token(ADD);
			return ADD;

		case 32: add_token(SUB);
			return SUB;

		case 33: add_token(MUL);
			return MUL;

		case 34: add_token(DOT);
			return DOT;

		case 35: if (current_char == '&') { state = 36; ++point_inbuff; }
				 break;

		case 36: add_token(AND);
			return AND;

		case 37: if (current_char == '|') { state = 38; ++point_inbuff; }
				 break;

		case 38: add_token(OR);
			return OR;

		case 39: if (current_char == '=') { state = 41; ++point_inbuff; }
				 else state = 40;
				 break;

		case 40: add_token(NOT);
			return NOT;

		case 41: add_token(NOTEQ);
			return NOTEQ;

		case 42: if (current_char == '=') { state = 44; ++point_inbuff; }
				 else state = 43;
				 break;

		case 43: add_token(ASSIGN);
			return ASSIGN;

		case 44: add_token(EQUAL);
			return EQUAL;

		case 45: if (current_char == '=') { state = 47; ++point_inbuff; }
				 else state = 46;
				 break;

		case 46: add_token(LESS);
			return LESS;

		case 47: add_token(LESSEQ);
			return LESSEQ;

		case 48: if (current_char == '=') { state = 50; ++point_inbuff; }
				 else state = 49;
				 break;

		case 49: add_token(GREATER);
			return GREATER;

		case 50: add_token(GREATEREQ);
			return GREATEREQ;

		case 51: if (current_char == '/') { state = 52; ++point_inbuff; }
				 else if (current_char == '*') { state = 54; ++point_inbuff; }
				 else state = 53;
				 break;

		case 52: if (current_char != '\n' && current_char != '\r' && current_char != '\0') { state = 52; ++point_inbuff; }
				 else state = 0;
				 break;

		case 53: add_token(DIV);
			return DIV;

		case 54: if (current_char != '*') { state = 54; ++point_inbuff; }
				 else if (current_char == '*') { state = 55; ++point_inbuff; }
				 //here we add a new line in the comment part for every enter
				 if (current_char == '\n') line++;
				 break;

		case 55: if (current_char != '*' && current_char != '/') { state = 54; ++point_inbuff; }
				 else if (current_char == '*') { state = 55; ++point_inbuff; }
				 else if (current_char == '/') { state = 0; ++point_inbuff; }
				 break;

		case 56: add_token(END);
			return END;

		default: throw_error("Invalid State %d (%c)", state, current_char);
		}
	}
}


//main function
void main()
{
	printf("Program Starting...\n");

	//needed variables
	char inbuff[SIZE + 1];
	FILE *file;

	//open the specified file
	if ((file = fopen(file_name, "r")) == NULL)
	{
		printf("Error OPENING the file.");
		_getch();
		exit(-1);
	}

	//read the input text from the speficied file
	int text_len = fread(inbuff, 1, SIZE, file);
	inbuff[text_len] = '\0';
	fclose(file);

	//go through every element from the input text
	point_inbuff = inbuff;
	while (get_next_token() != END){}

	//print the token list for user to see
	print_token_list();

	//release the remaining allocated memory
	release_memory();

	//make the program CMD wait before closing
	printf("\n\nProgram Ended...\n");
	_getch();
}
