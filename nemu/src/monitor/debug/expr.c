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
  TK_NOTYPE = 256,
  
  HEX_NUM,REG_NUM,NAT_NUM,
  TK_EQ,TK_NE,TK_AND,
  NEG,POINTER,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spacesa


  {"\\+", '+'},         // plus
  {"-", '-'},           //sub and neg
  {"\\*", '*'},         //multiply
  {"\\/", '/'},         //divide
  {"\\(", '('},         //left-bracket
  {"\\)", ')'},         //right-bracket

  {"0[xX][a-fA-F0-9]+",HEX_NUM}, //hex number
  {"\\$[a-zA-Z]+",REG_NUM},      //register
  {"[0-9]+",NAT_NUM} ,  //natural number

  {"==", TK_EQ}     ,    // equal
  {"!=", TK_NE}      ,   //unequal
  {"&&", TK_AND}      ,  //and 



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

Token tokens[65536];
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
	
	   	 /* if(nr_token>31){
			printf("\033[01;31m TOO MANY INPUTS!!!PLEASE LESS OR EQUAL THAN 32!!! \033[0m \n");
			return false;}
*/
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

 //       Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
   //         i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
//printf("%d\n",position);
        switch (rules[i].token_type) {
			case TK_NOTYPE: break;
			case NAT_NUM:
					tokens[nr_token].type=rules[i].token_type;
					if(substr_len<32){memcpy(tokens[nr_token].str,substr_start,substr_len);}
					else{Log("Error");return false;}
					nr_token++;break;
			case HEX_NUM:
				    tokens[nr_token].type=rules[i].token_type;
					memcpy(tokens[nr_token].str,substr_start,substr_len);
					nr_token++;
					break;
			case REG_NUM:	
				    tokens[nr_token].type=rules[i].token_type;
					memcpy(tokens[nr_token].str,substr_start+1,substr_len-1);
					nr_token++;
					break;
			case '-':if(nr_token==0||((tokens[nr_token-1]).type!=HEX_NUM&&(tokens[nr_token-1]).type!=NAT_NUM&&(tokens[nr_token-1]).type!=')'))
					 {tokens[nr_token].type=NEG;
					 nr_token++;break;
					 }
			
			case '*':if(nr_token==0||((tokens[nr_token-1]).type!=HEX_NUM&&(tokens[nr_token-1]).type!=NAT_NUM&&(tokens[nr_token-1]).type!=')'))
					 {
						tokens[nr_token].type=POINTER;
						nr_token++;break;

					 }	 
			case '/':;

			case '+':;
	   		default: tokens[nr_token].type=rules[i].token_type;
					 nr_token++;
				   break;
    }

// printf("%d%s",tokens[nr_token].type,tokens[nr_token].str);   
        break;
      }
	 // ++nr_token;
	 //plus
	// printf("nr_token: %d\n",nr_token);
	  // break;
    }
//printf("%d%s",tokens[nr_token].type,tokens[nr_token].str);
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
//printf("out of check par\n");
	return true;
	}
  
/*int priority(int i){
switch (tokens[i].type){
	case (int)'+':return 5;
	case (int)'-':return 5;
	case (int)'*':return 2;
	case (int)'/':return 2;
	default:return -1;
}
}
  */
//find the op_main;
uint32_t getOp(uint32_t p,uint32_t q)
{
	int par=0;
	int op=p;
	int pri=0;
	for(;p<=q;p++)
	{
		if(tokens[p].type==NAT_NUM||tokens[p].type==HEX_NUM||tokens[p].type==NEG)
			continue;
		else if(tokens[p].type=='(')
		{
			par++;
			p++;
			while(par!=0){
			if(tokens[p].type=='(')par++;
			else if(tokens[p].type==')')par--;
			p++;}
			p--;
		}
		else if(tokens[p].type=='/'||tokens[p].type=='*'){
		if(pri<=3){
		op=p;pri=3;
		}
		}
		else if(tokens[p].type=='+'||tokens[p].type=='-'){
		if(pri<=4){
		op=p;pri=4;
			}
		}
		else if(tokens[p].type==TK_EQ||tokens[p].type==TK_NE)
		{
		if(pri<=6)
		{
		op=p;pri=6;
		}
		}
        else if(tokens[p].type==TK_AND)
		{
		if(pri<=10)
		{
		op=p;pri=10;
		}
		}
	}
	return op;
}
	
uint32_t eval(int p,int q){
//plus
//printf("orignal p=%d,q=%d",p,q);
int tempresult;
int tempresult2;
	if(p>q){
		printf("overflow:p>q,p=%d,q=%d\n",p,q);
		assert(0);
	}
	else if(p==q){
//  plus
//	printf("stuck in p==q");
//	char regwhat[32];
	switch(tokens[p].type){

		case NAT_NUM://int tempresult=0;
				     sscanf(tokens[p].str,"%d",&tempresult);
				     return tempresult;
					 
		case HEX_NUM://int tempresult2=0;
					 tempresult2=strtol(tokens[p].str,NULL,16);
					 return tempresult2;
					 		 
		case REG_NUM:
					if(strcmp(tokens[p].str,"eax")==0)return cpu.eax; 
					 
					else if(strcmp(tokens[p].str,"ebx")==0)return cpu.ebx; 		
					 
					else if(strcmp(tokens[p].str,"ecx")==0)return cpu.ecx; 
					 
					else if(strcmp(tokens[p].str,"edx")==0)return cpu.edx; 
					 
					else if(strcmp(tokens[p].str,"esp")==0)return cpu.esp; 
					 
					else if(strcmp(tokens[p].str,"ebp")==0)return cpu.ebp; 
					 
					else if(strcmp(tokens[p].str,"esi")==0)return cpu.esi; 
				 
					else if(strcmp(tokens[p].str,"eip")==0)return cpu.eip; 
					else if(strcmp(tokens[p].str,"edi")==0)return cpu.edi; 
	    default: printf("type overflow");assert(0);				 		 
         	}
	}
	else if(check_parentheses(p,q)==true){

		return eval(p+1,q-1);
			}
	else if(tokens[p].type==NEG){
	return -eval(p+1,q);
	}
	else if(tokens[p].type==POINTER)
	{
	return vaddr_read(eval(p+1,q),4);	
	}
	 else {
/*int if_check=0;
int if_main=true;
int main_op=p;
for (int i=p;i<=q;i++)
{
	if_main=(if_check==0);
	if(tokens[i].type=='('){
		if_check++;}
	if(tokens[i].type==')'){
		if_check--;}
	if(if_main){
		if(tokens[i].type=='+'||tokens[i].type=='-'){
			main_op=i;
			break;
		}else if(tokens[i].type=='*'||tokens[i].type=='/'){
			main_op=i;
		}
		
	}
}

*/

	uint32_t op=getOp(p,q);
	//plus
//	printf("stuck in op");
//	printf("p=%d,q=%d",p,q);
//	printf("mainop=%d\n",op);

		uint32_t val1=eval(p,op-1);
		//plus
//		printf("val1=%d\n ",val1);
		uint32_t val2=eval(op+1,q);
//		printf("val2=%d\n ",val2);
		switch(tokens[op].type){
			case '+':    return val1+val2;
			case '-':    return val1-val2;
			case '*':    return val1*val2;
			case '/':    return val1/val2;
			case TK_EQ:  return val1==val2;
			case TK_NE:  return val1!=val2;	 
            case TK_AND: return val1&&val2;

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
  else{
	 
	  *success=true;
	//  assert (0);
	  return eval(0,nr_token-1);
  }
  return 0;
}
