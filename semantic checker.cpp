/*----------------------------------------------------------------------------
 Note: the code in this file is not to be shared with anyone or posted online.
 (c) Rida Bazzi, 2015, Adam Doupe, 2015
 ----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "syntax.h"



/* ------------------------------------------------------- */
/* -------------------- LEXER SECTION -------------------- */
/* ------------------------------------------------------- */

#define KEYWORDS  11
void addtonk_varassign_util(struct exprNode* expr,char *right);
 int flag=0;
 char *imptype;
 struct nknode *nklist = NULL;
  struct newlist *newlist1 = NULL;
typedef enum
{
    END_OF_FILE = -1, VAR = 1, WHILE, INT, REAL, STRING, BOOLEAN,
    TYPE, LONG, DO, CASE, SWITCH,
    PLUS, MINUS, DIV, MULT, EQUAL,
    COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN, LBRACE, RBRACE,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ, DOT,
    ID, NUM, REALNUM,
    ERROR
} token_type;

const char *reserved[] = {"",
    "VAR", "WHILE", "INT", "REAL", "STRING", "BOOLEAN",
    "TYPE", "LONG", "DO", "CASE", "SWITCH",
    "+", "-", "/", "*", "=",
    ":", ",", ";",
    "[", "]", "(", ")", "{", "}",
    "<>", ">", "<", "<=", ">=", ".",
    "ID", "NUM", "REALNUM",
    "ERROR"
};

// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH]; // token string
token_type t_type; // token type
bool activeToken = false;
int tokenLength;
int dangerflag=0;
int line_no = 1;
char *c2_global;
struct newlist* insertnew(struct newlist *head,char *value);
void chkifthereandupdate(char *val,char *updateval);
int c2_flag=0;
char *rightback ;
int cflag=0;
int finalflag=0;
int nk_l_flag=0;
int nk_r_flag=0;
int lb_varflag = 0;
int rb_varflag = 0;
void recursivecheck(char *rightval);
void assignval(char *compare,char *compare1);
void searchandprintl(char *compare1);
//new linkedlist
int c4_varflag=0;
char *leftback;
int c5_varflag=0;
void c3_check(struct conditionNode *condition);
void c4_check(struct conditionNode *condition);
struct nknode* insert(struct nknode *head,char *leftval1,char *rightval1,int impexpflag1,int typevarflag1,int not_builtin1)
{
//printf("HELLO");
    struct nknode *new = (struct nknode*)malloc(sizeof(struct nknode));
  
   
    new->leftval = malloc(strlen(leftval1) + 1);
    strcpy(new->leftval,leftval1);
    new->rightval = malloc(strlen(rightval1) + 1);
    strcpy(new->rightval,rightval1);
    
    new->impexpflag = impexpflag1;
    new->typevarflag = typevarflag1;
    new->not_builtin = not_builtin1;
    
    
    new->next = NULL;
    struct nknode *cur = head;
    if(head==NULL)
    {
        head = new;
        return head;
    }
    while(cur->next!=NULL)
    {
	    cur = cur->next;
    }

    cur->next = new;
    return head;
}



struct newlist* insertnew(struct newlist *head,char *value)
{
//printf("HELLO");
    struct newlist *new = (struct newlist*)malloc(sizeof(struct newlist));
  
   
    new->val = malloc(strlen(value) + 1);
    strcpy(new->val,value);
    
    new->next = NULL;
    struct newlist *cur = head;
    if(head==NULL)
    {
        head = new;
        return head;
    }
    while(cur->next!=NULL)
    {
	    cur = cur->next;
    }

    cur->next = new;
    return head;
}



void skipSpace()
{
    char c;

    c = getchar();
    line_no += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line_no += (c == '\n');
    }
    ungetc(c, stdin);
}

int isKeyword(char *s)
{
    int i;

    for (i = 1; i <= KEYWORDS; i++)
    {
        if (strcmp(reserved[i], s) == 0)
        {
            return i;
        }
    }
    return false;
}

/*
 * ungetToken() simply sets a flag so that when getToken() is called
 * the old t_type is returned and the old token is not overwritten.
 * NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
 * AT LEAST ONE CALL TO getToken(). CALLING TWO ungetToken() WILL NOT
 * UNGET TWO TOKENS
 */
void ungetToken()
{
    activeToken = true;
}

token_type scan_number()
{
    char c;

    c = getchar();
    if (isdigit(c))
    {
        // First collect leading digits before dot
        // 0 is a NUM by itself
        if (c == '0')
        {
            token[tokenLength] = c;
            tokenLength++;
            token[tokenLength] = '\0';
        }
        else
        {
            while (isdigit(c))
            {
                token[tokenLength] = c;
                tokenLength++;
                c = getchar();
            }
            ungetc(c, stdin);
            token[tokenLength] = '\0';
        }
        // Check if leading digits are integer part of a REALNUM
        c = getchar();
        if (c == '.')
        {
            c = getchar();
            if (isdigit(c))
            {
                token[tokenLength] = '.';
                tokenLength++;
                while (isdigit(c))
                {
                    token[tokenLength] = c;
                    tokenLength++;
                    c = getchar();
                }
                token[tokenLength] = '\0';
                if (!feof(stdin))
                {
                    ungetc(c, stdin);
                }
                return REALNUM;
            }
            else
            {
                ungetc(c, stdin);
                c = '.';
                ungetc(c, stdin);
                return NUM;
            }
        }
        else
        {
            ungetc(c, stdin);
            return NUM;
        }
    }
    else
    {
        return ERROR;
    }
}

token_type scan_id_or_keyword()
{
    token_type the_type;
    int k;
    char c;

    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c))
        {
            token[tokenLength] = c;
            tokenLength++;
            c = getchar();
        }
        if (!feof(stdin))
        {
            ungetc(c, stdin);
        }
        token[tokenLength] = '\0';
        k = isKeyword(token);
        if (k == 0)
        {
            the_type = ID;
        }
        else
        {
            the_type = (token_type) k;
        }
        return the_type;
    }
    else
    {
        return ERROR;
    }
}

token_type getToken()
{
    char c;

    if (activeToken)
    {
        activeToken = false;
        return t_type;
    }
    skipSpace();
    tokenLength = 0;
    c = getchar();
    switch (c)
    {
        case '.': return DOT;
        case '+': return PLUS;
        case '-': return MINUS;
        case '/': return DIV;
        case '*': return MULT;
        case '=': return EQUAL;
        case ':': return COLON;
        case ',': return COMMA;
        case ';': return SEMICOLON;
        case '[': return LBRAC;
        case ']': return RBRAC;
        case '(': return LPAREN;
        case ')': return RPAREN;
        case '{': return LBRACE;
        case '}': return RBRACE;
        case '<':
            c = getchar();
            if (c == '=')
            {
                return LTEQ;
            }
            else if (c == '>')
            {
                return NOTEQUAL;
            }
            else
            {
                ungetc(c, stdin);
                return LESS;
            }
        case '>':
            c = getchar();
            if (c == '=')
            {
                return GTEQ;
            }
            else
            {
                ungetc(c, stdin);
                return GREATER;
            }
        default:
            if (isdigit(c))
            {
                ungetc(c, stdin);
                return scan_number();
            }
            else if (isalpha(c))
            {
                ungetc(c, stdin);
                return scan_id_or_keyword();
            }
            else if (c == EOF)
            {
                return END_OF_FILE;
            }
            else
            {
                return ERROR;
            }
    }
}


/* ----------------------------------------------------------------- */
/* -------------------- SYNTAX ANALYSIS SECTION -------------------- */
/* ----------------------------------------------------------------- */

void syntax_error(const char* msg)
{
    printf("Syntax error while parsing %s line %d\n", msg, line_no);
    exit(1);
}

