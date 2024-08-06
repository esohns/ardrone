%top{
#ifndef YY_TYPEDEF_YY_MAVLINK_SCANNER_T
#define YY_TYPEDEF_YY_MAVLINK_SCANNER_T

#define YY_STRUCT_YY_BUFFER_STATE
#include "ardrone_mavlink_scanner.h"
#undef YY_STRUCT_YY_BUFFER_STATE

#endif

// forward declarations
typedef void* yyscan_t;
class ARDrone_MAVLink_IParser;

#define YY_DECL int ARDrone_MAVLink_Scanner_lex (yyscan_t yyscanner)
// ... and declare it for the parser's sake
YY_DECL;

void ARDrone_MAVLink_Scanner_set_column (int, yyscan_t);

#define YY_NO_UNISTD_H

//#define FLEXINT_H
}

%{
#include "ace/Log_Msg.h"

#include "ardrone_common.h"
#include "ardrone_macros.h"

#include "ardrone_module_mavlink_decoder.h"
%}

%option header-file="ardrone_mavlink_scanner.h" outfile="ardrone_mavlink_scanner.cpp"
%option batch 8bit nodefault never-interactive nostdinit yylineno yywrap
/* %option c++ yyclass="ARDrone_MAVLink_Scanner" */
/* *NOTE*: the 'line' directives confuse gdb */
/* *NOTE*: 'noline' not supported (on Linux, flex 2.5.39)
           --> use --noline and (manually) remove '#line's introduced by %top */
/* %option ansi-definitions ansi-prototypes */
/* *IMPORTANT NOTE*: 'read' requires 'unistd'(.h) */
%option reentrant pointer prefix="ARDrone_MAVLink_Scanner_" nomain
/* *TODO*: find out why 'read' does not compile (on Linux, flex 2.5.39) */
%option align read full
%option backup debug perf-report perf-report verbose warn
/* *IMPORTANT NOTE*: flex 2.5.4 does not recognize 'reentrant, nounistd,
                     ansi-definitions, ansi-prototypes, header-file extra-type'
*/
%option extra-type="ARDrone_MAVLink_IParser*"
%option nounistd nounput noyymore

OCTET                  [\x00-\xFF]
V20_START_CODE         \xFD
V10_START_CODE         \xFE
V09_START_CODE         \x55

%s                     length
%s                     sequence
%s                     system_id
%s                     component_id
%s                     message_id
%s                     message_id_2
%s                     data
%s                     _checksum
%s                     end_of_frame

%% /* end of definitions */

%{
  // sanity check(s)
  ACE_ASSERT (yyscanner);

  ARDrone_MAVLink_IParser* iparser_p =
    ARDrone_MAVLink_Scanner_get_extra (yyscanner);
  ACE_ASSERT (iparser_p);

  struct __mavlink_message* message_p = NULL;
  static size_t remaining_data_bytes_i = 0;
%}

<INITIAL>{
{V09_START_CODE}|{V10_START_CODE} { message_p = &iparser_p->current ();

                                    BEGIN (length); }
} // end <INITIAL>
<length>{
{OCTET}                           { ACE_ASSERT (message_p);
                                    message_p->len =
                                      *reinterpret_cast<uint8_t*> (yytext);
                                    remaining_data_bytes_i = message_p->len;
                                    BEGIN (sequence); }
} // end <length>
<sequence>{
{OCTET}                           { ACE_ASSERT (message_p);
                                    message_p->seq =
                                      *reinterpret_cast<uint8_t*> (yytext);
                                    BEGIN (system_id); }
} // end <sequence>
<system_id>{
{OCTET}                           { ACE_ASSERT (message_p);
                                    message_p->sysid =
                                      *reinterpret_cast<uint8_t*> (yytext);
                                    BEGIN (component_id); }
} // end <system_id>
<component_id>{
{OCTET}                           { ACE_ASSERT (message_p);
                                    message_p->compid =
                                      *reinterpret_cast<uint8_t*> (yytext);
                                    BEGIN (message_id); }
} // end <component_id>
<message_id>{
{OCTET}                           { ACE_ASSERT (message_p);
                                    message_p->msgid =
                                      *reinterpret_cast<uint8_t*> (yytext);
                                    BEGIN (data); }
} // end <message_id>
<message_id_2>{
{OCTET}{3}                        { ACE_ASSERT (message_p);
                                    ACE_ASSERT (yyleng == 3);
                                    message_p->msgid =
                                      ((*reinterpret_cast<uint8_t*> (yytext[0]))      |
                                       (*reinterpret_cast<uint8_t*> (yytext[1]) << 1) |
                                       (*reinterpret_cast<uint8_t*> (yytext[2]) << 2));

                                    BEGIN (data); }
} // end <message_id_2>
<data>{
{OCTET}                           { ACE_ASSERT (message_p);

                                    if (remaining_data_bytes_i)
                                      --remaining_data_bytes_i;

                                    if (!remaining_data_bytes_i)
                                      BEGIN (_checksum);
                                  }
} // end <data>
<_checksum>{
{OCTET}{2}                        { ACE_ASSERT (message_p);
                                    ACE_ASSERT (yyleng == sizeof (uint16_t));
                                    message_p->checksum =
                                      *reinterpret_cast<uint16_t*> (yytext);
                                    if (ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN)
                                      message_p->checksum =
                                          ACE_SWAP_WORD (message_p->checksum);

                                    ACE_ASSERT (iparser_p);
                                    struct __mavlink_message* message_2 =
                                        message_p;
                                    try {
                                      iparser_p->record (message_2);
                                    } catch (...) {
                                      ACE_DEBUG ((LM_ERROR,
                                                  ACE_TEXT ("failed to Stream_IYaccStreamParser_T::record(), aborting\n")));
                                      yyterminate ();
                                    }

                                    BEGIN (INITIAL); }
} // end <checksum>
<<EOF>>                { return -1; }
<*>{OCTET}             { /* *TODO*: use (?s:.) ? */
                         yyterminate (); }

%% /* end of rules */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
ARDrone_MAVLink_Scanner_wrap (yyscan_t yyscanner)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDrone_MAVLink_Scanner_wrap"));

  struct yyguts_t* yyg = static_cast<struct yyguts_t*> (yyscanner);
  ARDrone_MAVLink_IParser* iparser_p =
    ARDrone_MAVLink_Scanner_get_extra (yyscanner);
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
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("failed to Common_IScannerBase::switchBuffer(), aborting\n")));
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
