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
#line 100 "./simuverif.y" // lalr1.cc:847
    {holder.dimensions = (int)(yystack_[1].value.doubVal); 
					holder.initMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.initMax = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMax = (double*)malloc(holder.dimensions*sizeof(double));
					printf(" dim - %d - \n",holder.dimensions);}
#line 543 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 6:
#line 106 "./simuverif.y" // lalr1.cc:847
    {holder.dimensions = (int)(yystack_[0].value.doubVal); 
					holder.initMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.initMax = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMax = (double*)malloc(holder.dimensions*sizeof(double));
					printf(" dim - %d - \n",holder.dimensions);}
#line 554 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 7:
#line 112 "./simuverif.y" // lalr1.cc:847
    {holder.modes = (int)(yystack_[0].value.doubVal); holder.kConst = (double*)malloc(holder.modes*sizeof(double));
					holder.gamma = (double*)malloc(holder.dimensions*sizeof(double));
					holder.islinear = (int*)malloc(holder.dimensions*sizeof(int));}
#line 562 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 8:
#line 115 "./simuverif.y" // lalr1.cc:847
    {holder.modes = (int)(yystack_[1].value.doubVal); holder.kConst = (double*)malloc(holder.modes*sizeof(double));
					holder.gamma = (double*)malloc(holder.modes*sizeof(double));
					holder.islinear = (int*)malloc(holder.modes*sizeof(int));}
#line 570 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 9:
#line 118 "./simuverif.y" // lalr1.cc:847
    {holder.simulator = (char*)(yystack_[1].value.str); /*printf(" sim - %s -\n",$4);*/}
#line 576 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 10:
#line 119 "./simuverif.y" // lalr1.cc:847
    {holder.simulator = (char*)(yystack_[0].value.str); /*printf(" sim - %s -\n",$3);*/}
#line 582 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 11:
#line 120 "./simuverif.y" // lalr1.cc:847
    {holder.absoluteError = (double)(yystack_[1].value.doubVal); /*printf(" abserror - %f -\n",$4);*/}
#line 588 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 12:
#line 121 "./simuverif.y" // lalr1.cc:847
    {holder.relativeError = (double)(yystack_[1].value.doubVal); /*printf(" relerror - %f -\n",$4);*/}
#line 594 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 13:
#line 122 "./simuverif.y" // lalr1.cc:847
    {holder.absoluteError = (double)(yystack_[0].value.doubVal); /*printf(" abserror - %f -\n",$3);*/}
#line 600 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 14:
#line 123 "./simuverif.y" // lalr1.cc:847
    {holder.relativeError = (double)(yystack_[0].value.doubVal); /*printf(" relerror - %f -\n",$3);*/}
#line 606 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 15:
#line 124 "./simuverif.y" // lalr1.cc:847
    {holder.initMode = (int)(yystack_[1].value.doubVal);}
#line 612 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 16:
#line 125 "./simuverif.y" // lalr1.cc:847
    {holder.initMode = (int)(yystack_[0].value.doubVal);}
#line 618 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 17:
#line 126 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[7].value.intVal); 
							*(holder.initMin+(curdim-1)) = (yystack_[4].value.doubVal); *(holder.initMax+(curdim-1)) = (yystack_[2].value.doubVal); 
							/*printf(" dim - %d - lower %f - higher %f -\n",curdim,$7,$9);*/}
#line 626 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 18:
#line 129 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[6].value.intVal); 
							*(holder.initMin+(curdim-1)) = (yystack_[3].value.doubVal); *(holder.initMax+(curdim-1)) = (yystack_[1].value.doubVal); 
							/*printf(" dim - %d - lower %f - higher %f -\n",curdim,$6,$8);*/}
#line 634 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 19:
#line 132 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[1].value.doubVal);
							holder.initEqns = numEqns;
							/*printf(" init equations - %d -\n",numEqns);*/}
#line 642 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 20:
#line 135 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[0].value.doubVal);
							holder.initEqns = numEqns;
							/*printf(" init equations - %d -\n",numEqns);*/}
#line 650 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 21:
#line 138 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.dimensions*holder.initEqns*sizeof(double)); holder.index=0; printf("Starting init matrix");}
#line 656 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 22:
#line 138 "./simuverif.y" // lalr1.cc:847
    { holder.initMatrix = holder.ptr; printf(" Done parsing initial matrix \n");}
