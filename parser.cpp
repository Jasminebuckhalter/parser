
//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 3
// created: 3-14-2016
//  author: Jia Zhao
//*****************************************************************************
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <cstdlib>

using namespace std;

#include "lexer.h"

extern "C"
{
    // Instantiate global variables
    extern FILE *yyin;         // input stream
    extern FILE *yyout;        // output stream
    extern int   yyleng;       // length of current lexeme
    extern char *yytext;       // text of current lexeme
    extern int   yylex();      // the generated lexical analyzer
    extern int   yyline;       // the current source code line
}


// Production Functions
void  P(void);
void  S(void);
void  A(void);
float E(void);
float B(void);
float R(void);
float T(void);
float F(void);
float U(void);
void  G(void);
void  O(void);
void  C(void);
void  W(void);


// Function declaration for checking whether the current token is
// in the first set of each production rule.
bool IsFirstOfP(void);
bool IsFirstOfS(void);
bool IsFirstOfA(void);
bool IsFirstOfE(void);
bool IsFirstOfB(void);
bool IsFirstOfR(void);
bool IsFirstOfT(void);
bool IsFirstOfF(void);
bool IsFirstOfU(void);
bool IsFirstOfG(void);
bool IsFirstOfO(void);
bool IsFirstOfC(void);
bool IsFirstOfW(void);


// Function to help print the tree nesting
string psp( int );

// Data type for the Symbol Table
typedef map<string, float> SymbolTableT;

// Needed global variables
int iTok;                  // The current token
SymbolTableT SymbolTable;  // The symbol table

int main(int argc, char *argv[]){
    int token;
    
    //input ouput stream
    yyin = stdin;
    yyout = stdout;
    
    iTok = yylex();
    
    try {
        if (!IsFirstOfP()) { //check for {
            throw "missing '{' at beginning of program";
        }
        //process P Production
        P();
        if (iTok != TOK_EOF) {
            throw "end of file expected, but there is more";
        }
    } catch (char const *errmsg) {
        cout << endl << "***ERROR (line " << yyline << ") " << errmsg << endl;
        return 1;
    }
    
    // Parse success!!
    cout << endl << "=== GO BULLDOGS! Your parse was successful! ===" << endl << endl;
    
    // Print out the symbol table
    SymbolTableT::iterator it;
    for( it = SymbolTable.begin(); it != SymbolTable.end(); ++it ) {
        cout << "symbol = " << it->first << ", value = " << it->second << endl;
    }
    
    return 0;
}

//*****************************************************************************
// P --> { \{ S \} }
void P(void){
    static int Pcnt = 0;
    int CurPcnt = Pcnt++;
    char const *Perr = "statement does not start with '{'";
    cout << psp(CurPcnt) << "enter P " << CurPcnt << endl;
    cout << "-->found " << yytext << endl;
    // the first token is {, read next token
    iTok = yylex();
    // expect to see S
    if (IsFirstOfS()) {
        S();
    }
    else{
        throw Perr;
    }
    while (IsFirstOfS()) {
        S();
    
    }
    //last should be a }
    if (iTok != TOK_CLOSEBRACE) {
        throw "missing '}' ar the end of program";
    }
    cout << "-->found " << yytext << endl;
    iTok = yylex();
    cout << psp(CurPcnt) << "exit P " << CurPcnt << endl;
}

//*****************************************************************************
// S --> A | G | O | C | W
void S(void){
    static int Scnt = 0;
    int CurScnt = Scnt++;
    char const *Serr = "illegal statement";
    cout << psp(CurScnt) << "enter S " << CurScnt << endl;
    switch (iTok) {
        case TOK_LET:
            A();
            break;
        case TOK_READ:
            G();
            break;
        case TOK_PRINT:
            O();
            break;
        case TOK_IF:
            C();
            break;
        case TOK_WHILE:
            W();
            break;
            
        default:
            throw Serr;
    }
    cout << psp(CurScnt) << "exit S " << CurScnt << endl;
}

