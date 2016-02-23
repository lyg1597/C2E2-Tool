// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.
#line 21 "./simuverif.y" // lalr1.cc:399


 #include <iostream>
 #include <fstream>
 #include <stdio.h>
 #include <vector>
 using namespace std;

// extern FILE *yyin;
// extern int yyparse();
// extern int yylex();
// void yyerror(const char *);

// vector<char*> biddenstr;


#line 53 "./simuverif.tab.c" // lalr1.cc:399

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "simuverif.tab.h"

// User implementation prologue.

#line 67 "./simuverif.tab.c" // lalr1.cc:407
// Unqualified %code blocks.
#line 15 "./simuverif.y" // lalr1.cc:408

	// Prototype for the yylex function
	#include "simuverifParser.h"
	static int yylex(yy::BisonParser::semantic_type * yylval, class FlexScanner &holder);

#line 75 "./simuverif.tab.c" // lalr1.cc:408


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyempty = true)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 142 "./simuverif.tab.c" // lalr1.cc:474

  /// Build a parser object.
  BisonParser::BisonParser (class FlexScanner &holder_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      holder (holder_yyarg)
  {}

  BisonParser::~BisonParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  BisonParser::syntax_error::syntax_error (const std::string& m)
    : std::runtime_error (m)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
  {
    value = other.value;
  }


  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v)
    : Base (t)
    , value (v)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::~basic_symbol ()
  {
  }

  template <typename Base>
  inline
  void
  BisonParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
  }

  // by_type.
  inline
  BisonParser::by_type::by_type ()
     : type (empty)
  {}

  inline
  BisonParser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  BisonParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  BisonParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.type = empty;
  }

  inline
  int
  BisonParser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  BisonParser::by_state::by_state ()
    : state (empty)
  {}

  inline
  BisonParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  BisonParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  BisonParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  BisonParser::symbol_number_type
  BisonParser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  BisonParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  BisonParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s)
  {
    value = that.value;
    // that is emptied.
    that.type = empty;
  }

  inline
  BisonParser::stack_symbol_type&
  BisonParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }


  template <typename Base>
  inline
  void
  BisonParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  BisonParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  BisonParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  BisonParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  BisonParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  BisonParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  BisonParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  BisonParser::debug_level_type
  BisonParser::debug_level () const
  {
    return yydebug_;
  }

  void
  BisonParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline BisonParser::state_type
  BisonParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  BisonParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  BisonParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  BisonParser::parse ()
  {
    /// Whether yyla contains a lookahead.
    bool yyempty = true;

    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyempty)
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, holder));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
        yyempty = false;
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Discard the token being shifted.
    yyempty = true;

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 5:
#line 102 "./simuverif.y" // lalr1.cc:847
    {holder.dimensions = (int)(yystack_[1].value.doubVal); 
					holder.initMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.initMax = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMax = (double*)malloc(holder.dimensions*sizeof(double));
					/*printf(" dim - %d - \n",holder.dimensions);*/}
#line 543 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 6:
#line 108 "./simuverif.y" // lalr1.cc:847
    {holder.dimensions = (int)(yystack_[0].value.doubVal); 
					holder.initMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.initMax = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMax = (double*)malloc(holder.dimensions*sizeof(double));
					/*printf(" dim - %d - \n",holder.dimensions);*/}
#line 554 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 7:
#line 114 "./simuverif.y" // lalr1.cc:847
    {holder.modes = (int)(yystack_[0].value.doubVal); holder.kConst = (double*)malloc(holder.modes*sizeof(double));
					holder.gamma = (double*)malloc(holder.dimensions*sizeof(double));
					holder.islinear = (int*)malloc(holder.dimensions*sizeof(int));}
#line 562 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 8:
#line 117 "./simuverif.y" // lalr1.cc:847
    {holder.modes = (int)(yystack_[1].value.doubVal); holder.kConst = (double*)malloc(holder.modes*sizeof(double));
					holder.gamma = (double*)malloc(holder.modes*sizeof(double));
					holder.islinear = (int*)malloc(holder.modes*sizeof(int));}