#line 662 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 23:
#line 139 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.initEqns*sizeof(double)); holder.index=0; printf("Starting init b");}
#line 668 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 24:
#line 139 "./simuverif.y" // lalr1.cc:847
    { holder.initB = holder.ptr; printf(" Done parsing initial B \n");}
#line 674 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 25:
#line 140 "./simuverif.y" // lalr1.cc:847
    {holder.deltaVal = (double)(yystack_[1].value.doubVal); /*printf(" delta - %f -\n",$4);*/}
#line 680 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 26:
#line 141 "./simuverif.y" // lalr1.cc:847
    {holder.tStep = (double)(yystack_[1].value.doubVal); /*printf(" tstep - %f -\n",$4);*/}
#line 686 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 27:
#line 142 "./simuverif.y" // lalr1.cc:847
    {holder.tGlobal = (double)(yystack_[1].value.doubVal); /*printf(" thorizon - %f -\n",$4);*/}
#line 692 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 28:
#line 143 "./simuverif.y" // lalr1.cc:847
    {holder.deltaVal = (double)(yystack_[0].value.doubVal); /*printf(" delta - %f -\n",$3);*/}
#line 698 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 29:
#line 144 "./simuverif.y" // lalr1.cc:847
    {holder.tStep = (double)(yystack_[0].value.doubVal); /*printf(" tstep - %f -\n",$3);*/}
#line 704 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 30:
#line 145 "./simuverif.y" // lalr1.cc:847
    {holder.tGlobal = (double)(yystack_[0].value.doubVal); /*printf(" thorizon - %f -\n",$3);*/}
#line 710 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 31:
#line 146 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[7].value.intVal); 
							*(holder.forbMin+(curdim-1)) = (yystack_[4].value.doubVal); *(holder.forbMax+(curdim-1)) = (yystack_[2].value.doubVal); 
							/*holder.biddenstr.push_back($3); printf(" forbidden - %s -\n",$3);*/}
#line 718 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 32:
#line 149 "./simuverif.y" // lalr1.cc:847
    {int curdim; curdim = (int)(yystack_[6].value.intVal); 
							*(holder.forbMin+(curdim-1)) = (yystack_[3].value.doubVal); *(holder.forbMax+(curdim-1)) = (yystack_[1].value.doubVal); 
							/*holder.biddenstr.push_back($3); printf(" forbidden - %s -\n",$3);*/}
#line 726 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 33:
#line 152 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[1].value.doubVal);
							holder.forbEqns = numEqns;
							/*printf(" forbidden equations - %d -\n",numEqns);*/}
#line 734 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 34:
#line 155 "./simuverif.y" // lalr1.cc:847
    {int numEqns; numEqns = (int)(yystack_[0].value.doubVal);
							holder.forbEqns = numEqns;
							/*printf(" forbidden - %d -\n",numEqns);*/}
#line 742 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 35:
#line 158 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.dimensions*holder.forbEqns*sizeof(double)); holder.index=0; printf("Starting forb matrix");}
#line 748 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 36:
#line 158 "./simuverif.y" // lalr1.cc:847
    { holder.forbMatrix = holder.ptr; printf(" Done parsing forbidden matrix \n");}
#line 754 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 37:
#line 159 "./simuverif.y" // lalr1.cc:847
    { holder.ptr = (double*)malloc(holder.forbEqns*sizeof(double)); holder.index=0; printf("Starting forb B");}
#line 760 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 38:
#line 159 "./simuverif.y" // lalr1.cc:847
    { holder.forbB = holder.ptr; printf(" Done parsing forbidden B \n");}
#line 766 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 39:
#line 160 "./simuverif.y" // lalr1.cc:847
    { holder.curMode = (int) (yystack_[1].value.doubVal);}
#line 772 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 40:
#line 161 "./simuverif.y" // lalr1.cc:847
    {holder.curMode = (int) (yystack_[0].value.doubVal);}
