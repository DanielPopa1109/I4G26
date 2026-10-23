#include "g26_update.h"
#include "g26_tp.h"
#include "g26_uds.h"
#include "g26_mcp2515.h"
#include <assert.h>
#ifdef NDEBUG
#error Test assertions must be enabled
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static uint8_t flash_memory[G26_FLASH_END - G26_FLASH_BASE];
static int erase_app(void *ignored)
{
    (void)ignored;
    memset(flash_memory + G26_APP_BASE - G26_FLASH_BASE, 0xFF,
           G26_FLASH_END - G26_APP_BASE);
    return 0;
}
static int write_flash(void *ignored, uint32_t address, const uint8_t *data, size_t length)
{
    size_t i;
    (void)ignored;
    if (!G26_AppRangeValid(address, length)) return -1;
    for (i = 0; i < length; ++i) {
        uint8_t *target = &flash_memory[address - G26_FLASH_BASE + i];
        if ((*target & data[i]) != data[i]) return -1;
        *target &= data[i];
    }
    return 0;
}
static int read_flash(void *ignored, uint32_t address, uint8_t *data, size_t length)
{
    (void)ignored;
    if (!G26_AppRangeValid(address, length)) return -1;
    memcpy(data, flash_memory + address - G26_FLASH_BASE, length);
    return 0;
}

static void test_update(void)
{
    G26_Flash flash = {0, erase_app, write_flash, read_flash};
    G26_Update update;
    uint8_t image[16] = {0x00,0xFC,0x01,0x20,0x09,0x02,0x02,0x08,
                         1,2,3,4,5,6,7,8};
    uint32_t crc = G26_Crc32(0xFFFFFFFFu, image, sizeof image) ^ 0xFFFFFFFFu;
    memset(flash_memory, 0xA5, G26_APP_BASE - G26_FLASH_BASE);
    erase_app(0);
    assert(!G26_ImageValid(&flash));
    assert(!G26_AppRangeValid(G26_APP_BASE - 1, 1));
    assert(!G26_AppRangeValid(G26_FLASH_END - 1, 2));
    G26_UpdateInit(&update, flash);
    assert(G26_UpdateBegin(&update, sizeof image, crc) == G26_UPDATE_OK);
    assert(flash_memory[0] == 0xA5);
    assert(G26_UpdateBlock(&update, 2, image, 8) == G26_UPDATE_SEQUENCE);
    assert(G26_UpdateBlock(&update, 1, image, 8) == G26_UPDATE_OK);
    assert(G26_UpdateBlock(&update, 1, image, 8) == G26_UPDATE_OK);
    assert(G26_UpdateFinish(&update) == G26_UPDATE_SEQUENCE);
    assert(!G26_ImageValid(&flash));
    assert(G26_UpdateBlock(&update, 2, image + 8, 8) == G26_UPDATE_OK);
    assert(G26_UpdateFinish(&update) == G26_UPDATE_OK);
    assert(G26_ImageValid(&flash));
    assert(G26_UpdateBegin(&update, sizeof image, crc) == G26_UPDATE_OK);
    assert(!G26_ImageValid(&flash));
    assert(G26_UpdateBlock(&update, 1, image, 8) == G26_UPDATE_OK);
    update.expected_crc ^= 1;
    assert(G26_UpdateBlock(&update, 2, image + 8, 8) == G26_UPDATE_OK);
    assert(G26_UpdateFinish(&update) == G26_UPDATE_CRC);
    assert(!G26_ImageValid(&flash));
}

static void test_tp(void)
{
    G26_Tp tp;
    G26_CanFrame in = {G26_DIAG_REQUEST_ID,8,{G26_DIAG_ROUTE,0x03,0x22,0xF1,0x90}}, out;
    uint8_t response[18] = {0x62,0xF1,0x90};
    G26_TpInit(&tp);
    assert(G26_TpReceive(&tp, &in, 0, &out) == 1);
    assert(tp.rx_length == 3 && tp.rx[0] == 0x22);
    in.data[1] = 0x10; in.data[2] = 11;
    assert(!G26_TpReceive(&tp, &in, 1, &out));
    assert(out.dlc == 8 && out.data[1] == 0x30 && out.data[2] == 8);
    in.data[1] = 0x11; in.data[2] = 1; /* 257 exceeds bounded buffer */
    assert(!G26_TpReceive(&tp, &in, 1, &out) && out.data[1] == 0x32);
    in.data[1] = 0x10; in.data[2] = 11;
    assert(!G26_TpReceive(&tp, &in, 1, &out));
    in.data[1] = 0x21;
    assert(G26_TpReceive(&tp, &in, 2, &out) == 1);
    assert(tp.rx_length == 11);
    assert(G26_TpStart(&tp, response, sizeof response, 3, &out));
    assert(out.data[1] == 0x10 && tp.tx_wait_fc);
    in.data[0]=0x41; in.data[1] = 0x30; in.data[2] = 0; in.data[3] = 0;
    G26_TpReceive(&tp, &in, 4, &out);
    assert(G26_TpNext(&tp, 4, &out) && out.data[1] == 0x21);
    assert(G26_TpNext(&tp, 4, &out) && out.data[1] == 0x22);
    assert(G26_TpNext(&tp, 4, &out) && out.data[1] == 0x23);
    assert(!tp.tx_active);
    assert(G26_TpStart(&tp, response, sizeof response, 5, &out));
    in.data[3] = 0xF1;
    G26_TpReceive(&tp, &in, 6, &out);
    assert(tp.tx_stmin == 1 && !tp.tx_wait_fc);
    assert(G26_TpNext(&tp, 6, &out));
    assert(!G26_TpNext(&tp, 6, &out));
}

