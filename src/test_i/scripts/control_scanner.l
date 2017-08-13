%top{
#ifndef YY_TYPEDEF_YY_CONTROL_SCANNER_T
#define YY_TYPEDEF_YY_CONTROL_SCANNER_T
typedef void* yyscan_t;
#endif

class ARDrone_Control_IParser;

#define YY_DECL                                                   \
int                                                               \
ARDrone_Control_Scanner_lex (yyscan_t yyscanner,                  \
                             ARDrone_Control_IParser* iparser_in)
// ... and declare it for the parser's sake
YY_DECL;

void ARDrone_Control_Scanner_set_column (int, yyscan_t);

#define YY_NO_UNISTD_H

//#define FLEXINT_H
}

%{
#include <sstream>

#include "ace/Log_Msg.h"

#include "common_tools.h"

#include "ace/Synch.h"
#include "ardrone_common.h"
#include "ardrone_macros.h"
%}

%option header-file="ardrone_control_scanner.h" outfile="ardrone_control_scanner.cpp"
%option batch 8bit nodefault never-interactive nostdinit yylineno yywrap
/* %option c++ yyclass="ARDrone_Control_Scanner" */
/* *NOTE*: the 'line' directives confuse gdb */
/* *NOTE*: 'noline' not supported (on Linux, flex 2.5.39)
           --> use --noline and (manually) remove '#line's introduced by %top */
/* %option ansi-definitions ansi-prototypes */
/* *IMPORTANT NOTE*: 'read' requires 'unistd'(.h) */
%option reentrant pointer prefix="ARDrone_Control_Scanner_" nomain
/* *TODO*: find out why 'read' does not compile (on Linux, flex 2.5.39) */
%option align read full
%option backup debug perf-report perf-report verbose warn
/* *IMPORTANT NOTE*: flex 2.5.4 does not recognize 'reentrant, nounistd,
                     ansi-definitions, ansi-prototypes, header-file extra-type'
*/
%option extra-type="ARDrone_Control_IParser*"
%option nounistd nounput noyymore

OCTET                  [\x00-\xFF]
EQUALS                 =
KEY                    [^=]
VALUE                  [^\n]

%s                     key
%s                     delimiter
%s                     value
%s                     newline

%% /* end of definitions */

%{
  // sanity check(s)
  ACE_ASSERT (iparser_in);

  ARDrone_DeviceConfiguration_t& configuration_r = iparser_in->current ();

  static std::string current_key_string;
%}

<INITIAL>{
[[:blank:]\r\n] {}
[^=]            { yyless (0);
                  BEGIN (key); }
} // end <INITIAL>
<key>{
{KEY}*          { current_key_string = Common_Tools::strip (yytext);
                  BEGIN (delimiter); }
} // end <key>
<delimiter>{
=               { BEGIN (value); }
} // end <delimiter>
<value>{
{VALUE}*        { ACE_ASSERT (!current_key_string.empty ());
                  configuration_r.insert (std::make_pair (current_key_string,
                                                          Common_Tools::strip (yytext)));
                  BEGIN (newline); }
} // end <value>
<newline>{
\n              { current_key_string.clear ();
                  BEGIN (key); }
} // end <newline>
<<EOF>>                { return -1; }
<*>{OCTET}             { /* *TODO*: use (?s:.) ? */
                         yyterminate (); }

%% /* end of rules */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
ARDrone_Control_Scanner_wrap (yyscan_t yyscanner)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDrone_Control_Scanner_wrap"));

  // sanity check(s)
  ACE_ASSERT (yyscanner);
//  struct yyguts_t* yyg = static_cast<struct yyguts_t*> (yyscanner);
//  ACE_UNUSED_ARG (yyg);
  ARDrone_Control_IParser* iscanner_p =
    ARDrone_Control_Scanner_get_extra (yyscanner);

  // sanity check(s)
  ACE_ASSERT (iscanner_p);
  if (!iscanner_p->isBlocking ())
    return 1; // not enough data, cannot proceed

  // *NOTE*: there is more data
  // 1. gobble/save the rest
  // 2. switch buffers
  // 3. unput the rest
  // 4. continue scanning

  // step1
  //std::string the_rest;
  //the_rest.append (yytext, yyleng);
//  for (char c = yyinput (yyscanner);
//       c != EOF;
//       c = yyinput (yyscanner));
  //yyg->yy_c_buf_p += yyleng;
  //yyg->yy_hold_char = *yyg->yy_c_buf_p;
  //if (yy_flex_debug)
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("the rest: \"%s\" (%d byte(s))\n"),
  //              ACE_TEXT (the_rest.c_str ()),
  //              the_rest.size ()));

  // step2
  if (!iscanner_p->switchBuffer ())
  {
    // *NOTE*: most probable reason: received session end message
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to Common_IScanner::switchBuffer(), aborting\n")));
    return 1;
  } // end IF

  // step3
  //for (std::string::reverse_iterator iterator = the_rest.rbegin ();
  //     iterator != the_rest.rend ();
  //     ++iterator)
  //  unput (*iterator);

  // step4
  //yyg->yy_did_buffer_switch_on_eof = 1;
  // yymore ();

  return 0;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