/* -------------------- PRINTING PARSE TREE -------------------- */
/*
void print_parse_tree(struct programNode* program)
{
    print_decl(program->decl);
    print_body(program->body);
}

void print_decl(struct declNode* dec)
{
    if (dec->type_decl_section != NULL)
    {
        print_type_decl_section(dec->type_decl_section);
    }
    if (dec->var_decl_section != NULL)
    {
        print_var_decl_section(dec->var_decl_section);
    }
}

void print_body(struct bodyNode* body)
{
    printf("{\n");
    print_stmt_list(body->stmt_list);
    printf("}\n");
}

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
    printf("VAR\n");
    if (varDeclSection->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclSection->var_decl_list);
    }
}

void print_var_decl_list(struct var_decl_listNode* varDeclList)
{
    print_var_decl(varDeclList->var_decl);
    if (varDeclList->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclList->var_decl_list);
    }
}

void print_var_decl(struct var_declNode* varDecl)
{
    print_id_list(varDecl->id_list);
    printf(": ");
    print_type_name(varDecl->type_name);
    printf(";\n");
}

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
    printf("TYPE\n");
    if (typeDeclSection->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclSection->type_decl_list);
    }
}

void print_type_decl_list(struct type_decl_listNode* typeDeclList)
{
    print_type_decl(typeDeclList->type_decl);
    if (typeDeclList->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclList->type_decl_list);
    }
}

void print_type_decl(struct type_declNode* typeDecl)
{
    print_id_list(typeDecl->id_list);
    printf(": ");
    print_type_name(typeDecl->type_name);
    printf(";\n");
}

void print_type_name(struct type_nameNode* typeName)
{
    if (typeName->type != ID)
    {
        printf("%s ", reserved[typeName->type]);
    }
    else
    {
        printf("%s ", typeName->id);
    }
}

void print_id_list(struct id_listNode* idList)
{
    printf("%s ", idList->id);
    if (idList->id_list != NULL)
    {
        printf(", ");
        print_id_list(idList->id_list);
    }
}

void print_stmt_list(struct stmt_listNode* stmt_list)
{
    print_stmt(stmt_list->stmt);
    if (stmt_list->stmt_list != NULL)
    {
        print_stmt_list(stmt_list->stmt_list);
    }

}

void print_assign_stmt(struct assign_stmtNode* assign_stmt)
{
    printf("%s ", assign_stmt->id);
    printf("= ");
    print_expression_prefix(assign_stmt->expr);
    printf("; \n");
}

void print_stmt(struct stmtNode* stmt)
{
    switch (stmt->stmtType)
    {
        case ASSIGN:
            print_assign_stmt(stmt->assign_stmt);
            break;
        case WHILE:
            print_while_stmt(stmt->while_stmt);
            break;
        case DO:
            print_do_stmt(stmt->while_stmt);
            break;
        case SWITCH:
            print_switch_stmt(stmt->switch_stmt);
            break;
    }
}

void print_expression_prefix(struct exprNode* expr)
{
    if (expr->tag == EXPR)
    {
        printf("%s ", reserved[expr->op]);
        print_expression_prefix(expr->leftOperand);
        print_expression_prefix(expr->rightOperand);
    }
    else if (expr->tag == PRIMARY)
    {
        if (expr->primary->tag == ID)
        {
            printf("%s ", expr->primary->id);
        }
        else if (expr->primary->tag == NUM)
        {
            printf("%d ", expr->primary->ival);
        }
        else if (expr->primary->tag == REALNUM)
        {
            printf("%.4f ", expr->primary->fval);
        }
    }
}

void print_while_stmt(struct while_stmtNode* while_stmt)
{
    // TODO: implement this for your own debugging purposes
 /*   printf("WHILE");
   print_condition(while_stmt->condition);
    print_body(while_stmt->body);

}

void print_do_stmt(struct while_stmtNode* do_stmt)
{
    // TODO: implement this for your own debugging purposes
  /*   printf("DO");
     print_body(do_stmt->body);
     printf("WHILE");
     print_condition(do_stmt->condition);
     printf(";");
   
   
}



void print_case(struct caseNode* cas)
{
    // TODO: implement this for your own debugging purposes
   
}

void print_case_list(struct case_listNode* case_list)
{
    // TODO: implement this for your own debugging purposes
}

void print_switch_stmt(struct switch_stmtNode* switc)
{
    // TODO: implement this for your own debugging purposes
}

*/

/* -------------------- PARSING AND BUILDING PARSE TREE -------------------- */

// Note that the following function is not
// called case because case is a keyword in C/C++

void print_condition(struct conditionNode* condition)
{
    // TODO: implement this for your own debugging purposes
    if(condition->relop)
    {
        //left operand
        if(strcmp(reserved[condition->left_operand->tag],"ID")==0)
            printf("%s",condition->left_operand->id);
         else if(strcmp(reserved[condition->left_operand->tag],"NUM")==0)
            printf("%d",condition->left_operand->ival);
            else if(strcmp(reserved[condition->left_operand->tag],"REALNUM")==0)
            printf("%f",condition->left_operand->fval);
    
       printf("%s",reserved[condition->relop]);
        
        //right operand
        
         if(strcmp(reserved[condition->right_operand->tag],"ID")==0)
            printf("%s",condition->right_operand->id);
         else if(strcmp(reserved[condition->right_operand->tag],"NUM")==0)
            printf("%d",condition->right_operand->ival);
            else if(strcmp(reserved[condition->right_operand->tag],"REALNUM")==0)
            printf("%f",condition->right_operand->fval);
        
    }
    else
    {
         //left operand alone
        if(strcmp(reserved[condition->left_operand->tag],"ID")==0)
            printf("%s",condition->left_operand->id);
         else if(strcmp(reserved[condition->left_operand->tag],"NUM")==0)
            printf("%d",condition->left_operand->ival);
            else if(strcmp(reserved[condition->left_operand->tag],"REALNUM")==0)
            printf("%f",condition->left_operand->fval);
    
    }
    
}



struct caseNode* cas()
{
    // TODO: implement this for extra credit
     struct caseNode *obj;
    obj = ALLOC(struct caseNode);
    t_type = getToken();
    if(t_type==CASE)
    {
        t_type = getToken();
        if(t_type==NUM)
        {
             obj->num = atoi(token);
             t_type = getToken();
             if(t_type==COLON)
             {
                t_type = getToken(); //check for lbrace and unget
                if(t_type == LBRACE)
                {
                    ungetToken();
                    obj->body = body();
                }
                else
                {
                    syntax_error("body LBRACE cas failed--"); 
                }
        
            }
            else
            {
                syntax_error("colon cas failed--"); 
            }
    
        }
        else
        {
            syntax_error("NUM check cas failed--"); 
        }
        return obj;
    }
    else
    {
        syntax_error("CASE keyword cas failed--"); 
    }
    
    return NULL;
}

struct case_listNode* case_list()
{
    // TODO: implement this for extra credit
     struct case_listNode *obj;
    obj = ALLOC(struct case_listNode);
    t_type = getToken();
    if(t_type==CASE)
    {
        ungetToken();
        obj->cas = cas();
        
            t_type = getToken(); //check another
            if(t_type==CASE){
            ungetToken();
                obj->case_list = case_list();
            }
            else if(t_type == RBRACE)
            {
                ungetToken();
            }
            else
            {
                 syntax_error("CASE keyword case_list failed--"); 
            }
            
        return obj;
    }
    else
    {
        syntax_error("CASE keyword case_list failed--"); 
    }
    
    return NULL;
}


void c5_check(struct switch_stmtNode *obj){

char *compareval = (char *)malloc(1000);
char *compare = (char *)malloc(1000);
strcpy(compareval,obj->id);

//its always ID only in switch
                struct nknode *temp = nklist;
                strcpy(rightback,"");
                strcpy(compare,"");
                while(temp!=NULL)
                {
                    if(strcmp(temp->leftval,compareval)==0)
                    {
                    
                           strcpy(rightback,temp->rightval);
                           recursivecheck(temp->rightval);
                            strcpy(compare,rightback);
                    }
                temp = temp->next;
                }
                
              if(strcmp(compare,"NUM")==0)
        strcpy(compare,"INT");
        else if(strcmp(compare,"REALNUM")==0)
        strcpy(compare,"REAL");
        
        
          
          
         if(strcmp(compare,"INT")!=0 && strcmp(compare,"REAL")!=0 && strcmp(compare,"STRING")!=0 && strcmp(compare,"BOOLEAN")!=0  && strcmp(compare,"TYPE")!=0 && strcmp(compare,"LONG")!=0 && strcmp(compare,"NUM")!=0 && strcmp(compare,"REALNUM")!=0){
       //  printf("HI");
         c5_varflag = 1;
         }
         
       
       
       
       //assign will be left will be assinged to right eg: int to t
        if(c5_varflag==1)
        {
            assignval("INT",compare);
           //searchandprintl(compare);
            strcpy(compare,"INT");
        }
        
       
        
           //INT check
       if(strcmp(compare,"INT")!=0)
       {
        
                printf("TYPE MISMATCH %d C5",line_no);
                exit(0);
        }
}

