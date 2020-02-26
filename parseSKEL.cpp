/*
 * parse.cpp
 */

#include "parse.h"


namespace Parser {
bool pushed_back = false;
Lex	pushed_token;

static Lex GetNextToken(istream& in, int& line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Lex& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

ParseTree *Prog(istream& in, int& line)
{
	ParseTree *sl = Slist(in, line);

	if( sl == 0 )
		ParseError(line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream& in, int& line) {
	ParseTree *s = Stmt(in, line);
	if( s == 0 )
		return 0;
    Lex t = Parser::GetNextToken(in, line);
    if(t != SC)
    {
      ParseError(line,"Missing Statement Separater");
      return 0;
    }

	return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream& in, int& line) {
    ParseTree *s = 0;
    
    Lex t = Parser::GetNextToken(in, line);
    switch(t.GetToken()){
        case IF:
            s = IfStmt(in,line);
            break;
        case PRINT:
            s = PrintStmt(in,line);
            break;
        case LET:
            s = LetStmt(in,line);
            break;
        case LOOP:
            s = LoopStmt(in,line);
            break;
        case ERR:
            ParseError(line,"Invalid Token");
            break;
        case DONE:
            break;
        case SC:
            s = Stmt(in, line);
            break;
        case ID:
            ParseError(line,"Invalid Identifier");
            break;
        default:
            Parser::PushBackToken(t);
            break;
    }
    return s;
}

ParseTree *LetStmt(istream& in, int& line) {
    Lex t;
    ParseTree *expression;
    t = Parser::GetNextToken(in, line); 
    if(t != ID)
    {
        ParseError(line,"Identifier Not Found!");
        return 0;
    }
    expression = Expr(in,line);
    if (expression == 0)
    {
        ParseError(line,"Missing Expression");
        return 0;
    }
    return new Let(t,expression);
}

ParseTree *PrintStmt(istream& in, int& line) {
    ParseTree *expression = Expr(in,line);
    //Lex t = Parser::GetNextToken(in,line);
    if(expression == 0)
    {
        ParseError(line,"Expression Missing");
        return 0;
    } 
    return new Print(line, expression);
}

ParseTree *LoopStmt(istream& in, int& line) {
    //Lex t1 = Parser::GetNextToken(in,line);
    ParseTree *expression, *statement;
    expression = Expr(in,line);
    if (expression == 0)
    {
        ParseError(line,"Mission Expression");
        return 0;
    }
    Lex t2 = Parser::GetNextToken(in,line);
    if (t2 != BEGIN)
    {
        ParseError(line,"BEGIN Missing");
        return 0;
    }
    statement = Slist(in,line);
    if (statement == 0)
    {
        ParseError(line,"Missing Slist");
        return 0;
    }
    Lex t3 = Parser::GetNextToken(in,line);
    if (t3 != END)
    {
        ParseError(line,"END Missing");
        return 0;
    }
    return new Loop(line,expression,statement);
}

ParseTree *IfStmt(istream& in, int& line) {
    //Lex t1 = Parser::GetNextToken(in,line);
    ParseTree *expression, *statement;
    expression = Expr(in,line);
    if (expression == 0)
    {
        ParseError(line,"Mission Expression");
        return 0;
    }
    Lex t2 = Parser::GetNextToken(in,line);
    if (t2 != BEGIN)
    {
        ParseError(line,"BEGIN Missing");
        return 0;
    }
    statement = Slist(in,line);
    if (statement == 0)
    {
        ParseError(line,"Missing Slist");
        return 0;
    }
    Lex t3 = Parser::GetNextToken(in,line);
    if (t3 != END)
    {
        ParseError(line,"END Missing");
        return 0;
    }
    return new If(line,expression,statement);
}

ParseTree *Expr(istream& in, int& line) {
	ParseTree *t1 = Prod(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Lex t = Parser::GetNextToken(in, line);

		if( t != PLUS && t != MINUS ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Prod(istream& in, int& line) {
    ParseTree *t1 = Rev(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Lex t = Parser::GetNextToken(in, line);

		if( t != STAR && t != SLASH ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Rev(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new TimesExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new DivideExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Rev(istream& in, int& line) { //incomplete 
    Lex t = Parser::GetNextToken(in,line);
    if (t == BANG)
    {
        ParseTree *t1 = Rev(in,line);
        if(t1 == 0)
        {
            ParseError(line,"Missing Rev after BANG");
            return 0;
        }
        return new BangExpr(line,t1);
    }
    Parser::PushBackToken(t);
    ParseTree *t2 = Primary(in,line);
    if (t2 == 0){
        ParseError(line,"Missing Primary");
        return 0;
    }
    return t2;
    
}

ParseTree *Primary(istream& in, int& line) {
	Lex t = Parser::GetNextToken(in, line);

	if( t == ID ) {
		return new Ident(t);
	}
	else if( t == INT ) {
		return new IConst(t);
	}
	else if( t == STR ) {
		return new SConst(t);
	}
	else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(line, "Missing expression after (");
			return 0;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return 0;
	}

	ParseError(line, "Primary expected");
	return 0;
}