//*****************************************************************************
//A --> let ID := E;
void A(void){
    float rValue;        // Value returned from expression
    static int Acnt = 0; // Count the number of A's
    int CurAcnt = Acnt++;
    cout << psp( CurAcnt ) << "enter A " << CurAcnt << endl;
    // We know we have found a 'let' token
    cout << "-->found " << yytext << endl;
    // Next should be an identifier; save its name
    iTok = yylex();
    if (iTok != TOK_IDENTIFIER ) {
        throw "missing identifier in assignment statement";
    }
    cout << "-->found ID: " << yytext << endl;
    string IDname = yytext;
    
    // Next shoud be an assignment statement
    iTok = yylex();
    if (iTok != TOK_ASSIGN) {
        throw "missing ':=' symbol in assignment statement";
    }
    cout << "-->found " << yytext << endl;
    
    iTok = yylex();
    //maybe its EBRTF
    if (IsFirstOfE()) {
        //cout << yytext << endl;
        rValue = E();
    }
    else{
        throw "missing expression";
    }
    // If the identifier is not yet in the symbol table, store it there
    SymbolTableT::iterator it = SymbolTable.find( IDname );
    if (it == SymbolTable.end()) {
        SymbolTable.insert(pair<string, float>(IDname,1.0));
    }
    // Update ID in symbol table with value from expression
    it = SymbolTable.find( IDname );
    it->second = rValue;
    
    //last should be ;
    if (iTok != TOK_SEMICOLON) {
        throw "missing ';' at end of statement";
    }
    cout << "-->found " << yytext << endl;
    
    iTok = yylex();
    cout << psp(CurAcnt) << "exit A" << CurAcnt << endl;
}


//*****************************************************************************
//E -> B { ( and | or ) B }
float E(void){
    float rValue1 = 0;   // The value to return
    float rValue2;
    static int Ecnt = 0; // Count the number of E's
    int CurEcnt = Ecnt++;
    char const *Eerr = "B does not start with 'not'|'-'|'('|'ID'|'FLOATLIT'";
    
    cout << psp( CurEcnt ) << "enter E " << CurEcnt << endl;
    if (IsFirstOfB()) {
        rValue1 = B();
    }
    else{
        throw Eerr;
    }
    while ((iTok == TOK_AND) || (iTok == TOK_OR)) {
        cout << "-->found " << yytext << endl;
        int iTokLast = iTok;
        iTok = yylex();
        if (IsFirstOfB()) {
            rValue2 = B();
        }
        else{
            throw Eerr;
        }
        switch (iTokLast) {
            case TOK_AND:
                rValue1 = (float)(rValue1 != 0) && (rValue2 != 0);
                break;
            case TOK_OR:
                rValue1 = (float)(rValue1 != 0) || (rValue2 != 0);
                break;
                
            default:
                break;
        }
    }
    
    cout << psp(CurEcnt) << "exit E " << CurEcnt << endl;
    return rValue1;
}


//*****************************************************************************
//B --> R [ ( < | > | == ) R ]
float B(void){
    float rValue1 = 0;   // The value to return
    float rValue2;
    static int Bcnt = 0; // Count the number of E's
    int CurBcnt = Bcnt++;
    char const *Berr = "Bollean does not start with R";

    cout << psp(CurBcnt)<< "enter B " << CurBcnt << endl;
    if (IsFirstOfR()) {
        rValue1 = R();
    }
    else{
        throw Berr;
    }
    if ((iTok == TOK_LESSTHAN) || (iTok == TOK_GREATERTHAN) || (iTok == TOK_EQUALTO)) {
        cout << "-->found " << yytext << endl;
        int iTokLast = iTok;
        iTok = yylex();
        if (IsFirstOfR())
            rValue2=R();
        else
            throw "Boolean not end with a R";
        switch(iTokLast){
            case TOK_LESSTHAN:
                rValue1 =(float)(rValue1 < rValue2);
                break;
                
            case TOK_GREATERTHAN:
                rValue1 =(float)(rValue1 > rValue2);
                break;
                
            case TOK_EQUALTO:
                rValue1 =(float)(rValue1 == rValue2);
                break;
                
            default:
                break;
        }
    }
    cout << psp(CurBcnt) << "exit B " << CurBcnt << endl;
    return rValue1;
}

