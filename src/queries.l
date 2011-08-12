%{
/*
   Copyright 2011 Carl Anderson

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "queries.hpp"

namespace ash {
namespace query {

// Temp variables for parsing.
std::string * name, * desc, * sql;

}  // namespace query

using std::cout;
using std::endl;
using std::string;
using std::map;

map<string, string> Queries::descriptions;
map<string, string> Queries::queries;


void Queries::add(string & name, string & desc, string & sql) {
  Queries::descriptions[name] = desc;
  Queries::queries[name] = sql;
}


void Queries::insert(ostream & out) {
  map<string, string>::iterator i, e;
  for (i = queries.begin(), e = queries.end(); i != e; ++i) {
    out << i -> first << endl;
    out << "  " << descriptions[i -> first] << endl;
    out << "  " << i -> second << endl;
  }
}


map<string, string> Queries::get_desc() {
  return Queries::descriptions;
}


string Queries::get_desc(const string & name) {
  if (Queries::descriptions.find(name) == Queries::descriptions.end())
    return "";
  return Queries::descriptions[name];
}


map<string, string> Queries::get_sql() {
  return Queries::queries;
}


string Queries::get_sql(const string & name) {
  if (Queries::queries.find(name) == Queries::queries.end())
    return "";
  return Queries::queries[name];
}


void fail(const char * message) {
  printf("ERROR: Line %d - %s.\n\n", yylineno, message);
  exit(1);
}


void expected(const char * message) {
  printf("ERROR: Line %d - Expected %s\n\n", yylineno, message);
  exit(1);
}

}  // namespace ash
%}

%option yylineno
%x Q1 Q2 QUERY D1 DESC STR SQL

%%
<INITIAL>[ \t\n]+	;  // WHITESPACE
<INITIAL>#.*\n		;  // # LINE COMMENT.
<INITIAL>[a-zA-Z_0-9-]+	{
			  ash::query::desc = ash::query::sql = 0;
			  ash::query::name = new std::string(yytext);
			  BEGIN(Q1);
			}

  /* State Q1 - Read a queary name, expecting a COLON. */
<Q1>#.*			;  // LINE COMMENT.
<Q1>[ \t\n]+		;  // WHITESPACE
<Q1>":"			BEGIN(Q2);
<Q1>[^#: \t\n]+		ash::expected(":");


 /* State Q2 - Read a query name and COLON, expecting an LBRACE. */
<Q2>#.*			;  // LINE COMMENT.
<Q2>[ \t\n]+		;  // WHITESPACE
<Q2>"{"			BEGIN(QUERY);
<Q2>[^#: \t\n]+		ash::expected("{");


 /* State QUERY - Expecting a description and sql definition. */
<QUERY>#.*		;  // LINE COMMENT.
<QUERY>[ \t\n]+		;  // WHITESPACE
<QUERY>"description"	{
			  if (ash::query::desc)
			    ash::fail("multiple descriptions defined");
			  BEGIN(D1);
			}
<QUERY>"sql"		{
			  if (ash::query::sql)
			    ash::fail("multiple sql sections defined");
			  BEGIN(SQL);
			}
<QUERY>"}"		{
			  using namespace ash;
			  using namespace ash::query;

			  // These checks should never be needed.
			  if (!name) expected("a query name for the query.");
			  if (!desc) expected("a description in the query.");
			  if (!sql) expected("a sql field in the query.");

			  Queries::add(*name, *desc, *sql);

			  // Clean up for the next query to be parsed.
			  delete name; delete desc; delete sql;
			  name = desc = sql = 0;
			  BEGIN(INITIAL);
			}

 /* State D1 - Read keyword 'description', expecting a COLON. */
<D1>#.*			;  // LINE COMMENT.
<D1>[ \t\n]+		;  // WHITESPACE
<D1>":"			BEGIN(DESC);
<D1>[^#: \t\n]+		ash::expected(":");

 /* State DESC - Read 'description:' - expecting a quoted string. */
<DESC>#.*		;  // LINE COMMENT.
<DESC>[ \t\n]+		;  // WHITESPACE
<DESC>\"		BEGIN(STR);
<DESC>[^# \t\n\"]	ash::expected("\"");

 /* State STR - Read a quoted string. */
<STR>[^"\n]*\"		{
			  ash::query::desc = new std::string(yytext, yyleng-1);
			  BEGIN(QUERY);
			}
<STR>\n			ash::expected("\" - Multi-line strings are illegal.");

 /* State SQL - read 'sql' token, expecting a COLON. */
<SQL>#.*		;  // LINE COMMENT.
<SQL>[ \t\n]+		;  // WHITESPACE
<SQL>:[ \t\n]*\{[^}]*	{
			  // Eat the colon and brace.
			  while (yytext[0] != '{') ++yytext;
			  ash::query::sql = new std::string(++yytext);
			}
<SQL>"}"		BEGIN(QUERY);
<SQL>[^# \t\n}]		ash::expected(":");

  /* FAIL BUCKET - this matches any character that is not covered above. */
.			{
			  std::cerr << "Unexpected character in file." << std::endl;
			  exit(1);
			}
%%


int yywrap() {
  printf("THAT'S A YYWRAP!!!\n");
  return 1;
}
