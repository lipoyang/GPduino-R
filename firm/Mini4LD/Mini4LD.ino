#include <GPduinoR.h>

// サーボ番号
enum {
    SERVO_RF_Y = 0, // 右前股
    SERVO_RF_Z,     // 右前膝
    SERVO_LF_Y,     // 左前股
    SERVO_LF_Z,     // 左前膝
    SERVO_RB_Y,     // 右後股
    SERVO_RB_Z,     // 右後膝
    SERVO_LB_Y,     // 左後股
    SERVO_LB_Z,     // 左後膝
};
// サーボチャンネル開始番号
#define SERVO_BEGIN 8

// サーボの向きとゼロ点オフセット
static const int SERVO_POLARITY[]={
// 0:右前股 1:右前膝 2:左前股 3:左前膝 4:右後股 5:右後膝 6:左後股 7:左後膝
  -1,      -1,      +1,      +1,      -1,      +1,      +1,      -1
};
static const int SERVO_OFFSET[]={
// 0:右前股 1:右前膝 2:左前股 3:左前膝 4:右後股 5:右後膝 6:左後股 7:左後膝
  18,       0,     -14,       3,     -17,     -13,      22,       5
};

// 1コマの時間[msec]
#define T_ORBIT 0//1//5//10
int t_orbit = 10; 

// 軌道テーブルのコマ数
#define N_TABLE 16
// 軌道テーブルの補間コマ数
#define N_INTERPOL 8
// 軌道の一周コマ数
#define N_ORBIT (N_TABLE * N_INTERPOL)
// 軌道の半周期のコマ数
#define N_HALF  (N_ORBIT/2)
// 歩きはじめ軌道のコマ数
#define N_ORBIT0 24

// 軌道テーブル(股)
const float Y_ORBIT[] = {
//     0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15 (コマ番号)
//    25,  15,   5,  -5, -15, -25, -25, -25, -25, -15,  -5,   5,  15,  25,  25,  25
       0, -10, -20, -25, -25, -25, -20, -10,   0,  10,  20,  25,  25,  25,  20,  10 // (1)
//     0,  -8, -16, -24, -24, -24, -16,  -8,   0,   8,  16,  24,  24,  24,  16,   8 // (2)
};
// 軌道テーブル(膝)
const float Z_ORBIT[] = {
//     0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15 (コマ番号)
//    10,  10,  10,  10,  10,  10,  15,  20,  25,  25,  25,  25,  25,  25,  20,  15
      10,  10,  10,  13,  18,  23,  26,  26,  26,  26,  26,  23,  18,  13,  10,  10 // (1)
//    10,  10,  10,  10,  18,  26,  26,  26,  26,  26,  26,  26,  18,  10,  10,  10 // (2)
};
// 軌道テーブル(膝) 歩きはじめ/歩きおわり用
const float Z0_ORBIT[] = {
//     0    1    2    3    4(コマ番号)
      10,  15,  20,  26,  26
};

// 軌道の位相(コマ数)
static int phase;

// 歩行方向 (前後方向)
static int walk_dir;

// 停止指令フラグ
static bool to_stop;

// 左右の歩幅倍率
static float r_step; // 右側歩幅指令値
static float l_step; // 左側歩幅指令値
static float rf_step, rb_step, lf_step, lb_step; // 各足の現在の歩幅
// 旋回時の歩幅倍率
#define STEP_LONG   1.5
#define STEP_SHORT  0.25

// 歩行状態
static int walk_state;
#define STATE_STOP      0   //停止状態
#define STATE_STARTING  1   //歩きはじめ状態
#define STATE_WALKING   2   //歩行状態
#define STATE_STOPPING  3   //歩きおわり状態

// プロポ状態保持用
static int g_fb;
static int g_lr;

// 送信バッファ
static char txbuff[256];
// 3.5V未満でローバッテリーとする
// (3.5V / 11) / 1.0V * 1024 =  325
#define LOW_BATTERY    325