struct switch_stmtNode* switch_stmt()
{
    // TODO: implement this for extra credit
    struct switch_stmtNode *obj;
    obj = ALLOC(struct switch_stmtNode);
    t_type = getToken();
    if(t_type==SWITCH)
    {
        t_type = getToken();
        if(t_type==ID)
        {
            obj->id = strdup(token);
            t_type = getToken();
            if(t_type==LBRACE)
            {
                t_type = getToken();
                if(t_type==CASE)
                {
                    ungetToken();
                    obj->case_list = case_list();
                    t_type = getToken();
                    if(t_type!=RBRACE)
                    {
                         syntax_error("RBRACE switch_stmt failed--"); 
                    }
                    
                }
                else{
                     syntax_error("CASE switch_stmt failed--"); 
                }
            }
            else
            {
                 syntax_error("LBRACE switch_stmt failed--"); 
            }
        }
        else
        {
             syntax_error("ID switch_stmt failed--"); 
        }
    c5_check(obj);    
    return obj;
    }
    else
    {
     syntax_error("SWITCH switch_stmt failed--"); 
     }
    
    
    
    return NULL;
}

struct while_stmtNode* do_stmt()
{
    // TODO: implement this
    
     struct while_stmtNode *obj;
    obj = ALLOC(struct while_stmtNode);
    t_type = getToken();
    if(t_type == DO)
    {
        t_type = getToken(); //check for lbrace and unget
        if(t_type == LBRACE)
        {
            ungetToken();
            obj->body = body();
            t_type = getToken();
            if(t_type == WHILE)
            {
                t_type = getToken();
                if(t_type == ID ||t_type ==  NUM ||t_type ==  REALNUM)
                {
                    ungetToken();
                    obj->condition = condition();
                     t_type = getToken();
                    if(t_type != SEMICOLON)
                    {
                    syntax_error("Semicolon do_stmt failed--"); 
                    }
                }
                else
                {
                     syntax_error("condition do_stmt failed--"); 
                }
               
         
            }
            else
            {
                 syntax_error("while keyword do_stmt failed--"); 
            }
        }
        else
        {
             syntax_error("LBRACE body do_stmt failed--"); 
        }
        return obj;
    }
    else
    {
        syntax_error("DO check do_stmt failed--"); 
    }
    
    return NULL;
}



struct primaryNode* primary()
{
    // TODO: implement this
    struct primaryNode *obj;
     obj = ALLOC(struct primaryNode);
     t_type = getToken();
    if(t_type == ID || t_type == NUM || t_type == REALNUM)
    {
        if(t_type==ID)
        {
            obj->tag = t_type;
            obj->id =  strdup(token);
            
        }
        else if(t_type==NUM)
        {
            obj->tag = t_type;
            obj->ival = atoi(token);
        }
        else if(t_type==REALNUM)
        {
            obj->tag = t_type;
            obj->fval = atof(token);
           // printf("inside prim:%f",obj->fval);
        }
        else
        {
             syntax_error("inside primary check failed--"); 
        }
       
        return obj;
    
    }
    else
    {
     syntax_error("primary failed--"); 
    }
    
    return NULL;
}

struct conditionNode* condition()
{
    // TODO: implement this
    rightback= (char *)malloc(1000);
    struct conditionNode *obj;
    obj = ALLOC(struct conditionNode);
    t_type = getToken();
    if(t_type == ID || t_type == NUM || t_type == REALNUM)
    {
          ungetToken();
         obj->left_operand = primary();
         t_type = getToken();
         //relop check
         if(t_type == GREATER ||t_type ==  GTEQ||t_type ==  LESS||t_type ==  NOTEQUAL||t_type ==  LTEQ)
         {
            obj->relop = t_type;
            t_type = getToken();
            if(t_type == ID || t_type == NUM || t_type == REALNUM)
            {
            ungetToken();
                 obj->right_operand = primary();
            }
            else
            {
                syntax_error(" second primary check failed--");
            }
           
         }
         else if(t_type == LBRACE || t_type == SEMICOLON)
         {
            ungetToken();
            
         }
         else
         {
            syntax_error("other check failed--");
         }
         
    // print_condition(obj);   
    c3_check(obj);
    c4_check(obj);
    return obj;
    }
    else
    {
    
     syntax_error("condition check failed--"); 
     }
    
    return NULL;
}

void c3_check(struct conditionNode *condition)
{
 lb_varflag = 0;
 rb_varflag = 0;
char *compare = (char *)malloc(1000);
char *compare1 = (char *)malloc(1000);

 if(condition->relop)
    {
        //left operand
       if(strcmp(reserved[condition->left_operand->tag],"ID")==0)
         //   printf("%s",condition->left_operand->id);
            
          {
                //search and find the id type
                struct nknode *temp = nklist;
              strcpy(rightback,"");
               strcpy(compare,"");
                while(temp!=NULL)
                {
                    if(strcmp(temp->leftval,condition->left_operand->id)==0)
                    {
               //     printf("RIGHT:%s",temp->rightval);
                    
                             strcpy(rightback,temp->rightval);
                           recursivecheck(temp->rightval);
                         strcpy(compare,rightback);
                    
                    }
                
                temp = temp->next;
                }
            
            
            }
         else if(strcmp(reserved[condition->left_operand->tag],"NUM")==0)
           strcpy(compare,reserved[condition->left_operand->tag]);
            else if(strcmp(reserved[condition->left_operand->tag],"REALNUM")==0)
            strcpy(compare,reserved[condition->left_operand->tag]);
    
     //  printf("%s",reserved[condition->relop]);
        
        //right operand
        
       if(strcmp(reserved[condition->right_operand->tag],"ID")==0)
         //   printf("%s",condition->left_operand->id);
            
          {
                //search and find the id type
                struct nknode *temp = nklist;
               strcpy(rightback,"");
                strcpy(compare1,"");
                while(temp!=NULL)
                {
                    if(strcmp(temp->leftval,condition->right_operand->id)==0)
                    {
                     //printf("RIGHT2:%s",temp->rightval);
                      strcpy(rightback,temp->rightval);
                         recursivecheck(temp->rightval);
                         strcpy(compare1,rightback);
                
                    
                    }
                
                temp = temp->next;
                }
            
            
            
            }
         else if(strcmp(reserved[condition->right_operand->tag],"NUM")==0)
           strcpy(compare1,reserved[condition->right_operand->tag]);
            else if(strcmp(reserved[condition->right_operand->tag],"REALNUM")==0)
            strcpy(compare1,reserved[condition->right_operand->tag]);
            
        //   printf("compare:%s",compare);
            
         //  printf("compare1:%s",compare1);
         
           if(strcmp(compare,"NUM")==0)
        strcpy(compare,"INT");
        else if(strcmp(compare,"REALNUM")==0)
        strcpy(compare,"REAL");
        
        
         if(strcmp(compare1,"NUM")==0)
        strcpy(compare1,"INT");
        else if(strcmp(compare1,"REALNUM")==0)
        strcpy(compare1,"REAL");
          
          
          
         if(strcmp(compare,"INT")!=0 && strcmp(compare,"REAL")!=0 && strcmp(compare,"STRING")!=0 && strcmp(compare,"BOOLEAN")!=0  && strcmp(compare,"TYPE")!=0 && strcmp(compare,"LONG")!=0 && strcmp(compare,"NUM")!=0 && strcmp(compare,"REALNUM")!=0){
       //  printf("HI");
         lb_varflag = 1;
         }
         
         
         if(strcmp(compare1,"INT")!=0 && strcmp(compare1,"REAL")!=0 && strcmp(compare1,"STRING")!=0 && strcmp(compare1,"BOOLEAN")!=0  && strcmp(compare1,"TYPE")!=0 && strcmp(compare1,"LONG")!=0 && strcmp(compare1,"NUM")!=0 && strcmp(compare1,"REALNUM")!=0){
         
        // printf("Hs");
         rb_varflag = 1;
         }
         
   //    printf("compare:%s\n",compare);
//printf("compare1:%s\n",compare1); 
//printf("L:%d",lb_varflag);
//printf("R:%d",rb_varflag);
         
         
     
    //assign will be left will be assinged to right eg: int to t
    if(lb_varflag==0 && rb_varflag==1){
    assignval(compare,compare1);
   //searchandprintl(compare);
    strcpy(compare1,compare);
    }
    //other way t to int
    
    else if(lb_varflag==1 && rb_varflag==0){
    assignval(compare1,compare);
  //  searchandprintl(compare1);
    strcpy(compare,compare1);
    }
    
    else if(lb_varflag==1 && rb_varflag==1){
       
    
    
        //propogate compare1 on compare coz compare is null
        if(strcmp(compare,"")==0)
        {
         assignval(compare1,compare);
    //   searchandprintl(compare1);
          strcpy(compare,compare1);
        }
        //propogate compare on compare1 coz compare1 is null
        else if(strcmp(compare1,"")==0)
        {
         assignval(compare,compare1);
      //   searchandprintl(compare1);
            strcpy(compare1,compare);
        }
    
    }    
         
         
         
         
         
         
          
         //   printf("varflag:%d",varflag);
            if(strcmp(compare,compare1)!=0)
            {
             printf("TYPE MISMATCH %d C3",line_no);
             exit(0);
            }
        
    }
    
    
    
    

}