#line 570 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 9:
#line 120 "./simuverif.y" // lalr1.cc:847
    {holder.simulator = (char*)(yystack_[1].value.str); /*printf(" sim - %s -\n",$4);*/}
#line 576 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 10:
#line 121 "./simuverif.y" // lalr1.cc:847
    {holder.simulator = (char*)(yystack_[0].value.str); /*printf(" sim - %s -\n",$3);*/}
#line 582 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 11:
#line 122 "./simuverif.y" // lalr1.cc:847
    {holder.absoluteError = (double)(yystack_[1].value.doubVal); /*printf(" abserror - %f -\n",$4);*/}
#line 588 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 12:
#line 123 "./simuverif.y" // lalr1.cc:847
    {holder.relativeError = (double)(yystack_[1].value.doubVal); /*printf(" relerror - %f -\n",$4);*/}
#line 594 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 13:
#line 124 "./simuverif.y" // lalr1.cc:847
    {holder.absoluteError = (double)(yystack_[0].value.doubVal); /*printf(" abserror - %f -\n",$3);*/}
#line 600 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 14:
#line 125 "./simuverif.y" // lalr1.cc:847
    {holder.relativeError = (double)(yystack_[0].value.doubVal); /*printf(" relerror - %f -\n",$3);*/}
#line 606 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 15:
#line 126 "./simuverif.y" // lalr1.cc:847
    {holder.initMode = (int)(yystack_[1].value.doubVal);}
#line 612 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 16:
#line 127 "./simuverif.y" // lalr1.cc:847
    {holder.initMode = (int)(yystack_[0].value.doubVal);}
#line 618 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 17:
#line 128 "./simuverif.y" // lalr1.cc:847
    {holder.simuval = (int)(yystack_[1].value.doubVal);}
#line 624 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 18:
#line 129 "./simuverif.y" // lalr1.cc:847
    {holder.simuval = (int)(yystack_[0].value.doubVal);}
#line 630 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 19:
#line 130 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[7].value.intVal); 
							*(holder.initMin+(curdim-1)) = (yystack_[4].value.doubVal); *(holder.initMax+(curdim-1)) = (yystack_[2].value.doubVal); 
							/*printf(" dim - %d - lower %f - higher %f -\n",curdim,$7,$9);*/}
#line 638 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 20:
#line 133 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[6].value.intVal); 
							*(holder.initMin+(curdim-1)) = (yystack_[3].value.doubVal); *(holder.initMax+(curdim-1)) = (yystack_[1].value.doubVal); 
							/*printf(" dim - %d - lower %f - higher %f -\n",curdim,$6,$8);*/}
#line 646 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 21:
#line 136 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[1].value.doubVal);
							holder.initEqns = numEqns;
							/*printf(" init equations - %d -\n",numEqns);*/}
#line 654 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 22:
#line 139 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[0].value.doubVal);
							holder.initEqns = numEqns;
							/*printf(" init equations - %d -\n",numEqns);*/}
#line 662 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 23:
#line 142 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.dimensions*holder.initEqns*sizeof(double)); holder.index=0; /*printf("Starting init matrix")*/;}
#line 668 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 24:
#line 142 "./simuverif.y" // lalr1.cc:847
    { holder.initMatrix = holder.ptr; /*printf(" Done parsing initial matrix \n");*/}
#line 674 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 25:
#line 143 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.initEqns*sizeof(double)); holder.index=0; /*printf("Starting init b");*/}
#line 680 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 26:
#line 143 "./simuverif.y" // lalr1.cc:847
    { holder.initB = holder.ptr; /*printf(" Done parsing initial B \n");*/}
#line 686 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 27:
#line 144 "./simuverif.y" // lalr1.cc:847
    {holder.refineVal = (double)(yystack_[1].value.doubVal); /*printf(" delta - %f -\n",$4);*/}
#line 692 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 28:
#line 145 "./simuverif.y" // lalr1.cc:847
    {holder.tStep = (double)(yystack_[1].value.doubVal); /*printf(" tstep - %f -\n",$4);*/}
#line 698 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 29:
#line 146 "./simuverif.y" // lalr1.cc:847
    {holder.tGlobal = (double)(yystack_[1].value.doubVal); /*printf(" thorizon - %f -\n",$4);*/}