static void test_uds(void)
{
    G26_Flash flash = {0, erase_app, write_flash, read_flash};
    G26_Uds uds;
    uint8_t out[16], req[] = {0x34,0x00,0x44,0x08,0x02,0x02,0x00,0,0,0,16};
    G26_UdsInit(&uds, flash);
    assert(G26_UdsHandle(&uds, req, sizeof req, 0, out, sizeof out) == 3);
    assert(out[0] == 0x7F && out[2] == 0x7F);
    req[0] = 0x10; req[1] = 2;
    assert(G26_UdsHandle(&uds, req, 2, 1, out, sizeof out) == 6);
    req[0] = 0x34; req[1] = 0;
    assert(G26_UdsHandle(&uds, req, 11, 2, out, sizeof out) == 3);
    assert(out[0] == 0x7F && out[2] == 0x24);
    {
        const uint8_t erase[] = {0x31,0x01,0x00,0x01,0x08,0x02,0x02,0x00,
                                 0,0,0,16};
        assert(G26_UdsHandle(&uds, erase, sizeof erase, 3, out, sizeof out) == 4);
        assert(out[0] == 0x71);
    }
    assert(G26_UdsHandle(&uds, req, 11, 4, out, sizeof out) == 4);
    assert(out[0] == 0x74 && out[3] == 242);
    G26_UdsTick(&uds, 6003);
    assert(uds.session == 1 && !uds.update.active);
}

static void test_full_programming(const char *filename)
{
 FILE *file=fopen(filename,"rb");assert(file);
 uint8_t *image=malloc(G26_FLASH_END-G26_APP_VECTOR);assert(image);
 size_t n=fread(image,1,G26_FLASH_END-G26_APP_VECTOR,file);assert(n>512 && fgetc(file)==EOF);fclose(file);
 G26_Flash flash={0,erase_app,write_flash,read_flash};G26_Uds uds;G26_UdsInit(&uds,flash);
 uint8_t r[256]={0x10,2},out[16];assert(G26_UdsHandle(&uds,r,2,0,out,16)==6);
 r[0]=0x31;r[1]=1;r[2]=0;r[3]=1;
 uint32_t address=G26_APP_VECTOR;
 for(unsigned i=0;i<4;++i){r[4+i]=(uint8_t)(address>>(24-8*i));r[8+i]=(uint8_t)(n>>(24-8*i));}
 assert(G26_UdsHandle(&uds,r,12,1,out,16)==4 && out[0]==0x71);
 r[0]=0x36;r[1]=1;r[2]=image[0];G26_UdsHandle(&uds,r,3,2,out,16);assert(out[0]==0x7F && out[2]==0x24);
 r[0]=0x34;r[1]=0;r[2]=0x44;
 for(unsigned i=0;i<4;++i){r[3+i]=(uint8_t)(address>>(24-8*i));r[7+i]=(uint8_t)(n>>(24-8*i));}
 assert(G26_UdsHandle(&uds,r,11,3,out,16)==4 && out[0]==0x74);
 uint8_t bsc=1;uint32_t now=4;
 for(size_t at=0;at<n;){size_t amount=n-at;if(amount>240)amount=240;
  r[0]=0x36;r[1]=bsc++;memcpy(r+2,image+at,amount);
  assert(G26_UdsHandle(&uds,r,amount+2,now++,out,16)==2 && out[0]==0x76);
  assert(G26_UdsHandle(&uds,r,amount+2,now++,out,16)==2 && out[0]==0x76);
  at+=amount;
 }
 uint32_t crc=G26_Crc32(~0U,image,n)^~0U;r[0]=0x37;
 for(unsigned i=0;i<4;++i)r[1+i]=(uint8_t)(crc>>(24-8*i));
 assert(G26_UdsHandle(&uds,r,5,now,out,16)==1 && out[0]==0x77 && G26_ImageValid(&flash));
 free(image);puts("PASS resident FBL: actual linked APP binary UDS erase/download/transfer/duplicate blocks/CRC/commit/validation");
}

