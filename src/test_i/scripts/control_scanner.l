%top{
#ifndef YY_TYPEDEF_YY_CONTROL_SCANNER_T
#define YY_TYPEDEF_YY_CONTROL_SCANNER_T

#include "ardrone_types.h"

// forward declarations
typedef void* yyscan_t;
template <typename ConfigurationType,
          typename RecordType>
class Stream_IYaccRecordParser_T;
struct Common_ParserConfiguration;
typedef Stream_IYaccRecordParser_T<struct Common_ParserConfiguration,
                                   ARDrone_DeviceConfiguration_t> ARDrone_Control_IParser_t;
template <typename ScannerStateType,
          typename ParserInterfaceType>
class Common_ILexScanner_T;
struct Common_ScannerState;
typedef Common_ILexScanner_T<struct Common_ScannerState,
                             ARDrone_Control_IParser_t> ARDrone_Control_IScanner_t;
#endif

#define YY_DECL int ARDrone_Control_Scanner_lex (yyscan_t yyscanner)
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
%option extra-type="ARDrone_Control_IScanner_t*"
%option nounistd nounput noyymore

/* *IMPORTANT NOTE*: "...A negated character class such as the example "[^A-Z]"
                     above will match a newline unless "\n" (or an equivalent
                     escape sequence) is one of the characters explicitly
                     present in the negated character class (e.g., "[^A-Z\n]").
                     This is unlike how many other regular expression tools
                     treat negated character classes, but unfortunately the
                     inconsistency is historically entrenched. Matching newlines
                     means that a pattern like [^"]* can match the entire input
                     unless there's another quote in the input. ..." */
CATEGORY               [^:\n]
COLON                  :
DOT                    [\x00]
EQUALS                 =
KEY                    [^=\n]
VALUE                  [^\n]
OCTET                  [\x00-\xFF]

%s                     dot_or_category
%s                     delimiter_1
%s                     key
%s                     delimiter_2
%s                     value
%s                     newline

%% /* end of definitions */

%{
  static std::string current_string;
  static ARDrone_DeviceConfigurationKeyValue_t current_key_value;
%}

<INITIAL>{
{OCTET}{1}      { yyless (0);
                  BEGIN (dot_or_category); }
} // end <INITIAL>
<dot_or_category>{
{DOT}           { // sanity check(s)
                  ACE_ASSERT (yyscanner);

                  ARDrone_Control_IScanner_t* iscanner_p =
                      ARDrone_Control_Scanner_get_extra (yyscanner);
                  ACE_ASSERT (iscanner_p);
                  ARDrone_Control_IParser_t* iparser_p =
                      const_cast<ARDrone_Control_IParser_t*> (iscanner_p->getP_2 ());
                  ACE_ASSERT (iparser_p);
                  ARDrone_DeviceConfiguration_t* configuration_p =
                      &iparser_p->current ();

                  try {
                    iparser_p->record (configuration_p);
                  } catch (...) {
                    ACE_DEBUG ((LM_ERROR,
                                ACE_TEXT ("caught exception in Net_IYaccStreamParser_T::record(), continuing\n")));
                  }
                  BEGIN (INITIAL); }
{CATEGORY}*     { current_string = yytext;
                  BEGIN (delimiter_1); }
} // end <dot_or_category>
<delimiter_1>{
:               { BEGIN (key); }
} // end <delimiter_1>
<key>{
{KEY}*          { current_key_value.first = yytext;
                  current_key_value.first =
                    Common_Tools::strip (current_key_value.first);
                  BEGIN (delimiter_2); }
} // end <key>
<delimiter_2>{
=               { BEGIN (value); }
} // end <delimiter_2>
<value>{
{VALUE}*/\n     { current_key_value.second = yytext;
                  current_key_value.second =
                    Common_Tools::strip (current_key_value.second);

                  // sanity check(s)
                  ACE_ASSERT (yyscanner);

                  ARDrone_Control_IScanner_t* iscanner_p =
                    ARDrone_Control_Scanner_get_extra (yyscanner);
                  ACE_ASSERT (iscanner_p);
                  ARDrone_Control_IParser_t* iparser_p =
                    const_cast<ARDrone_Control_IParser_t*> (iscanner_p->getP_2 ());
                  ACE_ASSERT (iparser_p);
                  ARDrone_DeviceConfiguration_t& configuration_r =
                    iparser_p->current ();

                  // sanity check(s)
                  ACE_ASSERT (!current_string.empty ());

                  ARDrone_DeviceConfigurationIterator_t iterator =
                    configuration_r.find (current_string);
                  if (iterator == configuration_r.end ())
                  {
                    ARDrone_DeviceConfigurationCategory_t category_s;
                    configuration_r.insert (std::make_pair (current_string,
                                                            category_s));
                    iterator =
                      configuration_r.find (current_string);
                    ACE_ASSERT (iterator != configuration_r.end ());
                  } // end IF
                  (*iterator).second.push_back (current_key_value);
                  BEGIN (newline); }
} // end <value>
<newline>{
\n               { current_string.clear ();
                   current_key_value.first.clear ();
                   current_key_value.second.clear ();
                   BEGIN (dot_or_category); }
} // end <newline>
<*><<EOF>>       { return -1; }

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
  ARDrone_Control_IScanner_t* iscanner_p =
    ARDrone_Control_Scanner_get_extra (yyscanner);

  // sanity check(s)
//  ACE_UNUSED_ARG (yyg);
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
                ACE_TEXT ("failed to Common_IScannerBase::switchBuffer(), aborting\n")));
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
