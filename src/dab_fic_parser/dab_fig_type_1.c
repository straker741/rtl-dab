/*
This file is part of rtl-dab
trl-dab is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with rtl-dab.  If not, see <http://www.gnu.org/licenses/>.

Autor:
david may 2012
david.may.muc@googlemail.com

Contributor:
Jakub Svajka 2020
*/

#include "dab_fic_parser.h"

struct ProgrammeServiceLabel *appendServiceLabel(struct ProgrammeServiceLabel *lst, uint16_t SId, uint8_t * label, uint16_t charFlag) {

    struct ProgrammeServiceLabel *temp1;
    temp1 = lst;

    while(temp1->next!=NULL){
        if (temp1->SId == SId)
            return lst;
        temp1 = temp1->next;
    }

    struct ProgrammeServiceLabel *temp;
    temp = malloc(sizeof(struct ProgrammeServiceLabel));
    temp->SId = SId;
    memcpy(temp->label,label,16);
    temp->label[16] = '\0';
    temp->chFlag = charFlag;
    temp->next = lst;
    return temp;
}

uint8_t dab_fig_type_1(uint8_t *fig, Ensemble *sinfo) {
    // First byte of FIG consists of:  Charset (4 bits)  |  Rfu (1 bit)  |  Extension (3 bits)
    uint8_t extension = fig[0] & 0x03;
    //fprintf(stderr,"%u\n",extension);
    
    /* --- Ensemble label --- */
    if (extension == 0) {
        //fprintf(stderr,"FIG 1/0\n");
        // Ensemble Identifier (EId): this 16-bit field shall identify the ensemble. The coding details are given in clause 6.4.
        //The ensemble label has a repetition rate of once per second.
        sinfo->esl->EId = ((uint16_t)fig[1]<<8) + fig[2];   // 16 bits
        memcpy(sinfo->esl->label, &fig[3], 16);             // 16 bytes
        sinfo->esl->label[16] = '\0';                       // ending with \0 symbol to indicate end of string
        sinfo->esl->chFlag = ((uint16_t)fig[4]<<8) + fig[5];
    }
    /* --- Programm service label --- */
    else if (extension == 1) {
        sinfo->psl = appendServiceLabel(sinfo->psl,((uint16_t)fig[1]<<8) + fig[2],&fig[3],
            ((uint16_t)fig[4]<<8) + fig[5]);

    }
    else if (extension == 2) {
    }
    else if (extension == 3) {
    }
    /* --- Service component label --- */
    else if (extension == 4) {
        fprintf(stderr,"FIG 1/4\n");
        //fprintf(stderr,"%s\n",++fig);

    }
    /* --- Data service label --- */
    else  if (extension == 5) {
        fprintf(stderr,"FIG 1/5\n");
        //fprintf(stderr,"%u\n",(uint32_t)fig);
    }
    /* --- X-PAD User Application label --- */
    else if (extension == 6) {
        fprintf(stderr,"FIG 1/4\n");
        //fprintf(stderr,"%s\n",++fig);
    }
    else {
        fprintf(stderr,"FIG 1/%u\n",extension);
    }

    return 0;
}

/*
Summary of label FIGs
FIG type/ext    Clause      Description
________________________________________________________
FIG 1/0, 2/0    8.1.13      Ensemble label 
FIG 1/1, 2/1    8.1.14.1    Programme service label 
FIG 1/2, 2/2                - 
FIG 1/3, 2/3                - 
FIG 1/4, 2/4    8.1.14.3    Service component label 
FIG 1/5, 2/5    8.1.14.2    Data service label 
FIG 1/6, 2/6    8.1.14.4    X-PAD User Application label 
FIG 1/7, 2/7                -
*/