#line 704 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 30:
#line 147 "./simuverif.y" // lalr1.cc:847
    {holder.refineVal = (double)(yystack_[0].value.doubVal); /*printf(" delta - %f -\n",$3);*/}
#line 710 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 31:
#line 148 "./simuverif.y" // lalr1.cc:847
    {holder.tStep = (double)(yystack_[0].value.doubVal); /*printf(" tstep - %f -\n",$3);*/}
#line 716 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 32:
#line 149 "./simuverif.y" // lalr1.cc:847
    {holder.tGlobal = (double)(yystack_[0].value.doubVal); /*printf(" thorizon - %f -\n",$3);*/}
#line 722 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 33:
#line 150 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[7].value.intVal); 
							*(holder.forbMin+(curdim-1)) = (yystack_[4].value.doubVal); *(holder.forbMax+(curdim-1)) = (yystack_[2].value.doubVal); 
							/*holder.biddenstr.push_back($3); printf(" forbidden - %s -\n",$3);*/}
#line 730 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 34:
#line 153 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[6].value.intVal); 
							*(holder.forbMin+(curdim-1)) = (yystack_[3].value.doubVal); *(holder.forbMax+(curdim-1)) = (yystack_[1].value.doubVal); 
							/*holder.biddenstr.push_back($3); printf(" forbidden - %s -\n",$3);*/}
#line 738 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 35:
#line 156 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[1].value.doubVal);
							holder.forbEqns = numEqns;
							/*printf(" forbidden equations - %d -\n",numEqns);*/}
#line 746 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 36:
#line 159 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[0].value.doubVal);
							holder.forbEqns = numEqns;
							/*printf(" forbidden - %d -\n",numEqns);*/}
#line 754 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 37:
#line 162 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.dimensions*holder.forbEqns*sizeof(double)); holder.index=0; /*printf("Starting forb matrix")*/;}
#line 760 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 38:
#line 162 "./simuverif.y" // lalr1.cc:847
    { holder.forbMatrix = holder.ptr; /*printf(" Done parsing forbidden matrix \n");*/}
#line 766 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 39:
#line 163 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.forbEqns*sizeof(double)); holder.index=0; /*printf("Starting forb B");*/}
#line 772 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 40:
#line 163 "./simuverif.y" // lalr1.cc:847
    { holder.forbB = holder.ptr; /*printf(" Done parsing forbidden B \n");*/}
#line 778 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 41:
#line 164 "./simuverif.y" // lalr1.cc:847
    { holder.curMode = (int) (yystack_[1].value.doubVal);}
#line 784 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 42:
#line 165 "./simuverif.y" // lalr1.cc:847
    {holder.curMode = (int) (yystack_[0].value.doubVal);}
#line 790 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 43:
#line 166 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 2; /*printf(" annot - linear \n");*/}
#line 796 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 44:
#line 167 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 3; /*printf(" annot - contraction \n");*/}
#line 802 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 45:
#line 168 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 4; /*printf(" annot - exponential \n");*/}
#line 808 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 46:
#line 169 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 2; /*printf(" annot - linear \n");*/}
#line 814 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 47:
#line 170 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 3; /*printf(" annot - contraction \n");*/}
#line 820 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 48:
#line 171 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 4; /*printf(" annot - exponential \n");*/}
#line 826 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 49:
#line 172 "./simuverif.y" // lalr1.cc:847
    {holder.beta = (yystack_[0].value.str); /*printf(" beta - %s -\n",$3);*/}
#line 832 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 50:
#line 173 "./simuverif.y" // lalr1.cc:847
    {holder.V = (yystack_[0].value.str); /*printf(" annot - %s -\n",$3);*/}
#line 838 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 51:
#line 174 "./simuverif.y" // lalr1.cc:847
    {*(holder.islinear+(holder.curMode-1)) = (int)(yystack_[1].value.doubVal);}
#line 844 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 52:
#line 175 "./simuverif.y" // lalr1.cc:847
    {*(holder.kConst+(holder.curMode-1)) = (double)(yystack_[1].value.doubVal); /*printf(" Kconst - %f -\n",$4);*/}
#line 850 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 53:
#line 176 "./simuverif.y" // lalr1.cc:847
    {*(holder.gamma+(holder.curMode-1)) = (double)(yystack_[1].value.doubVal); /*printf(" Gamma - %f -\n",$4);*/}