void c4_check(struct conditionNode *condition)
{
char *compare = (char *)malloc(1000);
char *val = (char *)malloc(1000);

 if(!condition->relop)
    {
        if(strcmp(reserved[condition->left_operand->tag],"ID")==0)
        {
            strcpy(val,condition->left_operand->id);
            struct nknode *temp = nklist;
            strcpy(rightback,"");
            strcpy(compare,"");
                while(temp!=NULL)
                {
                    if(strcmp(temp->leftval,val)==0)
                    {
                     //printf("RIGHT2:%s",temp->rightval);
                      strcpy(rightback,temp->rightval);
                         recursivecheck(temp->rightval);
                         strcpy(compare,rightback);
                
                    
                    }
                
                temp = temp->next;
                }
        
        if(strcmp(compare,"NUM")==0)
        strcpy(compare,"INT");
        else if(strcmp(compare,"REALNUM")==0)
        strcpy(compare,"REAL");
        
        
         if(strcmp(compare,"INT")!=0 && strcmp(compare,"REAL")!=0 && strcmp(compare,"STRING")!=0 && strcmp(compare,"BOOLEAN")!=0  && strcmp(compare,"TYPE")!=0 && strcmp(compare,"LONG")!=0 && strcmp(compare,"NUM")!=0 && strcmp(compare,"REALNUM")!=0)
         {
       //  printf("HI");
         c4_varflag = 1;
         }
        
        
        //assign will be left will be assinged to right eg: int to t
        if(c4_varflag==1)
        {
            assignval("BOOLEAN",compare);
           //searchandprintl(compare);
            strcpy(compare,"BOOLEAN");
        }
        
     
        
        
           //condition check
       if(strcmp(compare,"BOOLEAN")!=0)
       {
        
                printf("TYPE MISMATCH %d C4",line_no);
                exit(0);
        }
        }
        else
        {
             printf("TYPE MISMATCH %d C4",line_no);
                exit(0);
        }
    
    }
}



struct while_stmtNode* while_stmt()
{
    // TODO: implement this
    struct while_stmtNode *obj;
    obj = ALLOC(struct while_stmtNode);
    t_type = getToken();
    if(t_type==WHILE)
    {
        t_type = getToken();
        if(t_type == ID || t_type == NUM || t_type == REALNUM)
        {
        
        
        
            ungetToken(); //it ll be checked in condition()
            obj->condition = condition();
            obj->body = body();
            return obj;
        }
        else
        {
        syntax_error("Condition failed--");
        }
    }
    else
    {
    syntax_error("WHILE Failed--");
    }
    return NULL;
}

struct exprNode* factor()
{
    struct exprNode* facto;