/**
 * バッテリー電圧チェック
 */
void battery_check()
{
    static char txbuff[256];
    
    if(!UdpComm.isReady()) return;
    
    static int cnt1 = 0;
    static int cnt2 = 0;
    
    cnt1++;
    if(cnt1 < 10000) return;
    cnt1 = 0;

    unsigned short Vbat_ave = 500;//Battery.getAdc();
    //Serial.print("Vbat_ave");Serial.println(Vbat_ave);

    // 1秒ごとに電圧値送信
    cnt2++;
    if(cnt2 >= 10)
    {
        cnt2=0;
        
        txbuff[0]='#';
        txbuff[1]='B';
        Uint16ToHex(&txbuff[2], Vbat_ave, 3);
        txbuff[5]='$';
        txbuff[6]='\0';
        UdpComm.send(txbuff);
    }
}

// 軌道の補間
float orbit_interpol(const float* orbit, int phase)
{
    int n = phase / N_INTERPOL;
    int m = phase % N_INTERPOL;
    
    float x1 = orbit[n];
    n++;
    if(n>=N_TABLE) n=0;
    float x2 = orbit[n];
    
    float x = x1 + (x2-x1) * m / N_INTERPOL;
    
    return x;
}

// 初期設定
void setup() {
    Serial.begin(115200);
    delay(100);

    // UDP通信の設定
    UdpComm.beginAP(NULL, "12345678");
    //UdpComm.beginSTA("SSID", "password", "gpduino");
    UdpComm.onReceive = UdpComm_callback;

    // サーボ初期値設定
    RcServo.begin();
    for(int i=0;i<8;i++){
        RcServo.setPolarity(SERVO_BEGIN+i, SERVO_POLARITY[i]);
        RcServo.setOffset  (SERVO_BEGIN+i, SERVO_OFFSET[i]);
    }
    RcServo.out(SERVO_BEGIN+SERVO_RF_Y, orbit_interpol(Y_ORBIT, 0));
    RcServo.out(SERVO_BEGIN+SERVO_RF_Z, orbit_interpol(Z_ORBIT, 0));
    RcServo.out(SERVO_BEGIN+SERVO_LF_Y, orbit_interpol(Y_ORBIT, 0));
    RcServo.out(SERVO_BEGIN+SERVO_LF_Z, orbit_interpol(Z_ORBIT, 0));
    RcServo.out(SERVO_BEGIN+SERVO_RB_Y, orbit_interpol(Y_ORBIT, 0));
    RcServo.out(SERVO_BEGIN+SERVO_RB_Z, orbit_interpol(Z_ORBIT, 0));
    RcServo.out(SERVO_BEGIN+SERVO_LB_Y, orbit_interpol(Y_ORBIT, 0));
    RcServo.out(SERVO_BEGIN+SERVO_LB_Z, orbit_interpol(Z_ORBIT, 0));
    RcServo.powerOn();
    
    Battery.begin();
  
    phase = 0;
    walk_dir = 0;
    r_step = 1.0;
    l_step = 1.0;
    rf_step = r_step;
    rb_step = r_step;
    lf_step = l_step;
    lb_step = l_step;
    walk_state = STATE_STOP;
    to_stop = false;
    
    g_fb = 0;
    g_lr = 0;
}