#line 856 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 54:
#line 177 "./simuverif.y" // lalr1.cc:847
    {*(holder.kConst+(holder.curMode-1)) = (double)(yystack_[0].value.doubVal); /*printf(" Kconst - %f -\n",$3);*/}
#line 862 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 55:
#line 178 "./simuverif.y" // lalr1.cc:847
    {*(holder.gamma+(holder.curMode-1)) = (double)(yystack_[0].value.doubVal); /*printf(" Gamma - %f -\n",$3);*/}
#line 868 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 56:
#line 179 "./simuverif.y" // lalr1.cc:847
    {holder.val1 = (int)(yystack_[4].value.intVal); holder.val2 = (int)(yystack_[2].value.intVal); holder.visuFile = (char*) (yystack_[0].value.str); /*printf(" visualize - %d and %d to %s \n",$2,$4,$6);*/}
#line 874 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 57:
#line 180 "./simuverif.y" // lalr1.cc:847
    {holder.val1 = -1; holder.val2 = -1; holder.visuFile = (char*) (yystack_[0].value.str); /*printf(" visualize - -1 and -1 to %s \n",$4);*/}
#line 880 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 58:
#line 181 "./simuverif.y" // lalr1.cc:847
    {holder.val1 = (yystack_[4].value.intVal); holder.val2 = 0; holder.visuFile = (char*) (yystack_[0].value.str); /*printf(" visualize - %d and time to %s \n",$2,$6);*/}
#line 886 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 59:
#line 185 "./simuverif.y" // lalr1.cc:847
    {double val = (double)(yystack_[0].value.doubVal); *(holder.ptr+holder.index) = val; /*printf("The value is %f and index is %d \n",val,holder.index); */holder.index++;}
#line 892 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 60:
#line 186 "./simuverif.y" // lalr1.cc:847
    {double val = (double)(yystack_[0].value.doubVal); *(holder.ptr+holder.index) = val; /*printf("The value is %f and index is %d \n",val,holder.index);*/ holder.index++;}
#line 898 "./simuverif.tab.c" // lalr1.cc:847
    break;


