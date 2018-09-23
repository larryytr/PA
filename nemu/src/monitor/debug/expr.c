#include "nemu.h"
#define COLORRED " \033[1;33m"
#define COLORNORMAL "\033[0m"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
enum {
  TK_NOTYPE = 256, TK_EQ,NAT_NUM,NEG

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},           //sub and neg
  {"\\*", '*'},         //multiply
  {"\\/", '/'},         //divide
  {"\\(", '('},         //left-bracket
  {"\\)", ')'},         //right-bracket
  {"[0-9]+",NAT_NUM} ,  //natural number
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* R333ules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
	
	  	  if(nr_token>31){
			printf("\033[01;31m TOO MANY INPUTS!!!PLEASE LESS OR EQUAL THAN 32!!! \033[0m \n");
			return false;}

        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
int j=0;
        switch (rules[i].token_type) {
			case TK_NOTYPE: break;
			case NAT_NUM:
					for(j=0;j<substr_len;j++){
					tokens[nr_token].str[j]=e[position+j];
					}
		/*	case '-':if(nr_token==0)
					 {tokens[nr_token].type=NEG;}
					 else {tokens[nr_token].type=rules[i].token_type;
					 }
*/
	   		default: tokens[nr_token].type=rules[i].token_type;
				   break;
        }
    
        break;
      }
	  ++nr_token;
	 // break;
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
//begin
bool check_parentheses(int p,int q){
	int i,par=0;
	if(tokens[p].type!='('||tokens[q].type!=')')
	return false;
	for(i=p;i<q;++i){
		if(tokens[i].type=='(')
		par++;
		else if(tokens[i].type==')')
		par--;
		if(par==0&&i<q)
		return false;
}
//plus
printf("out of check par\n");
	return true;
	}
  
int priority(int i){
switch (i){
	case (int)'+':return 5;
	case (int)'-':return 5;
	case (int)'*':return 2;
	case (int)'/':return 2;
	default:return -1;
}
}
  
int dominant_operator_place(int p,int q){
//plus
 printf("in dominant_operator\n");
	int i;
	int pri,dom;
	int cnt;
	dom=-1;
	pri=20;
	int min=20;
	for( i=p;i<=q;++i){ 
		cnt=0;
		if(tokens[i].type=='('){
			i++;cnt++;
			while(1)
			{if(tokens[i].type=='(')
				++cnt;
			else if(tokens[i].type==')')
				--cnt;
			i++;
			if(cnt==0)
			break;}
			if(i>q)
				break;
 		}
//	tp=tokens[i].type;
	//wait to continue
	pri=priority(i);
	if(pri<min&&pri!=-1)
	{	min=pri;
		dom=i;
		}
	}
	if(pri==-1){
	printf(COLORRED "The operator can't be found here.\n" COLORNORMAL);
	assert(0);
	}
	//plus
	printf("out of dom\n");
	return dom;
}
	
uint32_t eval(int p,int q){
printf("in eval!");
	if(p>q){
		printf("overflow:p>q,p=%d,q=%d\n",p,q);
		assert(0);
	}
	else if(p==q){
		int type=tokens[p].type;
//	uint32_t t=0;
//  plus
	printf("stuck in p==q");
	switch(type){
		case NAT_NUM:return atoi(tokens[p].str);

		default:assert(0);
	}
	}
	else if(check_parentheses(p,q)==true){

		return eval(p+1,q-1);
			}
	 else {
int op=dominant_operator_place(p,q) ;
	//plus
	printf("stuck in op");


		int val1=eval(p,op-1);
		int val2=eval(op+1,q);
		switch(tokens[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;



			default:assert(0);
			}
		}
}

uint32_t expr(char *e, bool *success) {

  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
 // TODO();
  else{*success=true;
	  return eval(0,nr_token-1);
  }
  return 0;
}