#line 778 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 41:
#line 162 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 2; /*printf(" annot - linear \n");*/}
#line 784 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 42:
#line 163 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 3; /*printf(" annot - contraction \n");*/}
#line 790 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 43:
#line 164 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 4; /*printf(" annot - exponential \n");*/}
#line 796 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 44:
#line 165 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 2; /*printf(" annot - linear \n");*/}
#line 802 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 45:
#line 166 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 3; /*printf(" annot - contraction \n");*/}
#line 808 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 46:
#line 167 "./simuverif.y" // lalr1.cc:847
    {holder.typeAnnot = 4; /*printf(" annot - exponential \n");*/}
#line 814 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 47:
#line 168 "./simuverif.y" // lalr1.cc:847
    {holder.beta = (yystack_[0].value.str); /*printf(" beta - %s -\n",$3);*/}
#line 820 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 48:
#line 169 "./simuverif.y" // lalr1.cc:847
    {holder.V = (yystack_[0].value.str); /*printf(" annot - %s -\n",$3);*/}
#line 826 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 49:
#line 170 "./simuverif.y" // lalr1.cc:847
    {*(holder.islinear+(holder.curMode-1)) = (int)(yystack_[1].value.doubVal);}
#line 832 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 50:
#line 171 "./simuverif.y" // lalr1.cc:847
    {*(holder.kConst+(holder.curMode-1)) = (double)(yystack_[1].value.doubVal); /*printf(" Kconst - %f -\n",$4);*/}
#line 838 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 51:
#line 172 "./simuverif.y" // lalr1.cc:847
    {*(holder.gamma+(holder.curMode-1)) = (double)(yystack_[1].value.doubVal); /*printf(" Gamma - %f -\n",$4);*/}
#line 844 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 52:
#line 173 "./simuverif.y" // lalr1.cc:847
    {*(holder.kConst+(holder.curMode-1)) = (double)(yystack_[0].value.doubVal); /*printf(" Kconst - %f -\n",$3);*/}
#line 850 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 53:
#line 174 "./simuverif.y" // lalr1.cc:847
    {*(holder.gamma+(holder.curMode-1)) = (double)(yystack_[0].value.doubVal); /*printf(" Gamma - %f -\n",$3);*/}
#line 856 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 54:
#line 175 "./simuverif.y" // lalr1.cc:847
    {holder.val1 = (int)(yystack_[4].value.intVal); holder.val2 = (int)(yystack_[2].value.intVal); holder.visuFile = (char*) (yystack_[0].value.str); printf(" visualize - %d and %d to %s \n",(yystack_[4].value.intVal),(yystack_[2].value.intVal),(yystack_[0].value.str));}
#line 862 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 55:
#line 176 "./simuverif.y" // lalr1.cc:847
    {holder.val1 = -1; holder.val2 = -1; holder.visuFile = (char*) (yystack_[0].value.str); printf(" visualize - -1 and -1 to %s \n",(yystack_[0].value.str));}
#line 868 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 56:
#line 177 "./simuverif.y" // lalr1.cc:847
    {holder.val1 = (yystack_[4].value.intVal); holder.val2 = 0; holder.visuFile = (char*) (yystack_[0].value.str); /*printf(" visualize - %d and time to %s \n",$2,$6);*/}
#line 874 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 57:
#line 181 "./simuverif.y" // lalr1.cc:847
    {double val = (double)(yystack_[0].value.doubVal); *(holder.ptr+holder.index) = val; printf("The value is %f and index is %d \n",val,holder.index); holder.index++;}
#line 880 "./simuverif.tab.c" // lalr1.cc:847
    break;

  case 58:
#line 182 "./simuverif.y" // lalr1.cc:847
    {double val = (double)(yystack_[0].value.doubVal); *(holder.ptr+holder.index) = val; printf("The value is %f and index is %d \n",val,holder.index); holder.index++;}
#line 886 "./simuverif.tab.c" // lalr1.cc:847
    break;


