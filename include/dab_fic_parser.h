#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "dab_helper_functions.h"
#include "dab_analyzer.h"

typedef struct{
  uint8_t locked;
  struct ServiceList *sl;
  struct BasicSubchannelOrganization *sco;
  struct ProgrammeServiceLabel *psl;
  struct EnsembleLabel *esl;
  struct DateTime *dt;
}Ensemble;

struct ServiceComponents{
  uint8_t TMId;
  uint8_t ASCTy;
  uint8_t DSCTy;
  uint16_t SCId;
  uint8_t SubChId;
  uint8_t FIDCId;
  uint8_t PS;
  uint8_t CAFlag;
  struct ServiceComponents *next;
};

struct ServiceList {
  uint32_t SId;
  uint8_t ECC;
  uint8_t CountryId;
  uint32_t ServiceReference;
  uint8_t localFlag;
  uint8_t CAId;
  uint8_t NumberOfSCs;
  struct ServiceComponents * scp;
  struct ServiceList *next;
};

/* FIG 0/1 */
struct BasicSubchannelOrganization {
  uint8_t SubChId;
  uint16_t startAddr;
  uint8_t shortlong;
  uint8_t tableSwitch;
  uint8_t tableIndex;
  uint8_t option;
  uint8_t protectionLevel;
  uint16_t subchannelSize;
  struct BasicSubchannelOrganization * next;
};

/* FIG 1/1 */
struct ProgrammeServiceLabel {
  uint8_t charset;
  uint8_t OE;
  uint8_t extension;
  uint16_t SId;
  uint8_t label[17];
  uint16_t chFlag;
  struct ProgrammeServiceLabel * next;
};

struct EnsembleLabel {
  uint8_t charset;        //this 4-bit field shall identify a character set to qualify the character information contained in the FIG type 1 field. The interpretation of this field shall be as defined in ETSI TS 101 756
  uint8_t OE;             //this 1-bit flag shall indicate, according to the Extension, whether the information is related to this or another ensemble
  uint8_t extension;      //this 5-bit field, expressed as an unsigned binary number, identifies one of 32 interpretations of the FIG type 0 field. Those extensions, which are not defined, are reserved for future use.
  uint16_t EId;           //this field is defined individually for each extension of the FIG type 1 field
  uint8_t label[17];      //this 16-byte field shall define the label. It shall be coded as a string of up to 16 characters, which are chosen from the character set signalled by the Charset field in the first byte of the FIG type 1 data field.
                          //The characters are coded from byte 15 to byte 0. The first character starts at byte 15. Bytes at the end of the character field that are not required to carry the label shall be filled with 0x00.
  uint16_t chFlag;        //Character flag field: this 16-bit flag field shall indicate which of the characters of the character field are to be displayed in an abbreviated form of the label
};

struct DateTime {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t miliseconds;
};

uint8_t dab_fic_parser(uint8_t fibs[12][256],Ensemble * ens,Analyzer *ana);
void dab_fic_parser_init(Ensemble *ens);

uint8_t dab_fig_type_1(uint8_t * fig,Ensemble * ens);
uint8_t dab_fig_type_0(uint8_t * fig,Ensemble * ens,uint32_t length);
