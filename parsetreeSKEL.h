/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include <string>
#include "lex.h"
using std::vector;
using std::map;
using std::string;
using std::cout;
using std::endl;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

// a "forward declaration" for a class to hold values
class Value;
static map<string,bool> identifier;

class ParseTree {
protected:
	int			linenum;
	ParseTree	*left;
	ParseTree	*right;

public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}

	int GetLineNumber() const { return linenum; }

	int MaxDepth() const { 
        int leftSideCount = 1;
        int rightSideCount = 1;
        if (left) {
            leftSideCount += left->MaxDepth();
        }
        if (right){
            rightSideCount +=right->MaxDepth();
        }
        if ( leftSideCount > rightSideCount){
            return leftSideCount++;
        }
        else{
            return rightSideCount++;
        }
	}

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }
    virtual int IsBang() const { return 0; }
    virtual bool IsLet() const { return false; }

	int BangCount() const { //work needs to be done
        int bangCount = 0;
		if( left ) bangCount += left->BangCount();
		if( right ) bangCount += right->BangCount();
		if( IsBang())
        {
		 	bangCount++;
        }
		return bangCount;
	}

	virtual void CheckLetBeforeUse(map<string,bool>& var) {
    }
};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
};

class Let : public ParseTree {
	string id;

public:
	Let(Lex& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) { identifier.insert({id,IsLet()});}

	string GetId() const { return id; }
	bool IsLet() const { return true; }
};

class Print : public ParseTree {
public:
    Print(int line,ParseTree *e) : ParseTree(line,e) {}
};

class Loop : public ParseTree {
public:
    Loop(int line,ParseTree *e, ParseTree *s) : ParseTree(line,e,s) {}
};

class If : public ParseTree {
public:
    If(int line,ParseTree *e, ParseTree *s) : ParseTree(line,e,s) {}
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class TimesExpr : public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class DivideExpr : public ParseTree {
public:
	DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class BangExpr : public ParseTree {
public:
    BangExpr(int line, ParseTree *l): ParseTree(line,l) {}
    int IsBang() const { return 1; }
};

class IConst : public ParseTree {
	int val;

public:
	IConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Lex& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {CheckLetBeforeUse(identifier);}
	bool IsIdent() const { return true; }
	string GetId() const { return id; }
     void CheckLetBeforeUse(map<string,bool>& var) {
         if (var.find(id) == var.end())
         {
             cout << "UNDECLARED VARIABLE " << id << endl;
         }
	}
};

#endif /* PARSETREE_H_ */