// メインループ
void loop() {
    
    // バッテリーチェック
    battery_check();
    
    // UDP通信
    UdpComm.loop();
    
    int phase180; // 半周期ずれ位相
    
    switch(walk_state)
    {
    // 停止状態
    case STATE_STOP:
        if( walk_dir != 0){
            walk_state = STATE_STARTING;
            phase = 0;
        }
        break;
        
    // 歩きはじめ状態
    case STATE_STARTING:
        RcServo.out(SERVO_BEGIN+SERVO_RF_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_RF_Z, orbit_interpol(Z0_ORBIT, 0));
        RcServo.out(SERVO_BEGIN+SERVO_LF_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_LF_Z, orbit_interpol(Z0_ORBIT, phase));
        RcServo.out(SERVO_BEGIN+SERVO_RB_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_RB_Z, orbit_interpol(Z0_ORBIT, phase));
        RcServo.out(SERVO_BEGIN+SERVO_LB_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_LB_Z, orbit_interpol(Z0_ORBIT, 0));
        
        phase++;
        if(phase >  N_ORBIT0){
            phase = 0;
            walk_state = STATE_WALKING;
        }
        delay(t_orbit);//T_ORBIT);
        break;
        
    // 歩きおわり状態
    case STATE_STOPPING:
        RcServo.out(SERVO_BEGIN+SERVO_RF_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_RF_Z, orbit_interpol(Z0_ORBIT, 0));
        RcServo.out(SERVO_BEGIN+SERVO_LF_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_LF_Z, orbit_interpol(Z0_ORBIT, N_ORBIT0-phase));
        RcServo.out(SERVO_BEGIN+SERVO_RB_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_RB_Z, orbit_interpol(Z0_ORBIT, N_ORBIT0-phase));
        RcServo.out(SERVO_BEGIN+SERVO_LB_Y, 0);
        RcServo.out(SERVO_BEGIN+SERVO_LB_Z, orbit_interpol(Z0_ORBIT, 0));
        
        phase++;
        if(phase >  N_ORBIT0){
            walk_state = STATE_STOP;
        }
        delay(t_orbit);//T_ORBIT);
        break;
        
    // 歩行状態
    case STATE_WALKING:
        if( to_stop && (phase == 0)){
            walk_state = STATE_STOPPING;
            phase = 0;
            walk_dir = 0;
            to_stop = false;
            break;
        }
        phase180 = ((phase+N_HALF) % N_ORBIT);
        // 歩幅更新タイミング
        if(phase == 0){
            rf_step = r_step;
            lb_step = l_step;
        }
        if(phase180 == 0){
            lf_step = l_step;
            rb_step = r_step;
        }
        RcServo.out(SERVO_BEGIN+SERVO_RF_Y, orbit_interpol(Y_ORBIT, phase   ) * rf_step);
        RcServo.out(SERVO_BEGIN+SERVO_RF_Z, orbit_interpol(Z_ORBIT, phase   ));
        RcServo.out(SERVO_BEGIN+SERVO_LF_Y, orbit_interpol(Y_ORBIT, phase180) * lf_step);
        RcServo.out(SERVO_BEGIN+SERVO_LF_Z, orbit_interpol(Z_ORBIT, phase180));
        RcServo.out(SERVO_BEGIN+SERVO_RB_Y, orbit_interpol(Y_ORBIT, phase180) * rb_step);
        RcServo.out(SERVO_BEGIN+SERVO_RB_Z, orbit_interpol(Z_ORBIT, phase180));
        RcServo.out(SERVO_BEGIN+SERVO_LB_Y, orbit_interpol(Y_ORBIT, phase   ) * lb_step);
        RcServo.out(SERVO_BEGIN+SERVO_LB_Z, orbit_interpol(Z_ORBIT, phase   ));
        
        // 前進ないし後退
        if( walk_dir > 0 )
        {
            phase++;
            if(phase >=  N_ORBIT) phase = 0;
        }else{
            phase--;
            if(phase < 0) phase = N_ORBIT - 1;
        }
        
        delay(t_orbit);//T_ORBIT);
        break;
        
    // 異常な場合
    default:
        walk_state = STATE_STOP;
        break;
    }
}

