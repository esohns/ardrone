%top{
#ifndef YY_TYPEDEF_YY_NAVDATA_SCANNER_T
#define YY_TYPEDEF_YY_NAVDATA_SCANNER_T

#define YY_STRUCT_YY_BUFFER_STATE
#include "ardrone_navdata_scanner.h"
#undef YY_STRUCT_YY_BUFFER_STATE

#endif

// forward declarations
typedef void* yyscan_t;
class ARDrone_NavData_IParser;

#define YY_DECL int ARDrone_NavData_Scanner_lex (yyscan_t yyscanner)
// ... and declare it for the parser's sake
YY_DECL;

void ARDrone_NavData_Scanner_set_column (int, yyscan_t);

//#define YY_NO_UNISTD_H

//#define FLEXINT_H
}

%{
#include <sstream>

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "ardrone_common.h"
#include "ardrone_macros.h"

#include "ardrone_module_navdata_decoder.h"
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
HEADER                 \x88\x77\x66\x55

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
  ACE_ASSERT (yyscanner);

  ARDrone_NavData_IParser* iparser_p =
    ARDrone_NavData_Scanner_get_extra (yyscanner);
  ACE_ASSERT (iparser_p);

  struct _navdata_t& message_r = iparser_p->current ();

  static unsigned int option_offset = 0;
  static struct _navdata_option_t current_option_s;
  static size_t option_data_bytes_i = 0;
%}

<INITIAL>{
{HEADER}   { ACE_ASSERT (yyleng == 4);
             option_offset = yyleng;
             uint32_t* integer_p = reinterpret_cast<uint32_t*> (yytext);
             message_r.header =
               ((ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN) ? ACE_SWAP_LONG (*integer_p)
                                                      : *integer_p);

             BEGIN (state);
           }
} // end <INITIAL>
<state>{
{OCTET}{4} { ACE_ASSERT (yyleng == 4);
             option_offset += yyleng;
             uint32_t* integer_p = reinterpret_cast<uint32_t*> (yytext);
             message_r.ardrone_state =
               ((ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN) ? ACE_SWAP_LONG (*integer_p)
                                                      : *integer_p);

             BEGIN (_sequence);
           }
} // end <state>
<_sequence>{
{OCTET}{4} { ACE_ASSERT (yyleng == 4);
             option_offset += yyleng;
             uint32_t* integer_p = reinterpret_cast<uint32_t*> (yytext);
             message_r.sequence =
               ((ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN) ? ACE_SWAP_LONG (*integer_p)
                                                      : *integer_p);

             BEGIN (vision);
           }
} // end <sequence>
<vision>{
{OCTET}{4} { ACE_ASSERT (yyleng == 4);
             option_offset += yyleng;
             int32_t* integer_p = reinterpret_cast<int32_t*> (yytext);
             message_r.vision_defined =
               ((ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN) ? ACE_SWAP_LONG (*integer_p)
                                                      : *integer_p);

             ACE_OS::memset (&current_option_s, 0, sizeof (struct _navdata_option_t));
             BEGIN (option_id);
           }
} // end <vision>
<option_id>{
{OCTET}{2} { ACE_ASSERT (yyleng == 2);
             uint16_t* integer_p = reinterpret_cast<uint16_t*> (yytext);
             current_option_s.tag =
               ((ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*integer_p)
                                                      : *integer_p);

             ACE_ASSERT (iparser_p);
             try {
               iparser_p->addOption (option_offset);
             } catch (...) {
               ACE_DEBUG ((LM_ERROR,
                           ACE_TEXT ("failed to ARDrone_NavData_IParser::addOption(%u), aborting\n"),
                           option_offset));
               yyterminate ();
             }
             option_offset += yyleng;

             BEGIN (option_size);
           }
} // end <option_id>
<option_size>{
{OCTET}{2} { ACE_ASSERT (yyleng == 2);
             option_offset += yyleng;
             uint16_t* integer_p = reinterpret_cast<uint16_t*> (yytext);
             current_option_s.size =
               ((ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*integer_p)
                                                      : *integer_p);
             option_data_bytes_i = current_option_s.size - 4;
             BEGIN (option_data);
           }
} // end <option_size>
<option_data>{
{OCTET}    { 
             if (option_data_bytes_i)
             {
               --option_data_bytes_i;
               if (option_data_bytes_i)
                 break;
             } // end IF

             option_offset += current_option_s.size - 4;

             if (current_option_s.tag != NAVDATA_CKS_TAG)
               BEGIN (option_id);
             else
             {
               ACE_ASSERT (iparser_p);
               struct _navdata_t* message_p = &message_r;
               try {
                 iparser_p->record (message_p);
               } catch (...) {
                 ACE_DEBUG ((LM_ERROR,
                             ACE_TEXT ("failed to Stream_IYaccStreamParser_T::record(), aborting\n")));
                 yyterminate ();
               }
               BEGIN (INITIAL);
               option_offset = 0;

               return 0;
             } // end ELSE
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

  // sanity check(s)
  ACE_ASSERT (yyscanner);
//  struct yyguts_t* yyg = static_cast<struct yyguts_t*> (yyscanner);
//  ACE_UNUSED_ARG (yyg);
  ARDrone_NavData_IParser* iparser_p =
    ARDrone_NavData_Scanner_get_extra (yyscanner);
  ACE_ASSERT (iparser_p);

  if (!iparser_p->isBlocking ())
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
  if (!iparser_p->switchBuffer ())
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
