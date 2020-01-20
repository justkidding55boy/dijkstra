struct myftph {
  uint8_t   type;
  uint8_t   code;
  uint16_t  length;
  char      data[0];
};

//1024+4
#define PKTSIZE     1028

#define SRVPORT     51230


//type field
#define T_QUIT      0x01
#define T_PWD       0x02
#define T_CWD       0x03
#define T_LIST      0x04
#define T_RETR      0x05
#define T_STOR      0x06
#define T_OK        0x10
#define T_CMD_ERR   0x11
#define T_FILE_ERR  0x12
#define T_UNKWN_ERR 0x13
#define T_DATA      0x20

//code
#define C_NM        0x00
#define C_DATA_SC   0x01
#define C_DATA_CS   0x02
#define C_SYNTAX    0x01
#define C_UNDCLRD   0x02
#define C_PRTCL     0x03
#define C_UNEXIST   0x00
#define C_PRMIT     0x01
#define C_UNKWN_ERR 0x05
#define C_DATA_END  0x00
#define C_DATA_CONT 0x01