typedef struct { uint8_t reg[128], cmd, address, mask; unsigned pos; } FakeMcp;
static void fake_select(void *context, int active)
{
    FakeMcp *m = context;
    if (active) m->pos = 0;
}
static int fake_exchange(void *context, uint8_t tx, uint8_t *rx)
{
    FakeMcp *m = context;
    *rx = 0;
    if (m->pos++ == 0) {
        m->cmd = tx;
        if (tx == 0xC0) { memset(m->reg, 0, sizeof m->reg); m->reg[0x0E] = 0x80; }
        return 0;
    }
    if (m->cmd == 0x03) {
        if (m->pos == 2) m->address = tx;
        else *rx = m->reg[m->address++];
    } else if (m->cmd == 0x02) {
        if (m->pos == 2) m->address = tx;
        else {
            m->reg[m->address++] = tx;
            if (m->address == 0x10) m->reg[0x0E] = tx & 0xE0u;
        }
    } else if (m->cmd == 0x05) {
        if (m->pos == 2) m->address = tx;
        else if (m->pos == 3) m->mask = tx;
        else {
            m->reg[m->address] = (m->reg[m->address] & ~m->mask) | (tx & m->mask);
            if (m->address == 0x0F) m->reg[0x0E] = m->reg[0x0F] & 0xE0u;
        }
    }
    return 0;
}
static void fake_delay(void *context, uint32_t ms) { (void)context; (void)ms; }

static void test_mcp(void)
{
    FakeMcp fake = {{0},0,0,0,0};
    G26_McpBus bus = {&fake, fake_select, fake_exchange, fake_delay};
    G26_Mcp2515 device;
    G26_CanFrame tx = {G26_DIAG_RESPONSE_ID,3,{0xF0,0x01,0x77}}, rx;
    G26_McpInitState(&device, bus);
    assert(!G26_McpConfigureBitrate(&device,8000000U,500000U));
    assert(G26_McpStart(&device) == 0);
    assert(fake.reg[0x28] == 0x82 && fake.reg[0x29] == 0x90 && fake.reg[0x2A] == 0);
    assert(fake.reg[0x00] == (G26_DIAG_REQUEST_ID >> 3));
    assert(G26_McpSend(&device, &tx) == 0);
    assert(fake.reg[0x35] == 3 && fake.reg[0x36] == 0xF0);
    assert(G26_McpTxStatus(&device)==0); /* Buffer admission is not completion. */
    fake.reg[0x2C]|=4;assert(G26_McpTxStatus(&device)==1);
    assert(!G26_McpSend(&device,&tx));fake.reg[0x30]=0x10;assert(G26_McpTxStatus(&device)==-1);
    fake.reg[0x30]=0;
    for(unsigned osc=8000000;osc<=20000000;osc+=4000000){
      if(osc==12000000)continue;
      assert(!G26_McpConfigureBitrate(&device,osc,500000));assert(!G26_McpStart(&device));
      unsigned tq=1+(fake.reg[0x29]&7)+1+((fake.reg[0x29]>>3)&7)+1+(fake.reg[0x28]&7)+1;
      assert(osc/(2*((fake.reg[0x2A]&63)+1)*tq)==500000);
    }
    assert(G26_McpConfigureBitrate(&device,0,500000));
    assert(G26_McpStart(&device));
    assert(!G26_McpConfigureBitrate(&device,8000000,500000));
    fake.reg[0x2C] = 1;
    fake.reg[0x61] = G26_DIAG_REQUEST_ID >> 3;
    fake.reg[0x62] = (G26_DIAG_REQUEST_ID & 7u) << 5;
    fake.reg[0x65] = 3;
    fake.reg[0x66] = G26_DIAG_ROUTE;
    fake.reg[0x67] = 1;
    fake.reg[0x68] = 0x77;
    assert(G26_McpPoll(&device, &rx) == 1);
    assert(rx.id == G26_DIAG_REQUEST_ID && rx.dlc == 3 && rx.data[0] == G26_DIAG_ROUTE);
    assert(!(fake.reg[0x2C] & 1));
}

int main(int argc,char **argv)
{
    test_update();
    test_tp();
    test_uds();
    test_mcp();
    if(argc==2)test_full_programming(argv[1]);
    return 0;
}
