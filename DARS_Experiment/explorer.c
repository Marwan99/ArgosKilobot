#include <kilolib.h>
#include <stdio.h>
#include <stdlib.h>

#define pa1 5
#define pa2 5

long int cur_time, motion_prev_time=-401, commit_prev_time, send_prev_time;
char motion_state;
int commit_state;
unsigned char mes_from;
unsigned char mes_payload;
unsigned char rec_quality[2];
char flag=0;
int randvar;
int new_message;
int turn_time;
int x;
int message_sent = 0;
char com[2]={30,60};

message_t message;

enum commitment{Ci, Cj ,uncommited};

void led_colour(char x)
{
    if(x==0) //Ci
        set_color(RGB(0,0,1)); //blue
    else if(x==1) //Cj
        set_color(RGB(1,0,0)); //red
    else if(x==2) //uncommited
        set_color(RGB(0,1,0)); //green
}

// receive message callback
void message_rx(message_t *message, distance_measurement_t *distance_measurement)
{
    // Set flag on message reception.
    new_message = 1;

    mes_from = message->data[0];
    mes_payload = message->data[1];
}


void setup()
{
    //printf("Hello World\n");
    //printf("%d %d %d\n", Ci, Cj, uncommited);
    srand(rand_hard());

    motion_state=1;
    commit_state = uncommited;
    led_colour(commit_state);
}

void loop()
{
    cur_time = kilo_ticks;

    /*----------------motion state decision--------------------*/
    if(motion_state == 1 && kilo_ticks-motion_prev_time >500)
    {
        motion_state = 2;
        motion_prev_time = kilo_ticks;
        turn_time = rand()%100;
        spinup_motors();

        if(rand()%2)
            set_motors(0, kilo_turn_right);
        else
            set_motors(kilo_turn_left, 0);

    }
    else if(motion_state == 2 && kilo_ticks-motion_prev_time > turn_time)
    {
        motion_state = 1;
        motion_prev_time = kilo_ticks;
        spinup_motors();
        set_motors(255, 255);
    }
    /*--------------------Recording messages-----------------------*/
    if(new_message)
    {
        new_message = 0;
        rec_quality[mes_from]=mes_payload;
        flag=1;
    }

    /*----------------updating commitment state decision--------------------*/

    if(cur_time-commit_prev_time > 200)
    {

        commit_prev_time = cur_time;
        x = rand() % 100;

        printf("Before :%d ",commit_state);
        /*-----When uncommitted------*/
        if(commit_state==uncommited)
        {
            if (flag)
            {
                flag = 0;

                if (x < rec_quality[Ci])
                    commit_state = Ci;
                else if (x >= rec_quality[Ci] && x < (rec_quality[Ci] + rec_quality[Cj]))
                    commit_state = Cj;
            }
        }
            /*-----When committed------*/
        else
        {
            if(flag && commit_state==Cj && x < rec_quality[Ci])
                commit_state == uncommited;

            else if(flag && commit_state==Ci && x >= rec_quality[Ci] && x < (rec_quality[Ci] + rec_quality[Cj]))
                commit_state == uncommited;

            else if (commit_state == Ci && rand()%100 < pa1)
                commit_state = uncommited;

            else if (commit_state == Cj && rand()%100 < pa2)
                commit_state = uncommited;
        }

        rec_quality[Ci] = 0;
        rec_quality[Cj] = 0;

        printf(", rand :%d ",x);
        printf(", after :%d\n",commit_state);
        //printf("Payload: %d. From: %d Rand: %d. State: %d\n",mes_payload, mes_from, x, commit_state);

        led_colour(commit_state);
    }

    /*----------------Broadcasting state--------------------*/
    if(commit_state!=uncommited && cur_time - send_prev_time > 100)
    {
        send_prev_time = cur_time;
        message.type = NORMAL;
        message.data[0] = commit_state;
        message.data[1] = com[commit_state];
        message.crc = message_crc(&message);
    }
}

message_t *message_tx()
{
    return &message;
}

void message_tx_succes()
{
    message_sent = 1;
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_message_tx = message_tx;
    kilo_message_tx_success = message_tx_succes;
    kilo_start(setup, loop);
    return 0;
}
