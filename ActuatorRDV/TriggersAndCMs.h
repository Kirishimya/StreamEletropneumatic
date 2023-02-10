#ifndef TRIGGERSANDCMS_H
#define TRIGGERSANDCMS_H
namespace trs_and_cms {
  /*Aqui são definidos macros e constantes*/
#define MAXACTS      2
#define MAXBUTTONS   4
#define ACTPINSLEN   2
//tipos de gatilho para acionamento ou para o timer
//    enum TRIGGER_TYPES{
//      noTrigger=0,//não há gatilho
//      tCn,        //gatilho do tipo comando (char)
//      tYn,        //gatilho do tipo solenoide (bool)
//      tSn,        //gatilho do tipo sensor fim de curso (bool)
//      tEn         //gatilho externo (bool)
//    };
//    enum CMs{
//
//    };

//tipos de gatilho para acionamento ou para o timer
#define NOTRIGGER    0//não há gatilho
#define TCN          1//gatilho do tipo comando (char)
#define TYN          2//gatilho do tipo solenoide (bool)
#define TSN          3//gatilho do tipo sensor fim de curso (bool)
#define TAN          4//gatilha de acordo com o numero de avanços
#define TRN          5//gatilha de acordo com o numero de retornos
#define TEN          6//gatilho externo (bool);Ex.: um botão
//comandos
#define ADVANCE      'a'
#define RETREAT      'r'
#define TURNOFF      'f'
#define SETUP        's'//inicia uma linha de configuração
#define TIMER        'T'
#define VALUE        'Q'
//#define TIMERON      'h'
//#define TIMEROFF     'l'
#define ACTUATOR     'A'
#define TRIGTYPE     'P'
#define TRIGGER      't'
#define DONE         'd'//termina configuração
#define BREAK        'b'//sai de um loop infinito
#define WAITING      '?'//esperando dados
#define ALERT        '!'
#define RESET        'R'
#define RSPRING      'M'
#define TYPESPRING   'm'
#define RSOLENOID    'V'
#define TYPESOLENOID 'v'
#define SELECT       'S'
#define CLEAR        'C'
#define BUTTON       'B'
#define REQUEST      'W'//indica que o aplicativo necessita de dados um pedido vindo do aplicativo
#define CANCELREQUEST 'X'//cancela pedido
//M1b
//static const TRIGGER_TYPES tr_types[] = {noTrigger, tCn, tYn, tSn, tEn};
}
#endif