//*****************************************************************************
//R --> T { ( + | - ) T }
float R(void){
    float rValue1 = 0;   // The value to return
    float rValue2;
    static int Rcnt = 0; // Count the number of E's
    int CurRcnt = Rcnt++;
    char const *Rerr = "R does not start with T";
    
    cout << psp( CurRcnt ) << "enter R " << CurRcnt << endl;
    if (IsFirstOfT()) {
        rValue1 = T();
    }
    else{
        throw Rerr;
    }
    while ((iTok == TOK_PLUS) || (iTok == TOK_MINUS)) {
        cout<< "-->found "<< yytext<< endl;
        int iTokLast;
        iTokLast = iTok;
        iTok = yylex();
        if (IsFirstOfT()) {
            rValue2 = T();
        }
        else{
            throw Rerr;
        }
        switch (iTokLast) {
            case TOK_PLUS:
                rValue1 = rValue1 + rValue2;
                break;
            case TOK_MINUS:
                rValue1 = rValue1 - rValue2;
                break;
                
            default:
                break;
        }
    }
    cout << psp(CurRcnt) << "exit R " << CurRcnt << endl;
    return rValue1;
}


//*****************************************************************************
//T -> F { ( * | / ) F }
float T(void){
    float rValue1 = 0;   // The value to return
    float rValue2;
    static int Tcnt = 0; // Count the number of T's
    int CurTcnt = Tcnt++;
    char const *Terr = "T does not start with F";
    
    cout << psp( CurTcnt ) << "enter T " << CurTcnt << endl;
    
    // We next expect to see a F
    if(IsFirstOfF()){
        rValue1 = F();
    }
    else{
        throw Terr;
    }
    
    // As long as the next token is * or /, keep parsing F's
    while((iTok == TOK_MULTIPLY) || (iTok == TOK_DIVIDE) ){
        cout << "-->found " << yytext << endl;
        int iTokLast = iTok;
        iTok = yylex();
        if(IsFirstOfF()){
            rValue2 = F();
        }
        else{
            throw Terr;
        }
        
        // Perform the operation to update rValue1 acording to rValue2
        switch(iTokLast){
            case TOK_MULTIPLY:
                rValue1 = rValue1 * rValue2;
                break;
                
            case TOK_DIVIDE:
                rValue1 = rValue1 / rValue2;
                break;
            
            default:
                break;
        }
    }
    cout << psp( CurTcnt ) << "exit T " << CurTcnt << endl;
    return rValue1;
}


//*****************************************************************************
//F --> [ not | - ] U
float F(void){
    float rValue;        // Value returned from expression
    static int Fcnt = 0; // Count the number of A's
    int CurFcnt = Fcnt++;
    char const *Ferr = "F does not start with not | - U ";

    cout << psp( CurFcnt ) << "enter F " << CurFcnt << endl;
    if ((iTok == TOK_NOT) || (iTok == TOK_MINUS)) {
        cout << "-->found " << yytext << endl;
        int iTokLast = iTok;
        iTok = yylex();
        if (!IsFirstOfU()) {
            throw Ferr;
        }
        else{
            rValue = U();
        }
        switch (iTokLast) {
            case TOK_NOT:
                rValue = (float)(!(rValue != 0));
                break;
            case TOK_MINUS:
                rValue = (float)(-rValue);
                break;
                
            default:
                break;
        }
    }
    else if(IsFirstOfU()){
        rValue = U();
    }
    else{
        throw "U err";
    }
    
    cout << psp(CurFcnt) << "exit F " << CurFcnt << endl;
    return rValue;
}


