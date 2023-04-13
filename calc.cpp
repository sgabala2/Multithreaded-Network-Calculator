/*
* Implementation of calculator
* CSF Assignment 6
* Joy Neuberger and Shaina Gabala
* jneuber6@jhu.edu, sgabala2@jhu.edu
*/
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <ctype.h>
#include <algorithm>
#include <utility>

using::std::string;
using::std::vector;
using::std::pair;
using::std::cout;
using::std::endl;
using::std::map;

/* Forward declaration of the struct Calc data type. */
struct Calc {
	private:
		map<string, int> dictionary;
    pthread_mutex_t lock;

	public:
    Calc() { 
      pthread_mutex_init(&lock, NULL);
    }
    ~Calc() { 
			pthread_mutex_destroy(&lock);
		}

		pthread_mutex_t* getLock() { return &lock; }
		int evalExpr(const char* expr, int &result);

};


int undefinedVar() {
	cout << "Error: Undefined variable. A variable was used, but was not previously assigned a value." << endl;
	return 0;
}

/* Creates an instance of the struct Calc data type.
 * Return: a pointer to the calc.
 */
extern "C" struct Calc *calc_create(void) {
	return new Calc();
}

/* Determines if the first character of a string is a space character
 * Parameters: 
 *      s - the string to chek
 * Return: true if it is a space character, false if it isn't.
 */
bool isSpace(const string s) { 
	char c = s[0];
  return (c == ' ') || (c == '\r') || (c == '\t') || (c == '\n');
}

/* Determines if the first character of a string is a digit
 * Parameters: 
 *      s - the string to chek
 * Return: true if it is a digit, false if it isn't.
 */
bool isDigit(const string s) {
	char c = s[0]; 
	string s1 = &s[1];
	if((c > 47 && c < 58) || (c == '-' && isDigit(s1))) {
		return true;
	}
	return false ;
}

/* Checks that there is at least one space in between characters of a token.
 * Parameters: 
 *      s - the string to chek
 * Return: true if it spaces are valid, false if not.
 */
bool validSpaces(const string &s) {
	if(isDigit(s)) {
		return true;
	}

	return !(s.find('+')<s.length()) && !(s.find('-')<s.length()) && !(s.find('*')<s.length()) && !(s.find('/')<s.length()) && !(s.find('=')<s.length());
}

/* Tokenizes the expression to evaluate so that it is contained operator by opertor in a vecor.
 * Parameters: 
 *      expr - the expression to tokenize
 * Return: a vector with the tokenized expression.
 */
vector<string> tokenize(const string &expr) {
	vector<string> vec;
  std::stringstream s(expr);

  string tok;
  while (s >> tok) { 
		if(tok.length() > 1 && !validSpaces(tok)) {
			cout << "Error: Invalid Syntax. All operands and operators should be seperated by at least one space character." << endl;
			vec.clear();
			return vec;
		}
		vec.push_back(tok);
  }

  return vec;
}


/* Determines if the first character of a string is a letter 
 * Parameters: 
 *      s - the string to chek
 * Return: true if it is a letter, false if it isn't.
 */
bool isAlpha(const string s) { 
	char c = s[0];
	return (c > 64 && c < 91) || (c > 96 && c < 123);
}

/* Determines if the first character of a string is one of the valid operators
 * Parameters: 
 *      s - the string to chek
 * Return: true if it is a valid operator, false if it isn't.
 */
bool isOp(const string s) {
	char op = s[0];
	return (op == '+') || (op == '-') || (op == '*') || (op == '/');
}

/* Evaluate two integers and store there content's int the result variable
 * Parameters: 
 *      op - the operator type
 *			result - whre the result should be stored 
 *  		left - the left operand
 * 			right - the right operand
 * Return: 1 if successful, 0 if unsuccessful.
 */
int evalOp(const string op, int *result, int left, int right) {
  switch(op[0]) {
    case '-':
      *result = (left - right);
      break;
    case '+':
      *result = (left + right);
      break;
    case '*':
      *result = (left * right);
      break;
   	case '/':
     	if( right == 0 ) {
       	cout << "Error: Division by zero is undefined." << endl;
				return 0;
    	}
     	*result = (left / right);
     	break;     
  } 
	return 1;
}

/* Evaluates an expression and stores there content's int the result variable 
 * Parameters: 
 *      expr - the expression to evaluate
 * 			result - where the result should be stored
 *  		dic - the dictionary which should contain the variable
 * Return: the integer associated with a variable.
 */
int Calc::evalExpr(const char* expr, int &result) {
	int left;
	int right;
	bool assign = false;
	string to;
	vector<int> values;
	string s = expr;
	vector<string> tokenized = tokenize(s);
	if(tokenized.size() == 0) { return 0; }

	if (tokenized.size() > 1) {
		if(isAlpha(tokenized.at(0)) && (tokenized.at(1) == "=")) {
			to = tokenized.at(0);
			tokenized.erase (tokenized.begin());
			tokenized.erase (tokenized.begin());
			assign = true;
		}
	}

	for(unsigned int i = 0; i < tokenized.size(); i++) {
		if (isAlpha(tokenized.at(i))) { //var 
			if(dictionary.find(tokenized.at(i)) != dictionary.end()) { 
				values.push_back(dictionary.find(tokenized.at(i))->second);
			} else { 
				return undefinedVar(); 
			}
		} else if (isDigit(tokenized.at(i))) { 
			values.push_back(stoi(tokenized.at(i)));
		} else if (isOp(tokenized.at(i))) {
			if ((values.size() != 1)|| (i+1 == tokenized.size())) {
				cout << "Error: Invalid Syntax." << endl;
				return 0;
			} else if (isAlpha(tokenized.at(i+1))) {
				left = values.at(0);
				if (dictionary.find(tokenized.at(i+1)) != dictionary.end()) {
				  right = dictionary.find(tokenized.at(i+1))->second;
				} else { 
					return undefinedVar(); 
				}
				
				int ret = evalOp(tokenized.at(i), &result, left, right);
				if (ret != 0 && assign) {
					dictionary[to] = result;
				}
				return ret;
			} else if (isDigit(tokenized.at(i+1))) {
				left = values.at(0);
				right = stoi(tokenized.at(i+1));
				int ret = evalOp(tokenized.at(i), &result, left, right);
				if (ret != 0 && assign) {
					dictionary[to] = result;
				}
				return ret;
			}

		} else {
			cout << "Error: Invalid Syntax." << endl;
			return 0;
		}
	}

		//if we get here: 
		result = values.at(0);
		if (assign) {
			dictionary[to] = result;
		}
		return 1;
} 


extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
	int rc;
	pthread_mutex_lock(calc->getLock());
	rc = calc->evalExpr(expr, *result);
	pthread_mutex_unlock(calc->getLock());
	return rc;
}


extern "C" void calc_destroy(struct Calc *calc) {
	delete calc;
} 