    t_type = getToken();
    if (t_type == LPAREN)
    {
        facto = expr();
        t_type = getToken();
        if (t_type == RPAREN)
        {
            return facto;
        }
        else
        {
            syntax_error("factor. RPAREN expected");
        }
    }
    else if (t_type == NUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = NUM;
        facto->primary->ival = atoi(token);
        return facto;
    }
    else if (t_type == REALNUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = REALNUM;
        facto->primary->fval = atof(token);
        return facto;
    }
    else if (t_type == ID)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = ID;
        facto->primary->id = strdup(token);
        return facto;
    }
    else
    {
        syntax_error("factor. NUM, REALNUM, or ID, expected");
    }
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* term()
{
    struct exprNode* ter;
    struct exprNode* f;

    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        f = factor();
        t_type = getToken();
        if (t_type == MULT || t_type == DIV)
        {
            ter = ALLOC(struct exprNode);
            ter->op = t_type;
            ter->leftOperand = f;
            ter->rightOperand = term();
            ter->tag = EXPR;
            ter->primary = NULL;
            return ter;
        }
        else if (t_type == SEMICOLON || t_type == PLUS ||
                 t_type == MINUS || t_type == RPAREN)
        {
            ungetToken();
            return f;
        }
        else
        {
            syntax_error("term. MULT or DIV expected");
        }
    }
    else
    {
        syntax_error("term. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* expr()
{
    struct exprNode* exp;
    struct exprNode* t;

    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        t = term();
        t_type = getToken();
        if (t_type == PLUS || t_type == MINUS)
        {
            exp = ALLOC(struct exprNode);
            exp->op = t_type;
            exp->leftOperand = t;
            exp->rightOperand = expr();
            exp->tag = EXPR;
            exp->primary = NULL;
            return exp;
        }
        else if (t_type == SEMICOLON || t_type == MULT ||
                 t_type == DIV || t_type == RPAREN)
        {
            ungetToken();
            return t;
        }
        else
        {
            syntax_error("expr. PLUS, MINUS, or SEMICOLON expected");
        }
    }
    else
    {
        syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}


int chkimporexp_assign(struct nknode *head,char *val)
{
    struct nknode *cur = head;
    while(cur!=NULL)
    {
       if(strcmp(cur->leftval,val)==0 && cur->typevarflag==1)
       return 1;
        cur = cur->next;
    }
    return 0;

}

void assignval(char *left,char *right)
{
    struct nknode *temp = nklist;
    while(temp!=NULL)
    {
        if(strcmp(temp->rightval,right)==0)
        strcpy(temp->rightval,left);
        //adding new code for changing all occurence
        //if t is present in leftval
        else if(strcmp(temp->leftval,right)==0)
        {
        strcpy(temp->leftval,left);//change that to INT and take rightval and do recursive
        assignval(left,temp->rightval);
        }
    temp = temp->next;
    }

}

void searchandprintl(char *val)
{
    struct nknode *temp = nklist;
    while(temp!=NULL)
    {
    if(strcmp(temp->rightval,val)==0)
    printf("CHANGED:%s\n",temp->leftval);
    temp=temp->next;
    }

}

void C2_check(char *compare,char * c2_global)
{
   // printf("%s\n",compare);
   nk_l_flag=0;
   nk_r_flag=0;
    if(c2_flag==0){
    strcpy(c2_global,compare);
    c2_flag=1;
    }
    else{
    char *compare1 = (char *)malloc(1000);
                  strcpy(compare1,c2_global);
    
        if(strcmp(compare,"NUM")==0)
        strcpy(compare,"INT");
        else if(strcmp(compare,"REALNUM")==0)
        strcpy(compare,"REAL");
        
        
         if(strcmp(compare1,"NUM")==0)
        strcpy(compare1,"INT");
        else if(strcmp(compare1,"REALNUM")==0)
        strcpy(compare1,"REAL");
    
    
    if(strcmp(compare,"INT")!=0 && strcmp(compare,"REAL")!=0 && strcmp(compare,"STRING")!=0 && strcmp(compare,"BOOLEAN")!=0  && strcmp(compare,"TYPE")!=0 && strcmp(compare,"LONG")!=0 && strcmp(compare,"NUM")!=0 && strcmp(compare,"REALNUM")!=0){
       // it is implicit
         nk_l_flag = 1;
         }
    
    
     if(strcmp(compare1,"INT")!=0 && strcmp(compare1,"REAL")!=0 && strcmp(compare1,"STRING")!=0 && strcmp(compare1,"BOOLEAN")!=0  && strcmp(compare1,"TYPE")!=0 && strcmp(compare1,"LONG")!=0 && strcmp(compare1,"NUM")!=0 && strcmp(compare1,"REALNUM")!=0){
         
        // printf("Hs");
         nk_r_flag = 1;
         }
    

 // printf("compare:%s\n",compare);
 //printf("compare1:%s\n",compare1); 
 //printf("L:%d",nk_l_flag);
 //printf("R:%d",nk_r_flag);
    
    
    //assign will be left will be assinged to right eg: int to t
    if(nk_l_flag==0 && nk_r_flag==1){
    assignval(compare,compare1);
    //searchandprintl(compare);
    strcpy(compare1,compare);
    }
    //other way t to int
    
    else if(nk_l_flag==1 && nk_r_flag==0){
    assignval(compare1,compare);
    //searchandprintl(compare1);
    strcpy(compare,compare1);
    }
    
    else if(nk_l_flag==1 && nk_r_flag==1){
    
        
        //propogate compare1 on compare coz compare is null
        if(strcmp(compare,"")==0)
        {
         assignval(compare1,compare);
         //searchandprintl(compare1);
          strcpy(compare,compare1);
        }
        //propogate compare on compare1 coz compare1 is null
        else if(strcmp(compare1,"")==0)
        {
         assignval(compare,compare1);
           //searchandprintl(compare1);
            strcpy(compare1,compare);
        }
    
    }
    
    
    
    // printf("compare:%s\n",compare);
 // printf("compare1:%s\n",compare1); 
    
   
    //added new condition incase if the variable is implicit and doesnt have any type like testcse39
    //both shouldnt be equal to null, then only you can compare
        if(strcmp(compare1,compare)!=0  )
        {
         //    printf("c1:%s",compare);
          //   printf("c2:%s",compare1);
             printf("TYPE MISMATCH %d C2",line_no);
             exit(0);
        }
    
    }


}



void C1_check(struct exprNode* expr,char *left,char *c2_global)
{
     if (expr->tag == PRIMARY)
    {
     //   printf("left:%s",left);
        char *compare = (char *)malloc(1000);
        strcpy(compare,reserved[expr->primary->tag]);
        //one is to compare the explicit directly
       // printf("compareout:%s",compare); // compare holds the right values in assignment
       
       //if compare is ID , we have to get it from data strcuture
       
       
       if(strcmp(compare,"ID")==0)
       {
        char *rval = (char *)malloc(1000);
         strcpy(rval,expr->primary->id);
            struct nknode *temp = nklist;
               strcpy(rightback,"");
                  strcpy(compare,"");
        //    printf("rval:%s\n",rval);      
            while(temp!=NULL)
            {
                if(strcmp(temp->leftval,rval)==0)
                {
                
                            strcpy(rightback,temp->rightval);
                            strcpy(compare,rightback);
                            //adding condition if its not basic types only need to check
                           
                            if(strcmp(rightback,"INT")!=0 && strcmp(rightback,"REAL")!=0 && strcmp(rightback,"STRING")!=0 && strcmp(rightback,"BOOLEAN")!=0  && strcmp(rightback,"TYPE")!=0 && strcmp(rightback,"LONG")!=0 && strcmp(rightback,"NUM")!=0 && strcmp(rightback,"REALNUM")!=0)
                            {
                            recursivecheck(temp->rightval);
                            if(strcmp(rightback,"")!=0)
                            strcpy(compare,rightback);
                           
                            }
                           //  rightback=NULL;
                        
                 }
                temp = temp->next;
            }
       
       }
      
       
        struct nknode *t = nklist;
            while(t!=NULL){
                if(strcmp(t->leftval,left)==0 && t->typevarflag == 1) //means its explicit and already there so just compare
                {
                 //   printf("compare: %s",compare);
             //    printf("tleft:%s",t->leftval);
              //     printf("tright: %s",t->rightval);
                  char *compare1 = (char *)malloc(1000);
                  strcpy(compare1,t->rightval);
                    //due to limitations
                    if(strcmp(compare,"NUM")==0)
                    strcpy(compare,"INT");
                    else if(strcmp(compare,"REALNUM")==0)
                    strcpy(compare,"REAL");
                    
                    if(strcmp(compare1,"NUM")==0)
                    strcpy(compare1,"INT");
                    else if(strcmp(compare1,"REALNUM")==0)
                    strcpy(compare1,"REAL");
                    
                    
                    
              //      printf("COmpare1:%s\n",compare);
                //     printf("COmpare2:%s\n",compare1);
                   
                    
                   //both shouldnt be equal to null,then only you can compare
                    if(strcmp(compare,compare1)!=0 && strcmp(compare1,"")!=0 && strcmp(compare,"")!=0)
                    {
                    
                   
                        printf("TYPE MISMATCH %d C1",line_no);
                        exit(0);
                    }
                     
                }
                else
                {
                //ignore, coz it is implicit
                }
            
            t = t->next;
            }
        
        
        
        
    }
}



void C1_check_dup(struct exprNode* expr,char *left,char *c2_global)
{
    if (expr->tag == EXPR)
    {
        //printf("%s ", reserved[expr->op]);
        C1_check_dup(expr->leftOperand,left,c2_global);
        C1_check_dup(expr->rightOperand,left,c2_global);
    }
    else if (expr->tag == PRIMARY)
    {
        //printf("left:%s",left);
        char *compare = (char *)malloc(1000);
        strcpy(compare,reserved[expr->primary->tag]);
        //one is to compare the explicit directly
       // printf("compareout:%s",compare); // compare holds the right values in assignment
       
       //if compare is ID , we have to get it from data strcuture
       
       //printf("Com:%s",compare);
       if(strcmp(compare,"ID")==0)
       {
        char *rval = (char *)malloc(1000);
         strcpy(rval,expr->primary->id);
         //printf("rval:%s",rval);
            struct nknode *temp = nklist;
             strcpy(rightback,"");
             strcpy(compare,"");
            while(temp!=NULL)
            {
            
                if(strcmp(temp->leftval,rval)==0)
                {
               // printf("CHK:%s",temp->rightval);
                            strcpy(rightback,temp->rightval);
                            recursivecheck(temp->rightval);
                            strcpy(compare,rightback);
                          //  rightback = NULL;
                       
                }
                temp = temp->next;
            }
       
       }
     // printf("compare:%s",compare);
       C2_check(compare,c2_global);
    }
}



//for variables from assign - implicit
void addtonk_var_assign(struct assign_stmtNode *head)
{
c2_global = (char *)malloc(1000);

rightback= (char *)malloc(1000);
leftback= (char *)malloc(1000);
   char *typename = NULL;
    int typevarflag = 1; // all are vars so 1
    int impexpflag = 0; //all var from assign are implicit
    int not_builtin =-1;//simply putting as 0 intially
   
  
  
  char *left = (char *)malloc(1000);
  char *right = (char *)malloc(1000);
  
  strcpy(rightback,"");//default
  
  
   strcpy(left,head->id);
    
   strcpy(leftback,left);
   //int rval= head->expr->rightOperand->primary->tag;
   c2_flag=0;
   //printf("%s",reserved[rval]);
  
    impexpflag  = chkimporexp_assign(nklist,left); //it will return 1 if it is alrdy declared, which means explicit, we need implicit only
    
  
    
    //printf("FLAG:%d",impexpflag);
    // printf("LEFT:%s\n",left);
    
    //C1_check(head->expr,left)
   
  //  if(impexpflag==0) //we need only implicit
  //  {
  //printf("INSIDE\n");
  if(ifnotthere(left))
   nklist = insert(nklist,left,"",impexpflag,typevarflag,not_builtin); //finding the types then and 
   flag=0;
   addtonk_varassign_util(head->expr,right);
  
  //if alrdy there, ret 0 else 1
 //printf("rightback:%s\n",rightback);
  chkifthereandupdate(left,rightback);
 // nklist = insert(nklist,left,rightback,impexpflag,typevarflag,not_builtin); //finding the types then and there and inserting
   //printf("%s",right);
   //rightback = NULL;
   // }
    
   C1_check(head->expr,left,c2_global); // this is for c1 check
   C1_check_dup(head->expr,left,c2_global); //this is for c2 check only
   
    
    
  }
  
  
int ifnotthere(char *temp)
{
    struct nknode *curr = nklist;
    while(curr!=NULL)
    {
    if(strcmp(curr->leftval,temp)==0)
    return 0;
    curr = curr->next;
    }
    return 1;
}  
  
void chkifthereandupdate(char *val,char *updateval)
{
    struct nknode *temp = nklist;
    
    //only update if its not the basic types
    while(temp!=NULL)
    {
    if(strcmp(val,temp->leftval)==0 && strcmp(temp->rightval,"INT")!=0 && strcmp(temp->rightval,"REAL")!=0 && strcmp(temp->rightval,"STRING")!=0 && strcmp(temp->rightval,"BOOLEAN")!=0  && strcmp(temp->rightval,"TYPE")!=0 && strcmp(temp->rightval,"LONG")!=0 && strcmp(temp->rightval,"NUM")!=0 && strcmp(temp->rightval,"REALNUM")!=0)
    {
        strcpy(temp->rightval,updateval);
    }
    temp = temp->next;
    }
   
} 

  

    
int check1_4(char *val,struct nknode *head)
{
    struct nknode *temp=head;
    while(temp!=NULL)
    {
        if((strcmp(val,temp->leftval)==0 ||strcmp(val,temp->rightval)==0)  && temp->typevarflag==0)
        return 1;
        temp = temp->next;
    }
    return 0;

}


void addtonk_varassign_util(struct exprNode* expr,char *right)
{

    if (expr->tag == EXPR)
    {
    //printf("EXPR");
     //   printf("%s ", reserved[expr->op]);
        addtonk_varassign_util(expr->leftOperand,right);
       addtonk_varassign_util(expr->rightOperand,right);
    }
    else if (expr->tag == PRIMARY)
    {
    //printf("PRIN");
        strcpy(right,reserved[expr->primary->tag]); 
      // printf("right:%s\n",right);
    //  char *r = (char *)malloc(1000);
     //  strcpy(r,expr->primary->id);
     //   printf("rvaltemp:%s\n",r);
        if(strcmp(right,"ID")!=0)
        {
       //      printf("Rightway:%s\n",right);
             strcpy(rightback,right);
             flag = 1;         
        }
        else if(strcmp(right,"ID")==0 && flag==0)
        {
       // printf("JJ");
            //find the type of right value and put it here
            char *rval  = (char*)malloc(1000);
            strcpy(rval,expr->primary->id);
      //  printf("rval:%s\n",rval);
          if(check1_4(rval,nklist))
          {
            printf("ERROR CODE 1.4 %s",rval);    
            exit(0); 
          }
          else
          {
            struct nknode *t = nklist;
            strcpy(rightback,"");
            while(t!=NULL)
            {
                if(strcmp(t->leftval,rval)==0)
                {
                //printf("\nt->left:%s",t->leftval);
                 //printf("\nt->right:%s",t->rightval);
                  strcpy(rightback,t->rightval);
                  recursivecheck(t->rightval);
                  strcpy(t->rightval,rightback);
                }
                
            
            t = t->next;
            }
           
           // printf("\nBBBrighback:%s\n",rightback);
         
         
         //added new code
         
          
            int abal = chkimporexp_assign(nklist,rval);
           // printf("abal:%d\n",abal);
            //if rval is not there in the list
           if(strcmp(rightback,"")==0  && abal==0)
            {
             
           //   printf("\nBrval:%s\n",rval);
            //     printf("\nBrightback:%s\n",rightback);
          //  printf("RET:%d\n",chkimporexp_assign(nklist,rval));
           
         
            nklist = insert(nklist,rval,rightback,0,1,0);
            }
           
          }
       
    }
       
    
   
    }
}


void recursivecheck(char *rightval)
{
     //if that value is again there , run it
    
    char *newrval = (char *)malloc(1000);
    strcpy(newrval,rightval);
   
    struct nknode *temp1 = nklist;
    while(temp1!=NULL)
    {
        if(strcmp(temp1->leftval,newrval)==0)
        {
           // printf("temprigh: %s",temp1->rightval);
                      strcpy(rightback,temp1->rightval);
                     
           recursivecheck(temp1->rightval);
           
            
        }
        else{
       // strcpy(right,t->rightval);
         // strcpy(rightback,right);
          }
     temp1 = temp1->next;   
    }
   
}



struct assign_stmtNode* assign_stmt()
{
    struct assign_stmtNode* assignStmt;

    t_type = getToken();
    if (t_type == ID)
    {
        assignStmt = ALLOC(struct assign_stmtNode);
        assignStmt->id = strdup(token);
        t_type = getToken();
        if (t_type == EQUAL)
        {
            assignStmt->expr = expr();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                addtonk_var_assign(assignStmt);
                return assignStmt;
            }
            else
            {
                syntax_error("asign_stmt. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("assign_stmt. EQUAL expected");
        }
    }
    else
    {
        syntax_error("assign_stmt. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct stmtNode* stmt()
{
    struct stmtNode* stm;

    stm = ALLOC(struct stmtNode);
    t_type = getToken();
    if (t_type == ID) // assign_stmt
    {
        ungetToken();
        stm->assign_stmt = assign_stmt();
        stm->stmtType = ASSIGN;
    }
    else if (t_type == WHILE) // while_stmt
    {
        ungetToken();
        stm->while_stmt = while_stmt();
        stm->stmtType = WHILE;
    }
    else if (t_type == DO)  // do_stmt
    {
        ungetToken();
        stm->while_stmt = do_stmt();
        stm->stmtType = DO;
    }
    else if (t_type == SWITCH) // switch_stmt
    {
        ungetToken();
        stm->switch_stmt = switch_stmt();
        stm->stmtType = SWITCH;
    }
    else
    {
        syntax_error("stmt. ID, WHILE, DO or SWITCH expected");
    }
    return stm;
}

struct stmt_listNode* stmt_list()
{
    struct stmt_listNode* stmtList;

    t_type = getToken();
    if (t_type == ID || t_type == WHILE ||
        t_type == DO || t_type == SWITCH)
    {
        ungetToken();
        stmtList = ALLOC(struct stmt_listNode);
        stmtList->stmt = stmt();
        t_type = getToken();
        if (t_type == ID || t_type == WHILE ||
            t_type == DO || t_type == SWITCH)
        {
            ungetToken();
            stmtList->stmt_list = stmt_list();
            return stmtList;
        }
        else // If the next token is not in FOLLOW(stmt_list),
            // let the caller handle it.
        {
            ungetToken();
            stmtList->stmt_list = NULL;
            return stmtList;
        }
    }
    else
    {
        syntax_error("stmt_list. ID, WHILE, DO or SWITCH expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct bodyNode* body()
{
    struct bodyNode* bod;

    t_type = getToken();
    if (t_type == LBRACE)
    {
        bod = ALLOC(struct bodyNode);
        bod->stmt_list = stmt_list();
        t_type = getToken();
        if (t_type == RBRACE)
        {
            return bod;
        }
        else
        {
            syntax_error("body. RBRACE expected");
        }
    }
    else
    {
        syntax_error("body. LBRACE expected");
       
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_nameNode* type_name()
{
    struct type_nameNode* tName;

    tName = ALLOC(struct type_nameNode);
    t_type = getToken();
    if (t_type == ID || t_type == INT || t_type == REAL ||
        t_type == STRING || t_type == BOOLEAN || t_type == LONG)
    {
        tName->type = t_type;
        if (t_type == ID)
        {
            tName->id = strdup(token);
        }
        else
        {
            tName->id = NULL;
        }
        return tName;
    }
    else
    {
        syntax_error("type_name. type name expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct id_listNode* id_list()
{
    struct id_listNode* idList;

    idList = ALLOC(struct id_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        idList->id = strdup(token);
        t_type = getToken();
        if (t_type == COMMA)
        {
            idList->id_list = id_list();
            return idList;
        }
        else if (t_type == COLON)
        {
            ungetToken();
            idList->id_list = NULL;
            return idList;
        }
        else
        {
            syntax_error("id_list. COMMA or COLON expected");
        }
    }
    else
    {
        syntax_error("id_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

void printnklist(struct nknode *head)
{

    struct nknode *temp = head;
    while(temp!=NULL)
    {
        printf("%s\n",temp->leftval);
        printf("%s\n",temp->rightval);
        printf("%d\n",temp->impexpflag);
        printf("%d\n",temp->typevarflag);
        printf("%d\n\n",temp->not_builtin);
        temp = temp->next;
    }
}

int check1_1(struct nknode *head,char *val,int impexpflag)
{
    struct nknode *temp = head;
    while(temp!=NULL)
    {
        if((strcmp(temp->leftval,val)==0 || strcmp(temp->rightval,val)==0) && temp->typevarflag==0 &&temp->impexpflag == 1 &&impexpflag==1)
        return 1;
        temp = temp->next; 
    }
    return 0;

}

int check1_2(struct nknode *head,char *val,int impexpflag)
{
    struct nknode *temp = head;
    while(temp!=NULL)
    {
        if((strcmp(temp->rightval,val)==0 || strcmp(temp->leftval,val)==0) && temp->typevarflag==0 &&temp->impexpflag == 0 &&impexpflag==1)
        return 1;
        temp = temp->next; 
    }
    return 0;

}

int checkimporexp(struct nknode *head,char *val)
{
    struct nknode *temp = head;
    while(temp!=NULL)
    {
        if(strcmp(temp->leftval,val)==0 && temp->typevarflag==0)
        return 1;
        temp = temp->next; 
    }
    return 0;

}
void addtonk_type(struct type_declNode *head)
{
    rightback = (char *)malloc(1000);  
    int typevarflag = 0; // all are types so 0
    int impexpflag = 1; //all are explicit only in type section
    int not_builtin =0;
    //type alone
    char *typename = (char *)malloc(1000);
    int type = head->type_name->type; //int value , INT, REAL, STRING, BOOLEAN, ID, LONG
    if(type==ID) //if it is ID then it is not a built in type
    {
        strcpy(typename,head->type_name->id);
        
        
        //add the type 
        
        
        //adding new code to add the base type
        
        
        strcpy(rightback,typename);
        recursivecheck(typename);
        strcpy(typename,rightback);
        
        
        
        if(checkimporexp(nklist,typename)==0)
        {
            impexpflag = 0;
        }
        not_builtin = 0;
    }
    else
    {
        strcpy(typename,reserved[type]); //get the actual stirng from reserved array
        not_builtin = 1;
    }
    
    //var alone
    
    struct id_listNode* temp = head->id_list;
    while(temp!=NULL)
    {
    //printf("%s",temp->id);
     //  printf("%s",typename);
      //  int valchk = check1_1(nklist,temp->id);
     //   printf("valchk:%d",valchk);
      if(check1_1(nklist,temp->id,impexpflag)) //check for 1.1, 1 - returns 1.1 error
      {
            printf("ERROR CODE 1.1 %s",temp->id);
           exit(0);
      }
      else if(check1_2(nklist,temp->id,impexpflag)) //check for 1.1, 1 - returns 1.1 error
      {
            printf("ERROR CODE 1.2 %s",temp->id);
            exit(0);
      }
      else
      {
        nklist = insert(nklist,temp->id,typename,impexpflag,typevarflag,not_builtin);
     }
        temp = temp->id_list;
    }
    
     if(strcmp(typename,"INT")!=0 && strcmp(typename,"REAL")!=0 && strcmp(typename,"STRING")!=0 && strcmp(typename,"BOOLEAN")!=0  && strcmp(typename,"TYPE")!=0 && strcmp(typename,"LONG")!=0 && strcmp(typename,"NUM")!=0 && strcmp(typename,"REALNUM")!=0){
        
         imptype = (char *)malloc(1000);
       strcpy(imptype,"IMP");
       strcat(imptype,typename);
     
           nklist = insert(nklist,typename,imptype,0,0,0);
        }

    
}

int check1_3(char *val,struct nknode *head,int typevarflag)
{
    struct nknode *temp = head;
    while(temp!=NULL)
    {
        if((strcmp(val,temp->leftval)==0 || strcmp(val,temp->rightval)==0) && temp->typevarflag==0){
        //printf("HOLA:%d",temp->typevarflag); 
        return 1;
        
        }
        temp=temp->next;
    }
    return 0;

}
int check2_1(char *val,struct nknode *head)
{
    struct nknode *temp = head;
    while(temp!=NULL)
    {
        if(strcmp(temp->leftval,val)==0 && temp->typevarflag==1){
      //   printf("HELLO:%d",temp->typevarflag);
        return 1;
       
        }
        temp = temp->next;  
    }
    return 0;

}

int check2_2(char *val,struct nknode *head)
{
    struct nknode *temp = head;
    while(temp!=NULL)
    {
        if(strcmp(temp->leftval,val)==0 && temp->typevarflag==1)
        return 1;
        temp = temp->next;  
    }
    return 0;

}

//for variables
void addtonk_var(struct var_declNode *head)
{

rightback = (char *)malloc(1000);
   
    int typevarflag = 1; // all are vars so 1
    int impexpflag = 1; //all are explicit only in var section
    int not_builtin =0;//simply putting as 0 intially
    //type alone
    char *newtempval = (char *)malloc(1000);
    char *typename = (char *)malloc(1000);
    int type = head->type_name->type; //int value , INT, REAL, STRING, BOOLEAN, ID, LONG
    if(type==ID) //if it is ID then it is not a built in type
    {
        strcpy(typename,head->type_name->id);
        not_builtin = 0;
    }
    else
    {
        strcpy(typename,reserved[type]); //get the actual stirng from reserved array
        not_builtin = 1;
    }
    
   
    
    if(check2_2(typename,nklist))
      {
             printf("ERROR CODE 2.2 %s",typename);
             exit(0);
      }
    else{
    
    
    //printf("typename:%s\n",typename);
    
    //printf("rightback:%s\n",rightback);
    
     //adding new code to add the base type
     strcpy(newtempval,typename); //that segmentation malloc error
      strcpy(rightback,typename);
      
       if(strcmp(rightback,"INT")!=0 && strcmp(rightback,"REAL")!=0 && strcmp(rightback,"STRING")!=0 && strcmp(rightback,"BOOLEAN")!=0  && strcmp(rightback,"TYPE")!=0 && strcmp(rightback,"LONG")!=0 && strcmp(rightback,"NUM")!=0 && strcmp(rightback,"REALNUM")!=0)
       {
       recursivecheck(typename);
        strcpy(typename,rightback);
        }
    
    // printf("typenameafter:%s\n",typename);
    //var alone
    
    struct id_listNode* temp = head->id_list;
    while(temp!=NULL)
    {
    //printf("%s",temp->id);
     //  printf("%s",typename);
        
      if(check1_3(temp->id,nklist,typevarflag))
      {
             printf("ERROR CODE 1.3 %s",temp->id);
             exit(0);
      }
      
      else if(check2_1(temp->id,nklist))
      {
             printf("ERROR CODE 2.1 %s",temp->id);
             exit(0);
     
      }
      else{
        //printf("left:%s\n",temp->id);
        //printf("right:%s\n",newtempval);
        nklist = insert(nklist,temp->id,newtempval,impexpflag,typevarflag,not_builtin);
       
       }
       
      
        temp = temp->id_list;
    }
    
     //new comparison
        if(strcmp(typename,newtempval)==0 && strcmp(typename,"INT")!=0 && strcmp(typename,"REAL")!=0 && strcmp(typename,"STRING")!=0 && strcmp(typename,"BOOLEAN")!=0  && strcmp(typename,"TYPE")!=0 && strcmp(typename,"LONG")!=0 && strcmp(typename,"NUM")!=0 && strcmp(typename,"REALNUM")!=0)
        {
       imptype = (char *)malloc(1000);
       strcpy(imptype,"IMP");
       strcat(imptype,typename);
      //  strcpy(rightf,typename);
     // printf("typenameinside:%s",typename);
      //  printf("newtemp:%s",newtemp);
        nklist = insert(nklist,typename,imptype,0,0,0);
       
        }

    }
}




struct type_declNode* type_decl()
{
    struct type_declNode* typeDecl;

    typeDecl = ALLOC(struct type_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDecl->id_list = id_list();
        t_type = getToken();
        if (t_type == COLON)
        {
            typeDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                addtonk_type(typeDecl);
                return typeDecl;
            }
            else
            {
                syntax_error("type_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("type_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("type_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_declNode* var_decl()
{
    struct var_declNode* varDecl;

    varDecl = ALLOC(struct var_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDecl->id_list = id_list();
        t_type = getToken();
        if (t_type == COLON)
        {
            varDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                addtonk_var(varDecl);
                return varDecl;
            }
            else
            {
                syntax_error("var_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("var_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("var_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_listNode* var_decl_list()
{
    struct var_decl_listNode* varDeclList;

    varDeclList = ALLOC(struct var_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDeclList->var_decl = var_decl();
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            varDeclList->var_decl_list = var_decl_list();
            return varDeclList;
        }
        else
        {
            ungetToken();
            varDeclList->var_decl_list = NULL;
            return varDeclList;
        }
    }
    else
    {
        syntax_error("var_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_listNode* type_decl_list()
{
    struct type_decl_listNode* typeDeclList;

    typeDeclList = ALLOC(struct type_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDeclList->type_decl = type_decl();
       
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            typeDeclList->type_decl_list = type_decl_list();
            return typeDeclList;
        }
        else
        {
            ungetToken();
            typeDeclList->type_decl_list = NULL;
            return typeDeclList;
        }
    }
    else
    {
        syntax_error("type_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_sectionNode* var_decl_section()
{
    struct var_decl_sectionNode *varDeclSection;

    varDeclSection = ALLOC(struct var_decl_sectionNode);
    t_type = getToken();
    if (t_type == VAR)
    {
        // no need to ungetToken()
        varDeclSection->var_decl_list = var_decl_list();
        return varDeclSection;
    }
    else
    {
        syntax_error("var_decl_section. VAR expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_sectionNode* type_decl_section()
{
    struct type_decl_sectionNode *typeDeclSection;

    typeDeclSection = ALLOC(struct type_decl_sectionNode);
    t_type = getToken();
    if (t_type == TYPE)
    {
        typeDeclSection->type_decl_list = type_decl_list();
      
        return typeDeclSection;
    }
    else
    {
        syntax_error("type_decl_section. TYPE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct declNode* decl()
{
    struct declNode* dec;

    dec = ALLOC(struct declNode);
    dec->type_decl_section = NULL;
    dec->var_decl_section = NULL;
    t_type = getToken();
    if (t_type == TYPE)
    {
        ungetToken();
        dec->type_decl_section = type_decl_section();
        t_type = getToken();
        if (t_type == VAR)
        {
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
        }
        else
        {
            ungetToken();
            dec->var_decl_section = NULL;
        }
        return dec;
    }
    else
    {
        dec->type_decl_section = NULL;
        if (t_type == VAR)
        {
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
            return dec;
        }
        else
        {
            if (t_type == LBRACE)
            {
                ungetToken();
                dec->var_decl_section = NULL;
                return dec;
            }
            else
            {
                syntax_error("decl. LBRACE expected");
            }
        }
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct programNode* program()
{
    struct programNode* prog;

    prog = ALLOC(struct programNode);
    t_type = getToken();
    if (t_type == TYPE || t_type == VAR || t_type == LBRACE)
    {
        ungetToken();
        prog->decl = decl();
     
        prog->body = body();
        return prog;
    }
    else
    {
        syntax_error("program. TYPE or VAR or LBRACE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

void updatelinkedlist(struct nknode *head)
{
    
    struct nknode *temp = head;
    while(temp!=NULL)
    {
    struct nknode *curr = head;
        while(curr!=NULL)
        {
            if(strcmp(temp->leftval,curr->rightval)==0 && strcmp(temp->rightval,"")!=0)
            {
               strcpy(curr->rightval,temp->rightval); 
            }
            curr = curr->next;
        }
    temp = temp->next;
    }
    
}

void changenumtoint(struct nknode *head)
{
    struct nknode *temp = head;
    while(temp!=NULL){
        if(strcmp(temp->rightval,"NUM")==0)
        strcpy(temp->rightval,"INT");
        else if(strcmp(temp->rightval,"REALNUM")==0)
        strcpy(temp->rightval,"REAL");
        
        temp = temp->next;
    }

}

void printlistnew(struct newlist *head)
{
struct newlist *temp = head;
while(temp!=NULL)
{
printf("\n%s",temp->val);
temp = temp->next;
}

}

int insertifnotexist(char *value)
{
    struct newlist *temp = newlist1;
    while(temp!=NULL)
    {
        if(strcmp(temp->val,value)==0)
        return 0;
    temp = temp->next;
    }
return 1;

}

void findothervalues(struct nknode *nknode1)
{
    struct nknode *tempnk = nknode1;
    while(tempnk!=NULL)
    {
    if(strcmp(tempnk->rightval,"BOOLEAN")!=0 && strcmp(tempnk->rightval,"INT")!=0 &&strcmp(tempnk->rightval,"LONG")!=0 &&strcmp(tempnk->rightval,"REAL")!=0 &&strcmp(tempnk->rightval,"STRING")!=0){
    //printf("INSIDE:%s",tempnk->rightval);
    
    if(insertifnotexist(tempnk->rightval))
    newlist1 = insertnew(newlist1,tempnk->rightval);
    }
    
    tempnk=tempnk->next;
    }

}

void somefunction(char *value)
{
    struct nknode *temp = nklist;
    while(temp!=NULL)
    {
    
    if(strcmp(value,temp->rightval)==0)
    {
        struct nknode *temp2 = nklist;
        while(temp2!=NULL)
        {
        if(strcmp(temp->leftval,temp2->rightval)==0)
        dangerflag = 1;
        
        temp2 = temp2->next;
        }
    }
    temp = temp->next;
    }

}

void printfinal()
{
    struct newlist *head = newlist1;
    
    while(head!=NULL)
    {
                    //small check
                     if(strcmp(head->val,"BOOLEAN")!=0 && strcmp(head->val,"INT")!=0 &&strcmp(head->val,"LONG")!=0 &&strcmp(head->val,"REAL")!=0 &&strcmp(head->val,"STRING")!=0){
                   // printf("HERE");
                   finalflag=1;
                    }
                    
                    
                    if(finalflag==0){
                        printf("%s ",head->val);
                    }
                         
                        struct nknode *nkl = nklist;
                        while(nkl!=NULL)
                        {
                         if(strcmp(head->val,nkl->rightval)==0 )
                            {
                                printf("%s ",nkl->leftval);
                            
                            }
                            nkl = nkl->next;
                        }
                        printf("#\n");
                        
         finalflag=0;
        head = head->next;
       
       }

}

int main()
{
    struct programNode* parseTree;
    parseTree = program();
    // TODO: remove the next line after you complete the parser
    //print_parse_tree(parseTree); // This is just for debugging purposes
    // TODO: do type checking & print output according to project specification
    
    updatelinkedlist(nklist);
    changenumtoint(nklist);
    
   
    newlist1= insertnew(newlist1,"BOOLEAN");

    newlist1 = insertnew(newlist1,"INT");

    newlist1 = insertnew(newlist1,"LONG");

    newlist1 = insertnew(newlist1,"REAL");

    newlist1 = insertnew(newlist1,"STRING");
    findothervalues(nklist);
  //   printlistnew(newlist1);
 // printnklist(nklist);
    
    printfinal();
    
    return 0;
}
