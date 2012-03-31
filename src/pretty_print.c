#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pack_list.h"
#include "pretty_print.h"
#include "util.h"

/*debug*/
#include "layer2.h"
#include "layer3.h"

static void
print_header();

static char*
get_string_format();

extern void 
display_grid()
{
    struct pack_cap *hd;
    
    printf("Captured %d packets\n", packets_captured.len);

    print_header(); 

    FOREACH_PL_END(hd)
    {
        print_out(hd);
/*      dump_packet_data(hd); */
    }
}

static void
dump_packet_data(struct pack_cap *p)
{
    uint8_t i;
    uint8_t *ptr = (uint8_t *)(p->packet);

    printf("\n\nPartial Packet Data: \n");
    for(i=1; i<=52; i++)
    {
        printf("%02x ", *ptr++);
        if (!(i%4)) printf("\n");
    } 
}

extern void
debug_dump(struct pack_cap *p)
{
    uint8_t *ptr = (uint8_t *)(p->packet);
    uint8_t i;

    uint8_t vers_len;
    uint8_t code_cong;


    printf("Packet Dump (debug): \n");

    printf("**** Layer 2 ****\n");
    printf("SA: ");
    for(i=1; i<=6; i++)
    {
        printf("%02x ", *ptr++);
    }
    printf("\n");
    
    printf("DA: ");
    for(; i<=12; i++)
    {
        printf("%02x ", *ptr++);
    }
    printf("\n");

    printf("Type: ");
    for(; i<=14; i++)
    {
        printf("%02x ", *ptr++);
    }
    printf("\n");

    vers_len = *ptr++;
    code_cong = *ptr++;
    
    printf("Version: ");
    printf("%1x\n", vers_len >> 4);

    printf("\n**** Layer 3 ****\n");
    printf("Length: ");
    printf("%1x\n", vers_len & 0xF);

    printf("DSCP: ");
    printf("%02x\n", GET_BITS(code_cong, DSCPSTART_BIT, DSCPEND_BIT));

    printf("ECN: ");
    printf("%1x\n", GET_BITS(code_cong, ECNSTART_BIT, ECNEND_BIT));

    dump_packet_data(p);
}

extern void
print_out(struct pack_cap *pack)
{

    layer2_header *eth_header;
    uint8_t *ptr, loc_ptr = 0;
    char buffer[100];

    eth_header = (layer2_header *)(pack->packet);
    ptr = eth_header->source_addr;

    printf("%-5d | %-8s |", pack->packet_id, "2ms"); 

    /* Debug only... needs to be IP  */

    l2_get_da(pack, buffer);

    printf("%-18s  |", buffer);

    memset(buffer, 0, loc_ptr);

    l2_get_sa(pack, buffer);

    printf("%-18s  | ", buffer);
    printf("0x%-6x | ", l2_get_ethertype(pack));
    printf("%-5d ", pack->packet_len);

    printf("\n");
}

#define LINE_LENGTH 80

static void
print_header()
{
    uint8_t i = LINE_LENGTH;

    printf(get_string_format(), "ID", "Time Rx", "Source IP", "Dest IP", "Type", "Length");

    while(i--) printf("-");
    printf("\n");
    
}

#define ID_FORMAT "%-5s"
#define TIME_FORMAT "%-8s"
#define SOURCE_FORMAT "%-18s"
#define DEST_FORMAT SOURCE_FORMAT
#define TYPE_FORMAT "%-8s"
#define LENGTH_FORMAT "%-5s"

static char*
get_string_format()
{
    return ID_FORMAT " | " TIME_FORMAT " | " SOURCE_FORMAT " | " DEST_FORMAT " | " TYPE_FORMAT " | " LENGTH_FORMAT "\n";
}

