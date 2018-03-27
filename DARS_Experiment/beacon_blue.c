#include <kilolib.h>
#include <stdlib.h>

#define quality 30

int message_sent = 0;
message_t message;
uint32_t message_last_changed = 0;
int odd = 0;

enum commitment{Ci, Cj ,uncommited};

void setup()
{
    srand(rand_hard());
}

void loop()
{
    if (kilo_ticks > message_last_changed + 64)
    {
        message_last_changed = kilo_ticks;
        message.type = NORMAL;
        message.data[0] = Ci;
        message.data[1] = quality;
        message.crc = message_crc(&message);

    }

    // Blink the LED magenta whenever a message is sent.
    if (message_sent)
    {
        message_sent = 0;
        //printf("Blue sent: %d %d\n", message.data[0], message.data[1]);
        set_color(RGB(0, 0, 1));
        delay(100);
        set_color(RGB(0, 0, 0));
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
    kilo_message_tx = message_tx;
    kilo_message_tx_success = message_tx_succes;
    kilo_start(setup, loop);

    return 0;
}