#line 902 "./simuverif.tab.c" // lalr1.cc:847
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state,
                                           yyempty ? yyempty_ : yyla.type_get ()));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyempty)
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyempty = true;
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyempty)
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyempty)
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  BisonParser::error (const syntax_error& yyexc)
  {
    error (yyexc.what());
  }

  // Generate an error message.
  std::string
  BisonParser::yysyntax_error_ (state_type, symbol_number_type) const
  {
    return YY_("syntax error");
  }


  const signed char BisonParser::yypact_ninf_ = -116;

  const signed char BisonParser::yytable_ninf_ = -61;

  const short int
  BisonParser::yypact_[] =
  {
       7,    11,    38,    40,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    37,   100,  -116,     7,
       2,     3,    -2,     5,     6,    33,    13,  -116,  -116,    14,
      16,    17,    18,    35,    21,  -116,  -116,   -28,    25,    66,
      26,    67,    28,    30,    68,    70,    71,  -116,  -116,    69,
    -116,    73,  -116,    72,  -116,    74,  -116,    76,  -116,    78,
      83,    79,  -116,    75,    84,    81,  -116,    82,  -116,    85,
    -116,    86,  -116,    88,    89,    90,  -116,    87,    92,  -116,
    -116,  -116,   -25,    91,  -116,  -116,    93,  -116,    94,    95,
    -116,    96,  -116,  -116,    97,    10,    98,   103,   104,   105,
     107,   108,   106,   110,   109,   109,   111,   112,   113,   114,
     115,   116,   117,   109,   109,   119,   120,   121,   122,   123,
     124,   125,   126,  -116,   129,   130,  -116,  -116,  -116,  -116,
    -116,   127,   128,  -116,   131,   132,   133,  -116,  -116,  -116,
    -116,   135,   134,  -116,   136,   138,  -116,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,   137,   139,   141,   142,   143,  -116,
    -116,   145,   146,  -116,  -116,  -116,  -116,   147,   149,   109,
     150,   152,   153,   140,  -116,   154,   158,   159,  -116,   160,
    -116,   144,   156,  -116,  -116
  };

  const unsigned char
  BisonParser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     3,
       0,     0,     0,     0,     0,     0,     0,    23,    25,     0,
       0,     0,     0,     0,     0,    37,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,     4,     0,
       6,     0,     7,     0,    10,     0,    13,     0,    14,     0,
       0,     0,    22,     0,     0,     0,    16,     0,    31,     0,
      32,     0,    30,     0,     0,     0,    36,     0,     0,    47,
      46,    48,     0,     0,    42,    50,     0,    54,     0,     0,
      18,     0,    55,    49,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    57,     0,     0,     5,     8,     9,    11,
      12,     0,     0,    21,    59,     0,     0,    15,    28,    29,
      27,     0,     0,    35,     0,     0,    44,    43,    45,    41,
      52,    51,    17,    53,     0,     0,     0,     0,     0,    24,
      26,     0,     0,    38,    40,    58,    56,     0,     0,     0,
       0,     0,     0,     0,    61,     0,     0,     0,    20,     0,
      34,     0,     0,    19,    33
  };

  const signed char
  BisonParser::yypgoto_[] =
  {
    -116,  -116,    77,  -116,  -116,  -116,  -116,  -116,  -115,  -116
  };

  const short int
  BisonParser::yydefgoto_[] =
  {
      -1,    27,    28,    29,    73,    74,    87,    88,   145,   168
  };

  const short int
  BisonParser::yytable_[] =
  {
     146,    89,    90,    91,   125,   126,   127,    92,   154,   155,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    63,    24,    25,   134,    59,    61,    26,
      65,    67,    64,    60,    62,    30,    66,    68,    71,    75,
     135,    77,    79,    81,    72,    76,    85,    78,    80,    82,
      93,    96,    86,    99,   184,   101,    94,    97,    69,   100,
      83,   102,    31,    70,    32,    84,    55,    56,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      57,     0,    98,   104,     0,     0,    58,     0,     0,    95,
     106,   103,   114,   105,   107,   109,   108,   110,   111,   112,
     113,   115,   116,   117,   123,   121,   118,   119,   120,   124,
       0,   122,   128,   136,   129,   130,   131,   132,   137,   138,
     139,   133,   140,   142,   141,   143,   147,   148,   149,   150,
     144,   151,   153,   152,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,     0,     0,     0,     0,   167,
     169,   170,   171,   -60,   173,   172,   174,     0,   188,   193,
       0,   175,   177,   176,   178,   179,   180,     0,   181,   182,
     183,   194,   185,   186,   187,   189,   190,   191,   192
  };

  const short int
  BisonParser::yycheck_[] =
  {
     115,    29,    30,    31,    29,    30,    31,    35,   123,   124,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    35,    27,    28,    26,    35,    35,    32,
      35,    35,    44,    41,    41,    34,    41,    41,    35,    35,
      40,    35,    35,    35,    41,    41,    35,    41,    41,    41,
      35,    35,    41,    35,   179,    35,    41,    41,    35,    41,
      35,    41,    34,    40,    34,    40,    39,    40,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
       0,    -1,    35,    33,    -1,    -1,    29,    -1,    -1,    43,
      41,    43,    37,    42,    41,    41,    44,    41,    40,    36,
      41,    37,    41,    41,    37,    36,    41,    41,    40,    37,
      -1,    41,    41,    35,    41,    41,    41,    41,    35,    35,
      35,    44,    35,    37,    36,    35,    35,    35,    35,    35,
      41,    36,    35,    37,    35,    35,    35,    35,    35,    35,
      35,    35,    33,    33,    37,    -1,    -1,    -1,    -1,    41,
      38,    38,    37,    42,    38,    41,    38,    -1,    38,    35,
      -1,    44,    41,    44,    42,    42,    41,    -1,    42,    42,
      41,    35,    42,    41,    41,    41,    38,    38,    38
  };

  const unsigned char
  BisonParser::yystos_[] =
  {
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    27,    28,    32,    46,    47,    48,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    39,    40,     0,    47,    35,
      41,    35,    41,    35,    44,    35,    41,    35,    41,    35,
      40,    35,    41,    49,    50,    35,    41,    35,    41,    35,
      41,    35,    41,    35,    40,    35,    41,    51,    52,    29,
      30,    31,    35,    35,    41,    43,    35,    41,    35,    35,
      41,    35,    41,    43,    33,    42,    41,    41,    44,    41,
      41,    40,    36,    41,    37,    37,    41,    41,    41,    41,
      40,    36,    41,    37,    37,    29,    30,    31,    41,    41,
      41,    41,    41,    44,    26,    40,    35,    35,    35,    35,
      35,    36,    37,    35,    41,    53,    53,    35,    35,    35,
      35,    36,    37,    35,    53,    53,    35,    35,    35,    35,
      35,    35,    35,    35,    33,    33,    37,    41,    54,    38,
      38,    37,    41,    38,    38,    44,    44,    41,    42,    42,
      41,    42,    42,    41,    53,    42,    41,    41,    38,    41,
      38,    38,    38,    35,    35
  };

  const unsigned char
  BisonParser::yyr1_[] =
  {
       0,    45,    46,    47,    47,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    48,    50,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    51,    48,    52,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    53,
      54,    53
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     1,     2,     5,     3,     3,     5,     5,
       3,     5,     5,     3,     3,     5,     3,     5,     3,    11,
       9,     5,     3,     0,     6,     0,     6,     5,     5,     5,
       3,     3,     3,    11,     9,     5,     3,     0,     6,     0,
       6,     5,     3,     5,     5,     5,     3,     3,     3,     3,
       3,     5,     5,     5,     3,     3,     6,     4,     6,     1,
       0,     4
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const BisonParser::yytname_[] =
  {
  "$end", "error", "$undefined", "DIMENSIONS", "MODES", "SIMULATOR",
  "ABSERROR", "RELERROR", "INIT", "INITE", "INITM", "INITB", "INITMODE",
  "TSTEP", "THORIZON", "REFINE", "FORB", "FORBE", "FORBM", "FORBB",
  "ANNOTTYPE", "ANNOTMODE", "ANNOT", "KCONST", "ISLINEAR", "SIMU", "TIME",
  "GAMMA", "BETA", "CONTRACTION", "LINEAR", "EXPONENTIAL", "VISU", "TO",
  "EQ", "QU", "IN", "SO", "SC", "ALL", "XID", "NUMBER", "CO", "STRING",
  "WORD", "$accept", "ss", "sentences", "sentence", "$@1", "$@2", "$@3",
  "$@4", "numbersequence", "$@5", YY_NULLPTR
  };


  const unsigned char
  BisonParser::yyrline_[] =
  {
       0,    93,    93,    97,    98,   102,   108,   114,   117,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     133,   136,   139,   142,   142,   143,   143,   144,   145,   146,
     147,   148,   149,   150,   153,   156,   159,   162,   162,   163,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   185,
     186,   186
  };

  // Print the state stack on the debug stream.
  void
  BisonParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  BisonParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  BisonParser::token_number_type
  BisonParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44
    };
    const unsigned int user_token_number_max_ = 299;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // yy
#line 1341 "./simuverif.tab.c" // lalr1.cc:1155
#line 189 "./simuverif.y" // lalr1.cc:1156



// We have to implement the error function
void yy::BisonParser::error(const std::string &msg) {
	std::cerr << "Error: " << msg << std::endl;
}


/*
void yyerror(const char* err_string)
{
	cout << "Parse error! " << err_string << endl;
	exit(0);

}
*/

// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

#include "simuverifScanner.h"

static int yylex(yy::BisonParser::semantic_type * yylval,  class FlexScanner &holder) {
	return holder.yylex(yylval);
}

/*
int tricky(int argc, char* argv[])
{
	if(argc == 1){
		cout << "Please enter a file name \n";
		exit(0);
	}

	
	FILE* myfile = fopen((char*)argv[1],"r");
	if(!myfile){
		cout << "Error in opening the file , please give a correct file" << endl;
		exit(0);
	}
	yyin = myfile;
	while(!feof(myfile)){
		yyparse();
	}

	cout << "IS this coming \n";

       int l,m;
       for(l=0;l<biddenstr.size();l++){
	 cout << " strings - " << biddenstr.at(l) << "\n";
       }

}

*/


