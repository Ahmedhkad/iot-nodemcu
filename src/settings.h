/* special specs of NodeMCU pins */ 
//#define D0 16   //-pin is High on boot
//#define D1 5    //good
//#define D2 4    //good
//#define D3 0    //-boot failure if pulled low !!
//#define D4 2    //-boot failure if pulled low !!
//#define D5 14   //good  
//#define D6 12   //good  
//#define D7 13   //good
//#define D8 15 //-boot failure if pulled high !! must be low as in LDR with 4.7K R to GND
//#define RX 3    //-pin is High on boot , Output reversed

/* I/O pins selection of NodeMCU */ 
#define light01           16  //D0
#define light02           0   //D3
#define light03           2   //D4
#define relay01           5   //D1
#define relay02           4   //D2
#define dhtpin           14   //D5
#define button01         12   //D6
#define button02         13   //D7
#define button03         15   //D8
#define ldrpin           A0   //A0