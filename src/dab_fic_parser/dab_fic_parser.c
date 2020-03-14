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

void dab_fic_parser_init(Ensemble * ens) {
    ens->sl = malloc(sizeof(struct ServiceList));
    ens->locked = 0;
    ens->sl->next = NULL;
    ens->sl->scp = malloc(sizeof(struct ServiceComponents));
    ens->sl->scp->next = NULL;
    ens->esl = malloc(sizeof(struct EnsembleLabel));
    ens->sco = malloc(sizeof(struct BasicSubchannelOrganization));
    ens->sco->next = NULL;
    ens->psl = malloc(sizeof(struct ProgrammeServiceLabel));
    ens->psl->next = NULL;
}

uint8_t dab_fic_parser(uint8_t fibs[12][256],Ensemble *ens,Analyzer *ana) {
    int32_t i,j;
    uint8_t fib_c[12][32];
    uint8_t type=0,length=0,shift=0;
    uint8_t charEnc;
    uint16_t faulty_fibs=0;

    for (i=0;i<12;i++) {
        /* CRC check */
        j = dab_crc16(fibs[i], 256);
        //fprintf(stderr,"%i",j);
        dab_bit_to_byte(fibs[i],fib_c[i],256);
        if (ens->locked) {
            ana->received_fibs +=1;
        }
        if (j==0) {
            ens->locked = 1;
            type = 0;
            length = 0;
            shift = 0;
            while (shift<30) {  
                // FIB is 32 bytes long but last 2 bytes are CRC. So we check only 30 bytes for each FIB
                // FIG header consists of FIG type (3 bits) and length (5 bits)
                type = fib_c[i][shift]>>5;          // shift by 5 bits clears out length bits
                length = fib_c[i][shift] & 0x1f;    
                // 0x1f = 0b.0001.1111 - clears out first 3 bits
                // only last 5 bits corresponds to length
                switch(type)
                {
                    case 0:
                        dab_fig_type_0(&fib_c[i][shift+1],ens,length);  // position of FIG data field (without header)
                        break;
                    case 1:
                        dab_fig_type_1(&fib_c[i][shift+1],ens);
                        break;
                    case 2:
                        fprintf(stderr,"FIG 2/x\n");
                        break;
                    case 3:
                        fprintf(stderr,"FIG 3/x\n");
                        break;
                    case 4:
                        fprintf(stderr,"FIG 4/x\n");
                        break;
                    case 5:
                        fprintf(stderr,"FIG 5/x\n");
                        break;
                    case 6:
                        fprintf(stderr,"FIG 6/x\n");
                        break;
                    case 7:
                        if (length!=31) /* End Marker */
                            fprintf(stderr,"FIG 7/x, len: %u \n",length);
                        break;
                    default:
                        break;
                }
            shift = shift+length+1; // currect position + length of currect FIG + 1 header of the currect FIG
            }
        }
        else {
            ana->faulty_fibs += 1;
            faulty_fibs += 1;
        }
    }

    if (faulty_fibs == 12) {
        ens->locked = 0;
        dab_analyzer_init(ana);
    }
    return 1;
}

/*
List of FIG types:
FIG type number     FIG type    FIG application
0                   000         MCI and part of the SI
1                   001         Labels, etc. (part of the SI)
2                   010         Labels, etc. (part of the SI) 
3                   011         Reserved
4                   100         Reserved
5                   101         Reserved
6                   110         Conditional Access (CA)
7                   111         Reserved (except for Length 31)
*/