//*****************************************************************************
//U --> ID | FLOATLIT | ( E )
float U(void){
    float rValue = 0;           // the value to return
    SymbolTableT::iterator it;  // look up values in symbol table
    static int Ucnt = 0;        // Count the number of F's
    int CurUcnt = Ucnt++;
    
    cout << psp( CurUcnt ) << "enter U " << CurUcnt << endl;
    
    switch (iTok) {
        case TOK_IDENTIFIER:
            cout << "-->found ID: " << yytext << endl;
            
            // Look up value of identifier in symbol table
            it = SymbolTable.find( yytext );
            // If the symbol is not in the table, uninitialized identifier error
            if(it == SymbolTable.end()){
                throw "uninitialized identifier used in expression";
            }
            // Return the value of the identifier
            rValue = it->second;
            
            // Read past what we have found
            iTok = yylex();
            break;
            
        case TOK_FLOATLIT:
            cout << "-->found FLOATLIT: " << yytext << endl;
            
            // Capture the value of this literal
            rValue = (float) atof(yytext);
            
            iTok = yylex();
            break;
            
        case TOK_OPENPAREN:
            // We expect (E); parse it
            cout << "-->found " << yytext << endl;
            iTok = yylex();
            if (IsFirstOfE()) {
                rValue = E();
            }
            else{
                throw "Expect an E after '('";
            }
            if (iTok == TOK_CLOSEPAREN) {
                cout << "-->found " << yytext << endl;
                iTok = yylex();
            }
            else{
                throw "Expect ')' after E";
            }
            
            break;
            
        default:
            break;
    }
    cout << psp(CurUcnt) << "exit U "<< CurUcnt << endl;
    return rValue;
}


//*****************************************************************************
//G --> read [ STRINGLIT ] ID;
void G(void){
    float rValue = 0;
    static int Gcnt = 0;
    int CurGcnt = Gcnt++;
    cout << psp(CurGcnt) << "enter G "<< CurGcnt << endl;
    cout << "-->found "<< yytext << endl;
    
    iTok = yylex();
    if (iTok == TOK_STRINGLIT) {
        cout << "-->found string: " << yytext << endl;
        iTok = yylex();
        if (iTok == TOK_IDENTIFIER) {
            cin >> rValue;
            SymbolTableT::iterator it = SymbolTable.find(yytext);
            if (it == SymbolTable.end()) {
                SymbolTable.insert(pair<string,float>(yytext,rValue));
            }
            else{
                it->second = rValue;
            }
        }
        else{
            throw "No identifier found";
        }
    }
    
    else if(iTok == TOK_IDENTIFIER){
        cin >> rValue;
        SymbolTableT::iterator it = SymbolTable.find(yytext);
        if (it == SymbolTable.end()) {
            SymbolTable.insert(pair<string,float>(yytext,rValue));
        }
    }
    else{
        throw "No string of ID was found";
    }
    iTok = yylex();
    if (iTok != TOK_SEMICOLON) {
        throw "expect ';' at the end of a G";
    }
    cout << "-->found " << yytext << endl;
    cout << psp(CurGcnt) << "exit G " << CurGcnt << endl;
    iTok = yylex();
}


//*****************************************************************************
//O --> print [ STRINGLIT ] [ ID ];
void O(void){
    static int Ocnt =0;
    int CurOcnt =Ocnt++;
    
    cout << psp(CurOcnt) << "enter O " << CurOcnt << endl;
    cout << "-->found " << yytext << endl;
    
    iTok = yylex();
    if (iTok == TOK_STRINGLIT) {
        cout << "-->found string " << yytext << endl;
        iTok = yylex();
        if (iTok == TOK_IDENTIFIER) {
            cout << "-->found ID: " << yytext << endl;
            SymbolTableT::iterator it = SymbolTable.find(yytext);
            if (it == SymbolTable.end()) {
                throw "unrecognized variable";
            }
            iTok = yylex();
            if (iTok != TOK_SEMICOLON) {
                throw "expect ';' at the end of a O";
            }
            cout << "-->found string " << yytext << endl;
            iTok = yylex();
        }

        else{
            if (iTok == TOK_SEMICOLON) {
                cout << "-->found " << yytext << endl;
                iTok = yylex();
            }
            else{
                throw "expect ';' at the end of a O";
            }
        }
    }
    
    else if(iTok == TOK_IDENTIFIER){
        cout << "-->found ID: " << yytext << endl;
        SymbolTableT::iterator it = SymbolTable.find(yytext);
        if (it == SymbolTable.end()) {
            throw "unrecognized variable";
        }
        iTok = yylex();
        if (iTok != TOK_SEMICOLON) {
            throw "expect ';' at the end of a O";
        }
        cout << "-->found " << yytext << endl;
        iTok = yylex();
    }
    
    else if(iTok == TOK_SEMICOLON){
        cout << "-->found " << yytext << endl;
        iTok = yylex();
    }
    
    else{
        throw "unrecognized variable";
    }
    cout << psp(CurOcnt) << "exit O" << CurOcnt << endl;
}