#line 890 "./simuverif.tab.c" // lalr1.cc:847
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


  const signed char BisonParser::yypact_ninf_ = -112;

  const signed char BisonParser::yytable_ninf_ = -59;

  const short int
  BisonParser::yypact_[] =
  {
       7,    11,    31,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    32,    96,  -112,     7,     2,
       3,    -2,     5,     6,    33,    13,  -112,  -112,    14,    16,
      17,    18,    34,    21,  -112,  -112,   -27,    25,    63,    26,
      64,    29,    65,    67,    68,  -112,  -112,    66,  -112,    70,
    -112,    69,  -112,    71,  -112,    73,  -112,    75,    80,    76,
    -112,    72,    81,    78,  -112,    79,  -112,    82,  -112,    83,
    -112,    85,    86,    87,  -112,    84,    89,  -112,  -112,  -112,
     -24,    88,  -112,  -112,    90,  -112,    91,    92,  -112,  -112,
      93,    10,    95,    99,   100,   101,   103,   104,   102,   106,
     105,   105,   107,   108,   109,   110,   111,   112,   113,   105,
     105,   115,   116,   117,   118,   119,   120,   121,  -112,    94,
     124,  -112,  -112,  -112,  -112,  -112,   122,   123,  -112,   125,
     127,   128,  -112,  -112,  -112,  -112,   126,   129,  -112,   130,
     131,  -112,  -112,  -112,  -112,  -112,  -112,  -112,   114,   132,
     133,   135,   136,  -112,  -112,   134,   137,  -112,  -112,  -112,
    -112,   138,   140,   105,   141,   143,   144,   148,  -112,   146,
     150,   151,  -112,   152,  -112,   147,   156,  -112,  -112
  };

  const unsigned char
  BisonParser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     2,     3,     0,
       0,     0,     0,     0,     0,     0,    21,    23,     0,     0,
       0,     0,     0,     0,    35,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     1,     4,     0,     6,     0,
       7,     0,    10,     0,    13,     0,    14,     0,     0,     0,
      20,     0,     0,     0,    16,     0,    29,     0,    30,     0,
      28,     0,     0,     0,    34,     0,     0,    45,    44,    46,
       0,     0,    40,    48,     0,    52,     0,     0,    53,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    55,     0,
       0,     5,     8,     9,    11,    12,     0,     0,    19,    57,
       0,     0,    15,    26,    27,    25,     0,     0,    33,     0,
       0,    42,    41,    43,    39,    50,    49,    51,     0,     0,
       0,     0,     0,    22,    24,     0,     0,    36,    38,    56,
      54,     0,     0,     0,     0,     0,     0,     0,    59,     0,
       0,     0,    18,     0,    32,     0,     0,    17,    31
  };

  const signed char
  BisonParser::yypgoto_[] =
  {
    -112,  -112,    74,  -112,  -112,  -112,  -112,  -112,  -111,  -112
  };

  const short int
  BisonParser::yydefgoto_[] =
  {
      -1,    26,    27,    28,    71,    72,    85,    86,   140,   162
  };

  const short int
  BisonParser::yytable_[] =
  {
     141,    87,    88,    89,   121,   122,   123,    90,   149,   150,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    61,    23,    24,   129,    57,    59,    25,    63,
      65,    62,    58,    60,    29,    64,    66,    69,    73,   130,
      75,    77,    79,    70,    74,    83,    76,    78,    80,    91,
      94,    84,   178,    97,    30,    92,    95,    67,    81,    98,
      53,    54,    68,    82,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    55,     0,    96,   100,
       0,     0,    56,     0,     0,    93,   102,    99,   110,   101,
     103,   105,   104,   106,   107,   108,   109,   111,   112,   113,
     119,   117,   114,   115,   116,   120,   158,   118,   124,   131,
     125,   126,   127,   132,   133,   134,   128,   135,   137,   136,
     138,   142,   143,   144,   145,   139,   146,   148,   147,   151,
     152,   153,   154,   155,   156,   157,   159,   169,   160,     0,
       0,     0,   165,   161,   163,   164,   -58,   167,   168,   166,
       0,     0,     0,   171,   174,   170,   172,   173,   175,   176,
     177,   187,   179,   180,   181,   182,   183,   184,   185,   186,
     188
  };

  const short int
  BisonParser::yycheck_[] =
  {
     111,    28,    29,    30,    28,    29,    30,    34,   119,   120,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    34,    26,    27,    25,    34,    34,    31,    34,
      34,    43,    40,    40,    33,    40,    40,    34,    34,    39,
      34,    34,    34,    40,    40,    34,    40,    40,    40,    34,
      34,    40,   173,    34,    33,    40,    40,    34,    34,    40,
      38,    39,    39,    39,    33,    33,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    33,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    33,     0,    -1,    34,    32,
      -1,    -1,    28,    -1,    -1,    42,    40,    42,    36,    41,
      40,    40,    43,    40,    39,    35,    40,    36,    40,    40,
      36,    35,    40,    40,    39,    36,    32,    40,    40,    34,
      40,    40,    40,    34,    34,    34,    43,    34,    36,    35,
      34,    34,    34,    34,    34,    40,    35,    34,    36,    34,
      34,    34,    34,    34,    34,    34,    32,    43,    36,    -1,
      -1,    -1,    36,    40,    37,    37,    41,    37,    37,    40,
      -1,    -1,    -1,    40,    40,    43,    41,    41,    41,    41,
      40,    34,    41,    40,    40,    37,    40,    37,    37,    37,
      34
  };

  const unsigned char
  BisonParser::yystos_[] =
  {
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    26,    27,    31,    45,    46,    47,    33,
      33,    33,    33,    33,    33,    33,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    33,    33,    33,    33,    33,
      33,    33,    33,    38,    39,     0,    46,    34,    40,    34,
      40,    34,    43,    34,    40,    34,    40,    34,    39,    34,
      40,    48,    49,    34,    40,    34,    40,    34,    40,    34,
      40,    34,    39,    34,    40,    50,    51,    28,    29,    30,
      34,    34,    40,    42,    34,    40,    34,    34,    40,    42,
      32,    41,    40,    40,    43,    40,    40,    39,    35,    40,
      36,    36,    40,    40,    40,    40,    39,    35,    40,    36,
      36,    28,    29,    30,    40,    40,    40,    40,    43,    25,
      39,    34,    34,    34,    34,    34,    35,    36,    34,    40,
      52,    52,    34,    34,    34,    34,    35,    36,    34,    52,
      52,    34,    34,    34,    34,    34,    34,    34,    32,    32,
      36,    40,    53,    37,    37,    36,    40,    37,    37,    43,
      43,    40,    41,    41,    40,    41,    41,    40,    52,    41,
      40,    40,    37,    40,    37,    37,    37,    34,    34
  };

  const unsigned char
  BisonParser::yyr1_[] =
  {
       0,    44,    45,    46,    46,    47,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    48,    47,    49,    47,    47,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    50,    47,    51,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    52,    53,    52
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     1,     2,     5,     3,     3,     5,     5,
       3,     5,     5,     3,     3,     5,     3,    11,     9,     5,
       3,     0,     6,     0,     6,     5,     5,     5,     3,     3,
       3,    11,     9,     5,     3,     0,     6,     0,     6,     5,
       3,     5,     5,     5,     3,     3,     3,     3,     3,     5,
       5,     5,     3,     3,     6,     4,     6,     1,     0,     4
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const BisonParser::yytname_[] =
  {
  "$end", "error", "$undefined", "DIMENSIONS", "MODES", "SIMULATOR",
  "ABSERROR", "RELERROR", "INIT", "INITE", "INITM", "INITB", "INITMODE",
  "TSTEP", "THORIZON", "DELTA", "FORB", "FORBE", "FORBM", "FORBB",
  "ANNOTTYPE", "ANNOTMODE", "ANNOT", "KCONST", "ISLINEAR", "TIME", "GAMMA",
  "BETA", "CONTRACTION", "LINEAR", "EXPONENTIAL", "VISU", "TO", "EQ", "QU",
  "IN", "SO", "SC", "ALL", "XID", "NUMBER", "CO", "STRING", "WORD",
  "$accept", "ss", "sentences", "sentence", "$@1", "$@2", "$@3", "$@4",
  "numbersequence", "$@5", YY_NULLPTR
  };


  const unsigned char
  BisonParser::yyrline_[] =
  {
       0,    91,    91,    95,    96,   100,   106,   112,   115,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   129,   132,
     135,   138,   138,   139,   139,   140,   141,   142,   143,   144,
     145,   146,   149,   152,   155,   158,   158,   159,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   181,   182,   182
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
      35,    36,    37,    38,    39,    40,    41,    42,    43
    };
    const unsigned int user_token_number_max_ = 298;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // yy
#line 1323 "./simuverif.tab.c" // lalr1.cc:1155
#line 185 "./simuverif.y" // lalr1.cc:1156



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