void propo_ctrl()
{
    if(g_fb > 32){
        // 前進
        walk_dir = 1;
        to_stop = false;
        t_orbit = -((g_fb-31)*10)/96+10;
        if(t_orbit < 0) t_orbit = 0;
    }else if(g_fb < -32){
        // 後退
        walk_dir = -1;
        to_stop = false;
        t_orbit = ((g_fb+31)*10)/96+10;
        if(t_orbit < 0) t_orbit = 0;
    }else{
        // 停止
        to_stop = true;
    }
    
    if(g_fb != 0){
      if(g_lr > 64){
          // 右旋回
          r_step = STEP_SHORT;
          l_step = STEP_LONG;
      }else if(g_lr < -64){
          // 左旋回
          r_step = STEP_LONG;
          l_step = STEP_SHORT;
      }else{
          // 中立
          r_step = 1.0;
          l_step = 1.0;
      }
      //servo_ctrl(0);
    }else{
      if(g_lr > 64){
          // 右旋回
          r_step = STEP_SHORT;
          l_step = STEP_LONG;
          walk_dir = 1;
          to_stop = false;
      }else if(g_lr < -64){
          // 左旋回
          r_step = STEP_LONG;
          l_step = STEP_SHORT;
         walk_dir = 1;
        to_stop = false;
     }else{
          // 中立
          r_step = 1.0;
          l_step = 1.0;
          // 停止
          to_stop = true;
      }
      //servo_ctrl(g_lr);
    }
}

/**
 * 受信したコマンドの実行
 *
 * @param buff 受信したコマンドへのポインタ
 */
void UdpComm_callback(char* buff)
{
    int servoNum;
    unsigned short val;
    uint16_t pulselen;
    
    if(buff[0] != '#') return;
    buff++;
    
    switch(buff[0])
    {
    /* テスト:サーボコマンド */
    case 'S':
        // 全サーボ制御コマンド
        if(buff[1] == 'A')
        {
            for(servoNum=0; servoNum<8; servoNum++){
                if( HexToUint16(&buff[2+servoNum*2], &val, 2) == 0 ){
                    if(val <= 180){
                        RcServo.out(servoNum, (int)val - 90);
                    }
                }
            }
        }
        // テスト:個別サーボ制御コマンド
        else
        {
            servoNum = buff[1] - '0';
            if(servoNum < 8){
                if( HexToUint16(&buff[2], &val, 2) == 0 ){
                    if(val <= 180){
                        RcServo.out(servoNum, (int)val - 90);
                    }
                }
            }
        }
        break;
    
    /* テスト:歩行コマンド */
    case 'W':
        // 左右方向
        switch(buff[1]){
        // 左旋回
        case 'L':
            r_step = STEP_LONG;
            l_step = STEP_SHORT;
            break;
        // 右旋回
        case 'R':
            r_step = STEP_SHORT;
            l_step = STEP_LONG;
            break;
        // 直進
        case 'N':
            r_step = 1.0;
            l_step = 1.0;
            break;
        }
        // 前後方向
        switch(buff[2]){
        // 前進
        case 'F':
            walk_dir = 1;
            to_stop = false;
            break;
        // 後退
        case 'B':
            walk_dir = -1;
            to_stop = false;
            break;
        // 停止
        case 'N':
            // walk_dir = 0;
            to_stop = true;
            break;
        }
        break;
    
    /* GPduino標準コマンド */
    
    /* Dコマンド(前進/後退)
       書式: #Dxx$
       xx: 0のとき停止、正のとき前進、負のとき後退。
     */
    case 'D':
        // 値の解釈
        if( HexToUint16(&buff[1], &val, 2) != 0 ) break;
        g_fb = (int)((signed char)val);
        propo_ctrl();
        break;
        
    /* Tコマンド(旋回)
       書式: #Txx$
       xx: 0のとき中立、正のとき右旋回、負のとき左旋回
     */
    case 'T':
        // 値の解釈
        if( HexToUint16(&buff[1], &val, 2) != 0 ) break;
        g_lr = (int)((signed char)val);
        propo_ctrl();
        break;
    }
}