//*****************************************************************************
//C -> if ( E ) P [ else p ]
void C(void){
    float rValue = 0;
    static int Ccnt =0;
    int CurCcnt = Ccnt++;
    
    cout << psp(CurCcnt) << "enter C " << CurCcnt << endl;
    cout << "-->found " << yytext << endl;
    
    iTok = yylex();
    if (iTok != TOK_OPENPAREN) {
        throw "expect '(' after if";
    }
    cout << "-->found " << yytext << endl;
    iTok = yylex();
    if (!IsFirstOfE()) {
        throw " Expected E after '(' ";
    }
    rValue = E();
    if (iTok != TOK_CLOSEPAREN) {
        throw "expected ')' after E ";
    }
    cout << "-->found " << yytext << endl;
    iTok = yylex();
    
    
    if (!IsFirstOfP()) {
        throw " Expected P after ')' ";
    }
    P();
    if (iTok == TOK_ELSE) {
        cout << "-->found " << yytext << endl;
        iTok = yylex();
        if (!IsFirstOfP()) {
            throw "expected P after else";
        }
        P();
    }
    /*
    if (!IsFirstOfP()) {
        throw " Expected P after ')' ";
    }
    if (rValue != 0) {
        P();
    }
    else{
        P();
    }
    if (iTok == TOK_ELSE) {
        cout << "-->found " << yytext << endl;
        iTok = yylex();
        if (!IsFirstOfP()) {
            throw "expected P after else";
        }
        if (rValue == 0) {
            P();
        }
        else{
            P();
        }
    }*/
    
    cout << psp(CurCcnt) << "exit C " << CurCcnt << endl;
}


//*****************************************************************************
//W --> while ( E ) P
void W(void){
    static int Wcnt = 0;
    int CurWcnt = Wcnt++;
    cout << "-->found" << yytext << endl;
    cout << psp(CurWcnt) << "enter W " << CurWcnt << endl;
    
    iTok = yylex();
    if (iTok != TOK_OPENPAREN) {
        throw "expect a '(' after while";
    }
    iTok = yylex();
    if (!IsFirstOfE()) {
        throw "expect a E after '('";
    }
    E();
    if (iTok != TOK_CLOSEPAREN) {
        throw "expect a ')' after E";
    }
    iTok = yylex();
    if (!IsFirstOfP()) {
        throw "expect a P after ')'";
    }
    P();
    
    cout << psp(CurWcnt) << "exit W " << CurWcnt << endl;
}



//*****************************************************************************
bool IsFirstOfS(void){
    return IsFirstOfA() || IsFirstOfG() || IsFirstOfO() || IsFirstOfC() || IsFirstOfW();
}

bool IsFirstOfP(void){
    return iTok == TOK_OPENBRACE;
}

bool IsFirstOfA(void){
    return iTok == TOK_LET;
}

bool IsFirstOfE(void){
    return (iTok == TOK_NOT) || (iTok == TOK_MINUS) || IsFirstOfU();
}

bool IsFirstOfB(void){
    return (iTok == TOK_NOT) || (iTok == TOK_MINUS) || IsFirstOfU();
}

bool IsFirstOfR(void){
    return (iTok == TOK_NOT) || (iTok == TOK_MINUS) || IsFirstOfU();
}

bool IsFirstOfT(void){
    return (iTok == TOK_NOT) || (iTok == TOK_MINUS) || IsFirstOfU();
}

bool IsFirstOfF(void){
    return (iTok == TOK_NOT) || (iTok == TOK_MINUS) || IsFirstOfU();
}

bool IsFirstOfU(void){
    return (iTok == TOK_OPENPAREN) || (iTok == TOK_IDENTIFIER) || (iTok == TOK_FLOATLIT);
}

bool IsFirstOfG(void){
    return iTok == TOK_READ;
}

bool IsFirstOfO(void){
    return iTok == TOK_PRINT;
}

bool IsFirstOfC(void){
    return iTok == TOK_IF;
}

bool IsFirstOfW(void){
    return iTok == TOK_WHILE;
}

//*****************************************************************************
string psp( int n ) {// Stands for p-space
    string str(n, ' ');
    return str;
}












