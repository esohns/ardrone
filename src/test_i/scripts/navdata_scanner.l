%top{
#ifndef YY_TYPEDEF_YY_NAVDATA_SCANNER_T
#define YY_TYPEDEF_YY_NAVDATA_SCANNER_T
typedef void* yyscan_t;
#endif

class ARDrone_NavData_IParser;

#define YY_DECL                                                   \
int                                                               \
ARDrone_NavData_Scanner_lex (yyscan_t yyscanner,                  \
                             ARDrone_NavData_IParser* iparser_in)
// ... and declare it for the parser's sake
YY_DECL;

void ARDrone_NavData_Scanner_set_column (int, yyscan_t);

#define YY_NO_UNISTD_H

//#define FLEXINT_H
}

%{
#include <sstream>

//#include <ace/ace_wchar.h>
#include <ace/Log_Msg.h>
//#include <ace/Message_Block.h>
//#include <ace/OS_Memory.h>

#include "ardrone_common.h"
#include "ardrone_macros.h"
%}

%option header-file="ardrone_navdata_scanner.h" outfile="ardrone_navdata_scanner.cpp"
%option batch 8bit nodefault never-interactive nostdinit yylineno yywrap
/* %option c++ yyclass="ARDrone_NavData_Scanner" */
/* *NOTE*: the 'line' directives confuse gdb */
/* *NOTE*: 'noline' not supported (on Linux, flex 2.5.39)
           --> use --noline and (manually) remove '#line's introduced by %top */
/* %option ansi-definitions ansi-prototypes */
/* *IMPORTANT NOTE*: 'read' requires 'unistd'(.h) */
%option reentrant pointer prefix="ARDrone_NavData_Scanner_" nomain
/* *TODO*: find out why 'read' does not compile (on Linux, flex 2.5.39) */
%option align read full
%option backup debug perf-report perf-report verbose warn
/* *IMPORTANT NOTE*: flex 2.5.4 does not recognize 'reentrant, nounistd,
                     ansi-definitions, ansi-prototypes, header-file extra-type'
*/
%option extra-type="ARDrone_NavData_IParser*"
%option nounistd nounput noyymore

OCTET                  [\x00-\xFF]
HEADER                 \x55\x66\x77\x88

%s                     state
%s                     _sequence
%s                     vision
%s                     option_id
%s                     option_size
%s                     option_data
%s                     end_of_frame

%% /* end of definitions */

%{
  // sanity check(s)
  ACE_ASSERT (iparser_in);

  struct _navdata_t& message_r = iparser_in->current ();

  std::stringstream converter;
  static unsigned int option_offset = 0;
  struct _navdata_option_t current_option_s;
%}

<INITIAL>{
{HEADER}   { option_offset = yyleng;
             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
             converter.clear ();
             converter << yytext;
             uint32_t message_header;
             converter >> message_header;
             message_r.header = message_header;

             BEGIN (state);
           }
} // end <INITIAL>
<state>{
{OCTET}{4} { option_offset += yyleng;
             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
             converter.clear ();
             converter << yytext;
             uint32_t ardrone_state;
             converter >> ardrone_state;
             message_r.ardrone_state = ardrone_state;

             BEGIN (_sequence);
           }
} // end <state>
<_sequence>{
{OCTET}{4} { option_offset += yyleng;
             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
             converter.clear ();
             converter << yytext;
             uint32_t sequence;
             converter >> sequence;
             message_r.sequence = sequence;

             BEGIN (vision);
           }
} // end <sequence>
<vision>{
{OCTET}{4} { option_offset += yyleng;
             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
             converter.clear ();
             converter << yytext;
             uint32_t vision_flags;
             converter >> vision_flags;
             message_r.vision_defined = vision_flags;

             ACE_OS::memset (&current_option_s, 0, sizeof (struct _navdata_option_t));
             BEGIN (option_id);
           }
} // end <vision>
<option_id>{
{OCTET}{2} { converter.str (ACE_TEXT_ALWAYS_CHAR (""));
             converter.clear ();
             converter << yytext;
             converter >> current_option_s.tag;

             try {
               iparser_in->addOption (option_offset);
             } catch (...) {
               ACE_DEBUG ((LM_ERROR,
                           ACE_TEXT ("failed to ARDrone_NavData_IParser::addOption(%u), aborting\n"),
                           option_offset));
               yyterminate ();
             }

             BEGIN (option_size);
           }
} // end <option_id>
<option_size>{
{OCTET}{2} { converter.str (ACE_TEXT_ALWAYS_CHAR (""));
             converter.clear ();
             converter << yytext;
             converter >> current_option_s.size;

//            // *TODO*: iff there is only ever a single option (as the header
//            //         definition implies), framing the message accordingly
//            //         would be preferable
//            ACE_ASSERT (!message_r.options[0].data);
//            ACE_NEW_NORETURN (message_r.options[0].data,
//                              uint8_t[message_r.options[0].size]);
//            message_r.options[0].data =
//                (uint8_t[])new (std::nothrow) uint8_t[message_r.options[0].size];
//            if (!message_r.options[0].data)
//            {
//              ACE_DEBUG ((LM_CRITICAL,
//                          ACE_TEXT ("failed to allocate memory (%u byte(s)): \"%m\", aborting\n"),
//                          message_r.options[0].size));
//              yyterminate ();
//            } // end IF
             option_offset += current_option_s.size;

             BEGIN (option_data);
           }
} // end <option_size>
<option_data>{
{OCTET}    { yyless (0);

             // skip ahead message_r.len bytes
             /* undo the effects of YY_DO_BEFORE_ACTION */
             *yy_cp = yyg->yy_hold_char;

             char c = 0;
             for (unsigned int i = 0;
                  i < current_option_s.size;
                  ++i)
               c = yyinput (yyscanner);
             ACE_UNUSED_ARG (c);

             struct _navdata_t* message_p = &message_r;
             try {
               iparser_in->record (message_p);
             } catch (...) {
               ACE_DEBUG ((LM_ERROR,
                           ACE_TEXT ("failed to Net_IRecordParser_T::record(), aborting\n")));
               yyterminate ();
             }

             if (current_option_s.tag != NAVDATA_CKS_TAG)
               BEGIN (option_id);
             else
               return 0;
           }
} // end <option_data>
<<EOF>>                { return -1; }
<*>{OCTET}             { /* *TODO*: use (?s:.) ? */
                         yyterminate (); }

%% /* end of rules */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
ARDrone_NavData_Scanner_wrap (yyscan_t yyscanner)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDrone_NavData_Scanner_wrap"));

  struct yyguts_t* yyg = static_cast<struct yyguts_t*> (yyscanner);
  ARDrone_NavData_IParser* iscanner_p =
    ARDrone_NavData_Scanner_get_extra (yyscanner);

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
